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

