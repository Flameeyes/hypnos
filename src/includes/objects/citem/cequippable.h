/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file 
\brief Declaration of class cEquippable
*/

#ifndef __CEQUIPPABLE_H__
#define __CEQUIPPABLE_H__

#include "common_libs.h"
#include "objects/citem.h"

/*!
\brief Class for equippable items

This class is used to add support for equippable items, like weapons,
shields, armors and clothes.
This is an abstract class, thus no direct instance of this is possible.
Some special classes (like bankboxes and backpacks) can derive both from this
and from cContainer class.
*/
class cEquippable : virtual public cItem
{
public:
	cEquippable();
	cEquippable(uint32_t newSerial);
	virtual ~cEquippable();

	virtual void setContainer(pObject obj);
	
	inline const Layer getLayer() const
	{ return layer; }
	
	void doubleClicked(pClient client);

protected:
	Layer layer;
	static Layer findLayerFromID(uint16_t id);
};

#endif
