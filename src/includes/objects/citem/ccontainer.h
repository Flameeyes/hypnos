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

#include "common_libs.h"
#include "objects/citem.h"

/*!
\brief Container item
\note This class must be referred using cItem::toContainer() function
*/
class cContainer : public cItem
{
protected:
	//! Items in the container
	ItemList	items;

	bool		pileItem(pItem &item);
	void		setRandPos(pItem Item);

public:
	cContainer();
	cContainer(uint32_t serial);

	bool addItem(pItem item, uint16_t xx=-1, uint16_t yy=-1);
	void insertItem(pItem itm);

	uint16_t getGump();

	uint32_t removeItems(uint32_t amount, uint16_t id, uint16_t color = 0);
	uint32_t removeItems(uint32_t delAmount, uint16_t matchId, uint16_t matchColor, bool recures = false);
	void dropItem(pItem pi);

	uint32_t countItems(uint16_t id, uint16_t color = 0, bool recurse = false);

	pItem findFirstType(uint16_t type, bool recurse = false);

	uint32_t countSpellsInSpellBook(bool stdOnly = true);
	bool containsSpell(magic::SpellId spellnum);
	
	const float getWeightActual();
} PACK_NEEDED;

#endif

