/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator (Utility Library)                              |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in LICENSE file.               |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __LIBHYPNOS_HYPSTL_STRING_HPP__
#define __LIBHYPNOS_HYPSTL_STRING_HPP__

#ifdef HAVE_STRING
	#include <string>
	using std::string;
	using std::basic_string;
#elif defined HAVE_STRING_H
	#include <string.h>
#endif

typedef basic_string<uint16_t> unistring;	//!< Unicode string

#endif
