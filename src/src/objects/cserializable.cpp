/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
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
	SerializableMap::iterator it = objects.find(nSerial);
	
	return it == objects.end() ? NULL : *it;
}

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
	objects.insert( make_pair( serial, this ) );
}

cSerializable::~cSerializable()
{
	SerializableMap::iterator it = objects.find(nSerial);
	if ( it != objects.end() )
		objects.erase(it);
}
