/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Header defining cContainer class
*/
#ifndef __MAPS_H
#define __MAPS_H

class cMap;
typedef cMap* pMap;

#include "objects/citem.h"
#include "rtti.h"
#include "typedefs.h"

/*!
\brief Map item
\note This class must be identified via rtti() method to be sure
that the item is a map
*/
class cMap : public cItem
{
	protected:
        	struct pindataobject		        //!< this structure will hold x and y positions of a single pin in map
                {
                	uint16_t x;
                        uint16_t y;
                }
                std::vector<pindataobject> pinData;    	//!< this vector will hold all the pins data (= their placement in the map)
                bool writeable;				//!< map pins can be altered?
               	inline pindataobject getPin(int pin)	//!< Gets pin data for pin number "pin" (pin goes from 1 to 50, as it appears on client)
                	{ return pinData[pin - 1]; }
        public:
        	cMap(uint32_t ser);
                cMap &operator = (const cMap &oldmap); 	//!< assignment operator = (for duping maps)

               	inline const uint32_t rtti() const          //!< returns the right rtti
		{ return rtti::cMap; }


                bool addPin(uint16_t x, uint16_t y);		//!< Adds a new pin to the map
                bool insertPin(uint16_t x,uint16_t y, int pin); //!< Inserts a pin between 2 existing ones. existing pins >= pin get shifted by one
                bool changePin(uint16_t x,uint16_t y, int pin);	//!< Moves pin to another position
                bool removePin(int pin);		//!< Removes pin
                bool clearAllPins();			//!< Removes all pins
                bool toggleWritable();			//!< Toggle pin addability and replies to client actual writeability status


                inline uint16_t getX(int pin)               //!< Gets x "coordinate" for pin number "pin" (1<= pin <= 50)
			{ return pinData[pin - 1].x; }
                inline uint16_t getY(int pin)               //!< Gets y "coordinate" for pin number "pin" (1<= pin <= 50)
			{ return pinData[pin - 1].y; }
		inline int getPinsNumber()              //!< Returns number of pins in map
                	{ return pinData.size(); }


                inline bool isBlankMap()
	                { return type == ITYPE_BLANK_MAP; }
                inline bool isWrittenMap()
        	        { return type == ITYPE_MAP; }
                inline bool isTreasureMap()
                	{ return (type == ITYPE_TREASURE_MAP) || (type == ITYPE_DECIPHERED_MAP); }
}
