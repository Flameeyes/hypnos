/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __OBJECTS_CBODY_H__
#define __OBJECTS_CBODY_H__

#include "common_libs.h"
#include "objects/cobject.h"
#include "objects/citem/cequippablecontainer.h"
#include "enums.h"

/*!
\brief This class represent a body of a character

This class replaces the old cChar as cObject subclass
All skills, abilities, name, title, and so stuff must
be moved here.

\todo Move other stuff here
*/
class cBody : public cObject
{
friend class cChar;
public:
	cBody();
protected:
	std::string name;
	std::string title;
	uint16_t id;

public:
	//! Gets the character's name
	inline const std::string& getName() const
	{ return name; }

	//! Sets the character's name
	inline void setName(const std::string value)
	{ name = value; }

	//! Gets the character's title
	inline const std::string& getTitle() const
	{ return title; }

	//! Sets the character's title
	inline void setTitle(const std::string value)
	{ title = value; }

	//! Gets the character's id
	inline const uint16_t getId() const
	{ return id; }

	//! Sets the character's id
	inline void setId(const uint16_t value)
	{ id = value; }

	//! Returns true if the char is human
	inline const bool isHuman() const
	{ return id == bodyMale || id == bodyFemale; }

//@{
/*!
\name Items
\brief Item related stuff
*/
protected:
	pEquippable layers[0x1E];
public:
	//! Gets the item on the specified layer
	inline pEquippable getLayerItem(Layer layer) const
	{ assert(layer < 0x1E); return layers[layer]; }

	//! Sets the item on the specified layer
	inline void setLayerItem(Layer layer, pEquippable item);

	pEquippableContainer getBackpack(bool create = false);

	const bool isWearing(pItem pi) const;

	const uint8_t equip(pEquippable pi, bool drag = false);
	const uint8_t unEquip(pEquippable pi, bool drag = false);

	void checkEquipment();
	
	pWeapon getWeapon();
//@}

//@{
/*!
\name Flags
\brief Flags used by the body
*/
protected:
	//! Character is Female
	static const uint32_t flagFemale		= 0x0000000000000001ull;
public:
	
//@}

//@{
/*!
\name Skills
\brief Skill stuff
*/
protected:
	uint16_t skills[50];

public:
	inline const uint16_t getSkill(Skill sk) const
	{ return skills[sk]; }

	inline void setSkill(Skill sk, uint16_t value)
	{ skills[sk] = value; }

	Skill bestSkill() const;
	Skill nextBestSkill(uint8_t previous) const;
	
	uint32_t getSkillSum();
//@}

//@{
/*!
\name Stats
\brief Stats and company

These values are used by the packet 0x11
*/
protected:
	uint16_t strength;	//!< Strength of the character
	uint16_t hitPoints;	//!< Hitpoints of the character
	uint16_t maxHitPoints;	//!< Max hitpoints of the character

	uint16_t dextery;	//!< Dextery of the character
	uint16_t stamina;	//!< Stamina of the character
	uint16_t maxStamina;	//!< Max stamina of the character

	uint16_t intelligence;	//!< Intelligence of the character
	uint16_t mana;		//!< Mana of the character
	uint16_t maxMana;	//!< Max mana of the character

	uint16_t statCap;	//!< Statcap of the character [UO3?]
	uint8_t followers;	//!< Followers of the character [UO3?]
	uint8_t maxFollowers;	//!< Max followers of the character [UO3?]

	uint16_t luck;		//!< Luck of the character [UO4]

	uint16_t resistFire;	//!< Resistance to fire of the character [UO4]
	uint16_t resistCold;	//!< Resistance to cold of the character [UO4]
	uint16_t resistPoison;	//!< Resistance to poison of the character [UO4]
	uint16_t resistEnergy;	//!< Resistance to energy of the character [UO4]

	uint16_t damageMin;	//!< Minimum damage of the character [UO4]
	uint16_t damageMax;	//!< Maximum damage of the character [UO4]

	uint16_t skinColor;	//!< Skin color of the character
public:
	//! Gets the character strength
	inline const uint16_t getStrength() const
	{ return strength; }

	//! Gets the character hitpoints
	inline const uint16_t getHitPoints() const
	{ return hitPoints; }

	//! Gets the character max hitpoints
	inline const uint16_t getMaxHitPoints() const
	{ return maxHitPoints; }

	//! Gets the character dextery
	inline const uint16_t getDextery() const
	{ return dextery; }

	//! Gets the character stamina
	inline const uint16_t getStamina() const
	{ return stamina; }

	//! Gets the character max stamina
	inline const uint16_t getMaxStamina() const
	{ return maxStamina; }

	//! Gets the character intelligence
	inline const uint16_t getIntelligence() const
	{ return intelligence; }

	//! Gets the character mana
	inline const uint16_t getMana() const
	{ return mana; }

	//! Gets the character max mana
	inline const uint16_t getMaxMana() const
	{ return maxMana; }

	//! Gets the character stat cap [UO3?]
	inline const uint16_t getStatCap() const
	{ return statCap; }

	//! Gets the character followers [UO3?]
	inline const uint8_t getFollowers() const
	{ return followers; }

	//! Gets the character max followers [UO3?]
	inline const uint8_t getMaxFollowers() const
	{ return maxFollowers; }

	//! Gets the character luck [UO4]
	inline const uint16_t getLuck() const
	{ return luck; }

	//! Gets the character resistance to fire [UO4]
	inline const uint16_t getResistFire() const
	{ return resistFire; }

	//! Gets the character resistance to cold [UO4]
	inline const uint16_t getResistCold() const
	{ return resistCold; }

	//! Gets the character resistance to poison [UO4]
	inline const uint16_t getResistPoison() const
	{ return resistPoison; }

	//! Gets the character resistance to energy [UO4]
	inline const uint16_t getResistEnergy() const
	{ return resistEnergy; }

	//! Gets the character minimum damage [UO4]
	inline const uint16_t getDamageMin() const
	{ return damageMin; }

	//! Gets the character maximum damage [UO4]
	inline const uint16_t getDamageMax() const
	{ return damageMax; }

	//! Gets the skin color of the character
	inline const uint16_t getSkinColor() const
	{ return skinColor; }

	void setStrength(uint16_t value);
	void setHitPoints(uint16_t value);
	void setMaxHitPoints(uint16_t value);

	void setDextery(uint16_t value);
	void setStamina(uint16_t value);
	void setMaxStamina(uint16_t value);

	void setIntelligence(uint16_t value);
	void setMana(uint16_t value);
	void setMaxMana(uint16_t value);

	void setStatCap(uint16_t value);
	void setFollowers(uint8_t value);
	void setMaxFollowers(uint8_t value);

	void setLuck(uint16_t value);
	void setResistFire(uint16_t value);
	void setResistCold(uint16_t value);
	void setResistPoison(uint16_t value);
	void setResistEnergy(uint16_t value);
	void setDamageMin(uint16_t value);
	void setDamageMax(uint16_t value);
       	void setSkinColor(uint16_t value);
//@}

//@{
/*!
\name Mount
\brief Mount related stuff
*/
protected:
	pNPC mounting;	//!< Horse mounted (actually can be every mountable npc)
public:
	void mount(pNPC horse);
	void unmount();

	inline const bool isMounted() const
	{ return mounting; }

	inline const bool isMounting(pNPC horse) const
	{ return mounting == horse; }

	inline pNPC getHorse() const
	{ return mounting; }
//@}

//@{
/*!
\name Weight
\brief Weight stuff
*/
protected:
	float weight; //!< Weight of the body (with all equipment and backpack)
private:
	//! Gets the weight of the body, calculating it when necessary
	inline const float getWeight()
	{ if ( weight <= 0 ) calcWeight(); return weight; }
	
	void calcWeight();
	bool overloadedWalking();
	bool overloadedTeleport();
//@}

//@{
//! \name Bank
public:
	//! Gets the body bank box
	inline pEquippableContainer getBankBox() const
	{ return dynamic_cast<pEquippableContainer>(layers[layBank]); }
	
	//! Counts the gold in the bankbox
	inline uint32_t countBankGold()
	{
		return  getBankBox() ?
			getBankBox()->countItems(ITEMID_GOLD)
			: 0;
	}
//@}
};

#endif
