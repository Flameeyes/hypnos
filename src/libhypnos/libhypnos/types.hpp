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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDINT_H
	#include <stdint.h>
#elif define HAVE_SYS_TYPES_H
	#includ <sys/types.h>
#else
	#error "Your compiler doesn't support stdint.h header, and your system doesn't " \
		"provide a sys/types.h header, too. Report this to Hypnos maintainers " \
		"and they'll try to make hypnos work for you."
#endif

#ifdef HAVE_SET
	#include <set>
	using std::set;
#elif defined HAVE_SET_H
	#include <set.h>
#endif

#ifdef HAVE_VECTOR
	#include <vector>
	using std::vector;
#elif defined HAVE_VECTOR_H
	#include <vector.h>
#endif

#ifdef HAVE_LIST
	#include <list>
	using std::list;
#elif defined HAVE_LIST_H
	#include <list.h>
#endif

#ifdef HAVE_SLIST
	#include <slist>
	using std::slist;
#elif defined HAVE_EXT_SLIST
	#include <ext/slist>
	using __gnu_cxx::slist;
#elif defined HAVE_SLIST_H
	#include <slist.h>
#else
	// If we don't have available a singly-linked list, failback to
	// standard lists.
	template <typename T> typedef list<T> slist<T>;
#endif

#ifdef HAVE_STRING
	#include <string>
	using std::string;
	using std::basic_string;
#elif defined HAVE_STRING_H
	#include <string.h>
#endif

#ifdef HAVE_MAP
	#include <map>
	using std::map;
#elif defined HAVE_MAP_H
	#include <map.h>
#endif

#ifdef HAVE_HASH_MAP
	#include <hash_map>
	using std::hash_map;
#elif defined HAVE_EXT_HASH_MAP
	#include <ext/hash_map>
	using __gnu_cxx::hash_map;
#elif defined HAVE_HASH_MAP_H
	#include <hash_map.h>
#else
	// If we don't have available the hashed maps, failback to
	// standard maps.
	template <typename T> typedef map<T> hash_map<T>;
#endif

#ifdef HAVE_QUEUE
	#include <queue>
	using std::queue;
#elif defined HAVE_QUEUE_H
	#include <queue.h>
#endif

namespace nLibhypnos {
	typedef class cVariant *pVariant;	//!< Pointer to a variant instance
	typedef vector<cVariant> cVariantVector;	//!< Vector of variants
}

typedef set<uint32_t> uint32_set;		//!< A set of uint32_t
typedef vector<uint32_t> uint32_vector;	//!< A vector of uint32_t
typedef slist<uint32_t> uint32_slist;	//!< Singly-linked list of uint32_t

typedef vector<string> stringVector;	//!< A vector of strings

/*!
\brief Position offsets

This struct represent the offset of items in multi or to move a boat.
The size of the offsets is of only a byte because we don't want multis bigger
than 128 squares :)
*/
struct sPositionOffset {
	int8_t x;	//!< X-coord offset
	int8_t y;	//!< Y-coord offset
	int8_t z;	//!< Z-coord offset
	
	sPositionOffset(int8_t ox = 0, int8_t oy = 0, int8_t oz = 0) :
		x(ox), y(oy), z(oz)
	{ }
};

//! Point in the map
struct sPoint {
	uint16_t x;
	uint16_t y;
	sPoint(uint16_t X = 0, uint16_t Y = 0) : x(X), y(Y) { }
	
	inline sPoint operator +(const sPositionOffset &b) const
	{ return sPoint( x + b.x, y + b.y ); }
};

/*!
\brief Coordinates of (INVALID, INVALID)

This constant is here because we are having a circular dependency of structs.h
and constants.h.
*/
static const sPoint InvalidCoord(0xFFFF, 0xFFFF);

/*!
\brief Rectangle definition

This struct is used to define a rectangle and test if a point is inside it.
It's used in many place, like for example cBoat::step() function to test
if the boat is still into the movement area.
*/
struct sRect {
	sPoint ul;	//!< Upperleft corner
	sPoint br;	//!< Bottomright corner
	sRect();
	sRect(sPoint a, sPoint b);
	sRect(uint16_t ulx, uint16_t uly, uint16_t brx, uint16_t bry);
	
	bool isInside(sPoint p) const;
};

#endif
