/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

/*!
\file
\brief Header defining cContainer class
*/
#ifndef __CONTAINERS_H
#define __CONTAINERS_H

#include "citem.h"

class cContainer;
typedef cContainer* pContainer;

// will be fully implemented in 0.83
// Anthalir
/*

  properties of containers:
	- one or more key(s)
	- a list of items stored in it
	- a gump
*/
class cContainer : public cItem
{
private:
	vector<SI32>		ItemList;

	bool				pileItem(P_ITEM pItem);
	void				setRandPos(P_ITEM pItem);

public:
					cContainer(bool ser= true);
	UI16				getGump();
	bool				AddItem(P_ITEM item, short xx=-1, short yy=-1);	// Add Item to container
	UI32				removeItems(UI32 scriptID, UI32 amount/*= 1*/);
	void				dropItem(P_ITEM pi);
	UI32				countItems(UI32 scriptID, bool bAddAmounts= false);

} PACK_NEEDED;

#endif

