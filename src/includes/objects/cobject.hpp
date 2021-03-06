/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __OBJECTS_COBJECT_H__
#define __OBJECTS_COBJECT_H__

#include "common_libs.hpp"
#include "tmpeff.hpp"
#include "structs.hpp"
#include "objects/ceventthrower.hpp"
#include "inlines.hpp"
#include "libhypnos/hypstl/slist.hpp"

typedef slist< tempfx::cTempfx > TempfxVector;

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
	sLocation old_position;		//!< old position of object
	sLocation position;		//!< current position of object

public:
	//! return the position of the object
	inline sLocation getPosition() const
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

	//! Sets the position's Z-coordinate
	void setPositionDZ(int8_t dz)
	{ setPosition( sLocation(position.x, position.y, position.z, dz) ); }

	inline sLocation getOldPosition() const
	{ return old_position; }

	inline void setOldPosition(const sLocation where)
	{ old_position = where; }
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
	\brief Real name of the char, 30 chars max + '\\0'<br>
	Also used to store the secondary name of items.
	*/
	string	secondary_name;
	//! Name displayed everywhere for this object, 30 char max + '\\0'
	string	current_name;

public:
	//! return the real name of object
	inline const string &getRealName() const
	{ return secondary_name; }

	//! Set the real name of object
	inline void setRealName(string s)
	{ secondary_name = s; }

	//! return the current name of object
	inline const string &getCurrentName() const
	{ return current_name;  }

	//! Set the current name of object
	inline void setCurrentName(string s)
	{ current_name = s; }

	void setCurrentName(const char *format, ...) PRINTF_LIKE(2,3);

	//! Get the secondary name of the object
	inline string getSecondaryName() const
	{ return secondary_name; }

	//! Set the secondary name of object
	inline void setSecondaryName( string s )
	{ secondary_name = s; }

	void setSecondaryName(const char *format, ...) PRINTF_LIKE(2,3);

	inline void setId( uint16_t newId )
	{ id = newId; }

	inline uint16_t getId() const
	{ return id; }

	inline void setOldId( uint16_t oldId )
	{ id_old = oldId; }

	inline uint16_t getOldId() const
	{ return id_old; }

	inline void setColor( uint16_t newColor )
	{ color = newColor; }

	inline uint16_t getColor() const
	{ return color; }

	inline void setOldColor( uint16_t oldColor )
	{ color_old = oldColor; }

	inline uint16_t getOldColor() const
	{ return color_old; }

//@}

//@{
/*!
\name Flags
*/

protected:
	uint64_t flags;	//!< Common flags for objects

	static const uint64_t flagIncognito	= 0x8000000000000000ull; //!< Is under incognito effect?
public:
	//! Gets the object's flags
	inline uint64_t getFlags() const
	{ return flags; }

	//! Sets the object's flags
	inline void setFlags(uint64_t newFlags)
	{ flags = newFlags; }

	inline bool isIncognito() const
	{ return flags & flagIncognito; }
	
	inline void setIncognito(bool set = true)
	{ setFlag(flags, flagIncognito, set); }
//@}

//@{
/*!
\brief Resistance stuff [UO4}

UO:AoS implements a completely new resistence system: there are now five types
of resistences and damages, which are:
\ul
	\li Physical
	\li Fire
	\li Cold
	\li Poison
	\li Energy

All the spells and the weapons do one or more of these damage types. This
system simplify the old NoX one, which was a lot more complicated. To add more
types of resistance and damages, which will not be used direcly by the UO:AoS
client, you can work at scripting level.
*/
protected:
	uint8_t resistances[5];
public:
	virtual uint8_t getResistance(DamageType index);
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
	string disabledmsg;	//!< Object is disabled, so display this message.
};

#endif
