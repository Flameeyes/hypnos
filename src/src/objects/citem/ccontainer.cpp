/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Implementation of cContainer class
*/

#include "objects/citem/ccontainer.h"
#include "logsystem.h"
#include "basics.h"

cContainer::cContainer()
	: cItem()
{
}

cContainer::cContainer(uint32_t serial)
	: cItem(serial)
{
}

//! Gets the container's gump
uint16_t cContainer::getGump()
{
/*	CONTINFOMAP::iterator iter( contInfo.find( getId() ) );
	if( iter==contInfo.end() || iter->second==contInfoGump.end() )
		return 0x47;
	else
		return iter->second->second.gump;*/
	
	//!\todo Need to be wrote!
	
	return 0x47;
}

/*!
\brief try to find an item in the container to stack with and stack.
\param item item to pile
\return true if pileable [and piled]
\note after this function, item can be NULL
\todo cItem::refresh() missing ?
*/
bool cContainer::pileItem(pItem &item)
{
	for(ItemList::iterator it = items.begin(); it != items.end(); it++)
	{
		if ( ! *it ) continue;

		if ( ! (
			(*it)->isPileable() && item->isPileable() &&
			(*it)->getId() == item->getId() &&
			(*it)->getColor() == item->getColor()
		   )   )
			return false;

		if ( (*it)->getAmount() + item->getAmount() > 65535 )
		{
			item->setPosition( (*it)->getPosition().x, (*it)->getPosition().y, 9 );
			item->setAmount( (*it)->getAmount() + item->getAmount() - 65535 );
			(*it)->setAmount(65535);
//			item->refresh();
		} else {
			(*it)->setAmount( (*it)->getAmount() + item->getAmount() );
			item->Delete();
		}
//		(*it)->refresh();
	}
}

/*!
\brief Sets a random position for the given item in the container
\param item item to set the position for
\todo Missing CONTINFOMAP
*/
void cContainer::setRandPos(pItem item)
{
	Location p = item->getPosition();
	p.z = 9;

#if 0
	CONTINFOMAP::iterator iter( contInfo.find( pCont->getId() ) );
	if( iter==contInfo.end() || iter->second==contInfoGump.end()) {
		p.x = RandomNum(18, 118);
		p.y = RandomNum(50, 100);
		LogWarning("trying to put something INTO a non container, id=0x%X",pCont->getId() );
	} else {
		p.x = RandomNum(iter->second->second.upperleft.x, iter->second->second.downright.x);
		p.y = RandomNum(iter->second->second.upperleft.y, iter->second->second.downright.y);
	}
#else
	p.x = RandomNum(18, 118);
	p.y = RandomNum(50, 100);
#endif

	item->setPosition(p);
}

/*!
\brief Count items in container with given id
\author Flameeyes
\param matchId items' id
\param matchColor items' color (or 0xFFFF for all colors)
\param recurse If true, recurse into subcontainers
\return the number of items counted
*/
uint32_t cContainer::countItems(uint16_t matchId, uint16_t matchColor, bool recurse)
{
	uint32_t count = 0;

	for(ItemList::iterator it = items.begin(); it != items.end(); it++)
	{
		if ( ! *it ) {
			LogWarning("NULL item!");
			items.erase(it);
			continue;
		}

		if ( (*it)->getId() == matchId && ( matchColor == 0xFFFF || (*it)->getColor() == matchColor ) )
			count += (*it)->getAmount();

		if ( recurse && dynamic_cast<pContainer>(*it) )
			count += (dynamic_cast<pContainer>(*it))->countItems(matchId, matchColor, recurse);
	}

	return count;
}

/*!
\author Flameeyes
\param type item's type
\param recurse if true will recurse in all sub-containers
\return pointer to the first item found or NULL
\todo Restore type check
*/
pItem cContainer::findFirstType(uint16_t type, bool recurse)
{
	for(ItemList::iterator it = items.begin(); it != items.end(); it++)
	{
		if ( ! *it ) {
			LogWarning("NULL item!");
			items.erase(it);
			continue;
		}

#if 0
		if ( (*it)->getType() == type )
			return (*it);
#endif

		if ( recurse && dynamic_cast<pContainer>(*it) )
		{
			pItem ret = (dynamic_cast<pContainer>(*it))->findFirstType(type, true);
			if ( ret ) return ret;
		}
	}

	return NULL;
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
uint32_t cContainer::removeItems(uint32_t delAmount, uint16_t matchId, uint16_t matchColor, bool recures)
{
	uint32_t rest = amount;

	for(ItemList::iterator it = items.begin(); it != items.end(); it++)
	{
		if ( ! *it ) {
			LogWarning("NULL item!");
			items.erase(it);
			continue;
		}

		if ( (*it)->getId() == matchId && ( matchColor != 0xFFFF || matchColor == (*it)->getColor() ) )
			rest = (*it)->reduceAmount(rest);

		if ( recurse && rest && dynamic_cast<pContainer>(*it) )
			rest = (dynamic_cast<pContainer>(*it))->removeItems(delAmount, matchId, matchColor, true);

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
	int ser= pi->getSerial();
	vector<int32_t>::iterator it= ItemList.begin();

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
void cContainer::addItem(pItem item, uint16_t xx, uint16_t yy)
{
	if ( ! item )
		return;

	NxwSocketWrapper sw;
	sw.fillOnline( item );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
        {
		cPacketSendDeleteObj pk(item);
		sw->sendPacket(&pk);
        }

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
uint32_t cContainer::countSpellsInSpellBook(bool stdOnly)
{
	uint32_t spellcount=0;

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

/*!
\brief Gets the actual weight of the container, also with the sum of the item inside it
*/
const float cContainer::getWeightActual()
{
	float totalWeight = cItem::getWeightActual();
	
	for(ItemList::iterator iit = items.begin(); iit != items.end(); it++)
		totalWeight += (*iit)->getWeightActual();
	
	return totalWeight;
}

/*!
\author Flameeyes
\brief execute decay on the container
\param dontDelete Should be called by inherited classes to not delete the item.
\return true if decayed (so deleted), false else
\see cItem::doDecay()
*/
bool cContainer::doDecay(bool dontDelete = false)
{
	if ( ! cItem::doDecay(true) )
		return false;
	
	for ( ItemList::iterator it = items.begin(); it != items.end(); it++ )
	{
		(*it)->setContainer(NULL);
		(*it)->MoveTo( getPosition() );
		(*it)->setDecayTime();
		(*it)->Refresh();
	}
	
	Delete();
	return true;
}

/*!
\author Flameeyes
\brief Gets the first instrument found in the container
\return A pointer to the first instrument found or NULL if not found
*/
pItem cContainer::getInstrument()
{
	for ( ItemList::iterator it = items.begin(); it != items.end(); it++ )
		if ( (*it)->isInstument() )
			return (*it);
	
	return NULL;
}
