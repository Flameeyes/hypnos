/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "objects/citem/cmulti.h"
#include "objects/cchar.h"
#include "networking/cclient.h"

/*!
\brief Moves a multi, with all its content

This function is an overloaded version of cItem::MoveTo() function, which
moves the multi with all the items and chars that it has inside.
*/
void cMulti::MoveTo(sLocation newloc)
{
	for(MultiItemSList::iterator it = items.begin(); it != items.end(); it++)
	{
		(*it)->item->MoveTo(newloc + (*it)->offset);
		(*it)->item->Refresh();
	}
	
	for(CharSList::iterator it = chars.begin(); it != chars.end(); it++)
	{
		sPositionOffset off = (*it)->getBody()->getPosition() - getPosition();
		(*it)->MoveTo(newloc + off);
		(*it)->Refresh();
	}
	
	cItem::MoveTo(newloc);
	Refresh();
}

/*!
\brief Deletes a multi object

This function reimplements cItem::Delete() for multis, cleaning up the items
and chars lists (removing the items and releasing the chars).

cHouse reimplements this one more time deleting the npc vendors.
*/
void cMulti::Delete()
{
	for(MultiItemSList::iterator it = items.begin(); it != items.end(); it++)
		delete *it;
	
	for(CharSList::iterator it = chars.begin(); it != chars.end(); it++)
		(*it)->setMulti(NULL);
		
	cItem::Delete();
}

/*!
\brief Gets the actual area occupied by the multi
*/
const sRect cMulti::getArea() const
{
	return sRect(area.ul + sPositionOffset(getPosition().x, getPosition().y), area.br + sPositionOffset(getPosition().x, getPosition().y) );
}

/*!
\brief Adds an object to the list of multi's objects
\param obj Object to add

This function get the right type for the object (item or char) and then add it
to the right list of objects.
If the object is an item, adds it as a non-required object and stores its 
offset.
*/
void cMulti::add(pSerializable obj)
{
	pChar pc = dynamic_cast<pChar>(obj);
	pItem pi = dynamic_cast<pItem>(obj);

	// At least one should be valid
	if ( ! pc && ! pi ) return;
	
	if ( pc )
	{
		chars.push_front(pc);
	} else {
		sMultiItem mi;
		mi.item = pi;
		mi.offset = pi->getPosition() - getPosition();
		mi.required = false;
		items.push_front(mi);
	}
}

/*!
\brief Removes an object from the list of multi's objects
\param obj Object to remove
\retval true The multi was deleted because obj was a required item
\retval false The multi wasn't deleted, the obj was removed from the lists

This funciton get the right type for the object (item or char) and then tries
to remove it from the lists.

If the object is a required item, the multi is also going destroy itself
calling cMulti::Delete() function.
*/
bool cMulti::remove(pSerializable obj)
{
	pChar pc = dynamic_cast<pChar>(obj);
	pItem pi = dynamic_cast<pItem>(obj);
	
	// At least one should be valid
	if ( ! pc && ! pi ) return false;
	
	if ( pc )
	{
		CharSList::iterator it = find(chars.begin(), chars.end(), pc);
		if ( it != chars.end() )
			chars.erase(it);
	} else {
		MultiItemSList::iterator it = find(items.begin(), items.end(), pi);
		if ( it == items.end() )
			return false;
		
		if ( (*it)->required )
		{
			Delete();
			return true;
		}
		
		items.erase(it);
		return false;
	}
	
	return false;
}

/*!
\brief Search for a multi in the given point of the map
\param p Coords of the point to check for multi
\return The pointer to the multi in the place, or NULL if not found
*/
pMulti cMulti::getAt(sPoint p)
{
	for( ; ; ) //!\todo Need to be changed when the new region stuff is done
	{
		if( !(*it) ) continue;
		
		if ( (*it)->getArea().isInside(p) )
			return (*it);
	}
	return NULL;
}
