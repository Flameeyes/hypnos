/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Implementation of cNPC class
*/

#include "objects/cnpc.h"

#define NPCMAGIC_FLAGS (SPELLFLAG_DONTCRIMINAL+SPELLFLAG_DONTREQREAGENTS+SPELLFLAG_DONTCHECKSPELLBOOK+SPELLFLAG_IGNORETOWNLIMITS+SPELLFLAG_DONTCHECKSKILL)
void cNPC::heartbeat()
{
	if ( dead )
		return;
	if( stablemaster_serial != INVALID )
		return;

	if( mounted )
		return;

	if ( amxevents[EVENT_CHR_ONHEARTBEAT] )
	{
		g_bByPass = false;
		amxevents[EVENT_CHR_ONHEARTBEAT]->Call( getSerial(), uiCurrentTime );
		if ( g_bByPass == true )
			return;
		if( dead )	// Killed as result of action in script
			return;
	}
	/*
	g_bByPass = false;
	runAmxEvent( EVENT_CHR_ONHEARTBEAT, getSerial(), uiCurrentTime );
	if ( g_bByPass == true )
		return;
	*/
	if( dead )	// Killed as result of action in script
		return;
	//
	//	Enable if possible
	//
	if ( TIMEOUT( disabled ) )
		disabled = 0;
	else
		return;
	//
	generic_heartbeat();
	if( dead )	// Killed as result of action in generic heartbeat
		return;
	//
	//	Handle poisoning
	//
	checkPoisoning();
	if( dead )
		return;
	checkFieldEffects( uiCurrentTime, this, 0 );
	if( dead )
		return;
	//
	//	Handle summoned npc's
	//
	if ( TIMEOUT( summontimer ) && summontimer > 0 )
	{

		if ( amxevents[EVENT_CHR_ONDISPEL] )
		{
			g_bByPass = false;
			amxevents[EVENT_CHR_ONDISPEL]->Call( getSerial(), INVALID, DISPELTYPE_TIMEOUT );
			if ( g_bByPass == true ) return;
		}
		/*
		g_bByPass = false;
		runAmxEvent( EVENT_CHR_ONDISPEL, getSerial(), INVALID, DISPELTYPE_TIMEOUT );
		if ( g_bByPass == true ) return;
		*/
		// Dupois - Added Dec 20, 1999
		// QUEST expire check - after an Escort quest is created a timer is set
		// so that the NPC will be deleted and removed from the game if it hangs around
		// too long without every having its quest accepted by a player so we have to remove
		// its posting from the message board before icing the NPC
		// Only need to remove the post if the NPC does not have a follow target set
		if ( questType == cMsgBoard::ESCORTQUEST && ftargserial == INVALID )
		{
			cMsgBoard::MsgBoardQuestEscortRemovePost( DEREF_pChar(this) );
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
				case 5: emoteall( TRANSLATE( "* %s looks a little hungry *" ), 1, getCurrentNameC() );
					break;
				case 4: emoteall( TRANSLATE( "* %s looks fairly hungry *" ), 1, getCurrentNameC() );
					break;
				case 3: emoteall( TRANSLATE( "* %s looks extremely hungry *" ), 1, getCurrentNameC() );
					break;
				case 2: emoteall( TRANSLATE( "* %s looks weak from starvation *"), 1, getCurrentNameC() );
					break;
				case 1: emoteall( TRANSLATE("* %s must eat very soon or he will die! *"), 1, getCurrentNameC() );
					break;
				case 0:	ftargserial = INVALID;
					npcWander = WANDER_FREELY_CIRCLE;
					setOwnerSerial32( INVALID );
					emoteall( TRANSLATE("* %s appears to have decided that it is better off without a master *"), 0, getCurrentNameC());
					playSFX( 0x01FE);
					if( SrvParms->tamed_disappear )
					{
						Delete();
						return;
					}
					break;
			}
		}
		hungertime = uiCurrentTime + ( SrvParms->hungerrate * MY_CLOCKS_PER_SEC );
	}

	if( npcWander!=WANDER_FLEE ) {
		if( hp < getStrength() * fleeat / 100 ) {
			flee( pointers::findCharBySerial( ftargserial ) );
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
		npcs::checkAI( this );

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
			combatHit( pointers::findCharBySerial( swingtargserial ) );

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
			nextact = uiCurrentTime + uint32_t(MY_CLOCKS_PER_SEC*1.5);
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
		summontimer = ( uiCurrentTime + ( MY_CLOCKS_PER_SEC * SrvParms->escortinitexpire ) );

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
		ConOut( "createEscortQuest() Failed to add quest post for %s\n", getCurrentNameC() );
		ConOut( "createEscortQuest() Deleting NPC %s\n", getCurrentNameC() );
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

 	char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var
	// Calculate payment for services rendered
	int servicePay = ( RandomNum(0, 20) * RandomNum(1, 30) );  // Equals a range of 0 to 600 possible gold with a 5% chance of getting 0 gold

	// If they have no money, well, oops!
	if ( servicePay == 0 )
	{
		sprintf( temp, TRANSLATE("Thank you %s for thy service. We have made it safely to %s. Alas, I seem to be a little short on gold. I have nothing to pay you with."), pc->getCurrentNameC(), region[questDestRegion].name );
		talk( pc->getClient(), temp, 0 );
	}
	else // Otherwise pay the poor sod for his time
	{
		// Less than 75 gold for a escort is pretty cheesey, so if its between 1 and 75, add a randum amount of between 75 to 100 gold
		if ( servicePay < 75 ) servicePay += RandomNum(75, 100);
		pc->addGold(servicePay);
		pc->playSFX( goldsfx(servicePay) );
		sprintf( temp, TRANSLATE("Thank you %s for thy service. We have made it safely to %s. Here is thy pay as promised."), pc->getCurrentNameC(), region[questDestRegion].name );
		talk( pc->getClient(), temp, 0 );
	}

	// Inform the PC of what he has just been given as payment
	pc->getClient()->sysmsg(TRANSLATE("You have just received %d gold coins from %s %s"), servicePay, getCurrentNameC(), title.c_str() );

	// Take the NPC out of quest mode
	npcWander = WANDER_FREELY_CIRCLE;         // Wander freely
	ftargserial = INVALID;            // Reset follow target
	questType = QTINVALID;         // Reset quest type
	questDestRegion = 0;   // Reset quest destination region

	// Set a timer to automatically delete the NPC
	summontimer = ( uiCurrentTime + ( MY_CLOCKS_PER_SEC * SrvParms->escortdoneexpire ) );

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

