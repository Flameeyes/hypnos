/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file 
\brief Declaration of class cSerializable
*/

#ifndef __CSERIALIZABLE_H__
#define __CSERIALIZABLE_H__

#include "common_libs.h"

/*!
\brief Serializable object

This class is an abstraction of a serializable object, like an item or a char.
This was wrote becaues a body doesn't have a serial, but the char has, among
with cItem.

All the class who has a serial included in the main serial-stuff, should
use this class to register the instances.

The abstract method getNewSerial() \b must be implemented by the derived classes,
and \b must return an unique serial, so it should check if there's already an
object with the same serial.
*/
class cSerializable
{
//@{
/*!
\name Searching functions
*/
private:
	static SerializableMap objects;	//!< Map of all serialized object
	uint32_t	serial;		//!< Serial of the object
public:
	static pChar findCharBySerial(uint32_t serial);
	static pItem findItemBySerial(uint32_t serial);
	static pSerializable findBySerial(uint32_t serial);
//@}

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

//@{
/*!
\name Operators
*/
public:
	inline bool operator> (const cSerializable& obj) const
	{ return getSerial() >  obj.getSerial(); }

	inline bool operator< (const cSerializable& obj) const
	{ return getSerial() <  obj.getSerial(); }

	inline bool operator>=(const cSerializable& obj) const
	{ return getSerial() >= obj.getSerial(); }

	inline bool operator<=(const cSerializable& obj) const
	{ return getSerial() <= obj.getSerial(); }

	inline bool operator==(const cSerializable& obj) const
	{ return getSerial() == obj.getSerial(); }

	inline bool operator!=(const cSerializable& obj) const
	{ return getSerial() != obj.getSerial(); }
//@}
};

#endif
