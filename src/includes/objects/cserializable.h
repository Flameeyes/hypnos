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

\todo move findPtrBySerial stuff here
*/
class cSerializable
{
//@{
/*!
\name Searching functions
*/
protected:
	static SerializableMap objects;
public:
	inline static pChar findCharBySerial(uint32_t serial)
	{ return dynamic_cast<pChar>(findBySerial(serial)); }
	
	static pItem findItemBySerial(uint32_t serial);
	{ return dynamic_cast<pItem>(findBySerial(serial)); }
	
	static pSerializable findBySerial(uint32_t serial);
//@}

protected:
	cSerializable();
	cSerializable(uint32_t serial);
public:
	~cSerializable();

protected:
	uint32_t	serial;		//!< serial of the object
	
	//! Gets the new serial for the object
	virtual uint32_t getNewSerial() = 0;
public:
	inline const uint32_t getSerial() const
	{ return serial; }
	
	void setSerial(const uint32_t &newSerial);

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
}

#endif
