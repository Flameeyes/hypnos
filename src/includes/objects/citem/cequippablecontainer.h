/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file 
\brief Declaration of class cEquippableContainer
*/

#ifndef __CEQUIPPABLECONTAINER_H__
#define __CEQUIPPABLECONTAINER_H__

#include "common_libs.h"
#include "objects/citem/ccontainer.h"
#include "objects/citem/cequippable.h"

/*!
\brief Class for equippable containers

This class is used to add support for equippable containers like backpacks and
bank boxes.
*/
class cEquippableContainer : public cContainer, public cEquippable
{
public:
	cEquippableContainer();
	cEquippableContainer(uint32_t newSerial);
	virtual ~cEquippableContainer();

protected:
};

#endif
