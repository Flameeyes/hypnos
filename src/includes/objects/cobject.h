/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file object.h
\brief Declaration of class cObject
\author Anthalir
*/

#ifndef __OBJECT_H
#define __OBJECT_H

class cObject;
typedef cObject *pObject;

#include <stdarg.h>
#include "constants.h"
#include "typedefs.h"
#include "tmpeff.h"
#include "basics.h"

class cScpIterator;


typedef slist< tempfx::cTempfx > TempfxVector;

//! == operator redefinition for Location
inline bool operator ==(Location a, Location b)
{ return ((a.x==b.x) && (a.y==b.y) && (a.z==b.z)); }

//! != operator redefinition for Location
inline bool operator !=(Location a, Location b)
{ return ((a.x!=b.x) || (a.y!=b.y) || (a.z!=b.z)); }

/*!
\brief Base class of cItem and cChar
\author Anthalir
\since 0.82a
*/
class cObject
{
//@{
/*!
\name Operators
*/
public:
	inline bool operator> (const cObject& obj) const
	{ return(getSerial() >  obj.getSerial()); }

	inline bool operator< (const cObject& obj) const
	{ return(getSerial() <  obj.getSerial()); }

	inline bool operator>=(const cObject& obj) const
	{ return(getSerial() >= obj.getSerial()); }

	inline bool operator<=(const cObject& obj) const
	{ return(getSerial() <= obj.getSerial()); }

	inline bool operator==(const cObject& obj) const
	{ return(getSerial() == obj.getSerial()); }

	inline bool operator!=(const cObject& obj) const
	{ return(getSerial() != obj.getSerial()); }
//@}

public:
	static std::string	getRandomScriptValue( std::string section, std::string& sectionId );
protected:
	static cScpIterator*	getScriptIterator( std::string section, std::string& sectionId );

public:
	cObject();
	virtual ~cObject();
	virtual void Delete() = 0;
//@{
/*!
\name Serials
\brief functions for handle serials stuff
*/
protected:
	uint32_t		serial;		//!< serial of the object
	uint32_t		multi_serial;	//!< multi serial of the object (don't know what it is used for)

public:
	//! return the serial of the object
	inline const uint32_t getSerial() const
	{ return serial; }

	void setSerial(int32_t newserial);

	//! return the object's multi serial
	inline const uint32_t getMultiSerial() const
	{ return multi_serial; }

	//! Set the multi serial of the object
	inline void setMultiSerial(uint32_t newserial)
	{ multi_serial = newserial; }

//!\todo Need to reactive this
//	inline void setOwnerSerial32Only(uint32_t newserial)
//	{ owner_serial = newserial; }
//@}

//@{
/*!
\name Positions
\brief Position related stuff
*/
protected:
	Location		old_position;		//!< old position of object
	Location		position;		//!< current position of object

public:
	//! return the position of the object
	inline const Location getPosition() const
	{ return position; }

	void			setPosition(Location where);

	//! Set the position of the object
	inline void		setPosition(uint32_t x, uint32_t y, int8_t z)
	{ setPosition( Location( x, y, z ) ); }

	inline const Location getOldPosition() const
	{ return old_position; }

	inline void setOldPosition(const Location where)
	{ old_position = where; }

	inline void setOldPosition(int32_t x, int32_t y, signed char z, signed char dispz)
	{ setOldPosition( Location(x, y, z, dispz) ); }
//@}

//@{
/*!
\name Appearence
*/
protected:
	uint32_t ScriptID;	//!< Object's ScriptID

	uint16_t id;	//!< Object's ID
	uint16_t id_old;	//!< Object's old ID

	uint16_t color;	//!< Object's color
	uint16_t color_old;	//!< Object's old color

	/*!
	Real name of the char, 30 chars max + '\\0'<br>
	Also used to store the secondary name of items.
	*/
	std::string	secondary_name;
	//! Name displayed everywhere for this object, 30 char max + '\\0'
	std::string	current_name;

public:
	//! return the real name of object
	inline const std::string &getRealName() const
	{ return secondary_name; }

	//! Set the real name of object
	inline void setRealName(std::string s)
	{ secondary_name = s; }

	//! return the current name of object
	inline const std::string &getCurrentName() const
	{ return current_name;  }

	//! Set the current name of object
	inline void setCurrentName(std::string s)
	{ current_name = s; }

	void setCurrentName(char *format, ...);

	//! Get the secondary name of the object
	inline const std::string getSecondaryName() const
	{ return secondary_name; }

	//! Set the secondary name of object
	inline void setSecondaryName( std::string s )
	{ secondary_name = s; }

	void setSecondaryName(const char *format, ...);

	//! return the object's script number
	inline const uint32_t getScriptID() const
	{ return ScriptID; }

	//! set the object's script number
	inline void setScriptID(uint32_t sid)
	{ ScriptID = sid; }

	inline void setId( uint16_t newId )
	{ id = newId; }

	inline const uint16_t getId() const
	{ return id; }

	inline void setOldId( uint16_t oldId )
	{ id_old = oldId; }

	inline const uint16_t getOldId() const
	{ return id_old; }

	inline void setColor( uint16_t newColor )
	{ color = newColor; }

	inline const uint16_t getColor() const
	{ return color; }

	inline void setOldColor( uint16_t oldColor )
	{ color_old = oldColor; }

	inline const uint16_t getOldColor() const
	{ return color_old; }

//@}

//@{
/*!
\name Flags
*/

protected:
	uint64_t flags;	//!< Common flags for objects

	/*!
	\brief Set a given flag on or off
	\param flag Flag to set
	\param set true if the flag must be set on, else false
	*/
	inline void setFlag(uint64_t flag, bool set)
	{
		if ( set ) flags |= flag;
		else flags &= ~flag;
	}

public:
	//! Gets the character flags
	inline const uint64_t getFlags() const
	{ return flags; }

	//! Sets the character flags
	inline void setFlags(uint64_t newFlags)
	{ flags = newFlags; }

//@}

//@{
/*!
\name Temp - Fx
*/
protected:
	TempfxVector		*tempfx;

public:
	bool			addTempfx( cObject& src, int32_t num, int32_t more1 = 0, int32_t more2 = 0, int32_t more3 = 0, int32_t dur = 0, int32_t amxcback = INVALID );
	void			delTempfx( int32_t num, bool executeExpireCode = true, uint32_t funcidx = INVALID );
	void			checkTempfx();
	void			tempfxOn();
	void			tempfxOff();
	bool			hasTempfx();
	tempfx::cTempfx*	getTempfx( int32_t num, uint32_t funcidx = INVALID );
//@}

	uint32_t	disabled;	//!< Disabled object timer, cant trigger.
	std::string*	disabledmsg; //!< Object is disabled, so display this message.
} PACK_NEEDED;

#endif	// __OBJECT_H
