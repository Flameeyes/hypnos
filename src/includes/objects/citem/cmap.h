/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Header defining cContainer class
*/
#ifndef __CMAP_H__
#define __CMAP_H__

#include "objects/citem.h"
#include "typedefs.h"

/*!
\brief Map item
\note This class must be identified via rtti() method to be sure
that the item is a map
*/
class cMap : public cItem
{
protected:
	PointVector pinData;    		//!< this vector will hold all the pins data (= their placement in the map)
	
//@{
/*!
\name Flags
*/
protected:
	static const uint64_t flagWritable	= 0x0000000000010000ull; //!< The map is writable
	static const uint64_t flagTreasure	= 0x0000000000020000ull; //!< The map is a treasure map
	static const uint64_t flagDeciphered	= 0x0000000000040000ull; //!< The treasure map is deciphered
	static const uint64_t flagBlank		= 0x0000000000080000ull; //!< The map is blank
public:
	inline bool isBlankMap()
	{ return (flags & flagWritable) && !getPinsNumber() ; }

	inline bool isWrittenMap()
	{ return ! ( flags & flagDeciphered|flagTreasure ); }
	
	inline bool isTreasureMap()
	{ return flags & flagTreasure; }
//@}
public:
	cMap();
	cMap(uint32_t ser);
	cMap &operator = (const cMap &oldmap); 	//!< assignment operator = (for duping maps)

	bool addPin(sPoint p);		//!< Adds a new pin to the map
	bool insertPin(sPoint p, uint32_t pin); //!< Inserts a pin between 2 existing ones. existing pins >= pin get shifted by one
	bool changePin(sPoint p, uint32_t pin);	//!< Moves pin to another position
	bool removePin(uint32_t pin);		//!< Removes pin
	bool clearAllPins();			//!< Removes all pins
	bool toggleWritable(pClient client);	//!< Toggle pin addability and replies to client actual writeability status

	/*!
	\brief Gets the pin data for the given index
	\param pin Index of the pin (1-50)
	\return The point for the given index or InvalidCoord if out-of-bound
	*/
	inline const sPoint getPin(uint32_t pin)
	{
		if ( pin > pinData.size() )
			return InvalidCoord;
		
		return pinData[pin - 1];
	}
	
	//! Returns number of pins in map
	inline int getPinsNumber()
	{ return pinData.size(); }

	void doubleClicked(pClient client);
};

#endif
