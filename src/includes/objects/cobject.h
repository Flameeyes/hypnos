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
	UI32		serial;		//!< serial of the object
	UI32		multi_serial;	//!< multi serial of the object (don't know what it is used for)

public:
	//! return the serial of the object
	inline const UI32 getSerial() const
	{ return serial; }

	void setSerial32(SI32 newserial);

	//! return the object's multi serial
	inline const UI32 getMultiSerial() const
	{ return multi_serial; }

	//! Set the multi serial of the object
	inline void setMultiSerial(UI32 newserial)
	{ multi_serial = newserial; }

	inline void setOwnerSerial32Only(UI32 newserial)
	{ owner_serial = newserial; }
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
	inline void		setPosition(UI32 x, UI32 y, SI08 z)
	{ setPosition( Loc( x, y, z ) ); }

	inline const Location getOldPosition() const
	{ return old_position; }

	inline void setOldPosition(const Location where)
	{ old_position = where; }

	inline void setOldPosition(SI32 x, SI32 y, signed char z, signed char dispz)
	{ setOldPosition( Loc(x, y, z, dispz) ); }
//@}

//@{
/*!
\name Appearence
*/
protected:
	UI32 ScriptID;	//!< Object's ScriptID

	UI16 id;	//!< Object's ID
	UI16 id_old;	//!< Object's old ID

	UI16 color;	//!< Object's color
	UI16 color_old;	//!< Object's old color

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
	inline const UI32 getScriptID() const
	{ return ScriptID; }

	//! set the object's script number
	inline void setScriptID(UI32 sid)
	{ ScriptID = sid; }

	inline void setId( UI16 newId )
	{ id = newId; }

	inline const UI16 getId() const
	{ return id; }

	inline void setOldId( UI16 oldId )
	{ id_old = oldId; }

	inline const UI16 getOldId() const
	{ return id_old; }

	inline void setColor( UI16 newColor )
	{ color = newColor; }

	inline const UI16 getColor() const
	{ return color; }

	inline void setOldColor( UI16 oldColor )
	{ color_old = oldColor; }

	inline const UI16 getOldColor() const
	{ return color_old; }

//@}

//@{
/*!
\name Flags
*/

protected:
	UI64 flags;	//!< Common flags for objects

	/*!
	\brief Set a given flag on or off
	\param flag Flag to set
	\param set true if the flag must be set on, else false
	*/
	inline void setFlag(UI64 flag, bool set)
	{
		if ( set ) flags |= flag;
		else flags &= ~flag;
	}

public:
	//! Gets the character flags
	inline const UI64 getFlags() const
	{ return flags; }

	//! Sets the character flags
	inline void setFlags(UI64 newFlags)
	{ flags = newFlags; }

//@}

//@{
/*!
\name Temp - Fx
*/
protected:
	TempfxVector		*tempfx;

public:
	LOGICAL			addTempfx( cObject& src, SI32 num, SI32 more1 = 0, SI32 more2 = 0, SI32 more3 = 0, SI32 dur = 0, SI32 amxcback = INVALID );
	void			delTempfx( SI32 num, LOGICAL executeExpireCode = true, SERIAL funcidx = INVALID );
	void			checkTempfx();
	void			tempfxOn();
	void			tempfxOff();
	LOGICAL			hasTempfx();
	tempfx::cTempfx*	getTempfx( SI32 num, SERIAL funcidx = INVALID );
//@}

	UI32	disabled;	//!< Disabled object timer, cant trigger.
	std::string*	disabledmsg; //!< Object is disabled, so display this message.
} PACK_NEEDED;

#endif	// __OBJECT_H
