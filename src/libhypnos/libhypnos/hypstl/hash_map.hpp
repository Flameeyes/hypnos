/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator (Utility Library)                              |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in LICENSE file.               |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __LIBHYPNOS_HYPSTL_HASH_MAP_HPP__
#define __LIBHYPNOS_HYPSTL_HASH_MAP_HPP__

#ifdef HAVE_HASH_MAP
	#include <hash_map>
	using std::hash_map;
#elif defined HAVE_EXT_HASH_MAP
	#include <ext/hash_map>
	using __gnu_cxx::hash_map;
#elif defined HAVE_STLPORT_HASH_MAP
	#include <stlport/hash_map>
#elif defined HAVE_HASH_MAP_H
	#include <hash_map.h>
#else
	// If we don't have available the hashed maps, failback to
	// standard maps.
	template <typename T> typedef map<T> hash_map<T>;
#endif

#endif
