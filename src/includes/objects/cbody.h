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
class cBody : cObject
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
public:
	//! One handed weapon layer
	static const UI08 layWeapon1H	= 0x01;
	//! Two handed weapon, shield, or misc layer
	static const UI08 layWeapon2H	= 0x02;
	//! Shoes layer
	static const UI08 layShoes	= 0x03;
	//! Pants layer
	static const UI08 layPants	= 0x04;
	//! Shirt layer
	static const UI08 layShirt	= 0x05;
	//! Helm/Hat layer
	static const UI08 layHat	= 0x06;
	//! Gloves layer
	static const UI08 layGloves	= 0x07;
	//! Ring layer
	static const UI08 layRing	= 0x08;
	//! Unused layer
	static const UI08 layUnused	= 0x09;
	//! Neck layer
	static const UI08 layNeck	= 0x0A;
	//! Hair layer
	static const UI08 layHair	= 0x0B;
	//! Waist (half apron) layer
	static const UI08 layWaist	= 0x0C;
	//! Torso (inner) (chest armor) layer
	static const UI08 layTorsoInner	= 0x0D;
	//! Bracelet layer
	static const UI08 layBracelet	= 0x0E;
	//! Unused (backpack?) layer
	static const UI08 layUnused2	= 0x0F;
	//! Facial Hair (Beard) layer
	static const UI08 layBeard	= 0x10;
	//! Torso (middle) (sircoat, tunic, full apron, sash) layer
	static const UI08 layTorsoMiddle= 0x11;
	//! Earrings layer
	static const UI08 layEarrings	= 0x12;
	//! Arms layer
	static const UI08 layArms	= 0x13;
	//! Back (cloack) layer
	static const UI08 layBack	= 0x14;
	//! Backpack layer
	static const UI08 layBackpack	= 0x15;
	//! Torso (outer) (robe) layer
	static const UI08 layTorsoOuter	= 0x16;
	//! Legs (outer) (skirt/kilt) layer
	static const UI08 layLegsOuter	= 0x17;
	//! Legs (inner) (leg armor) layer
	static const UI08 layLegsInner	= 0x18;
	//! Mount (horse, ostarard, etc) layer
	static const UI08 layMount	= 0x19;
	//! NPC Buy Restock container layer (see cNPC::restock)
	static const UI08 layNPCBuyR	= 0x1A;
	//! NPC Buy no-restock container layer (see cNPC::bought)
	static const UI08 layNPCBuyN	= 0x1B;
	//! NPC Buy Sell container layer (see cNPC::sell)
	static const UI08 laySell	= 0x1C;
	//! PC Bank Box (see cPC::bankBox)
	static const UI08 layBank	= 0x1D;

protected:
	pItem layers[0x1E];
public:
	//! Gets the item on the specified layer
	inline pItem getLayerItem(UI08 layer) const
	{ return layers[layer]; }

	//! Sets the item on the specified layer
	inline pItem setLayerItem(UI08 layer, pItem item)
	{ layers[layer] = item; }

	pItem getBackpack(bool create = false);

	inline const bool isWearing(pItem pi) const
	{ return this == pi->getContainer(); }

	SI32 equip(pItem pi, bool drag = false);
	SI32 unEquip(pItem pi, bool drag = false);

	void checkEquipement();
//@}

//@{
/*!
\name Flags
\brief Flags used by the body
*/
public:
	//! Character is Female
	static const UI32 flagFemale		= 0x0000001;
//@}

//@{
/*!
\name Skills
\brief Skill stuff
*/
public:
	static const UI16 skillAlchemy	 	= 0x00;
	static const UI16 skillAnatomy		= 0x01;
	static const UI16 skillAnimalLore	= 0x02;
	static const UI16 skillItemID		= 0x03;
	static const UI16 skillArmsLore		= 0x04;
	static const UI16 skillParrying		= 0x05;
	static const UI16 skillBagging		= 0x06;
	static const UI16 skillBlacksmithing	= 0x07;
	static const UI16 skillBowcraft		= 0x08;
	static const UI16 skillPeacemaking	= 0x09;
	static const UI16 skillCamping		= 0x0a;
	static const UI16 skillCarpentry	= 0x0b;
	static const UI16 skillCartography	= 0x0c;
	static const UI16 skillCooking		= 0x0d;
	static const UI16 skillDetectHidden	= 0x0e;
	static const UI16 skillEnticement	= 0x0f;
	static const UI16 skillEvalutateIntell	= 0x10;
	static const UI16 skillHealing		= 0x11;
	static const UI16 skillFishing		= 0x12;
	static const UI16 skillForensic		= 0x13;
	static const UI16 skillHerding		= 0x14;
	static const UI16 skillHiding		= 0x15;
	static const UI16 skillProvocation	= 0x16;
	static const UI16 skillInscription	= 0x17;
	static const UI16 skillLockpicking	= 0x18;
	static const UI16 skillMagery		= 0x19;
	static const UI16 skillMagicResistance	= 0x1a;
	static const UI16 skillTactics		= 0x1b;
	static const UI16 skillSnooping		= 0x1c;
	static const UI16 skillMusicianship	= 0x1d;
	static const UI16 skillPoisoning	= 0x1e;
	static const UI16 skillArchery		= 0x1f;
	static const UI16 skillSpiritSpeak	= 0x20;
	static const UI16 skillStealing		= 0x21;
	static const UI16 skillTailoring	= 0x22;
	static const UI16 skillTaming		= 0x23;
	static const UI16 skillTasteID		= 0x24;
	static const UI16 skillTinkering	= 0x25;
	static const UI16 skillTracking		= 0x26;
	static const UI16 skillVeterinary	= 0x27;
	static const UI16 skillSwordsmanship	= 0x28;
	static const UI16 skillMaceFighting	= 0x29;
	static const UI16 skillFencing		= 0x2a;
	static const UI16 skillWrestling	= 0x2b;
	static const UI16 skillLumberjacking	= 0x2c;
	static const UI16 skillMining		= 0x2d;
	static const UI16 skillMeditation	= 0x2e;
	static const UI16 skillStealth		= 0x2f;
	static const UI16 skillRemoveTrap	= 0x30;
	static const UI16 skillNecromancy	= 0x31;

protected:
	UI16 skills[50];

public:
	inline const UI16 getSkill(UI16 skill) const
	{ return skills[skill]; }

	inline void setSkill(UI16 skill, UI16 value)
	{ skills[skill] = value; }
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

        UI16 skinColor;         //!< Skin color of the character
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

        //! Gets the skin color of the character
	inline const UI16 getSkinColor() const
	{ return skinColor; }

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
       	void setSkinColor(UI16 value);
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
	R32 weight; //!< Weight of the body (with all equipment and backpack)
private:
	//! Gets the weight of the body, calculating it when necessary
	inline const R32 getWeight()
	{ weight > 0 || calcWeight(); return weight; }
	
	void calcWeight();
	bool overloadedWalking();
	bool overloadedTeleport();
//@}
};

#endif