/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "objects/citem/cmulti.h"
#include "objects/cclient.h"

void cMulti::MoveTo(sLocation newloc)
{
	for(MultiItemSList::iterator it = items.begin(); it != items.end(); it++)
	{
		(*it)->item->MoveTo(newloc + (*it)->offset);
		(*it)->item->Refresh();
	}
	cItem::MoveTo(newloc);
	Refresh();
}
