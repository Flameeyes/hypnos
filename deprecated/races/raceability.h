/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __RACE_ABILITY_H__
#defien __RACE_ABILITY_H__

#include "common_libs.h"
#include "racescriptentry.h"

typedef enum { STRENGTH, DEXTERITY, INTELLIGENCE, CONSTITUTION, WISDOM, CHARISMA } RACIALABILITY;
#define RACIALABILITIES	6

class RaceAbilityModifier
{
public:
	RaceAbilityModifier();
	RaceAbilityModifier(const std::string& str);
	uint32_t getBase();
	int32_t getModifier();
private:
	uint32_t base;
	int32_t modification;
};

typedef map< uint32_t, class RaceAbilityModifier > RaceAbilityModifierMap;

class RaceAbilityModifiers
{
private:
	RaceAbilityModifierMap modifiers;
	RaceAbilityModifier dummy;
public:
	RaceAbilityModifiers();
	RaceAbilityModifier& getAbilityModifier( uint32_t baseAbility );
	RaceAbilityModifiers& operator+=( RaceAbilityModifier& that );
	RaceAbilityModifiers& operator=( RaceScriptEntry& rse );
};

#endif
