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
#include "objects/cchar.h"
#include "objects/citem.h"
#include "logsystem.h"

SerializableMap cSerializable::objects;

/*!
\brief Finds an object by its serial
\param serial Serial of the object
*/
pSerializable cSerializable::findBySerial(uint32_t serial)
{
	SerializableMap::iterator it = objects.find(serial);
	
	return (it == objects.end()) ? NULL : (*it).second;
}

pChar cSerializable::findCharBySerial(uint32_t serial)
{
	return dynamic_cast<pChar>(findBySerial(serial));
}

pItem cSerializable::findItemBySerial(uint32_t serial)
{
	return dynamic_cast<pItem>(findBySerial(serial));
}

/*!
\brief Constructor with given serial
\param givenSerial serial to use for the object

This constructor should be used by all the serializable classes, using passing
it the getNewSerial() function when creating a new serial.

\note This function can't generate a new serial for duplicated serials, so
	call it with a surely new serial
*/
cSerializable::cSerializable(uint32_t givenSerial)
{
	serial = givenSerial;
	if ( objects.find(givenSerial) != objects.end() )
	{
		LogWarning("Tried to create a new object with an existing serial number %08x.", givenSerial);
		delete this;
		return;
	}
	
	objects.insert( std::make_pair( serial, this ) );
}

/*!
\brief Destructor

The destructor take care of remove the object from the objects list.
*/
cSerializable::~cSerializable()
{
	SerializableMap::iterator it = objects.find(getSerial());
	if ( it != objects.end() )
		objects.erase(it);
}
