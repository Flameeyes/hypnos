/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "misc.h"

/*!
\brief makes an npc attacking someone
\author Luxor
\param pc the npc attacker
\param pc_target the victim
*/
void npcattacktarget(pChar pc, pChar pc_target)
{
	if (
		! pc || ! pc_target ||
		! pc->npc ||
		pc->isDead() || pc_target->isDead() ||
		pc == pc_target ||
		!pc->losFrom(pc_target)
	) return;
	
	pFunctionHandle evt;
	evt = pc->getEvent(cChar::evtChrOnBeginAttack);
	if (evt) {
		tVariantVector params = tVariantVector(2);
		params[0] = pc->getSerial(); params[1] = pc_target->getSerial();
		evt->setParams(params);
		evt->execute();
		if( evt->isBypassed() )
			return;
	}

	evt = pc->getEvent(cChar::evtChrOnBeginDefence);
	if (evt) {
		tVariantVector params = tVariantVector(2);
		params[0] = pc->getSerial(); params[1] = pc_target->getSerial();
		evt->setParams(params);
		evt->execute();
		if( evt->isBypassed() )
			return;
	}

	pc->playMonsterSound(SND_STARTATTACK);

	pc->targserial = pc_target->getSerial();
	pc->attackerserial = pc_target->getSerial();
	pc->SetAttackFirst();

	if ( !pc->war )
		pc->toggleCombat();
	pc->setNpcMoveTime();

	pChar pc_target_targ = cSerializable::findCharBySerial(pc_target->targserial);
	if ( !pc_target_targ || pc_target_targ->dead || pc_target->distFrom(pc_target_targ) > 15 ) {
		if (!pc_target->npc && pc_target->war) {
			pc_target->targserial = pc->getSerial();
			pc_target->attackerserial = pc->getSerial();
		} else if (pc_target->npc) {
			if ( !pc_target->war )
				pc_target->toggleCombat();

			pc_target->targserial = pc->getSerial();
			pc_target->attackerserial = pc->getSerial();
			pc_target->setNpcMoveTime();
		}
	}

	pc->emoteall( "You see %s attacking %s!", 1, pc->getCurrentName().c_str(), pc_target->getCurrentName().c_str() );

}

void usepotion(pChar pc, pItem pi)
{
	int x;

	if ( ! pc ) return;

	pPC tmp;
	pClient client = (tmp = dynamic_cast<pPC>(pc))? tmp->getClient() : NULL;

	switch(pi->morey)
	{
	case 1: // Agility Potion
		staticFX(pc, 0x373A, 0, 15);
		switch(pi->morez)
		{
		case 1:
			tempfx::add(pc, pc, tempfx::spellAgility, 5+RandomNum(1,10), 0, 0, 120);
			if(client) client->sysmessage("You feel more agile!");
			break;
		case 2:
			tempfx::add(pc, pc, tempfx::spellAgility, 10+RandomNum(1,20), 0, 0, 120);
			if(client) client->sysmessage("You feel much more agile!");
			break;
		default:
			ErrOut("Switch fallout. hypnos.cpp, usepotion()\n");
			return;
		}
		pc->playSFX(0x01E7);
		if (s!=INVALID)
			pc->updateStamina();
		break;

	case 2: // Cure Potion
		if ( pc->getBody()->getPoisoned() == poisonNone )
			if(client) client->sysmessage("The potion had no effect.");
		else
		{
			switch(pi->morez)
			{
			case 1:
				x=RandomNum(1,100);
				if (pc->getBody()->getPoisoned() == poisonWeak && x<81) pc->getBody()->setPoisoned(poisonNone);
				if (pc->getBody()->getPoisoned() == poisonNormal && x<41) pc->getBody()->setPoisoned(poisonNone);
				if (pc->getBody()->getPoisoned() == poisonGreater && x<21) pc->getBody()->setPoisoned(poisonNone);
				if (pc->getBody()->getPoisoned() == poisonDeadly && x< 6) pc->getBody()->setPoisoned(poisonNone);
				break;
			case 2:
				x=RandomNum(1,100);
				if (pc->getBody()->getPoisoned() == poisonWeak) pc->getBody()->setPoisoned(poisonNone);
				if (pc->getBody()->getPoisoned() == poisonNormal && x<81) pc->getBody()->setPoisoned(poisonNone);
				if (pc->getBody()->getPoisoned() == poisonGreater && x<41) pc->getBody()->setPoisoned(poisonNone);
				if (pc->getBody()->getPoisoned() == poisonDeadly && x<21) pc->getBody()->setPoisoned(poisonNone);
				break;
			case 3:
				x=RandomNum(1,100);
				if (pc->getBody()->getPoisoned() == poisonWeak) pc->getBody()->setPoisoned(poisonNone);
				if (pc->getBody()->getPoisoned() == poisonNormal) pc->getBody()->setPoisoned(poisonNone);
				if (pc->getBody()->getPoisoned() == poisonGreater && x<81) pc->getBody()->setPoisoned(poisonNone);
				if (pc->getBody()->getPoisoned() == poisonDeadly && x<61) pc->getBody()->setPoisoned(poisonNone);
				break;
			default:
				ErrOut("Switch fallout. hypnos.cpp, usepotion()\n"); //Morrolan
				return;
			}
			if (pc->poisoned)
				if(client) client->sysmessage("The potion was not able to cure this poison.");
			else
			{
				staticFX(pc, 0x373A, 0, 15);
				pc->playSFX(0x01E0); //cure sound - SpaceDog
				if(client) client->sysmessage("The poison was cured.");
			}
		}
		client->sendchar(pc);
		break;

	case 3: {// Explosion Potion
		if (region[pc->region].priv&0x01) // Ripper 11-14-99
		{
			if(client) client->sysmessage(" You cant use that in town!");
			return;
		}
		if(client) client->sysmessage("Now would be a good time to throw it!");
		tempfx::add(pc, pc, tempfx::EXPLOTIONMSG, 0, 1, 3);
		tempfx::add(pc, pc, tempfx::EXPLOTIONMSG, 0, 2, 2);
		tempfx::add(pc, pc, tempfx::EXPLOTIONMSG, 0, 3, 1);
		tempfx::add(pc, pi, tempfx::EXPLOTIONEXP, 0, 4, 0);

		pTarget targ= clientInfo[s]->newTarget( new cTarget() );
		targ->code_callback=target_expPotion;
		targ->buffer[0]= pi->getSerial();
		targ->send(client);
		client->sysmessage( "*throw*" );
		return;
	}
	case 4: // Heal Potion
		switch(pi->morez)
		{
		case 1:
			pc->hp=qmin(pc->hp+5+RandomNum(1,5)+pc->skill[17]/100,pc->getStrength());
			if(client) client->sysmessage("You feel better!");
			break;
		case 2:
			pc->hp=qmin(pc->hp+15+RandomNum(1,10)+pc->skill[17]/50,pc->getStrength());
			if(client) client->sysmessage("You feel more healty!");
			break;
		case 3:
			pc->hp=qmin(pc->hp+20+RandomNum(1,20)+pc->skill[17]/40, pc->getStrength());
			if(client) client->sysmessage("You feel much more healty!");
			break;

		default:
			ErrOut("Switch fallout. hypnos.cpp, usepotion()\n"); //Morrolan
			return;
		}

		if (client)
			pc->updateHp();

		staticFX(pc, 0x376A, 9, 6); // Sparkle effect
		pc->playSFX(0x01F2); //Healing Sound - SpaceDog
		break;

	case 5: // Night Sight Potion
		staticFX(pc, 0x376A, 9, 6);
		tempfx::add(pc, pc, tempfx::SPELL_LIGHT, 0, 0, 0,(720*secondsperuominute*SECS));
		pc->playSFX(0x01E3);
		break;

	case 6: // Poison Potion
		if(pc->poisoned < (PoisonType)pi->morez)
			pc->poisoned=(PoisonType)pi->morez;
		if(pi->morez>4)
			pi->morez=4;
		pc->poisonwearofftime=getclock()+(SECS*SrvParms->poisontimer); // lb, poison wear off timer setting
		client->sendchar(pc);
		pc->playSFX(0x0246); //poison sound - SpaceDog
		if(client) client->sysmessage("You poisoned yourself! *sigh*"); //message -SpaceDog
		break;

	case 7: // Refresh Potion
		switch(pi->morez)
		{
			case 1:
				pc->stm=qmin(pc->stm+20+RandomNum(1,10), pc->dx);
				if(client) client->sysmessage("You feel more energetic!");
				break;

			case 2:
				pc->stm=qmin(pc->stm+40+RandomNum(1,30), pc->dx);
				if(client) client->sysmessage("You feel much more energetic!");
				break;

			default:
				ErrOut("Switch fallout. hypnos.cpp, usepotion()\n"); //Morrolan
				return;
		}
		if (s!=INVALID)
			pc->updateStamina();
		
		staticFX(pc, 0x376A, 9, 6);
		pc->playSFX(0x01F2); //Healing Sound
		break;

	case 8: // Strength Potion
		staticFX(pc, 0x373A, 0, 15);
		switch(pi->morez)
		{
		case 1:
			tempfx::add(pc, pc, tempfx::spellStrength, 5+RandomNum(1,10), 0, 0, 120);
			if(client) client->sysmessage("You feel more strong!");
			break;
		case 2:
			tempfx::add(pc, pc, tempfx::spellStrength, 10+RandomNum(1,20), 0, 0, 120);
			if(client) client->sysmessage("You feel much more strong!");
			break;
		default:
			ErrOut("Switch fallout. hypnos.cpp, usepotion()\n");
			return;
		}
		pc->playSFX(0x01EE);
		break;

	case 9: // Mana Potion
		switch(pi->morez)
		{
		case 1:
			pc->mn=qmin(pc->mn+10+pi->morex/100, (unsigned)pc->in);
			break;

		case 2:
			pc->mn=qmin(pc->mn+20+pi->morex/50, (unsigned)pc->in);
			break;

		default:
			ErrOut("Switch fallout. hypnos.cpp, usepotion()\n");
			return;
		}
		if (client)
			pc->updateMana();
		staticFX(pc, 0x376A, 9, 6); // Sparkle effect
		pc->playSFX(0x01E7); //agility sound - SpaceDog
		break;

	case 10: //LB's LSD potion, 5'th november 1999
		if (pi->getId()!=0x1841) return; // only works with an special flask
		if (client) return;
		if( clientInfo[s]->lsd )
		{
			if(client) client->sysmessage("no,no,no,cant you get enough ?");
			return;
		}
		tempfx::add(pc, pc, tempfx::LSD, 60+RandomNum(1,120), 0, 0); // trigger effect
		staticFX(pc, 0x376A, 9, 6); // Sparkle effect
		pc->playSFX(0x00F8, true); // lsd sound :)
		break;

	default:
		ErrOut("Switch fallout. hypnos.cpp, usepotion()\n"); //Morrolan
		return;
	}

	pc->playSFX(0x0030);
	if (pc->HasHumanBody() && !pc->isMounting())
		pc->playAction(0x22);

	pi->ReduceAmount( 1 );

	if (pi->morey!=3)
	{
		int lsd=pi->morey; // save morey before overwritten

		pi = new cItem(cItem::nextSerial());

		pi->setId( 0x0F0E );

		if (lsd==10) // empty Lsd potions
		{
			pi->setId( 0x183d );
		}

		pi->pileable=1;

		pItem pack=pc->getBackpack();
		if (pack) {
			pack->AddItem( pi );
		}
		else {
			pi->MoveTo( pc->getPosition() );
			pi->setDecay();
			pi->Refresh();
		}
	}
}

/*!
\todo backport into cChar
*/
void callguards( pChar caller )
{
	if ( !caller )
		return;

	if( !(region[caller->region].priv&0x01 ) || !SrvParms->guardsactive || !TIMEOUT( caller->antiguardstimer ) || caller->dead )
		return;

	caller->antiguardstimer=getclock()+(SECS*10);

	/*
	Sparhawk:
	1. when instant guard is set and offender nearby caller spawn guard near caller and leave attacking to checkAI
	2. when instant guard is not set and offender nearby caller walk toward caller and leave attacking to checkAI
	*/
	bool offenders = false;
	vector < pChar > guards;

	NxwCharWrapper sc;
	sc.fillCharsNearXYZ( caller->getPosition(), VISRANGE, true, false  );
	for( sc.rewind(); !sc.isEmpty(); sc++ ) {

		pChar character=sc.getChar();
		if(	! character ||
			caller == character ||
			caller->distFrom(character) > 15 ||
			character->isDead() ||
			character->isHidden() )
				continue;
			
		if ((!character->IsInnocent() || character->npcaitype == NPCAI_EVIL) && !character->IsHidden() )
			offenders = true;
		else
			if ((character->npcaitype == NPCAI_TELEPORTGUARD || character->npcaitype == NPCAI_GUARD) && !character->war && character->npcWander != cNPC::WANDER_FOLLOW)
				guards.push_back( character );
	}
	
	if ( ! offenders ) return;
	
	if ( guards.empty() && nSettings::Server::hasInstantGuards() )
	{
		pNPC guard = npcs::AddNPCxyz( caller->getSocket(), region[caller->region].guardnum[(rand()%10)+1], caller->getPosition());

		if ( guard )
		{
			guard->npcaitype=NPCAI_TELEPORTGUARD;
			guard->npcWander=cNPC::WANDER_FREELY_CIRCLE;
			guard->setNpcMoveTime();
			guard->summontimer = getclock() + SECS * 25 ;

			guard->playSFX( 0x01FE );
			staticFX(guard, 0x372A, 9, 6);

			guard->teleport();
			guard->talkAll("Don't fear, help is near", false );
		}
	}
	else
	{
		pChar guard;
		while( !guards.empty() )
		{
			guard = guards.back();
			guard->oldnpcWander = guard->npcWander;
			guard->npcWander = cNPC::WANDER_FOLLOW;
			guard->ftargserial = caller->getSerial();
			guard->antiguardstimer=getclock()+(SECS*10); // Sparhawk this should become server configurable
			guard->talkAll("Don't fear, help is on the way", false );
			//guard->antispamtimer = getclock()+SECS*5;
			guards.pop_back();
		}
	}
}

/*!
\brief Plays the proper door sfx for doors/gates/secretdoors
\param pi Door item (to call the cItem::playSFX() function of)
\param id Base ID of the door
\param close If true, the door will be closed, else opened
*/
static void doorsfx(pItem pi, uint16_t id, bool close)
{
	static const uint16_t OPENWOOD = 0x00EA;
	static const uint16_t OPENGATE = 0x00EB;
	static const uint16_t OPENSTEEL = 0x00EC;
	static const uint16_t OPENSECRET = 0x00ED;
	static const uint16_t CLOSEWOOD = 0x00F1;
	static const uint16_t CLOSEGATE = 0x00F2;
	static const uint16_t CLOSESTEEL = 0x00F3;
	static const uint16_t CLOSESECRET = 0x00F4;
	
	if ( close ) // Request close door sfx
	{
		// Close wooden / ratan door
		if ( between(id, 0x0695, 0x06C4) || between(id, 0x06D5, 0x06F4) )
			pi->playSFX(CLOSEWOOD);

		// Close gate
		if (	between(id, 0x0839, 0x0848) ||
			between(id, 0x084C, 0x085B) ||
			between(id, 0x0866, 0x0875) )
				pi->playSFX(CLOSEGATE);

		// Close metal
		if ( between(id, 0x0675, 0x0694) || between(id, 0x06C5, 0x06D4) )
			pi->playSFX(CLOSESTEEL);

		// Close secret
		if ( between(x, 0x0314, 0x0365) )
			pi->playSFX(CLOSESECRET);
	} else { // Request open door sfx
		// Open wooden / ratan door
		if ( between(id, 0x0695, 0x06C4) || between(id, 0x06D5, 0x06F4) )
			pi->playSFX(OPENWOOD);

		// Open gate
		if (	between(id, 0x0839, 0x0848) ||
			between(id, 0x084C, 0x085B) ||
			between(id, 0x0866, 0x0875) )
				pi->playSFX(OPENGATE);

		// Open metal
		if ( between(id, 0x0675, 0x0694) || between(id, 0x06C5, 0x06D4) )
			pi->playSFX(OPENSTEEL);

		// Open secret
		if ( between(x, 0x0314, 0x0365) )
			pi->playSFX(OPENSECRET);
	}
}


/*!
\brief Uses of a door
\param pc The Player who used the door (is NULL for automatic door close)
\param pi Door to close
\note Don't validate pc, because it can be NULL for automatic door close
*/
void dooruse(pChar pc, pItem pi)
{
	if ( ! pi )
		return;

	int i, db, x;
	bool changed=0;
	
	pClient client = pc ? pc->getClient() : NULL;

	/*if (pc && ( !pc->hasInRange(pi, 2) ) && client ) {
		client->sysmessage("You cannot reach the handle from here");
		return;
	}*/

	x=pi->getId();
	for (i=0;i<DOORTYPES;i++)
	{
		db=doorbase[i];

		if (x==(db+0))
		{
			pi->setId( pi->getId()+1 );
			pi->setPosition( sLocation(pi->getPosition().x-1, pi->getPosition().y+1, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, false);
			tempfx::add(pc, pi, tempfx::AUTODOOR, 0, 0, 0);
			pi->dooropen=1;
		} else if (x==(db+1))
		{
			pi->setId( pi->getId()-1 );
			pi->setPosition( sLocation(pi->getPosition().x+1, pi->getPosition().y-1, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, true);
			pi->dooropen=0;
		} else if (x==(db+2))
		{
			pi->setId( pi->getId()+1 );
			pi->setPosition( sLocation(pi->getPosition().x+1, pi->getPosition().y+1, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, false);
			tempfx::add(pc , pi, tempfx::AUTODOOR, 0, 0, 0);
			pi->dooropen=1;
		} else if (x==(db+3))
		{
			pi->setId( pi->getId()-1 );
			pi->setPosition( sLocation(pi->getPosition().x-1, pi->getPosition().y-1, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, true);
			pi->dooropen=0;
		} else if (x==(db+4))
		{
			pi->setId( pi->getId()+1 );
			pi->setPosition( sLocation(pi->getPosition().x-1, pi->getPosition().y, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, false);
			tempfx::add(pc, pi, tempfx::AUTODOOR, 0, 0, 0);
			pi->dooropen=1;
		} else if (x==(db+5))
		{
			pi->setId( pi->getId()-1 );
			pi->setPosition( sLocation(pi->getPosition().x+1, pi->getPosition().y, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, true);
			pi->dooropen=0;
		} else if (x==(db+6))
		{
			pi->setId( pi->getId()+1 );
			pi->setPosition( sLocation(pi->getPosition().x+1, pi->getPosition().y-1, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, false);
			tempfx::add(pc, pi, tempfx::AUTODOOR, 0, 0, 0);
			pi->dooropen=1;
		} else if (x==(db+7))
		{
			pi->setId( pi->getId()-1 );
			pi->setPosition( sLocation(pi->getPosition().x-1, pi->getPosition().y+1, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, true);
			pi->dooropen=0;
		} else if (x==(db+8))
		{
			pi->setId( pi->getId()+1 );
			pi->setPosition( sLocation(pi->getPosition().x+1, pi->getPosition().y+1, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, false);
			tempfx::add(pc, pi, tempfx::AUTODOOR, 0, 0, 0);
			pi->dooropen=1;
		} else if (x==(db+9))
		{
			pi->setId( pi->getId()-1 );
			pi->setPosition( sLocation(pi->getPosition().x-1, pi->getPosition().y-1, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, true);
			pi->dooropen=0;
		} else if (x==(db+10))
		{
			pi->setId( pi->getId()+1 );
			pi->setPosition( sLocation(pi->getPosition().x+1, pi->getPosition().y-1, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, false);
			tempfx::add(pc, pi, tempfx::AUTODOOR, 0, 0, 0);
			pi->dooropen=1;
		} else if (x==(db+11))
		{
			pi->setId( pi->getId()-1 );
			pi->setPosition( sLocation(pi->getPosition().x-1, pi->getPosition().y+1, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, true);
			pi->dooropen=0;
		}
		else if (x==(db+12))
		{
			pi->setId( pi->getId()+1 );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, false);
			tempfx::add(pc, pi, tempfx::AUTODOOR, 0, 0, 0);
			pi->dooropen=1;
		} else if (x==(db+13))
		{
			pi->setId( pi->getId()-1 );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, true);
			pi->dooropen=0;
		} else if (x==(db+14))
		{
			pi->setId( pi->getId()+1 );
			pi->setPosition( sLocation(pi->getPosition().x, pi->getPosition().y-1, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, false);
			tempfx::add(pc, pi, tempfx::AUTODOOR, 0, 0, 0);
			pi->dooropen=1;
		} else if (x==(db+15))
		{
			pi->setId( pi->getId()-1 );
			pi->setPosition( sLocation(pi->getPosition().x, pi->getPosition().y+1, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, true);
			pi->dooropen=0;
		}
	}
	
	if ( !changed && pc )
	{
		pc->getClient()->sysmessage("This doesnt seem to be a valid door type. Contact a GM.");
		return;
	}
	
	if ( ! changed || ! pc )
		return;

	pc->objectdelay=getclock()+ (server_data.objectdelay/4)*SECS;
	// house refreshment when a house owner or friend of a houe opens the house door

	int j, houseowner_serial,ds;
	pHouse pi_house = reinterpret_cast<pHouse>(cMulti::getAt(pi->getPosition()));
	
	if ( ! pi_house )
		return;
	
	// Coowner also tests if owner
	if ( ! pi_house->canPerformCommand(pc) )
		return;
	
	if (SrvParms->housedecay_secs!=0)
		ds=((pi2->time_unused)*100)/(SrvParms->housedecay_secs);
	else ds=INVALID;

	if (ds < 50) // sysmessage iff decay status >=50%
		return;
	
	if ( client )
	{
		if (pi_house->isFriend(pc))
			client->sysmessage("You refreshed your friend's house");
		else
			client->sysmessage("You refreshed the house");
	}

	pi_house->time_unused=0;
	pi_house->timeused_last=getclock();
}

void endmessage(int x) // If shutdown is initialized
{
	uint32_t igetclock = getclock();

	if (endtime<igetclock)
		endtime=igetclock;

	sysbroadcast("server going down in %i minutes.\n",
		((endtime-igetclock)/SECS)/60);
	InfoOut("server going down in %i minutes.\n",
		((endtime-igetclock)/SECS)/60);
}

int checkBoundingBox(int xPos, int yPos, int fx1, int fy1, int fz1, int fx2, int fy2)
{
	if (xPos>=((fx1<fx2)?fx1:fx2) && xPos<=((fx1<fx2)?fx2:fx1))
		if (yPos>=((fy1<fy2)?fy1:fy2) && yPos<=((fy1<fy2)?fy2:fy1))
			if (fz1==INVALID || abs(fz1-getHeight(sLocation(xPos, yPos, fz1)))<=5)
				return 1;
			return 0;
}

int checkBoundingCircle(int xPos, int yPos, int fx1, int fy1, int fz1, int radius)
{
	if ( (xPos-fx1)*(xPos-fx1) + (yPos-fy1)*(yPos-fy1) <= radius * radius)
		if (fz1==INVALID || abs(fz1-getHeight(sLocation(xPos, yPos, fz1)))<=5)
			return 1;
		return 0;
}

/*!
\brief converts x,y coords to sextant coords
\author LB
\param p Coordinates of object
\param center Central coordinates
\note thanks goes to Balxan / UOAM for the basic alogithm
	could be optimized a lot, but the freuqency of being called is probably very low

Central coords for standard map are 1323,1624 (LB's throne) for Sosaria and 5936,3112 for
T2A.
New maps can redefine these values.
*/
std::string getSextantCoords(sPoint p, sPoint center)
{
	float Tx, Ty, Dx, Dy, Wx, Wy, Mx, My, Hx, Hy;
	
	//! \todo This is not always true! Should we pass it as a parameter maybe?
	//  map dimensions
	Wx = 5120.0;
	Wy = 4096.0;

	// main calculation
	Dx = ( (p.x - center.x) * 360.0 ) / Wx;
	Dy = ( (p.y - center.y) * 360.0 ) / Wy;
	
	// now let's get minutes, hours & directions from it
	Hx = (int16_t) Dx; // get hours (cut off digits after comma, no idea if there's a cleaner/better way)

	Mx = Dx - Hx; // get minutes
	Mx *= 60;

	Hy = (int16_t) Dy;
	My = (Dy - Hy) *60;

	char *temp;
	asprintf(&temp, "%uo %u' %c  %uo %u' %c",
		abs( (int16_t)Hx ), abs( (int16_t)Mx ), Hx > 0 ? 'E' : 'W',
		abs( (int16_t)Hy ), abs( (int16_t)My ), Hy > 0 ? 'S' : 'N'
		);
	
	std::string ret(temp);
	free(temp);
	
	return ret;
}

/*!
\brief trade System
\author Magius(CHE)
\todo Try to take a look to what is this and document it
*/
int calcGoodValue(pChar npc, pItem pi, int value,int goodtype)
{
	if ( ! npc ) return 0;
	if ( ! pi ) return value;
	
	int actreg=calcRegionFromXY( npc->getPosition() );
	int regvalue=0;
	int x;
	int good=pi->good;

	if (good<=INVALID || good >255 || actreg<=INVALID || actreg>255) return value;

	if (goodtype==1) regvalue=region[actreg].goodsell[pi->good]; // Vendor SELL
	if (goodtype==0) regvalue=region[actreg].goodbuy[pi->good]; // Vendor BUY

	x=(int) (value*abs(regvalue))/1000;

	if (regvalue<0)	value-=x;
	else value+=x;

	if (value<=0) value=1; // Added by Magius(CHE) (2)

	return value;
}
