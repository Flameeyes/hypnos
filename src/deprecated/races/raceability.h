  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include "nxwcommn.h"
#include "racescriptentry.h"

typedef enum { STRENGTH, DEXTERITY, INTELLIGENCE, CONSTITUTION, WISDOM, CHARISMA } RACIALABILITY;
#define RACIALABILITIES	6

class RaceAbilityModifier
{
	public:
														RaceAbilityModifier( void );
		uint32_t										getBase( void );
		int											getModifier( void );
		RaceAbilityModifier& 		operator=( const string& );
	private:
		uint32_t										base;
		int											modification;
};

typedef map< uint32_t, class RaceAbilityModifier > RaceAbilityModifierMap;

class RaceAbilityModifiers
{
	private:
		RaceAbilityModifierMap	modifiers;
		RaceAbilityModifier			dummy;
	public:
														RaceAbilityModifiers( void );
		RaceAbilityModifier&		getAbilityModifier( uint32_t baseAbility );
		RaceAbilityModifiers&		operator+=( RaceAbilityModifier& that );
		RaceAbilityModifiers&		operator=( RaceScriptEntry& rse );
};
