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

/*!
\brief Container item
\note This class must be identified via rtti() method to be sure
that the item is a Container
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

	bool		addItem(pItem item, UI16 xx=-1, UI16 yy=-1);
	void		insertItem(pItem itm);

	UI16		getGump();

	UI32		removeItems(UI32 amount, UI16 id, UI16 color);
	UI32		removeItems(UI32 amount, UI32 scriptID);
	void		dropItem(pItem pi);

	UI32		countItems(UI32 scriptID, bool total = false);
	UI32		countItems(UI16 id, UI16 color, bool recurse = false);

	pItem		findFirstType(UI16 type, bool recurse = false);

	UI32		countSpellsInSpellBook(bool stdOnly = true);
	bool		containsSpell(magic::SpellId spellnum);
} PACK_NEEDED;

#endif

