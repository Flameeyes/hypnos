/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator (Utility Library)                              |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in LICENSE file.               |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __LIBHYPNOS_HYPSTL_SET_HPP__
#define __LIBHYPNOS_HYPSTL_SET_HPP__

#ifdef HAVE_SET
	#include <set>
	using std::set;
#elif defined HAVE_SET_H
	#include <set.h>
#endif

typedef set<uint32_t> uint32_set;		//!< A set of uint32_t

#endif
