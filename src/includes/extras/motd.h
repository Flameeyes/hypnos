/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __EXTRAS_MOTD_H__
#define __EXTRAS_MOTD_H__

#include "common_libs.h"

/*!
\brief Message of the Day and Tips access

This namespace contains functions and variables to get tips and message of
the days from the xml datafile and provide an access to them to Hypnos
emulator.
*/
namespace nMOTD {
	void loadMOTD();
	
	const std::string &getMOTD();
	const std::string &getTip(uint16_t index);
	const uint16_t getTipsCount();
};

#endif
