/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "races/raceability.h"

RaceAbilityModifier::RaceAbilityModifier( void )
{
	base = modification = 0;
}

uint32_t RaceAbilityModifier::getBase( void )
{
	return this->base;
}

int RaceAbilityModifier::getModifier( void )
{
	return this->modification;
}

RaceAbilityModifier& RaceAbilityModifier::operator=( const string& that )
{
	sscanf( that.c_str(), "%u %d", &this->base, &this->modification );
	return *this;
}

RaceAbilityModifiers::RaceAbilityModifiers( void )
{
}

RaceAbilityModifier& RaceAbilityModifiers::getAbilityModifier( uint32_t baseSkill )
{
	RaceAbilityModifierMap::iterator it( modifiers.begin() ), end( modifiers.end() );
	while( it != end && it->second.getBase() < baseSkill ) ++it;

	if ( it != end )
		return it->second;
	else
		return dummy;
}

RaceAbilityModifiers& RaceAbilityModifiers::operator+=( RaceAbilityModifier& that )
{
	modifiers[ that.getBase() ] = that;
	return *this;
}

RaceAbilityModifiers& RaceAbilityModifiers::operator=( RaceScriptEntry& that )
{
	cScpIterator* iter = that.script->getNewIterator( that.section );
 	if ( iter == NULL )
		ErrOut( "RaceAbilityModifiers() no %s\n", that.section.c_str() );
	else
	{
		uint32_t 								loopexit=0;
		string 							str;
		RaceAbilityModifier	modifier;
		do
		{
			str = iter->getEntry()->getFullLine();
			if( str[0] != '{' && str[0] != '}' )
			{
				modifier = str;
				*this		+= modifier;
			}
		} while ( ( str[0] != '}' ) && ( ++loopexit < MAXLOOPS ) );
	}
	return *this;
}
