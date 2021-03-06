/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief cWeapon declaration
*/

#ifndef _CWEAPON_H__
#define _CWEAPON_H__

#include "common_libs.hpp"
#include "objects/citem/cequippable.hpp"

/*!
\brief Weapon Items
*/
class cWeapon : public cEquippable
{
public:
	cWeapon();
        cWeapon(uint32_t serial);
//@{
/*!
\name Weapon Types
*/
public:
	static const uint16_t weaponInvalid	= 0x0000; //!< Not a weapon
	static const uint16_t weaponSword1H	= 0x0001; //!< Sword 1 hand
	static const uint16_t weaponSword2H	= 0x0002; //!< Sword 2 hands
	static const uint16_t weaponAxe1H	= 0x0004; //!< Axe 1 hand
	static const uint16_t weaponAxe2H	= 0x0008; //!< Axe 2 hands
	static const uint16_t weaponMace1H	= 0x0010; //!< Mace 1 hand
	static const uint16_t weaponMace2H	= 0x0020; //!< Mace 2 hands
	static const uint16_t weaponFenc1H	= 0x0040; //!< Fencing 1 hand
	static const uint16_t weaponFenc2H	= 0x0080; //!< Fencing 2 hands
	static const uint16_t weaponStaff1H	= 0x0100; //!< Staff 1 hand
	static const uint16_t weaponStaff2H	= 0x0200; //!< Staff 2 hands
	static const uint16_t weaponBow		= 0x0400; //!< Bow
	static const uint16_t weaponXBow	= 0x1000; //!< Crossbow
	static const uint16_t weaponHXBow	= 0x2000; //!< Heavy Crossbow

	static void loadWeaponsInfo();
	static bool isWeaponLike(uint16_t id, uint16_t type);

	inline bool isWeaponLike(uint16_t type) const
	{ return isWeaponLike(getId(), type); }
	
	Skill getCombatSkill() const;
protected:
	typedef map<uint16_t,uint16_t> WeaponMap;
	//! Map with types of weapons
	static WeaponMap weaponinfo;		//!< Weapon's information
	Skill combatSkill;			//!< Combat skill used by the weapon
//@}

//@{
/*!
\name Test type of weapon
*/
public:
	inline bool isAxe() const
	{ return isWeaponLike( getId(), weaponAxe1H); }

	inline bool isSword() const
	{ return isWeaponLike( getId(), weaponSword1H ); }

	inline bool isSwordType() const
	{ return isWeaponLike( getId(), weaponSword1H | weaponAxe1H ); }

	inline bool isMace1H() const
	{ return isWeaponLike( getId(), weaponMace1H); }

	inline bool isMace2H() const
	{ return isWeaponLike( getId(), weaponMace2H); }

	inline bool isMaceType() const
	{ return isWeaponLike( getId(), weaponMace1H | weaponMace2H ); }

	inline bool isFencing1H() const
	{ return isWeaponLike( getId(), weaponFenc1H); }

	inline bool isFencing2H() const
	{ return isWeaponLike( getId(), weaponFenc2H); }

	inline bool isFencingType() const
	{ return isWeaponLike( getId(), weaponFenc1H | weaponFenc2H ); }

	inline bool isBow() const
	{ return isWeaponLike( getId(), weaponBow ); }

	inline bool isCrossbow() const
	{ return isWeaponLike( getId(), weaponXBow ); }

	inline bool isHeavyCrossbow() const
	{ return isWeaponLike( getId(), weaponHXBow ); }

	inline bool isBowType() const
	{ return isWeaponLike( getId(), weaponBow | weaponXBow | weaponHXBow ); }
	
	inline bool isStaff() const
	{ return isWeaponLike( getId(), weaponStaff1H | weaponStaff2H ); }

	//! -Fraz- The OSI macing weapons that do stamina and armor damage 2handed only
	inline bool IsSpecialMace() const
	{ return isWeaponLike( getId(), weaponStaff1H | weaponStaff2H | weaponMace2H ); }

//@}
};

#endif
