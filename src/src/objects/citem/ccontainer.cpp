/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "ccontainer.h"

cContainer::cContainer(bool ser/*= true*/) : cItem(ser)
{
	ItemList.empty();
}

UI16 cContainer::getGump()
{
	CONTINFOMAP::iterator iter( contInfo.find( getId() ) );
	if( iter==contInfo.end() || iter->second==contInfoGump.end() )
		return 0x47;
	else 
		return iter->second->second.gump;

}

bool cContainer::pileItem( P_ITEM item)	// try to find an item in the container to stack with
{
	NxwItemWrapper si;
	si.fillItemsInContainer( this, false );
	for( si.rewind(); !si.isEmpty(); si++ )
	{
		P_ITEM pi=si.getItem();
		if(!ISVALIDPI(pi)) continue;

		if (!(pileable && item->pileable &&
			getId()==item->getId() &&
			getColor()==item->getColor() ))
			return false;	//cannot stack.

		if (amount+item->amount>65535)
		{
			item->setPosition( getPosition("x"), getPosition("y"), 9);
			item->amount=(amount+item->amount)-65535;
			amount=65535;
			item->Refresh();
		}
		else
		{
			amount+=item->amount;
			item->Delete();
		}
		Refresh();
		return true;
	}
	return false;

}

void cContainer::setRandPos(P_ITEM item)
{
	item->setPosition("z", 9);
	CONTINFOMAP::iterator iter( contInfo.find( pCont->getId() ) );
	if( iter==contInfo.end() || iter->second==contInfoGump.end()) {
		item->setPosition("x", RandomNum(18, 118));
		item->setPosition("y", RandomNum(50, 100));
		LogWarning("trying to put something INTO a non container, id=0x%X",pCont->getId() );
	}
	else {
		item->setPosition("x", RandomNum(iter->second->second.upperleft.x, iter->second->second.downright.x));
		item->setPosition("y", RandomNum(iter->second->second.upperleft.y, iter->second->second.downright.y));
	}
}

UI32 cContainer::countItems(UI32 scriptID, bool bAddAmounts/*= false*/)
{
	UI32 count= 0;
	vector<SI32>::iterator it= ItemList.begin();

	do
	{
		P_ITEM pi= pointers::findItemBySerial(*it);
		if( !ISVALIDPI(pi) )
		{
			LogWarning("item's serial not valid: %d", *it);
			continue;
		}

		if( bAddAmounts )
			count+= pi->amount;
		else
			count++;
	}
	while( ++it != ItemList.end() );

	return count;
}

UI32 cContainer::removeItems(UI32 scriptID, UI32 amount/*= 1*/)
{
	UI32 rest= amount;
	vector<SI32>::iterator it= ItemList.begin();

	do
	{
		P_ITEM pi= pointers::findItemBySerial(*it);
		VALIDATEPIR(pi, 0);

		if( pi->getScriptID()==scriptID )
			rest= pi->ReduceAmount(rest);

		if (rest<= 0)
			break;

	}
	while( ++it!=ItemList.end() );

	return rest;
}

/*!
\brief remove item from container but don't delete it from world
*/
void cContainer::dropitem(P_ITEM pi)
{
	int ser= pi->getSerial32();
	vector<SI32>::iterator it= ItemList.begin();

	do
	{
		if( *it==ser )	// item found
			ItemList.erase(it);
	}
	while( ++it!=ItemList.end() );
}

/*!
\brief Add item to container
\author Endymion
\param item the item to add
\param xx the x location or INVALID if use rand pos
\param yy the y location or INVALID if use rand pos
*/
bool cContainer::AddItem(pItem item, short xx, short yy)
{
	if ( ! item )
		return false;

	NxwSocketWrapper sw;
	sw.fillOnline( item );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
		SendDeleteObjectPkt(sw.getSocket(), item->getSerial32() );


	if (xx!=-1)	// use the given position
	{
		item->setContainer( this );
		item->setPosition(xx, yy, 9);
	}
	else		// no pos given
	{
		if( !ContainerPileItem(item) )	{ // try to pile
			setRandPos(item);			// not piled, random pos
			item->setContainer(this);
		}
		else
			return true; //Luxor: we cannot do a refresh because item was piled
	}
	item->Refresh();
	return true;
}


