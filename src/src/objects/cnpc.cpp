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
		amxevents[EVENT_CHR_ONHEARTBEAT]->Call( getSerial32(), uiCurrentTime );
		if ( g_bByPass == true )
			return;
		if( dead )	// Killed as result of action in script
			return;
	}
	/*
	g_bByPass = false;
	runAmxEvent( EVENT_CHR_ONHEARTBEAT, getSerial32(), uiCurrentTime );
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
			amxevents[EVENT_CHR_ONDISPEL]->Call( getSerial32(), INVALID, DISPELTYPE_TIMEOUT );
			if ( g_bByPass == true ) return;
		}
		/*
		g_bByPass = false;
		runAmxEvent( EVENT_CHR_ONDISPEL, getSerial32(), INVALID, DISPELTYPE_TIMEOUT );
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
			cMsgBoard::MsgBoardQuestEscortRemovePost( DEREF_P_CHAR(this) );
			cMsgBoard::MsgBoardQuestEscortDelete( DEREF_P_CHAR(this) );
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
			nextact = uiCurrentTime + UI32(MY_CLOCKS_PER_SEC*1.5);
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
SI32 cNPC::getTeachingDelta(pPC pPlayer, SI32 skill, SI32 sum)
{
	UI32 delta = qmin(250,baseskill[skill]/2); 	// half the trainers skill, but not more than 250
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
