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

namespace Fishing
{
	extern TIMERVAL basetime;
	extern TIMERVAL randomtime;
	
	void target_fish( pClient client, pTarget t );
	void Fish(pClient client);
};

pNPC SpawnFishingMonster(pChar pc, char* cScript, char* cList, char* cNpcID);
int SpawnFishingItem(int nCharID,int nInPack, char* cScript, char* cList, char* cItemID);



#endif
