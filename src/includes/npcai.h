/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief NPC AI Stuff
*/
#ifndef __NPCAI_H__
#define __NPCAI_H__

enum NpcAi
{
	NPCAI_GOOD		= 0,
	NPCAI_HEALER,
	NPCAI_EVIL,
	NPCAI_EVILHEALER,
	NPCAI_TELEPORTGUARD,
	NPCAI_BEGGAR,
	NPCAI_ORDERGUARD,
	NPCAI_CHAOSGUARD,
	NPCAI_BANKER,
	NPCAI_GUARD,
	NPCAI_TAMEDDRAGON,
	NPCAI_ATTACKSRED,
	NPCAI_PLAYERVENDOR	= 17,
	NPCAI_PETGUARD		= 32,
	NPCAI_MADNESS		= 50,
	NPCAI_DRAGON2		= 60,
	NPCAI_DRAGON1		= 96
};

#endif
