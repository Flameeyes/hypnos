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

#include "common_libs.h"

static const uint16_t bodyMale		= 0x190;
static const uint16_t bodyFemale	= 0x191;

/*!
\brief This class represent a body of a character

This class replaces the old cChar as cObject subclass
All skills, abilities, name, title, and so stuff must
be moved here.
The serial itself should be moved here as well.

\todo Move serial stuff here
\todo Move other stuff here
*/
class cBody : public cObject
{
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
	{ return id == bodyMale || id == bodyHuman; }

//@{
/*!
\name Items
\brief Item related stuff
*/
public:
	//! One handed weapon layer
	static const uint8_t layWeapon1H	= 0x01;
	//! Two handed weapon, shield, or misc layer
	static const uint8_t layWeapon2H	= 0x02;
	//! Shoes layer
	static const uint8_t layShoes	= 0x03;
	//! Pants layer
	static const uint8_t layPants	= 0x04;
	//! Shirt layer
	static const uint8_t layShirt	= 0x05;
	//! Helm/Hat layer
	static const uint8_t layHat	= 0x06;
	//! Gloves layer
	static const uint8_t layGloves	= 0x07;
	//! Ring layer
	static const uint8_t layRing	= 0x08;
	//! Unused layer
	static const uint8_t layUnused	= 0x09;
	//! Neck layer
	static const uint8_t layNeck	= 0x0A;
	//! Hair layer
	static const uint8_t layHair	= 0x0B;
	//! Waist (half apron) layer
	static const uint8_t layWaist	= 0x0C;
	//! Torso (inner) (chest armor) layer
	static const uint8_t layTorsoInner	= 0x0D;
	//! Bracelet layer
	static const uint8_t layBracelet	= 0x0E;
	//! Unused (backpack?) layer
	static const uint8_t layUnused2	= 0x0F;
	//! Facial Hair (Beard) layer
	static const uint8_t layBeard	= 0x10;
	//! Torso (middle) (sircoat, tunic, full apron, sash) layer
	static const uint8_t layTorsoMiddle= 0x11;
	//! Earrings layer
	static const uint8_t layEarrings	= 0x12;
	//! Arms layer
	static const uint8_t layArms	= 0x13;
	//! Back (cloack) layer
	static const uint8_t layBack	= 0x14;
	//! Backpack layer
	static const uint8_t layBackpack	= 0x15;
	//! Torso (outer) (robe) layer
	static const uint8_t layTorsoOuter	= 0x16;
	//! Legs (outer) (skirt/kilt) layer
	static const uint8_t layLegsOuter	= 0x17;
	//! Legs (inner) (leg armor) layer
	static const uint8_t layLegsInner	= 0x18;
	//! Mount (horse, ostarard, etc) layer
	static const uint8_t layMount	= 0x19;
	//! NPC Buy Restock container layer (see cNPC::restock)
	static const uint8_t layNPCBuyR	= 0x1A;
	//! NPC Buy no-restock container layer (see cNPC::bought)
	static const uint8_t layNPCBuyN	= 0x1B;
	//! NPC Buy Sell container layer (see cNPC::sell)
	static const uint8_t laySell	= 0x1C;
	//! PC Bank Box (see cPC::bankBox)
	static const uint8_t layBank	= 0x1D;

protected:
	pItem layers[0x1E];
public:
	//! Gets the item on the specified layer
	inline pItem getLayerItem(uint8_t layer) const
	{ return layers[layer]; }

	//! Sets the item on the specified layer
	inline pItem setLayerItem(uint8_t layer, pItem item)
	{ layers[layer] = item; }

	pItem getBackpack(bool create = false);

	inline const bool isWearing(pItem pi) const
	{ return this == pi->getContainer(); }

	int32_t equip(pItem pi, bool drag = false);
	int32_t unEquip(pItem pi, bool drag = false);

	void checkEquipement();
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
public:
	static const uint16_t skillAlchemy		= 0x00;
	static const uint16_t skillAnatomy		= 0x01;
	static const uint16_t skillAnimalLore		= 0x02;
	static const uint16_t skillItemID		= 0x03;
	static const uint16_t skillArmsLore		= 0x04;
	static const uint16_t skillParrying		= 0x05;
	static const uint16_t skillBagging		= 0x06;
	static const uint16_t skillBlacksmithing	= 0x07;
	static const uint16_t skillBowcraft		= 0x08;
	static const uint16_t skillPeacemaking		= 0x09;
	static const uint16_t skillCamping		= 0x0a;
	static const uint16_t skillCarpentry		= 0x0b;
	static const uint16_t skillCartography		= 0x0c;
	static const uint16_t skillCooking		= 0x0d;
	static const uint16_t skillDetectHidden		= 0x0e;
	static const uint16_t skillEnticement		= 0x0f;
	static const uint16_t skillEvalutateIntell	= 0x10;
	static const uint16_t skillHealing		= 0x11;
	static const uint16_t skillFishing		= 0x12;
	static const uint16_t skillForensic		= 0x13;
	static const uint16_t skillHerding		= 0x14;
	static const uint16_t skillHiding		= 0x15;
	static const uint16_t skillProvocation		= 0x16;
	static const uint16_t skillInscription		= 0x17;
	static const uint16_t skillLockpicking		= 0x18;
	static const uint16_t skillMagery		= 0x19;
	static const uint16_t skillMagicResistance	= 0x1a;
	static const uint16_t skillTactics		= 0x1b;
	static const uint16_t skillSnooping		= 0x1c;
	static const uint16_t skillMusicianship		= 0x1d;
	static const uint16_t skillPoisoning		= 0x1e;
	static const uint16_t skillArchery		= 0x1f;
	static const uint16_t skillSpiritSpeak		= 0x20;
	static const uint16_t skillStealing		= 0x21;
	static const uint16_t skillTailoring		= 0x22;
	static const uint16_t skillTaming		= 0x23;
	static const uint16_t skillTasteID		= 0x24;
	static const uint16_t skillTinkering		= 0x25;
	static const uint16_t skillTracking		= 0x26;
	static const uint16_t skillVeterinary		= 0x27;
	static const uint16_t skillSwordsmanship	= 0x28;
	static const uint16_t skillMaceFighting		= 0x29;
	static const uint16_t skillFencing		= 0x2a;
	static const uint16_t skillWrestling		= 0x2b;
	static const uint16_t skillLumberjacking	= 0x2c;
	static const uint16_t skillMining		= 0x2d;
	static const uint16_t skillMeditation		= 0x2e;
	static const uint16_t skillStealth		= 0x2f;
	static const uint16_t skillRemoveTrap		= 0x30;
	static const uint16_t skillNecromancy		= 0x31;

protected:
	uint16_t skills[50];

public:
	inline const uint16_t getSkill(uint16_t skill) const
	{ return skills[skill]; }

	inline void setSkill(uint16_t skill, uint16_t value)
	{ skills[skill] = value; }
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
	{ return horse; }

	inline const bool isMouting(pNPC horse) const
	{ return mounting == horse; }

	inline pChar getHorse() const
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
	{ weight > 0 || calcWeight(); return weight; }
	
	void calcWeight();
	bool overloadedWalking();
	bool overloadedTeleport();
//@}
};

#endif
