/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file 
\brief Declaration of class cSerializable
*/

#ifndef __CSERIALIZABLE_H__
#define __CSERIALIZABLE_H__

#include "common_libs.h"
#include "inlines.h"

/*!
\class cSerializable cserializable.h "objects/cserializable.h"
\brief Serializable object
\author Flameeyes (class skeleton and documentation)

This class is an abstraction of a serializable object, like an item or a char.
This was wrote becaues a body doesn't have a serial, but the char has, among
with cItem.

All the class who has a serial included in the main serial-stuff, should
use this class to register the instances.

The abstract method getNewSerial() \b must be implemented by the derived classes,
and \b must return an unique serial, so it should check if there's already an
object with the same serial.

\section about About Serials

UO's serials are unsigned 32-bit integers (ie: uint32_t), which represents what
the client is seeing in the window, so the charactes and non-static items.

The serials are not identical between items and characters, and there are also
some special serials:
	
	\li Serial #0: is an \b invalid serial, and it's used for example to 
	tell the client to stop attack a char.
	\li Serials between 1 and 0x3FFFFFFF: are characters' serials, and are
	used for all characters (PCs and NPCs)
	\li Serials between 0x4000000 and 0x7FFFFFFF: are items' serials, and
	are used for all the non-static items

The MSB (0x80000000) is not used for serial (and so the serials are actually
31-bit integers), but is used when sending data about an item to the client,
stating that the item has an amount (or, for corpses, the corpse id).

\see nPackets::Sent::ObjectInformation

\section when When use Serials?

Serials are used mainly to send packets to the UO Client (which in fact knows
only about serials), and with the scripts (we don't want the scripts to mess up
with the pointers, do we?).

Also, we need the serials to save nad restore the savegames, because obviously
the pointers aren't consistent between different runs.

Least time we need to use serials to store informations in the cItem::more
value for a small variable. This is used, for example, by the
cBoat::searchByPlank() function which search for the boat's serial in the more
attribute.

For any other use inside the emulator we should always use the pointers.

*/
class cSerializable
{
//@{
/*!
\name Searching functions
\brief Functions to search for a serializable instance

These functions allow sto find the pointer starting from a serial, and are used
usually when parsing data from the 'external' system (like savegames, scripts
or the network protocol).
The main function for this all is the cSerializable::findBySerial() function,
which is called by cSerializable::findCharBySerial() and
cSerializable::findItemBySerial() to get a specific type instance.
*/
private:
	static SerializableMap objects;	//!< Map of all serialized object
	
public:
	static pChar findCharBySerial(uint32_t serial);
	static pItem findItemBySerial(uint32_t serial);
	static pSerializable findBySerial(uint32_t serial);

	//! Tells if a serial is of a character
	inline static bool isCharSerial( uint32_t ser )
	{
		// We don't want to export these constants...
		static const uint32_t minCharSerial = 1;
		static const uint32_t maxCharSerial = 0x3FFFFFFF;
		return between(ser, minCharSerial, maxCharSerial);
	}
	
	//! Tells if a serial is of an item
	inline static bool isItemSerial( uint32_t ser )
	{
		static const uint32_t minItemSerial = 0x40000000;
		static const uint32_t maxItemSerial = 0x7FFFFFFF;
		return between(ser, minItemSerial, maxItemSerial);
	}
//@}

private:
	uint32_t serial;		//!< Serial of the object

public:
	cSerializable();
	cSerializable(uint32_t serial);
	virtual ~cSerializable();
	
	//! Gets the current serial
	inline const uint32_t getSerial() const
	{ return serial; }
	
protected:
	//! Gets the new serial for the object
	virtual uint32_t getNewSerial() = 0;
	
	void resetData();

//@{
/*!
\name Extras
\brief Functions and attributes which are shared by all the serializable objects

The members here defined are usually used by the UO Protocol, and so are related
to the serial, so are registered here also if aren't relative to the serializable
class.
*/
public:
	//! Gets the popup help for the instance
	virtual const std::string getPopupHelp() const = 0;
	
	//! Gets the location of the instance
	virtual sLocation getWorldLocation() const = 0;
//@}

//@{
/*!
\name Multi and Owners
*/
protected:
	pMulti multi;	//!< Multi where the object is
	pChar owner;	//!< Owner of the object

public:
	//! Returns the object's multi serial
	pMulti getMulti() const
	{ return multi; }

	//! Sets the multi of the object
	virtual void setMulti(pItem nmulti);
	
	//! Returns the object's owner
	pChar getOwner() const
	{ return owner; }
	
	//! Sets the object's owner
	virtual void setOwner(pChar nowner);
//@}

//@{
/*!
\name Operators
*/
public:
	inline bool operator> (const cSerializable& obj) const
	{ return serial >  obj.serial; }

	inline bool operator< (const cSerializable& obj) const
	{ return serial <  obj.serial; }

	inline bool operator>=(const cSerializable& obj) const
	{ return serial >= obj.serial; }

	inline bool operator<=(const cSerializable& obj) const
	{ return serial <= obj.serial; }

	inline bool operator==(const cSerializable& obj) const
	{ return serial == obj.serial; }

	inline bool operator!=(const cSerializable& obj) const
	{ return serial != obj.serial; }
//@}
};

#endif
