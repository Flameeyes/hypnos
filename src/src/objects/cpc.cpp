/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Implementation of cPC class
*/

#include "objects/cpc.h"
#include "objects/caccount.h"

const std::string cPC::getPopupHelp() const
{
	pAccount acc = getAccount();
	
	if ( ! acc )
		return std::string("Error, missing account");
	
	if ( acc->getPrivLevel() >= cAccount::privGM )
		return std::string("He/She is a Game Master. You can ask his help if you're stuck or have bugs or other problems");
	else if ( acc->getPrivLevel() >= cAccount::privCounselor )
		return std::string("He/She is a Counselor. You can ask him if you need help on the game");
	else
		return std::string("He/She is a player like you. You've met new people!!");
}

void cPC::heartbeat()
{
	if ( dead )
		return;
	if ( Accounts->GetInWorld( account ) == getSerial() && logout > 0 && ( logout <= (int32_t)getclock()  ) )
	{
		Accounts->SetOffline( account);
		logout = INVALID;
		teleport( TELEFLAG_NONE );
		return;
	}
	if( !IsOnline() )
		return;

	if ( events[evtChrOnHeartBeat] ) {
		tVariantVector params = tVariantVector(2);
		params[0] = getSerial(); params[1] = getclock();
		events[evtChrOnHeartBeat]->setParams(params);
		events[evtChrOnHeartBeat]->execute();
		if ( events[evtChrOnResurrect]->isBypassed() )
			return;
	}
	
	if( isDead() )	// Killed as result of script action
		return;
	generic_heartbeat();
	if( isDead() )	// Killed as result of generic heartbeat action
		return;

	if ( nSettings::Hunger::isEnabled() && TIMEOUT( hungertime ) && nSettings::Hunger::getRate() > 1 && !IsGMorCounselor() )
	{
		--hunger;
		sayHunger();
		hungertime = getclock()+(nSettings::Hunger::getRate()*MY_CLOCKS_PER_SEC); // Bookmark
	}
	if ( nSettings::Hunger::isEnabled() && TIMEOUT( hungerdamagetimer ) && nSettings::Hunger::getDamage() > 0 )
	// Damage them if they are very hungry
	{
		hungerdamagetimer=getclock()+(nSettings::Hunger::getDamageRate()*MY_CLOCKS_PER_SEC); /** set new hungertime **/
		if (hp > 0 && hunger<2 && !IsCounselor() && !dead)
		{
			client->sysmessage("You are starving !");
			hp -= nSettings::Hunger::getDamage();
			updateStats(0);
			if(hp<=0)
			{
				Kill();
				client->sysmessage("You have died of starvation");
			}
		}
	}
	if ( isDead() )	// Starved to death
		return;

	checkFieldEffects( getclock(), this, 1 );
	if ( isDead() )
		return;

	checkPoisoning();
	if ( isDead() )	// Poison took it's toll
		return;

	int timer;

	if     ( swingtargserial == INVALID )
		doCombat();
	else //if( TIMEOUT( timeout ) )
		combatHit( cSerializable::findCharBySerial( swingtargserial ) );


	if ( !TIMEOUT( smoketimer ) )
	{
		if ( TIMEOUT( smokedisplaytimer ) )
		{
			smokedisplaytimer = getclock() + 5 * MY_CLOCKS_PER_SEC;
			staticFX(0x3735, 0, 30);
			playSFX( 0x002B );
			switch( RandomNum( 0, 6 ) )
			{
			 case 0:	emote(socket,"*Drags in deep*" ,1);		break;
			 case 1:	emote(socket,"*Coughs*",1);				break;
			 case 2:	emote(socket,"*Retches*",1);				break;
			 case 3:	emote(socket,"*Hacking cough*",1);		break;
			 case 4:	emote(socket,"*Sighs in contentment*",1 );break;
			 case 5:	emote(socket,"*Puff puff*",1);			break;
			 case 6:	emote(socket,"Wheeeee!!! Smoking!",1);	break;
			 default:	break;
			}
		}
	}

	if ( clientInfo[socket]->lsd )
		do_lsd(); //LB's LSD potion-stuff

	if ( TIMEOUT( mutetime ) && squelched == 2 )
	{
		squelched = 0;
		mutetime  = 0;
		client->sysmessage("You are no longer squelched!");
	}

/*	if ( IsCriminal() && ( crimflag <= getclock()  ) )
	{
		client->sysmessage("You are no longer a criminal.");
		crimflag = 0;
		SetInnocent();
	}*/ //Luxor: now criminal flag is handled by CRIMINAL tempfx

	if ( TIMEOUT( murderrate ) )
	{
		if ( kills > 0)
			--kills;
		if ( kills == repsys.maxkills && repsys.maxkills > 0 )
		{
			client->sysmessage("You are no longer a murderer.");
			SetInnocent();
		}
		murderrate = ( repsys.murderdecay * MY_CLOCKS_PER_SEC ) + getclock();
	}

	updateFlag();

	if ( flags & flagIsCasting )
	{
		if ( TIMEOUT( spelltime ) )//Spell is complete target it.
		{
			if ( magic::spellRequiresTarget( spell ) ) {
				pTarget targ = clientInfo[socket]->newTarget( new cTarget() );
				targ->code_callback = target_castSpell;
				targ->buffer[0]=spell;
				targ->send(client);
				client->sysmessage("Select your target");
			}
			else
			{
		    		TargetLocation TL( this );
				magic::castSpell( spell, TL, this );
			}
			flags &= ~flagIsCasting;
			spelltime = 0;
		}
		else if ( TIMEOUT( nextact ) ) //redo the spell action
		{ //<Luxor>
			nextact = getclock() + uint32_t(MY_CLOCKS_PER_SEC*1.5);
			if ( isMounting() )
				playAction( 0x1b );
			else
				playAction( spellaction );
		//</Luxor>
		}
	}

	if( SrvParms->bg_sounds >= 1 )
	{
		if( SrvParms->bg_sounds > 10 )
			SrvParms->bg_sounds = 10;
		timer = SrvParms->bg_sounds * 100;
		if ( !timer ) ++timer;
		if( rand() % timer == timer / 2 )
			bgsound( this );
	}

	if( TIMEOUT( spiritspeaktimer ) && spiritspeaktimer > 0 )
		spiritspeaktimer = 0;


	if( onhorse )
	{
		pItem pHorse = GetItemOnLayer(LAYER_MOUNT);
		if(!pHorse)
			onhorse = false;	// turn it off, we aren't on one because there's no item!
		else
			if( pHorse->canDecay() && pHorse->getDecayTime() != 0 && TIMEOUT( pHorse->getDecayTime() ) )
			{
				onhorse = false;
				pHorse->Delete();
			}
	}

	if( GetFame() < 0 )
		SetFame(0);
}


void cPC::do_lsd()
{
	//!\todo add char scrambling to item mess-up in lsd effect?

	if (rand()%15) return;	//only about 1 every 15 heartbeats we do an lsd twist

	pClient client = getClient();

	int ctr = 0,xx,yy,icnt=0;
	int8_t zz;

	Location charpos = getPosition();

	NxwItemWrapper si;
	si.fillItemsNearXYZ( charpos, client->getVisualRange(), false );
	for( si.rewind(); !si.isEmpty(); si++ )
        {

		pItem pi=si.getItem();
		if( ! pi ) continue;

		uint16_t color=pi->getColor(); // fetch item's color
		if (rand()%44==0) color+= pi->getPosition().x  - pi->getPosition().y;
		else color+= charpos.x + charpos.y;

		color+= rand()%3; // add random "noise"
                if( (color&0x4000) || (color&0x8000) ) color = 0x1000 + rand()%255;

		// lots of color consistancy checks
		color=color%0x03E9;

		if( color<0x0002 || color>0x03E9 ) color = 0x03E9;

                Location pos = pi->getPosition();

		if (rand()%10==0) pos.x= pi->getPosition().x + rand()%3;
		if (rand()%10==0) pos.y= pi->getPosition().y + rand()%3;
		if (rand()%10==0) pos.z= pi->getPosition().z + rand()%33;

		if (distFrom(pi)<13 && rand()%7==0) //Only one item about every seven is twisted, to a maximum of 10 for each heartbeat event
		{
			icnt++;
			if (icnt%10==0 || icnt<10) client->senditem_lsd(pi,color,pos); // attempt to cut packet-bombing by this thing
		}
	}

	if (rand()%33==0)
	{
		if (rand()%10>3) client->playSFX(0x00F8, true);
		else
		{
			int snd=rand()%19;
			if (snd>9) client->playSFX((0x01<<8)|((snd-10)%256), true);
			else client->playSFX(246+snd, true);
		}
	}
}

/*!
\brief PC-related implementation of the cChar::updateFlag() method
\todo Check the comments inside it
*/
bool cPC::updateFlag()
{
	bool flagHasChanged = false;
	if ( kills >= (unsigned) repsys.maxkills )
	{
		if( !IsMurderer() )
			flagHasChanged = true;
		//
		//! \todo - TODO check out logic of next 2 statements (Sparhawk)
		//
		SetMurderer();
		murderrate = (repsys.murderdecay*MY_CLOCKS_PER_SEC)+getclock();
	}
	/*else

		if ( crimflag == 0 )
		{
			if( !IsInnocent() )
				flagHasChanged = true;
			SetInnocent();
		}
		else
		{
			if( !IsCriminal() )
				flagHasChanged = true;
			SetCriminal();
		}
		*/
	return flagHasChanged;
}

/*!
\brief Send the update skill packet to the client
\param skill Skill to update
\author Flameeyes
*/
void cChar::updateSkill(uint16_t skill)
{
	if ( ! client )
		return;

	cPacketSendUpdateSkill pk(this, skill);

	client->sendPackage(&pk);
}

void cPC::deadAttack (pChar victim)
{
	if ( ! victim ) return;
	if(victim->npc)
	{
		if(victim->npcaitype==NPCAI_HEALER)
		{
			if( isInnocent() )
			{
				if ( distFrom( victim ) <= 3 )
				{//let's resurrect him!
					victim->playAction(0x10);
					resurrect();
					victim->staticFX(0x376A, 9, 6);
					switch(RandomNum(0, 4))
					{
					case 0: victim->talkAll("Thou art dead, but 'tis within my power to resurrect thee.  Live!", false); break;
					case 1: victim->talkAll("Allow me to resurrect thee ghost.  Thy time of true death has not yet come.", false); break;
					case 2: victim->talkAll("Perhaps thou shouldst be more careful.  Here, I shall resurrect thee.", false); break;
					case 3: victim->talkAll("Live again, ghost!  Thy time in this world is not yet done.", false); break;
					case 4: victim->talkAll("I shall attempt to resurrect thee.", false); break;
					}
				}
				else
				{//if dist>3
					victim->talkAll("Come nearer, ghost, and i'll give you life!",1);
				}
			}
			else
			{//if a bad guy
				victim->talkAll("I will not give life to a scoundrel like thee!",1);
			}
		}
		else if( victim->npcaitype == NPCAI_EVILHEALER )
		{
			if( isMurderer())
			{
				if ( distFrom( victim ) <=3 )
				{//let's resurrect him!
					victim->playAction(0x10);
					resurrect();
					victim->staticFX(0x3709, 9, 25); //Flamestrike effect
					switch(rand()%5)
					{
						case 0: victim->talkAll("Fellow minion of Mondain, Live!!", false); break;
						case 1: victim->talkAll("Thou has evil flowing through your vains, so I will bring you back to life.", false); break;
						case 2: victim->talkAll("If I res thee, promise to raise more hell!.", false); break;
						case 3: victim->talkAll("From hell to Britannia, come alive!.", false); break;
						case 4: victim->talkAll("Since you are Evil, I will bring you back to consciouness.", false); break;
					}
				}
				else
				{//if dist >3
					victim->talkAll("Come nearer, evil soul, and i'll give you life!", true);
				}
			}
			else
			{//if player is a good guy
				victim->talkAll("I dispise all things good. I shall not give thee another chance!"), true);
			}
		}
		else
		{
			client->sysmessage("You are dead and cannot do that.");
		}//npcaitype check
	}
	else
	{//if this not a npc but a player
		if(SrvParms->persecute)
		{//start persecute stuff - AntiChrist
			targserial = victim->getSerial();
			Skills::Persecute(getClient());
		}
		else
		{
			client->sysmessage("You are dead and cannot do that.");
		}
	}//if npc
}

/*!
\brief Tells the user his hunger state
\author Flameeyes
*/
void cPC::sayHunger()
{
	if ( isGMorCounselor() )
		return;
	switch( hunger )
	{
		case 6:
		case 5: client->sysmessage("You are still stuffed from your last meal");
			break;
		case 4: client->sysmessage("You are not very hungry but could eat more");
			break;
		case 3: client->sysmessage("You are feeling fairly hungry");
			break;
		case 2: client->sysmessage("You are extremely hungry");
			break;
		case 1: client->sysmessage("You are very weak from starvation");
			break;
		case 0:	client->sysmessage("You must eat very soon or you will die!");
			break;
	}
}

void cPC::setMurderer()
{
	if ( events[evtPcOnFlagChange] ) {
		tVariantVector params = tVariantVector(2);
		params[0] = getSerial(); params[1] = karmaMurderer;
		events[evtPcOnFlagChange]->setParams(params);
		events[evtPcOnFlagChange]->execute();
		if ( events[evtPcOnFlagChange]->isBypassed() )
			return;
	}

	reputation = karmaMurderer;
}

void cPC::setInnocent()
{
	if ( events[evtPcOnFlagChange] ) {
		tVariantVector params = tVariantVector(2);
		params[0] = getSerial(); params[1] = karmaInnocent;
		events[evtPcOnFlagChange]->setParams(params);
		events[evtPcOnFlagChange]->execute();
		if ( events[evtPcOnFlagChange]->isBypassed() )
			return;
	}

	reputation = karmaInnocent;
}

void cPC::setCriminal()
{
	if ( events[evtPcOnFlagChange] ) {
		tVariantVector params = tVariantVector(2);
		params[0] = getSerial(); params[1] = karmaCriminal;
		events[evtPcOnFlagChange]->setParams(params);
		events[evtPcOnFlagChange]->execute();
		if ( events[evtPcOnFlagChange]->isBypassed() )
			return;
	}
	
	reputation = karmaCriminal;
}

//! Makes someone criminal
void cPC::makeCriminal()
{
	if ( isCriminal() || isMurderer() )
		return;
	
	tempfx::add(this, this, tempfx::CRIMINAL, 0, 0, 0); //Luxor
	if(::region[region].priv&0x01 && SrvParms->guardsactive) { //guarded
		if ( nSettings::Server::hasInstantGuards() )
			npcs::SpawnGuard( this, this, getPosition() ); // LB bugfix
	}
}

/*!
\author Luxor
\brief Calculates total attack power
\return character's attack
*/
uint16_t cPC::calcAtt()
{
	pWeapon pw = getBody()->getWeapon();
	
	if ( ! pw )
		return skill[skWrestling]/100;

	return RandomNum(pi->lodamage, pi->hidamage);
}
