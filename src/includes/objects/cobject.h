/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __OBJECT_H
#define __OBJECT_H

#include "common_libs.h"
#include "tmpeff.h"
#include "structs.h"
#include "objects/ceventthrower.h"

typedef std::slist< tempfx::cTempfx > TempfxVector;

/*!
\class cObject cobject.h "objects/cobject.h"
\brief Base class of cItem and cChar
*/
class cObject : virtual public cEventThrower
{
//@{
/*!
\name Constructor and Operators
*/
public:
	cObject();
	virtual ~cObject();
//@}

public:
	virtual void Delete() = 0;

//@{
/*!
\name Positions
\brief Position related stuff
*/
protected:
	sLocation		old_position;		//!< old position of object
	sLocation		position;		//!< current position of object

public:
	//! return the position of the object
	inline const sLocation getPosition() const
	{ return position; }

	void setPosition(sLocation where);
	
	//! Sets the position's X-coordinate
	void setPositionX(uint16_t x)
	{ setPosition( sLocation(x, position.y, position.z) ); }
	
	//! Sets the position's Y-coordinate
	void setPositionY(uint16_t y)
	{ setPosition( sLocation(position.x, y, position.z) ); }
	
	//! Sets the position's Z-coordinate
	void setPositionZ(int8_t z)
	{ setPosition( sLocation(position.x, position.y, z) ); }

	inline const sLocation getOldPosition() const
	{ return old_position; }

	inline void setOldPosition(const sLocation where)
	{ old_position = where; }

	inline void setOldPosition(int32_t x, int32_t y, signed char z, signed char dispz)
	{ setOldPosition( sLocation(x, y, z, dispz) ); }
//@}

//@{
/*!
\name Appearence
*/
protected:
	uint16_t id;		//!< Object's ID
	uint16_t id_old;	//!< Object's old ID

	uint16_t color;		//!< Object's color
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

	static const uint64_t flagIncognito	= 0x8000000000000000ull; //!< Is under incognito effect?
	
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

	inline const bool isIncognito() const
	{ return flags & flagIncognito; }
	
	inline void setIncognito(bool set = true)
	{ setFlag(flagIncognito, set); }
//@}

//@{
/*!
\name Temp - Fx
*/
protected:
	TempfxVector		*tempfx;

public:
	bool addTempfx( cObject& src, int32_t num, int32_t more1 = 0, int32_t more2 = 0, int32_t more3 = 0, int32_t dur = 0, int32_t amxcback = INVALID );
	void delTempfx( int32_t num, bool executeExpireCode = true, uint32_t funcidx = INVALID );
	void checkTempfx();
	void tempfxOn();
	void tempfxOff();
	bool hasTempfx();
	tempfx::cTempfx* getTempfx( int32_t num, uint32_t funcidx = INVALID );
//@}

	uint32_t disabled;		//!< Disabled object timer, cant trigger.
	std::string* disabledmsg;	//!< Object is disabled, so display this message.
};

#endif	// __OBJECT_H
