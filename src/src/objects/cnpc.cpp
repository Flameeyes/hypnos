/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Implementation of cNPC class
*/

#include "objects/cnpc.h"

#define NPCMAGIC_FLAGS (SPELLFLAG_DONTCRIMINAL+SPELLFLAG_DONTREQREAGENTS+SPELLFLAG_DONTCHECKSPELLBOOK+SPELLFLAG_IGNORETOWNLIMITS+SPELLFLAG_DONTCHECKSKILL)


const std::string cNPC::getPopupHelp() const
{
	if (npcaitype==NPCAI_PLAYERVENDOR)
		return std::string("He/She is a vendor which sells items owned by a player. Good shopping!");
	else if (npcaitype==NPCAI_EVIL)
		return std::string("Run AWAY!!! It will kill you!!");
}

cNPC::cNPC()
	: cChar()
{
}

cNPC::cNPC(uint32_t serial)
	: cChar(serial)
{
}

void cNPC::heartbeat()
{
	if ( isDead() )
		return;
	if( stablemaster )
		return;

	if ( events[evtChrOnHeartBeat] ) {
		tVariantVector params = tVariantVector(2);
		params[0] = getSerial(); params[1] = getclock();
		events[evtChrOnHeartBeat]->setParams(params);
		events[evtChrOnHeartBeat]->execute();
		if ( events[evtChrOnHeartBeat]->isBypassed() )
			return;
		
		if ( isDead() )
			return;
	}
	
	//
	//	Enable if possible
	//
	if ( TIMEOUT( disabled ) )
		disabled = 0;
	else
		return;
	//
	generic_heartbeat();
	if ( isDead() )
		return;
	//
	//	Handle poisoning
	//
	checkPoisoning();
	if ( isDead() )
		return;
	checkFieldEffects( getclock(), this, 0 );
	if ( isDead() )
		return;
	//
	//	Handle summoned npc's
	//
	if ( TIMEOUT( summontimer ) && summontimer > 0 )
	{
		if ( events[evtNpcOnDispel] && getClient() ) {
			tVariantVector params = tVariantVector(3);
			params[0] = getSerial(); params[1] = INVALID;
			params[2] = dispelTimeout;
			events[evtNpcOnDispel]->setParams(params);
			events[evtNpcOnDispel]->execute();
			if ( events[evtNpcOnDispel]->isBypassed() )
				return;
		}

		// Dupois - Added Dec 20, 1999
		// QUEST expire check - after an Escort quest is created a timer is set
		// so that the NPC will be deleted and removed from the game if it hangs around
		// too long without every having its quest accepted by a player so we have to remove
		// its posting from the message board before icing the NPC
		// Only need to remove the post if the NPC does not have a follow target set
		if ( questType == cMsgBoard::ESCORTQUEST && ftargserial == INVALID )
		{
			cMsgBoard::MsgBoardQuestEscortRemovePost( this );
			deleteEscortQuest();
		}
		else
		{
			playSFX( 0x01FE );
			dead = true;
			Delete();
		}
		return;
	}
	//
	//	Handle hunger
	//
	if ( SrvParms->hungerrate > 1 && TIMEOUT( hungertime ) )
	{
		if ( hunger )
			--hunger;

		if( tamed && npcaitype != NPCAI_PLAYERVENDOR )
		{
			emotecolor = 0x0026;
			switch( hunger )
			{
				case 6: break;
				case 5: emoteall("* %s looks a little hungry *", true, getCurrentName().c_str() );
					break;
				case 4: emoteall("* %s looks fairly hungry *", true, getCurrentName().c_str() );
					break;
				case 3: emoteall("* %s looks extremely hungry *", true, getCurrentName().c_str() );
					break;
				case 2: emoteall("* %s looks weak from starvation *", true, getCurrentName().c_str() );
					break;
				case 1: emoteall("* %s must eat very soon or he will die! *", true, getCurrentName().c_str() );
					break;
				case 0:	ftargserial = INVALID;
					npcWander = WANDER_FREELY_CIRCLE;
					setOwnerSerial32( INVALID );
					emoteall("* %s appears to have decided that it is better off without a master *", false, getCurrentName().c_str());
					playSFX( 0x01FE);
					if( SrvParms->tamed_disappear )
					{
						Delete();
						return;
					}
					break;
			}
		}
		hungertime = getclock() + ( SrvParms->hungerrate * MY_CLOCKS_PER_SEC );
	}

	if( npcWander!=WANDER_FLEE ) {
		if( hp < getStrength() * fleeat / 100 ) {
			flee( cSerializable::findCharBySerial( ftargserial ) );
			setNpcMoveTime();
		}
	}
	else
		if( ( ( fleeTimer==INVALID ) && ( hp > getStrength() * reattackat / 100 ) ) ||
			( ( fleeTimer!=INVALID ) && TIMEOUT( fleeTimer ) ) )
		{
			npcWander = oldnpcWander;
			setNpcMoveTime();
			oldnpcWander = WANDER_NOMOVE; // so it won't save this at the wsc file
			fleeTimer=INVALID;
		}

	//
	//	Handle ai
	//
	if ( TIMEOUT( nextAiCheck ) )
		checkAI();

	//
	//	Handle walking
	//
	if ( TIMEOUT( npcmovetime ) )
		walk();

        //
        //	Handle combat
        //
	if ( swingtargserial == INVALID )
		doCombat();
	else
		if( TIMEOUT( timeout )  )
			combatHit( cSerializable::findCharBySerial( swingtargserial ) );

	updateFlags();

	//
	//	Handle spell casting (Luxor)
	//
	if ( flags & flagIsCasting ) {
		if ( TIMEOUT( spelltime ) ) {
			if ( spellTL != NULL ) {
	    			magic::castSpell( spell, *spellTL, this, NPCMAGIC_FLAGS );
				flags &= flagIsCasting;
				spelltime = 0;
				safedelete( spellTL );
			}
		} else if ( TIMEOUT( nextact ) ) {
			nextact = getclock() + uint32_t(MY_CLOCKS_PER_SEC*1.5);
			if ( isMounting() )
				playAction( 0x1b );
			else
				playAction( spellaction );
		}
	}
}
#undef NPCMAGIC_FLAGS

/*!
\brief calculates how much the given player can learn from this teacher
\author Duke
\param pPlayer pointer to the player
\param skill index of skill to teach
\return how much the given player can learn
*/
int32_t cNPC::getTeachingDelta(pPC pPlayer, int32_t skill, int32_t sum)
{
	uint32_t delta = qmin(250,baseskill[skill]/2); 	// half the trainers skill, but not more than 250
	delta -= pPlayer->baseskill[skill]; 				// calc difference
	if (delta <= 0)
		return 0;

	if (sum+delta >= SrvParms->skillcap * 10)			// would new skill value be above cap ?
		delta = (SrvParms->skillcap * 10) - sum;		// yes, so reduce it
	return delta;
}

/*!
\brief Sets the NPC's owner
\author Flameeyes
\param pc new owner
*/
void cNPC::setOwner(pChar pc)
{
	if ( owner )
		owner->removeOwned(this);

	owner = pc;

	if ( owner )
		owner->addOwned(this);
}

/*!
\brief NPC-related implementation of cChar::updateFlag function
*/
bool cNPC::updateFlag()
{
	bool flagHasChanged = false;
	switch (npcaitype)
	{
		case NPCAI_EVIL:
		case NPCAI_EVILHEALER:
		case NPCAI_MADNESS: // EV & BS
			if( !IsMurderer() )
				flagHasChanged = true;
			SetMurderer();
			break;
		case NPCAI_HEALER:
		case NPCAI_TELEPORTGUARD:
		case NPCAI_BEGGAR:
		case NPCAI_ORDERGUARD:
		case NPCAI_CHAOSGUARD:
		case NPCAI_BANKER:
		case NPCAI_GUARD:
		case NPCAI_PLAYERVENDOR:
		case 30: // ?
		case 40: // ? old teleport guards?
			if( !IsInnocent() )
				flagHasChanged = true;
			SetInnocent();
			break;

		default:
			if ( body && body->isHuman() )
			{
				if( !IsInnocent() )
					flagHasChanged = true;
				SetInnocent();
			}
			else if (server_data.animals_guarded == 1 && npcaitype == NPCAI_GOOD && !owner)
			{
				if (region[region].priv & RGNPRIV_GUARDED)	// in a guarded region, with guarded animals, animals == blue
				{
					if( !IsInnocent() )
						flagHasChanged = true;
					SetInnocent();
				}
				else				// if the region's not guarded, they're gray
				{
					if( !IsCriminal() )
						flagHasChanged = true;
					SetCriminal();
				}
			}
			else if ( owner )
			{
				if( flag != owner->getFlag() )
					flagHasChanged = true;
				flag = owner->getFlag();
			}
			else
			{
				if( !IsCriminal() )
					flagHasChanged = true;
				SetCriminal();
			}
			break;
	}
	return flagHasChanged;
}

/*
\brief Stable the NPC
\author Flameeyes (based on Endymion)
\param sm the stablemaster
\todo we should write a cStablemaster class..
*/
void cNPC::stable( pNPC sm )
{
	if ( ! sm ) return;

	stablemaster = sm;

	pointers::addToStableMap( this );
}

/*
\brief Unstable the NPC
\author Flameeyes (based on Endymion)
\todo we should write a cStablemaster class..
*/
void cNPC::unStable()
{
	if ( ! stablemaster ) return;

	pointers::delFromStableMap( this );

	stablemaster = NULL;
}

/*!
\brief Simpler attack for NPCs
\param pc_target target character
*/
void cChar::simpleAttack(pChar pc_target)
{
	if ( ! pc_target || pc_target == this || pc_target->isDead() || isDead() )
		return;

	fight( pc_target2 );
	setAttackFirst()
	pc_target2->fight( this );
	pc_target2->setAttackFirst(false);
}



/*!
\brief Creates an escort quest with this npc. Publishes on regional msgboards
*/

void cNPC::createEscortQuest()
{
	// Choose a random region as a destination for the escort quest (except for the same region as the NPC was spawned in)
	int loopexit=0;
	do
	{
		if ( escortRegions )  //escortRegions and validEscortRegion[] are global variables and are loaded in sregions.cpp
		{
			// If the number of escort regions is 1, check to make sure that the only
			// valid escort region is not the NPC's current location - if it is Abort
			if ( (escortRegions==1) && (validEscortRegion[0]== region) )
			{
				questDestRegion = 0;
				break;
			}

			questDestRegion = validEscortRegion[RandomNum(0, (escortRegions-1))];
		}
		else
		{
			questDestRegion = 0;  // If no escort regions have been defined in REGIONS.SCP then we can't do it!!
			break;
		}
	} while ( (questDestRegion == region) 	&& (++loopexit < MAXLOOPS)  );

	// Set quest type to escort
	questType = cMsgBoards::ESCORTQUEST;

	// Make sure they don't move until an player accepts the quest
	npcWander       = WANDER_NOMOVE;	// Don't want our escort quest object to wander off.
	npcaitype = NPCAI_GOOD;                	// Remove any AI from the escort (should be dumb, just follow please :)
	questOrigRegion = region;  // Store this in order to remeber where the original message was posted

	// Set the expirey time on the NPC if no body accepts the quest
	if ( SrvParms->escortinitexpire )
		summontimer = ( getclock() + ( MY_CLOCKS_PER_SEC * SrvParms->escortinitexpire ) );

	// Make sure the questDest is valid otherwise don't post and delete the NPC
	if ( !questDestRegion )
	{
		ErrOut("createEscortQuest() No valid regions defined for escort quests\n");
		Delete();
		return;
	}

	// Post the message to the message board in the same REGION as the NPC
        questEscortPostSerial = createQuestMessage(ESCORTQUEST,getSerial(), NULL, region);
	if ( !questEscortPostSerial )
	{
		ConOut( "createEscortQuest() Failed to add quest post for %s\n", getCurrentName().c_str() );
		ConOut( "createEscortQuest() Deleting NPC %s\n", getCurrentName().c_str() );
		Delete();
		return;
	}

	// Debugging messages
#ifdef DEBUG
	ErrOut("createEscortQuest() Escort quest for:\n       %s to be escorted to %s\n", name, region[questDestRegion].name );
#endif

}


/*!
\brief completed escort quest with this npc. Gets reward
*/
void cNPC::clearedEscordQuest(pPC pc)
{
	// Calculate payment for services rendered
	int servicePay = ( RandomNum(0, 20) * RandomNum(1, 30) );  // Equals a range of 0 to 600 possible gold with a 5% chance of getting 0 gold

	// If they have no money, well, oops!
	if ( servicePay == 0 )
	{
		talk( pc->getClient(), "Thank you %s for thy service. We have made it safely to %s. Alas, I seem to be a little short on gold. I have nothing to pay you with.", false, pc->getCurrentName().c_str(), region[questDestRegion].name );
	}
	else // Otherwise pay the poor sod for his time
	{
		// Less than 75 gold for a escort is pretty cheesey, so if its between 1 and 75, add a randum amount of between 75 to 100 gold
		if ( servicePay < 75 ) servicePay += RandomNum(75, 100);
		pc->addGold(servicePay);
		pc->playSFX( goldsfx(servicePay) );
		talk( pc->getClient(), "Thank you %s for thy service. We have made it safely to %s. Here is thy pay as promised.", false, pc->getCurrentName().c_str(), region[questDestRegion].name );
	}

	// Inform the PC of what he has just been given as payment
	pc->getClient()->sysmessage("You have just received %d gold coins from %s %s", servicePay, getCurrentName().c_str(), title.c_str() );

	// Take the NPC out of quest mode
	npcWander = WANDER_FREELY_CIRCLE;         // Wander freely
	ftargserial = INVALID;            // Reset follow target
	questType = QTINVALID;         // Reset quest type
	questDestRegion = 0;   // Reset quest destination region

	// Set a timer to automatically delete the NPC
	summontimer = ( getclock() + ( MY_CLOCKS_PER_SEC * SrvParms->escortdoneexpire ) );

    	setOwnerSerial32Only(-1);

}

/*!
\brief deletes msgboard post for this quest, since it has been accepted (or has expired)
*/
void cNPC::removepostEscortQuest()
{
	cMsgBoard::removeQuestMessage(questEscortPostSerial);
}

/*!
\brief deletes npc used for quest
\note it is mainly used as a wrapper, so if a particular necessity ever arises, it can be added here
*/
void cNPC::deleteEscortQuest()
{
	Kill();
	Delete();
}

/*!
\author Luxor
\brief Calculates total attack power
\return character's attack
*/
uint16_t cNPC::calcAtt()
{
	if (lodamage < 1 || hidamage < 1)
		return 1;
	return RandomNum(lodamage, hidamage);
}
