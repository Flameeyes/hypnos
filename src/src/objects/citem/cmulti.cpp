/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "objects/citem/cmulti.h"
#include "objects/cclient.h"

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
		sPositionOffset off = (*it)->getPosition() - getPosition();
		(*it)->MoveTo(newloc + off);
		(*it)->Refresh();
	}
	
	cItem::MoveTo(newloc);
	Refresh();
}
