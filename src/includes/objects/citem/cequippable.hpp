/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file 
\brief Declaration of class cEquippable
*/

#ifndef __CEQUIPPABLE_H__
#define __CEQUIPPABLE_H__

#include "common_libs.hpp"
#include "objects/citem.hpp"

/*!
\brief Class for equippable items

This class is used to add support for equippable items, like weapons,
shields, armors and clothes.
This is an abstract class, thus no direct instance of this is possible.
Some special classes (like bankboxes and backpacks) can derive both from this
and from cContainer class.
*/
class cEquippable : virtual public cItem
{
public:
	cEquippable();
	cEquippable(uint32_t newSerial);
	virtual ~cEquippable();

	virtual void setContainer(pObject obj);
	
	//! Gets the layer where the item \b is equipped
	inline Layer getLayer() const
	{ return layer; }
	
	//! Gets the layer where the item \b can be equipped
	inline Layer getPossibleLayer() const
	{ return findLayerFromID(getId()); }
	
	void doubleClicked(pClient client);

	//! Events for equippable items
	enum {
		evtEqpOnEquip = evtItmMax,
		evtEqpOnUnequip,
		evtEqpMax
	};

protected:
	Layer layer;	//!< Layer where the item \b is equipped
	static Layer findLayerFromID(uint16_t id);

	virtual uint16_t eventsNumber() const
	{ return evtEqpMax; }
//@{
/*!
\name Minimum skills & stats to equip
*/
protected:

	uint16_t minEquippingStrength;		//!< Minimum strength to equip item
	uint16_t minEquippingIntelligence;	//!< Minimum inteligence to equip item
	uint16_t minEquippingDexterity;		//!< Minimum dexterity to equip item
	uint16_t minEquippingSkill[3];		//!< holds up to 3 skills to be checked for equippability. if INVALID no skill check is done
	uint16_t minEquippingSkillValue[3];	//!< holds the 3 skill values of skills Minimum in the minEquippingSkill array

public:
	inline void setMinEquippingStrength(uint16_t newStrength)
	{ minEquippingStrength = newStrength; }

	inline void setMinEquippingDexterity(uint16_t newDexterity)
	{ minEquippingDexterity = newDexterity; }

	inline void setMinEquippingIntelligence(uint16_t newIntelligence)
	{ minEquippingIntelligence = newIntelligence; }

	inline void setMinEquippingSkill1(uint16_t skill, uint16_t value)
	{ minEquippingSkill[0] = skill; minEquippingSkillValue[0] = value; }

	inline void setMinEquippingSkill2(uint16_t skill, uint16_t value)
	{ minEquippingSkill[1] = skill; minEquippingSkillValue[1] = value; }

	inline void setMinEquippingSkill3(uint16_t skill, uint16_t value)
	{ minEquippingSkill[2] = skill; minEquippingSkillValue[2] = value; }

	inline uint16_t getMinEquippingStrength()
	{ return minEquippingStrength; }

	inline uint16_t getMinEquippingDexterity()
	{ return minEquippingDexterity; }

	inline uint16_t getMinEquippingIntelligence()
	{ return minEquippingIntelligence; }

	inline uint16_t getMinEquippingSkill1()
	{ return minEquippingSkill[0]; }

	inline uint16_t getMinEquippingSkillValue1()
	{ return minEquippingSkillValue[0]; }

	inline uint16_t getMinEquippingSkill2()
	{ return minEquippingSkill[1]; }

	inline uint16_t getMinEquippingSkillValue2()
	{ return minEquippingSkillValue[1]; }

	inline uint16_t getMinEquippingSkill3()
	{ return minEquippingSkill[2]; }

	inline uint16_t getMinEquippingSkillValue3()
	{ return minEquippingSkillValue[2]; }

//@}
};

#endif
