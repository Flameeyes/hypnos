/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Fishing Functions
*/

#ifndef __FISHING_H__
#define __FISHING_H__

#include "common_Libs.hpp"

namespace Fishing
{
	extern uint32_t basetime;
	extern uint32_t randomtime;
	
	void target_fish( pClient client, pTarget t );
	void Fish(pClient client);
};

pNPC SpawnFishingMonster(pChar pc, char* cScript, char* cList, char* cNpcID);
pItem SpawnFishingItem(pChar pc, int nInPack, char* cScript, char* cList, char* cItemID);



#endif
