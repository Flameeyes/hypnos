/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "logsystem.hpp"
#include "archs/tinterface.hpp"
#include "objects/citem/ccontainer.hpp"
#include "objects/citem/cmap.hpp"

#include <mxml.h>

cContainer::mapGumpsInfo cContainer::gumpinfos;
cContainer::mapContainerGumps cContainer::containers;

/*!
\brief Loads the containers' gumps' data

This funciton replaces the old loadcontainers() function which loaded from
containers.xss, instead reads from containers.xml.
*/
void cContainer::loadContainersData()
{
	outPlain("Loading constants information...\t\t");
	
	ifstream xmlfile("config/containers.xml");
	try {
		MXML::Document doc(xmlfile);
		
		MXML::Node *n = doc.main()->child();
		do {
			if ( n->name() != "gump" )
			{
				LogWarning("Unknown element in containers.xml: %s", n->name().c_str());
				continue;
			}
			
			try {
				sContainerGump g;
				g.gump = cVariant( n->getAttribute("id") ).toUInt16();
				g.upperleft.x = cVariant( n->getAttribute("x1") ).toUInt16();
				g.upperleft.y = cVariant( n->getAttribute("y1") ).toUInt16();
				g.downright.x = cVariant( n->getAttribute("x2") ).toUInt16();
				g.downright.y = cVariant( n->getAttribute("y2") ).toUInt16();
				
				gumpinfos[g.gump] = g;
				
				MXML::Node *id = n->child();
				do {
					if ( id->name() != "id" )
					{
						LogWarning("Unknown element in containers.xml: %s", id->name().c_str());
						continue;
					}
					
					uint16_t valId = cVariant( id->data() ).toUInt16();
					if ( valId )
						containers[valId] = id;
				} while ( (id = id->next() ) );
				
			} catch ( MXML::NotFoundError e ) {
				LogWarning("Incomplete element in containers.xml");
			}
			
		} while( (n = n->next()) );
		outPlain("[   OK   ]\n");
	} catch ( MXML::MalformedError e) {
		outPlain("[ Failed ]\n");
		LogCritical("containers.xml file not well formed.");
	}
}

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
	mapContainerGumps::iterator iter = containers.find( getId() );
	if ( iter == containers.end() || iter->second == gumpinfos.end() )
		return 0x47;
	else
		return iter->second->second.gump;
	
	return 0x47;
}

/*!
\brief try to find an item in the container to stack with and stack.
\param[in,out] item item to pile (note: it must already be in the container)
\retval true The item was pilable, so it was piled
\retval false The item wasn't pilable or wasn't possible to pile it
\note after this function, item can be NULL
\todo cItem::refresh() missing ?
*/
bool cContainer::pileItem(pItem &item)
{
	// Let's check if the item is already in this container
	if ( item->getContainer() != this ) return false;
	
	for(ItemSList::iterator it = items.begin(); it != items.end(); it++)
	{
		if ( ! *it || *it == item || !(*it)->isCombinableWith(item) )
			continue;

		if ( (*it)->getAmount() + item->getAmount() > 65535 )
		{
			setRandPos(item);
			item->setAmount( (*it)->getAmount() + item->getAmount() - 65535 );
			(*it)->setAmount(65535);
			item->refresh();
			return true;
		} else {
			(*it)->setAmount( (*it)->getAmount() + item->getAmount() );
			item->Delete();
			return true;
		}
		(*it)->refresh();
	}
        return false;	//if it exits the for without having found a combinable item, we can report the failure
}

/*!
\brief Sets a random position for the given item in the container
\param item item to set the position for
*/
void cContainer::setRandPos(pItem item)
{
	sLocation p = item->getPosition();
	p.z = 9;

	mapContainerGumps::iterator iter = containers.find( getId() );
	if ( iter == containers.end() || iter->second == gumpinfos.end() ) {
		p.x = RandomNum(18, 118);
		p.y = RandomNum(50, 100);
		LogWarning("trying to put something INTO a non container, id=0x%X", getId() );
	} else {
		p.x = RandomNum(iter->second->second.upperleft.x, iter->second->second.downright.x);
		p.y = RandomNum(iter->second->second.upperleft.y, iter->second->second.downright.y);
	}
	
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

	for(ItemSList::iterator it = items.begin(); it != items.end(); it++)
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
	for(ItemSList::iterator it = items.begin(); it != items.end(); it++)
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
uint32_t cContainer::removeItems(uint32_t delAmount, uint16_t matchId, uint16_t matchColor, bool recurse)
{
	uint32_t rest = amount;

	for(ItemSList::iterator it = items.begin(); it != items.end(); it++)
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

	ItemSList::iterator it = items.find(pi);

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
		nPackets::Sent::DeleteObj pk(item);
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
\author Flameeyes
\param stdOnly If true, count only standard spells
\return The number of spells in the spellbook
\note This function is here, because a spellbook is handled like a container
	with spells as items inside it
\todo Improve stdOnly support
\todo Move this on cSpellbook class when done
*/
uint32_t cContainer::countSpellsInSpellBook(bool stdOnly)
{
	uint32_t spellcount=0;

	for(ItemSList::iterator it = items.begin(); it != items.end(); it++)
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
\author Flameeyes
\brief Check if a spellbook contains a given spell
\param spellnum Spell identifier
\return true if the spell is present
\todo not using hardcoded ids... in this case we are
	using an extended spellbook... that can have
	only the base spells...
\todo Move this on cSpellbook class when done
*/
bool cContainer::containsSpell(SpellId spellnum)
{
	bool raflag = false;

	if (spellnum==magic::spellReactiveArmour)
		raflag = true;
	if ((spellnum>=magic::spellClumsy) && (spellnum < magic::spellReactiveArmour))
			spellnum = static_cast<SpellId>(static_cast<int>(spellnum)+1);
	if (raflag)
		spellnum=static_cast<SpellId>(0);

	for(ItemSList::iterator it = items.begin(); it != items.end(); it++)
	{
		if ( (*it)->getId() == ( 0x1F2D + spellnum ) || (*it)->getId() == 0x1F6D )
			return true;
	}

	return false;
}

/*!
\brief Adds an item to the internal items list
\param itm Item to add
\note Should be called \b only by cItem::setContainer() function
*/
void cContainer::insertItem(pItem itm)
{
	if ( find(itm, items.begin(), items.end()) != items.end() )
		return;
	items.push_front(itm);
}

/*!
\brief Gets the actual weight of the container, also with the sum of the item inside it
*/
const float cContainer::getWeightActual()
{
	float totalWeight = cItem::getWeightActual();
	
	for(ItemSList::iterator it = items.begin(); it != items.end(); it++)
		totalWeight += (*it)->getWeightActual();
	
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
	
	for ( ItemSList::iterator it = items.begin(); it != items.end(); it++ )
	{
		(*it)->setContainer(NULL);
		(*it)->MoveTo( getPosition() );
		(*it)->setDecayTime();
		(*it)->Refresh();
	}
	
	Delete();
	return true;
}

void cContainer::doubleClicked()
{
	// Wintermute: GMs or Counselors should be able to open trapped containers always
	if (moreb1 > 0 && !pc->IsGMorCounselor()) {
		magic::castAreaAttackSpell(getPosition(), magic::spellExplosion);
		moreb1--;
	}
	//Magic->MagicTrap(currchar[s], pi); // added by AntiChrist
	// only 1 and 63 can be trapped, so pleaz leave it here :) - Anti
}

/*!
\author Flameeyes
\brief Gets the first instrument found in the container
\param recurse If true will search also in subcontainers
\return A pointer to the first instrument found or NULL if not found

This function is called by all the musicianship-related functions inside the
nSkills namespace.
*/
pItem cContainer::getInstrument(bool recurse = false) const
{
	for ( ItemSList::const_iterator it = items.begin(); it != items.end(); it++ )
	{
		if ( (*it)->isInstument() )
			return (*it);
		
		pContainer cont = NULL; pItem itm = NULL;
		if (	recurse &&
			(cont = dynamic_cast<pContainer>(*it)) &&
			(itm = cont->getInstrument(recurse)) )
				return itm;
	}
	
	return NULL;
}

/*!
\author Flameeyes
\brief Searches for an empty map
\param recurse If true will search also in subcontainers
\return Pointer to the first empty map found or NULL if not found

This function is called by nSkills::Cartography() function.
*/
pMap cContainer::getEmptyMap(bool recurse = false)
{
	for ( ItemSList::const_iterator it = items.begin(); it != items.end(); it++ )
	{
		pMap m = dynamic_cast<pMap>(*it);
		if ( m && m->isBlankMap() )
			return m;
		
		pContainer cont = NULL; pItem itm = NULL;
		if (	recurse &&
			(cont = dynamic_cast<pContainer>(*it)) &&
			(itm = cont->getEmptyMap(recurse)) )
				return itm;
	}
	
	return NULL;
}
