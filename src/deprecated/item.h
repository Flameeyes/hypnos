  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

/*!
\file
\brief Header defining item namespace
\author Akron
\date 18/03/2003
\since 0.82r3
\todo move here eLayer and item type enums
*/
#ifndef __ITEM_H__
#define __ITEM_H__

#include "nxwcommn.h"

/*!
\brief Item related stuff
*/
namespace item
{

	P_ITEM		CreateFromScript( char* itemname, cObject* cont=NULL, int amount = INVALID );
	P_ITEM		CreateFromScript( SCRIPTID itemnum, cObject* cont=NULL, int amount = INVALID );
	P_ITEM		CreateScriptRandomItem( char* sItemList, cObject* cont=NULL );
	SI32		CreateRandomItem( char *sItemList );

	void		GetScriptItemSetting(P_ITEM pi); // by Magius(CHE)

	P_ITEM		CreateScriptItem(NXWSOCKET s, SI32 itemnum, LOGICAL nSpawned, cObject* cont=NULL );

	void		AddRespawnItem(int s, int x, int y);
	P_ITEM		SpawnRandomItem(NXWSOCKET s, char* cList, char* cItemID);
};

#endif
