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
*/
uint32_t nStrConstants::generic(std::string str)
{
	if ( strcmp(str.c_str(), "sk", 2) == 0 )
		return skills(str);
	if ( strcmp(str.c_str(), "weapon", 6) == 0 )
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
