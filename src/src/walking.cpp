/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "common_libs.h"
#include "walking.h"
#include "sregions.h"
#include "sndpkg.h"
#include "house.h"
#include "tmpeff.h"
#include "telport.h"
#include "boats.h"
#include "map.h"
#include "inlines.h"
#include "networking/network.h"
#include "libhypnos/cvariant.h"

/*!
\brief Checks if the Char is allowed to move at all (not frozen, overloaded...)
\param pc Character to acknowledge the walk
\param sequence Walk sequence number
\retval true The step is acknowledged and can be done
\retval false The step isn't acknowledged so can't be done
\todo Move walksequence[] array into cClient
\todo Split in pc- and npc-dependent stuff and common stuff, and move into
	cChar, cPC and cNPC classes
*/
bool WalkHandleAllowance(pChar pc, int sequence)
{
	assert(pc);

	if (pc->isStabled() || pc->mounted) return false; // shouldnt be called for stabled pets, just to be on the safe side

	pClient client = pc->getClient();
	if(client && walksequence[s]+1 != sequence && sequence != 256 )
	{
		nPackets::Sent::MoveReject(pc, sequence);
		pc->getClient()->sendPacket(&pk);
		walksequence[s]=INVALID;
		return false;
	}

	if (!pc->isDead() && !pc->npc && !(pc->IsGMorCounselor()))
	{
		if (! pc->getBody()->overloadedWalking() || (pc->stm<1)) // this has to executes, even if s==-1, thus that many !=-1's
		{
			if (s!=INVALID)
			{
				pc->sysmsg("You are too fatigued to move.");
			  	walksequence[s]=INVALID;
			  	pc->teleport( teleNone );
				nPackets::Sent::MoveReject(pc, sequence);
        	                pc->getClient()->sendPacket(&pk);
	                       	walksequence[s]=INVALID;
			  	return false;
			}
		}
	}

	if(pc->isFrozen())
	{
		pc->teleport();
		if (client)
		{
			if (pc->isCasting())
				client->sysmessage("You cannot move while casting.");
			else
				client->sysmessage("You are frozen and cannot move.");
			nPackets::Sent::MoveReject(pc, sequence);
                        pc->getClient()->sendPacket(&pk);
                       	walksequence[s]=INVALID;
		}
		return false;
	}
	return true;
}

/*!
\brief Handles steps under stealth/hiding skills effects
\param pc Character which is moving
\param dir Direction in which che character is moving
*/
void WalkingHandleHiding (pChar pc, uint8_t dir)
{
	if(!pc) return false;

	if (dir&0x80)
		//AntiChrist - we have to unhide always if running
                if (!pc->IsHiddenBySpell())
			pc->unHide();
	else {
		if(pc->stealth>INVALID)
		{ //AntiChrist - Stealth
			pc->stealth++;
			if( pc->stealth >= ((nSettings::Skills::getMaxStealthSteps()*pc->skill[skStealth])/1000.0) )
				pc->unHide();
		}
		else
		{
			if (pc->IsHidden() && !pc->IsHiddenBySpell()) //Luxor's invisibility spell
				pc->unHide();
		}
	}

	return true;

}
///////////////
// Name:	WalkHandleRunning
// history:	cut from walking() by Duke, 27.10.2000
// Purpose:	handles running, stamina
//
bool WalkHandleRunning(pChar pc, int dir)
{
	if(!pc) return false;

	if (dir&0x80)
	{ //AntiChrist -- if running
                pc->setRunning();
		//AntiChrist - we have to unhide always if running
		if( pc->IsHidden() && !pc->IsHiddenBySpell() )
			pc->unHide();
		//Running stamina - AntiChrist
		pc->updateRegenTimer( STAT_STAMINA );  //Don't regenerate stamina while running
		pc->running++;

		// Horse Stamina loss bug Fixed by blackwind.
		// Without loss of stamina players will be able to runaway for ever..
		if (!pc->dead)
		{
			if(
				( (!pc->isMounting() && pc->running>(SrvParms->runningstaminasteps)*2) ) ||
				( pc->isMounting() && pc->running>(SrvParms->runningstaminasteps*2)*2 )
				)
			{ //The first *2 it's because i noticed that a step(animation) correspond to 2 walking calls
				pc->running=0;
				--pc->stm;
				pc->updateStamina();
			}
		}
	}
	else
	{
		pc->running=0;
		if( pc->stealth > INVALID )
		{ //AntiChrist - Stealth
			if( 	/*pc->stealth >= 0 &&*/		// give them at least one step, regardless of skill (Duke)
				(uint32_t)pc->stealth > ((nSettings::Skills::getMaxStealthSteps()*pc->skill[skStealth])/1000)
				)
			{
				pc->unHide();
			}
			++pc->stealth;
		}
		else
		{
			if( pc->IsHidden() && !pc->IsHiddenBySpell() )
				pc->unHide();
		}
	}
	return true;

}



///////////////
// Name:	WalkHandleBlocking
// history:	cut from walking() by Duke, 27.10.2000
// Purpose:	Handles a 'real move' if the Char is not only changing direction
//

bool WalkHandleBlocking(pChar pc, int sequence, int dir, int oldx, int oldy)
{
	if(!pc) return false;

	if (pc->npc)
		pc->setNpcMoveTime(); //reset move timer

	sLocation pcpos= pc->getPosition();
	pcpos.move(dir&0x07, 1); // This calculate the new position
	
	int8_t z;

	if ( pc->npc )
		z = isWalkable( pc->getPosition(), WALKFLAG_MAP + WALKFLAG_STATIC + WALKFLAG_DYNAMIC, pc );
	else
		z = getHeight( pc->getPosition() );

	//WalkEvaluateBlockers(pc, &z, &dispz, blockers);

	//!\todo Should this actually be used only for npcs? Sure the owner and banned stuff, but the rest?
	if (pc->npc==0) // this is also called for npcs .. LB ?????? Sparhawk Not if you're excluding npc's
	{
		pMulti pi_multi = cMulti::getAt( pc->getPosition() );
		
		if ( ! pi_multi )
			pc->setMulti(NULL);
		else {
			pBoat pb = dynamic_cast<pBoat>(pi_multi);
			pHouse ph = dynamic_cast<pHouse>(pi_multi);
			
			if ( pb )
			{ //xan : probably the plr has entered the boat walking!
				//!\todo Change this when new owner-system is up and running
				NxwCharWrapper pets;
				pets.fillOwnedNpcs( pc, false, true );
				for( pets.rewind(); !pets.isEmpty(); pets++ )
				{

					pChar pc_b=pets.getChar();
					if (! pc_b ) return;
					
					pc_b->MoveTo( boat->getPosition() + sLocation(1,1,2) );
					pc_b->setMulti(boat);
					pc_b->teleport();
				}
			} else if ( ph ) {
				if ( ph->isBanned(pc) )
				{
					client->sysmesage("You are banned from that location.");
					sLocation newpos = pi_multi->getArea().br + sLocation(1, 1, pc->getPosition().z);
					pc->setPosition(newpos);
					pc->teleport();
					return false;
				}
				// house refreshment code moved to dooruse()
			}
		}
	} // end of is player

	if ( z == illegal_z )
	{
		pc->setPosition( sLocation(oldx, oldy) );
		pClient client = pc->getClient();
		if ( ! client )
		{
			pc->blocked = 1;
			return false
		}
			
		nPackets::Sent::MoveReject(pc, sequence);
		pc->getClient()->sendPacket(&pk);
		walksequence[s]=INVALID;
		return false;
	}

	sPoint now2 = pc->getPosition();

	pc->setPositionX(oldx);
	pc->setPositionY(oldy);
	pc->MoveTo( nowx2, nowy2, z );
	return true;
}


void WalkingHandleRainSnow(pChar pc)
{
	if (!pc) return;

	pClient client = pc->getSocket();

	int i;
	int wtype = region[pc->region].wtype;

  /********* LB's no rain & snow in buildings stuff ***********/
	if (!pc->npc && pc->IsOnline() && wtype!=0 ) // check for being in buildings (for weather) only for PC's, check only neccasairy if it rains or snows ...
	{
		int j=indungeon(pc); // dung-check
		i=staticTop( pc->getPosition() ); // static check

	// dynamics-check
		int x = dynamicElevation( pc->getPosition() );
		
		if ( dynamic_cast<pHouse>( cMulti::getAt(pc->getPosition()) ) )
			x = invalid_z;
		
		if (x==1 || x==0) x=-127; // 1 seems to be the multi-borders
	// bugfix LB

		bool old_weather=clientInfo[s]->noweather;
		if (j || i || x!=-127 )
			clientInfo[s]->noweather=true;
		else
			clientInfo[s]->noweather=false; // no rain & snow in static buildings+dungeons;
		if( old_weather != clientInfo[s]->noweather )
			weather(s, 0); // iff outside-inside changes resend weather ...
	// needs to be de-rem'd if weather is available again
  }
}


/*void WalkingHandleGlowingItems(pChar pc)
{
	if ( ! pc ) return;

	int i;
	if (pc->IsOnline())
	{
		int serial,serhash,ci;
		serial=pc->getSerial();
		serhash=serial%HASHMAX;
		for (ci=0;ci<glowsp[serhash].max;ci++)
		{
			i=glowsp[serhash].pointer[ci];
			if (i!=INVALID)
			{
				if (items[i].free==0)
				{
					pc->glowHalo(&items[i]);
				}
			}
		}
	}

}*/


void walking(pChar pc, int dir, int sequence)
{
	int newx, newy;
	if ( ! pc ) return;

	pClient client = pc->getSocket();

	if (!WalkHandleAllowance(pc,sequence))		// check sequence, frozen, weight etc.
		return;

	WalkHandleRunning(pc,dir);

	int oldx= pc->getPosition().x;
	int oldy= pc->getPosition().y;

	if ((dir&0x0F)==pc->dir )
		if( !WalkHandleBlocking(pc,sequence,dir, oldx, oldy) )
			return;

	pFunctionHandle evt = pc->getEvent(evtChrOnWalk);
	if( evt ) {
		cVariantVector params = cVariantVector(3);
		params[0] = pc->getSerial(); params[1] = dir; params[2] = sequence;
		evt->setParams(params);
		evt->execute();
		if( evt->isBypassed() )
			return;
	}

	WalkingHandleHiding(pc,dir);

	if (s!=INVALID)
	{
        	//!\todo verify notoriety... what does it do????????????
		uint8_t notoriety=0x41;
		//if (pc->hidden==1) walkok.notoriety=0x00;
		//if (pc->isHidden()) walkok.notoriety=0x00;

	        nPackets::Sent::MoveAcknowdledge pk(sequence, notoriety);
                getClient()->sendPacket(&pk);

		walksequence[s]=sequence;
		walksequence[s]%=255;
	}


	newx= pc->getPosition().x;
	newy= pc->getPosition().y;

	sendToPlayers( pc, dir );

	if (dir>INVALID && (dir&0x0F)<8)
		pc->dir=(dir&0x0F);
	else
		outPlain("dir-screwed : %i\n",dir);


	if( oldx!=newx || oldy!=newy )
	{
		//Luxor: moved WalkHandleItemsAtNewPos before socket check.
		handleItemsAtNewPos( pc, oldx, oldy, newx, newy );
		if (s!=INVALID)
		{
			handleCharsAtNewPos( pc );
			pc->LastMoveTime = getclock();
		}

		if( !pc->npc || pc->questType || pc->tamed )
			objTeleporters(pc);
		teleporters(pc);

		if( s!=INVALID )
			WalkingHandleRainSnow(pc); // while rain and snow are disabled its a waste of CPU cycles

		magic::checkGateCollision( pc ); // Luxor: gates :)
		checkregion(pc);
	}


	if(pc->getCombatSkill() ==skArchery)  // -Frazurbluu- add in changes for archery skill, and dexterity
    {                                        //  possibly weapon speed?? maybe not, cause crossbows notta running shooting
		if ( pc->targserial!= INVALID)
        {
            if( pc->timeout>= getclock())
               pc->timeout= getclock() + (3*CLOCKS_PER_SEC);

		}
    }

}

//</XAN>
int npcSelectDir(pChar pc_i, int j)
{
	if(!pc_i) return INVALID;

	if (pc_i->blocked)
	{
		short x=0;
		if (j/2.0!=j/2)
			x=1;
		if (pc_i->blocked<=2)
			j =pc_i->dir2;	/* =(j-2-x)%8; //works better  ????*/
		else
		{
			if (rand()%2) j=pc_i->dir2=(j-2-x)%8;
			else j=pc_i->dir2=(j+2+x)%8;
		}
	}
	if (j<0)
		j=rand()%8;
	return j;
}

int npcSelectDirWarOld(pChar pc_i, int j)
{
	if(!pc_i) return INVALID;

	if (pc_i->blocked)
	{
		short x=0;
		if (j/2.0!=j/2)
			x=1;
		if (pc_i->blocked<=2)
			j =pc_i->dir2 =(j-2-x)%8; //works better  ????
		else
		{
			if (rand()%2) j=pc_i->dir2=(j-2-x)%8;
			else j=pc_i->dir2=(j+2+x)%8;
		}
	}
	if (j<0)
		j=rand()%8;
	return j;
}

int checkBounds(pChar pc, int newX, int newY, int type)
{
	if(!pc) return 0;

	int move=0;
	switch (type)
	{
	case 0: move=1;break;
	case 1: move=checkBoundingBox(newX, newY, pc->fx1, pc->fy1, pc->fz1, pc->fx2, pc->fy2);break;
	case 2: move=checkBoundingCircle(newX, newY, pc->fx1, pc->fy1, pc->fz1, pc->fx2);break;
	default: move=0;	// invalid type given
	}
	return move;
}

void npcwalk( pChar pc_i, uint8_t newDirection, int type)   //type is npcwalk mode (0 for normal, 1 for box, 2 for circle) // Sparhawk should be changed to npcwander
{
	if( !pc_i || pc_i->isFrozen() ) return;

	sLocation charpos= pc_i->getPosition();

	/////////// LB's flying creatures stuff, flying animation if they stand still ///////

	pCreatureInfo creature = creatures.getCreature( pc_i->getId() );
	if( creature!=NULL )
		if( creature->canFly() && ( pc_i->fly_steps>0 ) )
			if ( chance( 20 ) )
				pc_i->playAction(0x13); // flying animation

	/////////////////////////////////////////////////////////////////////////////////////

	bool valid, move;
	if ( pc_i->dir != newDirection )  // If we're moving, not changing direction
	{
		walking(  pc_i , newDirection, 256);
		return;
	}	
	
	
	sLocation newpos = charpos;
	newpos.move(pc_i->getDirection(), 1);
	
	valid = ( isWalkable( newpos, WALKFLAG_ALL, pc_i ) != illegal_z );
	
	if ( valid )
	{
		move = checkBounds( pc_i, newX, newY, type );
		if ( move )
		{
			walking(  pc_i , newDirection, 256 );
		}
		else 	// We're out of the boundary, so we need to get back
		{
			uint8_t direction = pc_i->getDirFromXY(pc_i->fx1, pc_i->fy1);
			newpos.move(direction, 1);
			valid = ( isWalkable( newpos, WALKFLAG_ALL, pc_i ) != illegal_z );
			
			if ( !valid ) // try to bounce around obstacle
			{
				direction = pc_i->dir;
				
				newpos.move( pc_i->getDirection(), 1);
				valid = ( isWalkable( newpos, WALKFLAG_ALL, pc_i ) != illegal_z );
				
				bool clockwise = chance( 50 );
				while( direction != pc_i->getDirection() && !valid )
				{
					if ( clockwise )
						direction = getRightDir( direction );
					else
						direction = getLeftDir( direction );
					
					newpos.move( pc_i->getDirection(), 1 );
					valid = ( isWalkable( newpos, WALKFLAG_ALL, pc_i ) != illegal_z );
				}
			}
			if ( valid )
			{
				move = true;
				walking(  pc_i , direction, 256 );
			}
		}
	}
	else	// avoid obstacle
	{
		int direction;
		bool clockwise = chance( 50 );
		if ( clockwise )
			direction = getRightDir( pc_i->dir );
		else
			direction = getLeftDir( pc_i->dir );
		while( !valid && direction != pc_i->dir )
		{
			newpos.move(direction, 1);
			valid = ( isWalkable( newpos, WALKFLAG_ALL, pc_i ) != illegal_z );
			
			if ( clockwise )
				direction = getRightDir( direction );
			else
				direction = getLeftDir( direction );
		}
		if ( valid )
		{
			move = true;
			walking(  pc_i , direction, 256 );
		}
	}

	pFunctionHandle evt = pc_i->getEvent(evtChrOnBlock);
	if ( (!valid || !move) && evt )
	{
		cVariantVector params = cVariantVector(4);
		params[0] = pc_i->getSerial(); params[1] = newX; params[2] = newY; params[3] = charpos.z;
		evt->setParams(params);
		evt->execute();
		if( evt->isBypassed() )
			return;
	}
}

/*!
\author Luxor
*/
void handleCharsAtNewPos( pChar pc )
{
	if ( ! pc ) return;

	NxwCharWrapper sc;
	pChar pc_curr;
	sc.fillCharsAtXY( pc->getPosition(), !pc->IsGM(), false );

	for( sc.rewind(); !sc.isEmpty(); sc++ ) {
		pc_curr = sc.getChar();
		if ( !pc_curr )
			continue;
		if ( pc->IsGMorCounselor() || pc_curr == pc )
			continue;
		if ( pc_curr->dead || pc_curr->IsInvul() )
			continue;
		if ( pc_curr->IsHidden() )
			pc->sysmsg("You shoved something invisible aside.");
		else
			pc->sysmsg("Being perfectly rested, you shove %s out of the way.", pc_curr->getCurrentName().c_str() );

		pc->stm = qmax( pc->stm-ServerScp::g_nShoveStmDamage, 0 );
		pc->updateStamina();
		if ( pc->IsHidden() && !pc->IsHiddenBySpell() )
			pc->unHide(); //xan, shoving in stealth will unhide
	}
}

///////////////
// Name:	WalkHandleItemsAtNewPos
// history:	cut from walking() by Duke, 27.10.2000
// Purpose:	sends the newly visible items to the screen and checks for item effects
//
bool handleItemsAtNewPos(pChar pc, int oldx, int oldy, int newx, int newy)
{
	if ( ! pc ) return false;

	pClient ps=pc->getClient();
	if ( ps == NULL ) //Luxor
		return false;

	sLocation pcpos=pc->getPosition();

	NxwItemWrapper si;
	si.fillItemsNearXYZ( pcpos, VISRANGE + 1, false );
	for( si.rewind(); !si.isEmpty(); si++ ) {

		pItem pi=si.getItem();
		if(!pi)
			continue;
			if( pi->getId()>=0x407C && pi->getId()<=0x407E )
			{
				pc->distFrom(pi);
				int di = pc->distFrom(pi);

				if (di<=BUILDRANGE && di>=VISRANGE)
				{
					senditem(ps->toInt(), pi);
				}

			}
			else if ( pc->seeForFirstTime( *pi ) ) // Luxor
				senditem( ps->toInt(), pi );
			else
				pc->seeForLastTime( *pi );
	}
	return true;
}


/*!
\author Luxor
*/
void sendToPlayers( pChar pc, int8_t dir )
{
	if ( ! pc ) return;

	pClient ps = NULL;
	pClient cli = pc->getClient();
	NxwCharWrapper sc;
	sc.fillCharsNearXYZ( pc->getPosition(), VISRANGE + 1, !pc->IsGM() );

	for( sc.rewind(); !sc.isEmpty(); sc++ ) {
		pChar pc_curr = sc.getChar();
		if( !pc_curr )
			continue;

		if ( pc->seeForLastTime( *pc_curr ) ) {
			if ( cli != NULL )
				cli->sendRemoveObject( pObject(pc_curr) );
		}
		if ( pc->seeForFirstTime( *pc_curr ) )
		{
			if ( cli )
			{
				nPackets::Sent::DrawObject pk(cli, pc_curr, true);
				cli->sendPacket(&pk);
			}
		}

		ps = pc_curr->getClient();
		if ( ps == NULL )
			continue;

		// pc has just walked out pc_curr's vis circle
		if ( pc_curr->seeForLastTime( *pc ) )
		{
			nPackets::Sent::DeleteObj pk(pc);
			ps->sendPacket(&pk);
			continue;
		}

		// It's seen for the first time, send a draw packet
		if ( pc_curr->seeForFirstTime( *pc ) )
		{
			nPackets::Sent::DrawObject pk(ps, pc, true);
			ps->sendPacket(&pk);
			continue;
		}

                if ( !pc_curr->canSee( *pc ) )
			continue;

		uint8_t flag, hi_color;

		// If it's an npc, and it's fighting or following something let's show it running
		if ( pc->npc && ( pc->war || pc->ftargserial != INVALID ) )
			dir |= 0x80;

		if ( pc->npc && !(dir&0x80) ) { // If npc and it isn't already running

			pCreatureInfo creature = creatures.getCreature( pc->getId() );
			if( creature!=NULL )
				if( creature->canFly() )
					if( pc->fly_steps > 0 )
					{
						pc->fly_steps--;
						dir |= 0x80; // run mode = fly for that ones that can fly
					}
					else
					{
						if ( (rand()%18)==0 )
							pc->fly_steps = ( rand()%27 ) + 2;
					}
		}

		if ( pc->war )
			flag = 0x40;
		else
			flag = 0x00;
		if ( pc->IsHidden() )
			flag |= 0x80;
		if ( pc->dead && !pc->war )
			flag |= 0x80; // Ripper
		if ( pc->poisoned )
			flag |= 0x04; // AntiChrist -- thnx to SpaceDog

		int32_t guild = Guilds->Compare( pc, pc_curr );
		if ( guild == 1 )		// Same guild (Green)
			hi_color = 2;
		else if ( guild == 2 )		// Enemy guild.. set to orange
			hi_color = 5;
		else if ( pc->IsGrey() )
			hi_color = 3;           // grey
		else if ( pc->IsMurderer() )
			hi_color = 6;		// If a bad, show as red.
		else if ( pc->IsInnocent() )
			hi_color = 1;		// If a good, show as blue.
		else if ( pc->flag == 0x08 )
			hi_color = 2;		// green (guilds)
		else if ( pc->flag == 0x10 )
			hi_color = 5;		// orange (guilds)
		else
			hi_color = 3;		// grey

                nPackets::Sent::UpdatePlayer pk(pc, dir, flag, hi_color );
                ps_i->sendPacket(&pk);
	}
}
