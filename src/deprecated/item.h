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

#include "common_libs.h"

/*!
\brief Item related stuff
*/
namespace item
{

	pItem		CreateFromScript( char* itemname, cObject* cont=NULL, int amount = INVALID );
	pItem		CreateFromScript( SCRIPTID itemnum, cObject* cont=NULL, int amount = INVALID );
	pItem		CreateScriptRandomItem( char* sItemList, cObject* cont=NULL );
	int32_t		CreateRandomItem( char *sItemList );

	void		GetScriptItemSetting(pItem pi); // by Magius(CHE)

	pItem		CreateScriptItem(NXWSOCKET s, int32_t itemnum, bool nSpawned, cObject* cont=NULL );

	void		AddRespawnItem(int s, int x, int y);
	pItem		SpawnRandomItem(NXWSOCKET s, char* cList, char* cItemID);
};

#endif
