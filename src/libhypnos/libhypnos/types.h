/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator (Utility Library)                              |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in LICENSE file.               |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Type definitions for libhypnos

The types defined here (and not in typedefs.h file) are used in all the
libhypnos library and are common to different applications of the suite.
*/

#ifndef __LIBHYPNOS_TYPES_H__
#define __LIBHYPNOS_TYPES_H__

typedef std::set<uint32_t> uint32_set;		//!< A set of uint32_t
typedef std::vector<uint32_t> uint32_vector;	//!< A vector of uint32_t
typedef std::slist<uint32_t> uint32_slist;	//!< Singly-linked list of uint32_t

typedef std::vector<std::string> stringVector;	//!< A vector of strings

#endif
