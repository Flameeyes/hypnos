/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "backend/strconstants.h"
#include "objects/citem/cweapon.h"

/*!
\brief Translate an unknown string constant into a c++ constant
\param str String representing the constant
\return The C++ constant represented by the string or an undefined value if not
	a valid constant

This functions test the prefix of a string to find out which can be it's mean
and then calls the right function to get the C++ constant.

\note The tests must be done from the shortest to the longest.
*/
uint32_t nStrConstants::generic(std::string str)
{
	if ( strncmp(str.c_str(), "ip", 2) == 0 )
		return itemPlace(str);
	if ( strncmp(str.c_str(), "sk", 2) == 0 )
		return skills(str);
	if ( strncmp(str.c_str(), "sa", 2) == 0 )
		return suspectAction(str);
	if ( strncmp(str.c_str(), "spell", 5) == 0 )
		return spellId(str);
	if ( strncmp(str.c_str(), "weapon", 6) == 0 )
		return weaponsTypes(str);
}

/*!
\brief Translate a sk??? string into the c++ constant
\param str String representing the skill
\return The skill constant or skInvalid if invalid string
*/
Skill nStrConstants::skills(std::string str)
{
	if ( str == "skAlchemy" ) return skAlchemy;
	if ( str == "skAnatomy" ) return skAnatomy;
	if ( str == "skAnimalLore" ) return skAnimalLore;
	if ( str == "skItemID" ) return skItemID;
	if ( str == "skArmsLore" ) return skArmsLore;
	if ( str == "skParrying" ) return skParrying;
	if ( str == "skBegging" ) return skBegging;
	if ( str == "skBlacksmithing" ) return skBlacksmithing;
	if ( str == "skBowcraft" ) return skBowcraft;
	if ( str == "skPeacemaking" ) return skPeacemaking;
	if ( str == "skCamping" ) return skCamping;
	if ( str == "skCarpentry" ) return skCarpentry;
	if ( str == "skCartography" ) return skCartography;
	if ( str == "skCooking" ) return skCooking;
	if ( str == "skDetectingHidden" ) return skDetectingHidden;
	if ( str == "skEnticement" ) return skEnticement;
	if ( str == "skEvaluatingIntelligence" ) return skEvaluatingIntelligence;
	if ( str == "skHealing" ) return skHealing;
	if ( str == "skFishing" ) return skFishing;
	if ( str == "skForensics" ) return skForensics;
	if ( str == "skHerding" ) return skHerding;
	if ( str == "skHiding" ) return skHiding;
	if ( str == "skProvocation" ) return skProvocation;
	if ( str == "skInscription" ) return skInscription;
	if ( str == "skLockPicking" ) return skLockPicking;
	if ( str == "skMagery" ) return skMagery;
	if ( str == "skMagicResistance" ) return skMagicResistance;
	if ( str == "skTactics" ) return skTactics;
	if ( str == "skSnooping" ) return skSnooping;
	if ( str == "skMusicianship" ) return skMusicianship;
	if ( str == "skPoisoning" ) return skPoisoning;
	if ( str == "skArchery" ) return skArchery;
	if ( str == "skSpiritSpeak" ) return skSpiritSpeak;
	if ( str == "skStealing" ) return skStealing;
	if ( str == "skTailoring" ) return skTailoring;
	if ( str == "skTaming" ) return skTaming;
	if ( str == "skTasteID" ) return skTasteID;
	if ( str == "skTinkering" ) return skTinkering;
	if ( str == "skTracking" ) return skTracking;
	if ( str == "skVeterinary" ) return skVeterinary;
	if ( str == "skSwordsmanship" ) return skSwordsmanship;
	if ( str == "skMacefighting" ) return skMacefighting;
	if ( str == "skFencing" ) return skFencing;
	if ( str == "skWrestling" ) return skWrestling;
	if ( str == "skLumberjacking" ) return skLumberjacking;
	if ( str == "skMining" ) return skMining;
	if ( str == "skMeditation" ) return skMeditation;
	if ( str == "skStealth" ) return skStealth;
	if ( str == "skRemoveTraps" ) return skRemoveTraps;
	if ( str == "skTrueSkills" ) return skTrueSkills;
	
	return skInvalid;
}

/*!
\brief Translate a weapon??? string into the c++ constant
\param str String representing the weapon type
\return The weapon type constant or weaponInvalid if invalid string
*/
uint16_t nStrConstants::weaponsTypes(std::string str)
{
	if ( str == "weaponSword1H" ) return cWeapon::weaponSword1H;
	if ( str == "weaponSword2H" ) return cWeapon::weaponSword2H;
	if ( str == "weaponAxe1H" ) return cWeapon::weaponAxe1H;
	if ( str == "weaponAxe2H" ) return cWeapon::weaponAxe2H;
	if ( str == "weaponMace1H" ) return cWeapon::weaponMace1H;
	if ( str == "weaponMace2H" ) return cWeapon::weaponMace2H;
	if ( str == "weaponFenc1H" ) return cWeapon::weaponFenc1H;
	if ( str == "weaponFenc2H" ) return cWeapon::weaponFenc2H;
	if ( str == "weaponStaff1H" ) return cWeapon::weaponStaff1H;
	if ( str == "weaponStaff2H" ) return cWeapon::weaponStaff2H;
	if ( str == "weaponBow" ) return cWeapon::weaponBow;
	if ( str == "weaponXBow" ) return cWeapon::weaponXBow;
	if ( str == "weaponHXBow" ) return cWeapon::weaponHXBow;
	
	return cWeapon::weaponInvalid;
}

/*!
\brief Translate a sa?? string into the enumerated constant
\param str String representing the SuspectAction enum value
\return The SuspectAction enum value represented by the string, or saNormal if
	not a valid string
*/
SuspectAction nStrConstants::suspectAction(std::string str)
{
	if ( str == "saCriminal" ) return saCriminal;
	if ( str == "saGrey" ) return saGrey;
	
	// No need to check for saNormal :)
	return saNormal;
}

/*!
\brief Translates a ip?? string into the enumerated constant
\param str String representing the ItemPlace enum value
\return The ItemPlace enum value represented by the string, or ipBackpack if
	not a valid string
*/
ItemPlace nStrConstants::itemPlace(std::string str)
{
	if ( str == "ipBankbox" ) return ipBankbox;
	if ( str == "ipEquip" ) return ipEquip;
	
	// No need to check for ipBackpack :)
	return ipBackpack;
}

/*!
\brief Tranlates a spell?? string into the enumerated constant
\param str String representing th e SpellId enum value
\return The SpellId enum value represented by the string, or spellInvalid if
	not a valid spell identifier
*/
SpellId nStrConstants::spellId(std::string str)
{
	if ( str == "spellClumsy" ) return spellClumsy;
	if ( str == "spellCreateFood" ) return spellCreateFood;
	if ( str == "spellFeebleMind" ) return spellFeebleMind;
	if ( str == "spellHeal" ) return spellHeal;
	if ( str == "spellMagicArrow" ) return spellMagicArrow;
	if ( str == "spellNightSight" ) return spellNightSight;
	if ( str == "spellReactiveArmour" ) return spellReactiveArmour;
	if ( str == "spellWeaken" ) return spellWeaken;
	
	if ( str == "spellAgility" ) return spellAgility;
	if ( str == "spellCunning" ) return spellCunning;
	if ( str == "spellCure" ) return spellCure;
	if ( str == "spellHarm" ) return spellHarm;
	if ( str == "spellTrap" ) return spellTrap;
	if ( str == "spellUntrap" ) return spellUntrap;
	if ( str == "spellProtection" ) return spellProtection;
	if ( str == "spellStrength" ) return spellStrength;
	
	if ( str == "spellBless" ) return spellBless;
	if ( str == "spellFireball" ) return spellFireball;
	if ( str == "spellLock" ) return spellLock;
	if ( str == "spellPoison" ) return spellPoison;
	if ( str == "spellTelekinesys" ) return spellTelekinesys;
	if ( str == "spellTeleport" ) return spellTeleport;
	if ( str == "spellUnlock" ) return spellUnlock;
	if ( str == "spellWallStone" ) return spellWallStone;
	
	if ( str == "spellArchCure" ) return spellArchCure;
	if ( str == "spellArchProtection" ) return spellArchProtection;
	if ( str == "spellCurse" ) return spellCurse;
	if ( str == "spellFireField" ) return spellFireField;
	if ( str == "spellGreatHeal" ) return spellGreatHeal;
	if ( str == "spellLightning" ) return spellLightning;
	if ( str == "spellManaDrain" ) return spellManaDrain;
	if ( str == "spellRecall" ) return spellRecall;
	
	if ( str == "spellBladeSpirit" ) return spellBladeSpirit;
	if ( str == "spellDispelField" ) return spellDispelField;
	if ( str == "spellIncognito" ) return spellIncognito;
	if ( str == "spellReflection" ) return spellReflection;
	if ( str == "spellMindBlast" ) return spellMindBlast;
	if ( str == "spellParalyze" ) return spellParalyze;
	if ( str == "spellPoisonField" ) return spellPoisonField;
	if ( str == "spellSummon" ) return spellSummon;
	
	if ( str == "spellDispel" ) return spellDispel;
	if ( str == "spellEnergyBolt" ) return spellEnergyBolt;
	if ( str == "spellExplosion" ) return spellExplosion;
	if ( str == "spellInvisibility" ) return spellInvisibility;
	if ( str == "spellMark" ) return spellMark;
	if ( str == "spellMassCurse" ) return spellMassCurse;
	if ( str == "spellParalyzeField" ) return spellParalyzeField;
	if ( str == "spellReveal" ) return spellReveal;
	
	if ( str == "spellChainLighting" ) return spellChainLighting;
	if ( str == "spellEnergyField" ) return spellEnergyField;
	if ( str == "spellFlameStrike" ) return spellFlameStrike;
	if ( str == "spellGate" ) return spellGate;
	if ( str == "spellManaVampire" ) return spellManaVampire;
	if ( str == "spellMassDispel" ) return spellMassDispel;
	if ( str == "spellMeteorSwarm" ) return spellMeteorSwarm;
	if ( str == "spellPolymorph" ) return spellPolymorph;
	
	if ( str == "spellEarthquake" ) return spellEarthquake;
	if ( str == "spellEnergyVortex" ) return spellEnergyVortex;
	if ( str == "spellResurrection" ) return spellResurrection;
	if ( str == "spellSummonElemAir" ) return spellSummonElemAir;
	if ( str == "spellSummonDaemon" ) return spellSummonDaemon;
	if ( str == "spellSummonElemEarth" ) return spellSummonElemEarth;
	if ( str == "spellSummonElemFire" ) return spellSummonElemFire;
	if ( str == "spellSummonElemWater" ) return spellSummonElemWater;
	
	return spellInvalid;
}
