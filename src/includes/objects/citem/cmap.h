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
	bool writeable;				//!< map pins can be altered? \todo change in flag
	inline sPoint getPin(uint32_t pin)	//!< Gets pin data for pin number "pin" (pin goes from 1 to 50, as it appears on client)
	{ return pinData[pin - 1]; }
public:
	cMap();
	cMap(uint32_t ser);
	cMap &operator = (const cMap &oldmap); 	//!< assignment operator = (for duping maps)

	bool addPin(uint16_t x, uint16_t y);		//!< Adds a new pin to the map
	bool insertPin(uint16_t x, uint16_t y, uint32_t pin); //!< Inserts a pin between 2 existing ones. existing pins >= pin get shifted by one
	bool changePin(uint16_t x, uint16_t y, uint32_t pin);	//!< Moves pin to another position
	bool removePin(uint32_t pin);		//!< Removes pin
	bool clearAllPins();			//!< Removes all pins
	bool toggleWritable();			//!< Toggle pin addability and replies to client actual writeability status

	//! Gets x "coordinate" for pin number "pin" (1<= pin <= 50)
	inline uint16_t getX(uint32_t pin)
	{ return pinData[pin - 1].x; }
	
	//! Gets y "coordinate" for pin number "pin" (1<= pin <= 50)
	inline uint16_t getY(uint32_t pin)
	{ return pinData[pin - 1].y; }
	
	//! Returns number of pins in map
	inline int getPinsNumber()
	{ return pinData.size(); }

	inline bool isBlankMap()
	{ return type == ITYPE_BLANK_MAP; }
	inline bool isWrittenMap()
	{ return type == ITYPE_MAP; }
	inline bool isTreasureMap()
	{ return (type == ITYPE_TREASURE_MAP) || (type == ITYPE_DECIPHERED_MAP); }
	
	void doubleClicked(pClient client);
};

#endif
