/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __OBJECTS_CITEM_CMULTI_H__
#define __OBJECTS_CITEM_CMULTI_H__

#include "common_libs.h"
#include "objects/citem.h"

/*!
\class cMulti cmulti.h "objects/citem/cmulti.h"
\brief Multi item

This class represent and manages the 'multis': items which are represented
in game as a structure of many items.
In this category we have the houses (cHouse) and the boats (cBoat).
*/
class cMulti : public cItem
{
protected:
	/*!
	\brief Item in multi
	
	This structure represent an item present in the multi (like a door
	or the sign), which \b must be moved with the multi itself.
	*/
	struct sMultiItem {
		pItem item;		//!< Pointer to the item instance
		sPositionOffset offset;	//!< Offset of the item from the center of the multi
		bool required;		//!< True if the item is a fundamnetal component of the multi
					//!< i.e.: the multi should be deleted if this item is deleted
	};
	
	//! A singly-linked list of multi items
	typedef slist<sMultiItem> MultiItemSList;
	
	//! The items component of the multi
	MultiItemSList items;
	//! The chars which are inside the multi
	CharSList chars;
	
	sRect area;			//!< Area of the multi (region occupied by it)
public:
	cMulti();
	cMulti(uint32_t nserial);
	
	void MoveTo(sLocation newloc);
	
	void add(pSerializable obj);
	bool remove(pSerializable obj);
	
	const sRect &getArea() const;
	
	static pMulti getAt(sPoint p);
	
	virtual void Delete();
};

#endif
