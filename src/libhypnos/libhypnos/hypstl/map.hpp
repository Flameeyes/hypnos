/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator (Utility Library)                              |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in LICENSE file.               |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __LIBHYPNOS_HYPSTL_MAP_HPP__
#define __LIBHYPNOS_HYPSTL_MAP_HPP__

#ifdef HAVE_MAP
	#include <map>
	using std::map;
#elif defined HAVE_MAP_H
	#include <map.h>
#endif

#ifdef HYPNOS_SOURCES

#include "libhypnos/types.hpp"
#include "libhypnos/hypstl/string.hpp"

typedef map< string, class cCommand* > CommandMap;	//!< Map of commands
typedef map<uint32_t, class cMenu> MenuMap;		//!< Map of menus
typedef map<string, class cAccount* > cAccounts;	//!< map of accounts
typedef map<uint32_t, class cSerializable *> SerializableMap;	//!< Map of serializable objects (items, chars..)

#endif

#endif
