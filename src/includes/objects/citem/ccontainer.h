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
	inline const uint32_t rtti() const
	{ return rtti::cContainer; }

	bool		addItem(pItem item, uint16_t xx=-1, uint16_t yy=-1);
	void		insertItem(pItem itm);

	uint16_t		getGump();

	uint32_t		removeItems(uint32_t amount, uint16_t id, uint16_t color = 0);
	uint32_t		removeItems(uint32_t amount, uint32_t scriptID);
	void		dropItem(pItem pi);

	uint32_t		countItems(uint32_t scriptID, bool total = false);
	uint32_t		countItems(uint16_t id, uint16_t color = 0, bool recurse = false);

	pItem		findFirstType(uint16_t type, bool recurse = false);

	uint32_t		countSpellsInSpellBook(bool stdOnly = true);
	bool		containsSpell(magic::SpellId spellnum);
	
	const R32       getWeightActual();
} PACK_NEEDED;

#endif

