/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief cChar's effects (graphics and sound) methods
*/

#include "enums.hpp"
#include "objects/cchar.hpp"

/*!
\author Luxor
\brief Does a missed sound effect for combat
*/
void cChar::doMissedSoundEffect()
{
	uint8_t a=RandomNum(0,2);

	switch (a)
	{
		case 0: playSFX(0x0238); break;
		case 1: playSFX(0x0239); break;
		default: playSFX(0x023A);
	}
}

/*!
\brief Plays a monster sound effect
\param sfx Sound effect type
*/
void cChar::playMonsterSound(MonsterSound sfx)
{
	pCreatureInfo creature = creatures.getCreature( getId() );
	if( ! creature )
		return;

	uint16_t s = creature->getSound( sfx );
	if( s != 0xFFFF )
		client->playSFX( s );
}

/*!
\brief Makes the char doing an action
\author Luxor
\param action ID of the action to play
*/
void cChar::playAction(uint16_t action)
{
	switch (action)
	{
	case 0x1A:// Mining-Gravedigging
	case 0x0B:
		action = onhorse ? 0x1A : 0x0B;
		break;
	case 0x1C:// LumberJacking-Bowcraft
	case 0x0D:
		action = onhorse ? 0x1C : 0x0D;
		break;
	case 0x1D:// Swordtarget
	// case 0x0D:
		action = onhorse ? 0x1D : 0x0D;
		break;
	case 0x0A:// Fist Fighting
		action = onhorse ? 0x1A : 0x0A;
		break;
	case 0x0E:// Smelting irons
		action = onhorse ? 0x1C : 0x0E;
		break;
	case 0x09:// Working ingots
		action = onhorse ? 0x1A : 0x09;
		break;
	case 0x14:// These can be done only if not onhorse
	case 0x22:
		if (onhorse)
			action = 0x00;
		break;
	}

	if ( ! action )
		return;

	nPackets::Sent::Action pa(this, action);

	cClientSet cs;
	cs.fillOnline( this, false );

	for( cs.rewind(); !cs.isEmpty(); cs++ )
		cs.sendPacket(&pa);
}

void cChar::impAction(uint16_t action)
{
	if ( getBody()->isMounted() && (action==0x10 || action==0x11))
	{
		playAction(0x1b);
		return;
	}
	if ( getBody()->isMounted() || ( getId() < 0x190 ) && action == 0x22 )
		return;
	playAction(action);
}

/*!
\brief plays the combat animation
\author Luxor
*/
void cChar::playCombatAction()
{
	if ( !body->isHuman() ) {
		playAction(4+rand()%3);
		playMonsterSound(SND_ATTACK);
	} else if ( onhorse ) {
		combatOnHorse();
	} else {
		combatOnFoot();
	}
}
