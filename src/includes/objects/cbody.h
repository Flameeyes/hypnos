/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Declaration of cBody class
*/

#ifndef __CBODY_H__
#define __CBODY_H__

class cBody;
typedef cBody* pBody;


const UI16 bodyMale	= 0x190;
const UI16 bodyFemale	= 0x191;

/*!
\brief This class represent a body of a character
*/
class cBody
{
protected:
	//! The next serial to be used
	static UI32 nextSerial;
public:
	//! Get the next serial
	static const UI32 getNextSerial()
	{ return nextSerial++; }

protected:
	std::string name;
	std::string title;
	UI16 id;
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
	inline const UI16 getId() const
	{ return id; }

	//! Sets the character's id
	inline void setId(const UI16 value)
	{ id = value; }

	//! Returns true if the char is human
	inline const bool isHuman() const
	{ return id == bodyMale || id == bodyHuman; }

//@{
/*!
\name Items
\brief Item related stuff
*/
protected:
	std::map<Layer, pItem> layers;
public:
	inline pItem getHair() const
	{ return layers[layHair]; }

	inline pItem getBeard() const
	{ return layers[layBeard]; }

//@}

//@{
/*!
\name Flags
\brief Flags used by the body
*/
protected:
	UI32 flags;		//!< Flags of the character

public:
	//! Character is Female
	static const UI32 flagFemale		= 0x00000001;

	//! Gets the character flags
	inline const UI32 getFlags() const
	{ return flags; }
//@}

//@{
/*!
\name Stats
\brief Stats and company

These values are used by the packet 0x11
*/
protected:
	UI16 strength;		//!< Strength of the character
	UI16 hitPoints;		//!< Hitpoints of the character
	UI16 maxHitPoints;	//!< Max hitpoints of the character

	UI16 dextery;		//!< Dextery of the character
	UI16 stamina;		//!< Stamina of the character
	UI16 maxStamina;	//!< Max stamina of the character

	UI16 intelligence;	//!< Intelligence of the character
	UI16 mana;		//!< Mana of the character
	UI16 maxMana;		//!< Max mana of the character

	UI16 statCap;		//!< Statcap of the character [UO3?]
	UI08 followers;		//!< Followers of the character [UO3?]
	UI08 maxFollowers;	//!< Max followers of the character [UO3?]

	UI16 luck;		//!< Luck of the character [UO4]

	UI16 resistFire;	//!< Resistance to fire of the character [UO4]
	UI16 resistCold;	//!< Resistance to cold of the character [UO4]
	UI16 resistPoison;	//!< Resistance to poison of the character [UO4]
	UI16 resistEnergy;	//!< Resistance to energy of the character [UO4]

	UI16 damageMin;		//!< Minimum damage of the character [UO4]
	UI16 damageMax;		//!< Maximum damage of the character [UO4]
public:
	//! Gets the character strength
	inline const UI16 getStrength() const
	{ return strength; }

	//! Gets the character hitpoints
	inline const UI16 getHitPoints() const
	{ return hitPoints; }

	//! Gets the character max hitpoints
	inline const UI16 getMaxHitPoints() const
	{ return maxHitPoints; }

	//! Gets the character dextery
	inline const UI16 getDextery() const
	{ return dextery; }

	//! Gets the character stamina
	inline const UI16 getStamina() const
	{ return stamina; }

	//! Gets the character max stamina
	inline const UI16 getMaxStamina() const
	{ return maxStamina; }

	//! Gets the character intelligence
	inline const UI16 getIntelligence() const
	{ return intelligence; }

	//! Gets the character mana
	inline const UI16 getMana() const
	{ return mana; }

	//! Gets the character max mana
	inline const UI16 getMaxMana() const
	{ return maxMana; }

	//! Gets the character stat cap [UO3?]
	inline const UI16 getStatCap() const
	{ return statCap; }

	//! Gets the character followers [UO3?]
	inline const UI08 getFollowers() const
	{ return followers; }

	//! Gets the character max followers [UO3?]
	inline const UI08 getMaxFollowers() const
	{ return maxFollowers; }

	//! Gets the character luck [UO4]
	inline const UI16 getLuck() const
	{ return luck; }

	//! Gets the character resistance to fire [UO4]
	inline const UI16 getResistFire() const
	{ return resistFire; }

	//! Gets the character resistance to cold [UO4]
	inline const UI16 getResistCold() const
	{ return resistCold; }

	//! Gets the character resistance to poison [UO4]
	inline const UI16 getResistPoison() const
	{ return resistPoison; }

	//! Gets the character resistance to energy [UO4]
	inline const UI16 getResistEnergy() const
	{ return resistEnergy; }

	//! Gets the character minimum damage [UO4]
	inline const UI16 getDamageMin() const
	{ return damageMin; }

	//! Gets the character maximum damage [UO4]
	inline const UI16 getDamageMax() const
	{ return damageMax; }

	void setStrength(UI16 value);
	void setHitPoints(UI16 value);
	void setMaxHitPoints(UI16 value);

	void setDextery(UI16 value);
	void setStamina(UI16 value);
	void setMaxStamina(UI16 value);

	void setIntelligence(UI16 value);
	void setMana(UI16 value);
	void setMaxMana(UI16 value);

	void setStatCap(UI16 value);
	void setFollowers(UI08 value);
	void setMaxFollowers(UI08 value);

	void setLuck(UI16 value);
	void setResistFire(UI16 value);
	void setResistCold(UI16 value);
	void setResistPoison(UI16 value);
	void setResistEnergy(UI16 value);
	void setDamageMin(UI16 value);
	void setDamageMax(UI16 value);
//@}

	void checkEquipment();
};

#endif