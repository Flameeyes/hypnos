/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __CCONTAINER_H__
#define __CCONTAINER_H__

#include "common_libs.h"
#include "objects/citem.h"
#include "objects/citem/cequippable.h"
#include <wefts_mutex.h>

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
	typedef map< uint16_t, sContainerGump > mapGumpsInfo;
	//! Containers' ID map [index is Item's ID]
	typedef map< uint16_t, mapGumpsInfo::iterator > mapContainerGumps;
	
	static mapGumpsInfo gumpinfos;	//!< Gumps' information
	static mapContainerGumps containers;
					//!< Containers' gumps
	
	static void loadContainersData();
//@}

protected:
	virtual uint16_t eventsNumber() const
	{ return evtCntMax; }
public:
	cContainer();
	cContainer(uint32_t serial);
	
	virtual void doubleClicked(pClient);

//@{
/*!
\name Container specific stuff
*/
protected:
	ItemSList items;		//!< List of items in the container
	Wefts::Mutex itemsMutex;	//!< Mutex for the items' list

	bool		pileItem(pItem &item);
	void		setRandPos(pItem Item);
public:
	bool addItem(pItem item, sPoint pos = sPoint(0xFFFF, 0xFFFF));
	void insertItem(pItem itm);

	uint16_t getGump();

	uint32_t removeItems(uint32_t amount, uint16_t id, uint16_t color = 0);
	uint32_t removeItems(uint32_t delAmount, uint16_t matchId, uint16_t matchColor, bool recures = false);
	void dropItem(pItem pi);

	uint32_t countItems(uint16_t id, uint16_t color = 0, bool recurse = false);
	
	/*!
	\brief Find the first item of the given id
	\param id ID of the item to search for
	\param recurse If true, search in subcontainers
	*/
	pItem findItem(const string &id, bool recurse = false);
	pItem findFirstType(uint16_t type, bool recurse = false);

	//! Gets the items' list
	const ItemSList &getItems() const
	{ return items; }
	
	//! Locks the mutex for items' list to avoid concurrency
	void lockItemsMutex()
	{ itemsMutex.lock(); }
	
	//! Unlocks the mutex for items' list to resume concurrency
	void unlockItemsMutex()
	{ itemsMutex.unlock(); }
//@}

//@{
/*!
\name Special search stuff
\brief Functions used by skills stuff or similar.

The functions in this section are used by skills or script to find out specific
items, like empty maps, instruments and so on.

They were implemented as non-member functions, but they needed to lock the
items' list from outside while searching so it wasn't a good choice. The new
member functions are now used instead of the old ones.
*/
public:
	pMap getEmptyMap(bool recurse = false) const;
	pItem getInstrument(bool recurse = false) const;
//@}

	uint32_t countSpellsInSpellBook(bool stdOnly = true);
	bool containsSpell(SpellId spellnum);
	
	float getWeightActual();
	
	virtual bool doDecay(bool dontDelete = false);
	
	//! Events for containers
	enum {
		evtCntOnTakeItem = cEquippable::evtEqpMax,
		evtCntOnPutItem,
		evtCntMax
	};
	
};

#endif
