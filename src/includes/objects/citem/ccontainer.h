/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
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
#include "objects/citem/cequippable.h"

/*!
\brief Container item
\note This class must be referred using cItem::toContainer() function
*/
class cContainer : virtual public cItem
{
//@{
/*!
\name Containers' Gumps' Map
*/
protected:
	//! Info about the gumps
	struct sContainerGump {
		uint16_t gump;		//!< Gump's id
		sPoint upperleft;	//!< Minimum position
		sPoint downright;	//!< Maximum position
	};
	
	//! Containers' gumps' info [index is Gump's ID]
	typedef std::map< uint16_t, sContainerGump > mapGumpsInfo;
	//! Containers' ID map [index is Item's ID]
	typedef std::map< uint16_t, mapGumpsInfo::iterator > mapContainerGumps;
	
	static mapGumpsInfo gumpinfos;	//!< Gumps' information
	static mapContainerGumps containers;
					//!< Containers' gumps
	
	static void loadContainersData();
//@}

protected:
	//! Items in the container
	ItemList	items;

	bool		pileItem(pItem &item);
	void		setRandPos(pItem Item);

	virtual uint16_t eventsNumber() const
	{ return evtCntMax; }
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
	
	pItem getInstrument() const;

	virtual bool doDecay(bool dontDelete = false);
	
	//! Events for containers
	enum {
		evtCntOnTakeItem = cEquippable::evtEqpMax,
		evtCntOnPutItem,
		evtCntMax
	};
	
};

#endif

