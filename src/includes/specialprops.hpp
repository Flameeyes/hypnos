/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Special properties handling system [UO4]

This file contains data structures definition and functions declartion for
handle the special properties of item introduced with UO:AoS.

We store the magical properties in different structures, because not every one
of them are always present in items.
*/

#ifndef __SPECIALPROPS_H__
#define __SPECIALPROPS_H__

#include "common_libs.hpp"
#include "enums.hpp"
#include "libhypnos/hypstl/slist.hpp"

/*!
\brief Resistances

This struct contains the values for the resistances of items in UO:AoS.
Items which can have resistances are armors (obvioulsy), weapons and jewelry.

Note: also bodies has resistance
*/
struct sResists {
	int8_t physical;	//!< Physical resist (was AR)
	int8_t fire;		//!< Fire resist
	int8_t energy;		//!< Energy resist
	int8_t cold;		//!< Cold resist
	int8_t poison;		//!< Poison resist
};

/*!
\brief Hit spell effect

This structure contains a pair of spell/chance values to define the chance of
casting a spell during hit of the weapon.
*/
struct sHitSpell {
	int8_t chance;	//!< Chance to cast the spell
	SpellId spell;	//!< Spell to be casted
};

/*!
\brief Hit area damage

This structure contains a pair of damage type/chance values to define the
chance of making an area damage type during hit of the weapon.
*/
struct sHitAreaDamage {
	int8_t chance;	//!< Chance to make the damage
	DamageType type;//!< Type of damage done
	
};

/*!
\brief Hit effects

This struct contains properties which modifies weapon base properties, such as
hit effects, spell effects, and other properties.
*/
struct sWeaponModifiers {
	/*!
	\brief Swing speed increase
	
	This property increases the weapon's swing rate by a percentage,
	allowing the wielder to swing more frequently in combat.
	*/
	int8_t swingSpeed;
	
	/*!
	\brief Hit chance increase
	
	Increases the player's chance to hit a target with wrestling, melee and
	ranged weapons. (The maximum effect you can get from this property is
	capped at 45% by default).
	*/
	int8_t hitChance;
	
	/*!
	\brief Hit stamina leech
	
	Each time the weapon hits, it will do its normal damage. Then the
	weapon leeches a percentage of the damage done from the target's
	Stamina, transferring the leeched Stamina to the weapon's wielder.
	*/
	int8_t leechStamina;
	
	/*!
	\brief Hit mana leech
	
	Each time the weapon hits, it will do its normal damage. Then the
	weapon leeches a percentage of the damage done from the target's Mana,
	transferring the leeched Mana to the weapon's wielder.
	*/
	int8_t leechMana;
	
	/*!
	\brief Hit life leech
	
	Each time the weapon hits, it will do its normal damage. Then the
	weapon leeches a percentage of the damage done from the target's Hit
	Points, transferring the leeched Hit Points to the weapon's wielder.
	*/
	int8_t leechLife;
	
	/*!
	\brief Hit lower attack
	
	A successful hit with this weapon temporarily lowers the attack rating
	of the target by a certain percentage.
	*/
	int8_t lowerAttack;
	
	/*!
	\brief Hit lower defense
	
	A successful hit with this weapon temporarily lowers the defense rating
	of the target by a certain percentage. 
	*/
	int8_t lowerDefense;
	
	/*!
	\brief List of hit spell effects
	
	Each time a weapon with a spell effect property hits a target, it has a
	chance (given as a percentage) of firing the defined spell effect on
	that target. Hit Spell Effect items have unlimited uses.
	
	\see sHitSpell
	*/
	slist<sHitSpell> spellEffects;
	
	/*!
	\brief Hit area damage chance
	
	Like Hit Spell Effects, this property give weapons a chance to cause
	damage in an area of effect around the wielder. The damage done by
	this effect only harms targets the wielder can legally attack, so there
	is no worry of accidentally harming an innocent.
	
	\see sHitAreaDamage
	*/
	slist<sHitAreaDamage> areaDamages;
	
	/*!
	\brief Use best weapon skill
	
	Uses the player's highest weapon skill to attack with the weapon,
	instead of the specific skill needed for the weapon.
	*/
	bool bestSkill;
	
	/*!
	\brief Mage weapon
	
	Uses the player's Magery skill as a combat skill for the weapon (i.e.:
	Magery used in lieu of Swordsmanship for a sword). However, the
	wielder's Magery skill is lowered while wielding such a weapon. Note
	that using Special Moves associated with the weapon will still require
	the wielder to have the required amount of skill points in the actual
	weapon skill.
	
	This is a tristate int8_t variable (i.e.: a pointer), if the weapon
	is a mage weapon, this is not null and points to the relative modifier
	to sum to the magery skill of the caster.
	*/
	int8_t *mageWeapon;
	
	/*!
	\brief Constructor
	
	The constructor is needed to initialize the tristate variables.
	*/
	sWeaponModifiers() :
		mageWeapon(NULL)
	{
	}
	
	/*!
	\brief Destructor
	
	The destructor is needed to delete the tristat variables which can be
	initialized.
	*/
	~sWeaponModifiers()
	{
		delete mageWeapon;
	}
};

#endif
