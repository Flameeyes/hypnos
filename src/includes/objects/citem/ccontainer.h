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

class cContainer;
typedef cContainer* pContainer;

#include "objects/citem.h"
#include "rtti.h"
#include "typedefs.h"

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
protected:
	//! Items in the container
	ItemList	items;

	bool		pileItem(pItem &item);
	void		setRandPos(pItem Item);

public:
			cContainer(bool ser= true);

	//! Return the right rtti
	inline const UI32 rtti() const
	{ return rtti::cContainer; }

	UI16		getGump();
	bool		addItem(pItem item, UI16 xx=-1, UI16 yy=-1);	// Add Item to container
	UI32		removeItems(UI32 scriptID, UI32 amount/*= 1*/);
	void		dropItem(pItem pi);
	UI32		countItems(UI32 scriptID, bool total = false);

} PACK_NEEDED;

#endif

