/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __CMULTI_H__
#define __CMULTI_H__

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
public:
	cMulti();
	cMulti(uint32_t nserial);
};

#endif
