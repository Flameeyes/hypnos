/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file 
\brief Implementation of class cSerializable
*/

#include "objects/cserializable.h"

SerializableMap cSerializable::objects;

/*!
\brief Finds an object by its serial
\param serial Serial of the object
*/
pSerializable cSerializable::findBySerial(uint32_t serial)
{
	SerializableMap::iterator it = objects.find(serial);
	
	return it == objects.end() ? NULL : *it;
}

static pChar findCharBySerial(uint32_t serial)
{
	return dynamic_cast<pChar>(findBySerial(serial));
}

static pItem findItemBySerial(uint32_t serial)
{
	return dynamic_cast<pItem>(findBySerial(serial));
}

/*!
\brief Constructor with new serial

This constructor should be called when creating a new object, assigning
it a new serial number. This function calls the abstract function
getNewSerial() which needs to be implemented by the subclass.
*/
cSerializable::cSerializable()
{
	serial = getNewSerial();
	objects.insert( make_pair( serial, this ) );
}

/*!
\brief Constructor with given serial
\param givenSerial serial to use for the object

This constructor should be used when loading objects from load data,
never for other things!
*/
cSerializable::cSerializable(uint32_t givenSerial)
{
	serial = givenSerial;
	if ( objects.find(givenSerial) != objects.end() )
	{
		LogWarning("Tried to create a new object with an existing serial number %08x. Creating new serial...", givenSerial);
		serial = getNewSerial();
	}
	
	objects.insert( make_pair( serial, this ) );
}

/*!
\brief Destructor

The destructor take care of remove the object from the objects list.
*/
cSerializable::~cSerializable()
{
	SerializableMap::iterator it = objects.find(nSerial);
	if ( it != objects.end() )
		objects.erase(it);
}
