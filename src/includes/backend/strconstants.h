/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __STRCONSTANTS_H__
#define __STRCONSTANTS_H__

#include "common_libs.h"
#include "constants.h"
#include "typedefs.h"
#include "enums.h"

/*!
\namespace nStrConstants
\brief Functions for string constants translation

When loading datafiles, or when receiving parameters from the scripts, we often
need to translate a string constant (which can be a char* or an std::string
object) and translate it to an integer constant (or more often an enum entry).

To do that, we should compare the string with other strings (constants) until
we found the right value to return. This is done in the functions inside this
namespace, and is nothing more that a series of if( ) which tests the posible
values for a constant.

Because sometime we don't know exactly what a string represent (for example
when moving it out of a temporary variable of a script) we have a generic()
function which tries to figure out which kinds of constant its the one passed
watching at its prefix.

\section prefixes String constant prefixes

\li \b sk Skill index
\li \b weapon Weapon type
\li \b sa SuspectAction values
\li \b ip ItemPlace values
\li \b skill SkillId values 

\see enums.h

\note The function is case sensitive
\todo Make it case insensitive, maybe?
\todo Add the entire list of constant prefixes
*/
namespace nStrConstants {
	uint32_t generic(std::string str);
	
	Skill skills(std::string str);
	uint16_t weaponsTypes(std::string str);
	SuspectAction suspectAction(std::string str);
	ItemPlace itemPlace(std::string str);
	magic::SpellId spellId(std::string str);
};

#endif
