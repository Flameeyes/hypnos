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
	if ( str == "ipBankbox" ) return ipBankBox;
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
magic::SpellId nStrConstants::spellId(std::string str)
{
	if ( str == "spellClumsy" ) return magic::spellClumsy;
	if ( str == "spellCreateFood" ) return magic::spellCreateFood;
	if ( str == "spellFeebleMind" ) return magic::spellFeebleMind;
	if ( str == "spellHeal" ) return magic::spellHeal;
	if ( str == "spellMagicArrow" ) return magic::spellMagicArrow;
	if ( str == "spellNightSight" ) return magic::spellNightSight;
	if ( str == "spellReactiveArmour" ) return magic::spellReactiveArmour;
	if ( str == "spellWeaken" ) return magic::spellWeaken;
	
	if ( str == "spellAgility" ) return magic::spellAgility;
	if ( str == "spellCunning" ) return magic::spellCunning;
	if ( str == "spellCure" ) return magic::spellCure;
	if ( str == "spellHarm" ) return magic::spellHarm;
	if ( str == "spellTrap" ) return magic::spellTrap;
	if ( str == "spellUntrap" ) return magic::spellUntrap;
	if ( str == "spellProtection" ) return magic::spellProtection;
	if ( str == "spellStrength" ) return magic::spellStrength;
	
	if ( str == "spellBless" ) return magic::spellBless;
	if ( str == "spellFireball" ) return magic::spellFireball;
	if ( str == "spellLock" ) return magic::spellLock;
	if ( str == "spellPoison" ) return magic::spellPoison;
	if ( str == "spellTelekinesys" ) return magic::spellTelekinesys;
	if ( str == "spellTeleport" ) return magic::spellTeleport;
	if ( str == "spellUnlock" ) return magic::spellUnlock;
	if ( str == "spellWallStone" ) return magic::spellWallStone;
	
	if ( str == "spellArchCure" ) return magic::spellArchCure;
	if ( str == "spellArchProtection" ) return magic::spellArchProtection;
	if ( str == "spellCurse" ) return magic::spellCurse;
	if ( str == "spellFireField" ) return magic::spellFireField;
	if ( str == "spellGreatHeal" ) return magic::spellGreatHeal;
	if ( str == "spellLightning" ) return magic::spellLightning;
	if ( str == "spellManaDrain" ) return magic::spellManaDrain;
	if ( str == "spellRecall" ) return magic::spellRecall;
	
	if ( str == "spellBladeSpirit" ) return magic::spellBladeSpirit;
	if ( str == "spellDispelField" ) return magic::spellDispelField;
	if ( str == "spellIncognito" ) return magic::spellIncognito;
	if ( str == "spellReflection" ) return magic::spellReflection;
	if ( str == "spellMindBlast" ) return magic::spellMindBlast;
	if ( str == "spellParalyze" ) return magic::spellParalyze;
	if ( str == "spellPoisonField" ) return magic::spellPoisonField;
	if ( str == "spellSummon" ) return magic::spellSummon;
	
	if ( str == "spellDispel" ) return magic::spellDispel;
	if ( str == "spellEnergyBolt" ) return magic::spellEnergyBolt;
	if ( str == "spellExplosion" ) return magic::spellExplosion;
	if ( str == "spellInvisibility" ) return magic::spellInvisibility;
	if ( str == "spellMark" ) return magic::spellMark;
	if ( str == "spellMassCurse" ) return magic::spellMassCurse;
	if ( str == "spellParalyzeField" ) return magic::spellParalyzeField;
	if ( str == "spellReveal" ) return magic::spellReveal;
	
	if ( str == "spellChainLighting" ) return magic::spellChainLighting;
	if ( str == "spellEnergyField" ) return magic::spellEnergyField;
	if ( str == "spellFlameStrike" ) return magic::spellFlameStrike;
	if ( str == "spellGate" ) return magic::spellGate;
	if ( str == "spellManaVampire" ) return magic::spellManaVampire;
	if ( str == "spellMassDispel" ) return magic::spellMassDispel;
	if ( str == "spellMeteorSwarm" ) return magic::spellMeteorSwarm;
	if ( str == "spellPolymorph" ) return magic::spellPolymorph;
	
	if ( str == "spellEarthquake" ) return magic::spellEarthquake;
	if ( str == "spellEnergyVortex" ) return magic::spellEnergyVortex;
	if ( str == "spellResurrection" ) return magic::spellResurrection;
	if ( str == "spellSummonElemAir" ) return magic::spellSummonElemAir;
	if ( str == "spellSummonDaemon" ) return magic::spellSummonDaemon;
	if ( str == "spellSummonElemEarth" ) return magic::spellSummonElemEarth;
	if ( str == "spellSummonElemFire" ) return magic::spellSummonElemFire;
	if ( str == "spellSummonElemWater" ) return magic::spellSummonElemWater;
	
	return spellInvalid;
}
