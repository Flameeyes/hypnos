/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __EXTRAS_JAILS_H__
#define __EXTRAS_JAILS_H__

#include "common_libs.h"
#include "structs.h"

/*!
\brief Jails access

This namespace contains functions and variables to get jails locations read
from the jails.xml document.
*/
namespace nJails {
	void loadJails();
	
	const sLocation getCurrentJail();
};

#endif
