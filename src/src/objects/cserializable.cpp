/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "objects/cserializable.h"
#include "objects/cchar.h"
#include "objects/citem.h"
#include "objects/citem/cmulti.h"
#include "logsystem.h"

SerializableMap cSerializable::objects;

/*!
\brief Finds an object by its serial
\param serial Serial of the object
*/
pSerializable cSerializable::findBySerial(uint32_t serial)
{
	/*! \note This function checks if serial has the MSB set, and if so
	 *  returns null without check into the cSerializable::objects map.
	 */
	if ( serial & 0x80000000 ) return NULL;
	SerializableMap::iterator it = objects.find(serial);
	
	return (it == objects.end()) ? NULL : (*it).second;
}

pChar cSerializable::findCharBySerial(uint32_t serial)
{
	/*! \note The function calls isCharSerial() function to check if the
	 *  given serial is valid, this is a quicker check than a search
	 *  into the map and a dynamic_cast to NULL if item (or invalid) serial
	 *  :)
	 */
	if ( ! isCharSerial(serial) ) return NULL;
	return dynamic_cast<pChar>(findBySerial(serial));
}

pItem cSerializable::findItemBySerial(uint32_t serial)
{
	/*! \note The function calls isItemSerial() function to check if the
	 *  given serial is valid, this is a quicker check than a search
	 *  into the map and a dynamic_cast to NULL if char (or invalid) serial
	 *  :)
	 */
	if ( ! isItemSerial(serial) ) return NULL;
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
	
	// We must set them here because we can't use directly
	// the setMulti() and setOwner() functions of resetData()
	// else we can try to dereference a dirt pointer.
	multi = NULL;
	owner = NULL;
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

/*!
\brief Resets the data inside the instance of the class

This function is similar to cObject::resetData() and is called by the two
cItem::resetData() and cChar::resetData() to reset the data derived from the
serialization of the object iself.
*/
void cSerializable::resetData()
{
	setMulti(NULL);
	setOwner(NULL);
}

/*!
\brief Sets the object inside the given multi
\param nMulti New multi to set (if NULL, remove from multi)

This function calls the cMulti::remove() function to remove itself if already
in a multi and then cMulti::add() function to add itself if nMulti is not NULL.
*/
void cSerializable::setMulti(pMulti nMulti)
{
	if ( multi )
		multi->remove(this);

	if ( nMulti )
		nMulti->add(this);

	multi = nMulti;
}
