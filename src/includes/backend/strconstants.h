/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "common_libs.h"
#include "constants.h"
#include "typedefs.h"

#ifndef __STRCONSTANTS_H__
#define __STRCONSTANTS_H__

/*!
\brief Functions for string constants translation

The functions in this namespace converts a string read from xml (or received
from script) into the equivalent c++ constant / enum value.

\note The function is case sensitive
\todo Make it case insensitive, maybe?
*/
namespace nStrConstants {
	Skill skills(std::string str);
	uint16_t weaponsTypes(std::string str);
};

#endif
