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

#include "common_libs.h"
#include "objects/citem/cequippable.h"

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
	static const uint16_t weaponStave1H	= 0x0100; //!< Staff 1 hand
	static const uint16_t weaponStave2H	= 0x0200; //!< Staff 2 hands
	static const uint16_t weaponBow		= 0x0400; //!< Bow
	static const uint16_t weaponXBow	= 0x1000; //!< Crossbow
	static const uint16_t weaponHXBow	= 0x2000; //!< Heavy Crossbow

	static void loadWeaponsInfo();
	static const bool isWeaponLike(uint16_t id, uint16_t type);

protected:
	typedef std::map<uint16_t,uint16_t> WeaponMap;
	//! Map with types of weapons
	static WeaponMap weaponinfo;
//@}

//@{
/*!
\name Test type of weapon
*/
public:
	inline const bool isAxe() const
	{ return isWeaponLike( getId(), AXE1H); }

	inline const bool isSword() const
	{ return isWeaponLike( getId(), SWORD1H ); }

	inline const bool isSwordType() const
	{ return isWeaponLike( getId(), SWORD1H, AXE1H ); }

	inline const bool isMace1H() const
	{ return isWeaponLike( getId(), MACE1H); }

	inline const bool isMace2H() const
	{ return isWeaponLike( getId(), MACE2H); }

	inline const bool isMaceType() const
	{ return isWeaponLike( getId(), MACE1H, MACE2H ); }

	inline const bool isFencing1H() const
	{ return isWeaponLike( getId(), FENCING1H); }

	inline const bool isFencing2H() const
	{ return isWeaponLike( getId(), FENCING2H); }

	inline const bool isFencingType() const
	{ return isWeaponLike( getId(), FENCING1H, FENCING2H ); }

	inline const bool isBow() const
	{ return isWeaponLike( getId(), BOW ); }

	inline const bool isCrossbow() const
	{ return isWeaponLike( getId(), CROSSBOW ); }

	inline const bool isHeavyCrossbow() const
	{ return isWeaponLike( getId(), HEAVYCROSSBOW ); }

	inline const bool isBowType() const
	{ return isWeaponLike( getId(), BOW, CROSSBOW, HEAVYCROSSBOW ); }
	
	inline const bool isStave() const
	{ return isWeaponLike( getId(), STAVE1H, STAVE2H ); }

	//! -Fraz- The OSI macing weapons that do stamina and armor damage 2handed only
	inline const bool IsSpecialMace() const
	{ return isWeaponLike( getId(), STAVE1H, STAVE2H, MACE2H ); }

//@}
};

#endif
