/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Implementation of cContainer class
*/

#include "ccontainer.h"

cContainer::cContainer(bool ser/*= true*/) : cItem(ser)
{
}

//! Gets hte container's gump
UI16 cContainer::getGump()
{
	CONTINFOMAP::iterator iter( contInfo.find( getId() ) );
	if( iter==contInfo.end() || iter->second==contInfoGump.end() )
		return 0x47;
	else
		return iter->second->second.gump;

}

/*!
\brief try to find an item in the container to stack with and stack.
\param item item to pile
\return true if pileable [and piled]
\note after this function, item can be NULL
*/
bool cContainer::pileItem(pItem &item)
{
	for(ItemList::iterator it = items.begin(); it != items.end(); it++)
	{
		if ( ! *it ) continue;

		if ( ! (
			(*it)->pileable && item->pileable &&
			(*it)->getId() == item->getId() &&
			(*it)->getColor() == item->getColor()
		   )   )
			return false;

		if ( (*it)->getAmount() + item->getAmount() > 65535 )
		{
			item->setPosition( (*it)->getPosition().x, (*it)->getPosition().y, 9 );
			item->setAmount( (*it)->getAmount() + item->getAmount() - 65535 );
			(*it)->setAmount(65535);
			item->refresh();
		} else {
			(*it)->setAmount( (*it)->getAmount() + item->getAmount() );
			item->Delete();
		}
		(*it)->refresh();
	}
}

/*!
\brief Sets a random position for the given item in the container
\param item item to set the position for
*/
void cContainer::setRandPos(pItem item)
{
	Location p = item->getPosition();
	p.z = 9;

	CONTINFOMAP::iterator iter( contInfo.find( pCont->getId() ) );
	if( iter==contInfo.end() || iter->second==contInfoGump.end()) {
		p.x = RandomNum(18, 118);
		p.y = RandomNum(50, 100);
		LogWarning("trying to put something INTO a non container, id=0x%X",pCont->getId() );
	} else {
		p.x = RandomNum(iter->second->second.upperleft.x, iter->second->second.downright.x);
		p.y = RandomNum(iter->second->second.upperleft.y, iter->second->second.downright.y);
	}

	item->setPosition(p);
}

/*!
\brief Count items in container with given scriptID
\param scriptID scriptid of items to count
\param total if true, the returned value are the total amount of items
	else, it will be the number of instances of the items
\return the number of items counted (see total parameter)
*/
UI32 cContainer::countItems(UI32 scriptID, bool total/*= false*/)
{
	UI32 count = 0;

	for(ItemList::iterator it = items.begin(); it != items.end(); it++)
	{
		if ( ! *it ) {
			LogWarning("NULL item!");
			items.erase(it);
			continue;
		}

		if ( (*it)->getScriptID() != scriptID )
			continue;

		if ( total )
			count += (*it)->getAmount();
		else
			count++;
	}

	return count;
}

/*!
\brief Count items in container with given scriptID
\author Flameeyes
\param matchId items' id
\param matchColor items' color (or 0xFFFF for all colors)
\param recurse If true, recurse into subcontainers
\return the number of items counted
*/
UI32 cContainer::countItems(UI16 matchId, UI16 matchColor, bool recurse)
{
	UI32 count = 0;

	for(ItemList::iterator it = items.begin(); it != items.end(); it++)
	{
		if ( ! *it ) {
			LogWarning("NULL item!");
			items.erase(it);
			continue;
		}

		if ( (*it)->getId() == matchId && ( matchColor == 0xFFFF || (*it)->getColor() == matchColor ) )
			count += (*it)->getAmount();

		if ( recurse && (*it)->rtti() == rtti::cContainer )
			count += (reinterpret_cast<pContainer>(*it))->countItems(matchID, matchColor, recurse);
	}

	return count;
}

/*!
\author Flameeyes
\param type item's type
\param recurse if true will recurse in all sub-containers
\return pointer to the first item found or NULL
*/
pItem cContainer::findFirstType(UI16 type, bool recurse = false)
{
	for(ItemList::iterator it = items.begin(); it != items.end(); it++)
	{
		if ( ! *it ) {
			LogWarning("NULL item!");
			items.erase(it);
			continue;
		}

		if ( (*it)->getType() == type )
			return (*it);

		if ( recurse && (*it)->rtti() == rtti::cContainer )
		{
			pItem ret = (reinterpret_cast<pContainer>(*it))->findFirstType(type, true);
			if ( ret ) return ret;
		}
	}

	return NULL;
}

/*!
\author Flameeyes
\brief Removes the given amount of items with given scriptID
\param scriptID scriptID of the items to remove
\param amount amount of items to remove
\param recurse if true will recures in all sub-containers
\return how many items can't be removed (because not present)
*/
UI32 cContainer::removeItems(UI32 scriptID, UI32 delAmount, bool recurse)
{
	UI32 rest = delAmount;

	for(ItemList::iterator it = items.begin(); it != items.end(); it++)
	{
		if ( ! *it ) {
			LogWarning("NULL item!");
			items.erase(it);
			continue;
		}

		if ( (*it)->getScriptID() == scriptID )
			rest = (*it)->reduceAmount(rest);

		if ( recurse && rest && (*it)->rtti() == rtti::cContainer )
			rest = (reinterpret_cast<pContainer>(*it))->removeItems(scriptID, delAmount, true);

		if ( rest == 0 )
			break;
	}

	return rest;
}

/*!
\author Flameeyes
\brief Removes the give amount of item with given id and color (if not invalid)
\param delAmount amount of items to remove
\param matchId id to match for the remove
\param matchColor color to match for the remove (if not invalid 0xFFFF)
\param recurse if true, it will recurse in all the sub-containers
\return how many items can't be removed (because not presents)
*/
UI32 cContainer::removeItems(UI32 delAmount, UI16 matchId, UI16 matchColor, bool recures)
{
	UI32 rest = amount;

	for(ItemList::iterator it = items.begin(); it != items.end(); it++)
	{
		if ( ! *it ) {
			LogWarning("NULL item!");
			items.erase(it);
			continue;
		}

		if ( (*it)->getId() == matchId && ( matchColor != 0xFFFF || matchColor == (*it)->getColor() ) )
			rest = (*it)->reduceAmount(rest);

		if ( recurse && rest && (*it)->rtti() == rtti::cContainer )
			rest = (reinterpret_cast<pContainer>(*it))->
				removeItems(delAmount, matchId, matchColor, true);

		if ( rest == 0 )
			break;
	}

	return rest;
}

/*!
\brief remove item from container but don't delete it from world
\param item item to drop
\note This function doesn't put the item in world...
*/
void cContainer::dropitem(pItem pi)
{
	int ser= pi->getSerial32();
	vector<SI32>::iterator it= ItemList.begin();

	ItemList::iterator it = items.find(pi);

	if ( it != items.end() )
		items.erase(it);
}

/*!
\brief Add item to container
\author Endymion
\param item the item to add
\param xx the x location or UINVALID16 if use rand pos
\param yy the y location or UINVALID16 if use rand pos
*/
void cContainer::addItem(pItem item, UI16 xx, UI16 yy)
{
	if ( ! item )
		return;

	NxwSocketWrapper sw;
	sw.fillOnline( item );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
		SendDeleteObjectPkt(sw.getSocket(), item->getSerial32() );

	item->setContainer(this);
	if ( xx != 0xFFFF )
		item->setPosition(xx, yy, 9);
	else if ( ! pileItem(&item) )
		setRandPos(item);

	if ( item )
		item->refresh();
}

/*!
\brief Count spells
\author Flameeyes (based on Xanathar)
\param stdOnly If true, count only standard spells
\return The number of spells in the spellbook
\note This function is here, because a spellbook is handled like a container
	with spells as items inside it
\todo Improve stdOnly support
*/
UI32 cContainer::countSpellsInSpellBook(bool stdOnly)
{
	UI32 spellcount=0;

	for(ItemList::iterator it = items.begin(); it != items.end(); it++)
	{
		if ( ! *it ) {
			LogWarning("NULL item!");
			items.erase(it);
			continue;
		}

		if ( (*it)->getId() == 0x1F6D ) //! \todo Should be constantized?
		{
			if ( stdOnly )
				return 64;
			else
				spellcount += 64;
		} else
			spellcount++;
	}

	if ( stdOnly && spellcount > 64 )
		return 64;
	else
		return spellcount;
}

/*!
\author Flameeyes (based on Xanathar)
\brief Check if a spellbook contains a given spell
\param spellnum Spell identifier
\return true if the spell is present
\todo not using hardcoded ids... in this case we are
	using an extended spellbook... that can have
	only the base spells...
*/
bool cContainer::containsSpell(magic::SpellId spellnum)
{
	bool raflag = false;

	if (spellnum==magic::SPELL_REACTIVEARMOUR)
		raflag = true;
	if ((spellnum>=magic::SPELL_CLUMSY) && (spellnum < magic::SPELL_REACTIVEARMOUR))
			spellnum = static_cast<magic::SpellId>(static_cast<int>(spellnum)+1);
	if (raflag)
		spellnum=static_cast<magic::SpellId>(0);

	for(ItemList::iterator it = items.begin(); it != items.end(); it++)
	{
		if ( (*it)->getId() == ( 0x1F2D + spellnum ) || (*it)->getId() == 0x1F6D )
			return true;
	}

	return false;
}

/*!
\brief Adds an item to the internal items list
\param itm Item to add
\note Should be called <b>only</b> by cItem::setContainer() function
*/
void cContainer::insertItem(pItem itm)
{
	if ( items.find(itm) != items.end() )
		return;
	items.push_back(itm);
}
