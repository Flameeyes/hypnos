/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator (Utility Library)                              |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in LICENSE file.               |
|                                                                          |
| Integer limits copyright (C) 1997-2001 Free Software Foundation, Inc.    |
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
	#define __STDC_LIMIT_MACROS
	#include <stdint.h>
#elif defined HAVE_INTTYPES_H
	#include <inttypes.h>
#elif defined HAVE_SYS_TYPES_H
	#includ <sys/types.h>
#else
	#error "Hypnos needs standard integer types, to avoid errors on types' lenghts." \
		"Your platform or your compiler seems not to support them. Please report " \
		"this to Hypnos development team."
#endif

namespace nLibhypnos {
	typedef class cVariant *pVariant;	//!< Pointer to a variant instance
}

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

#ifndef __INT64_C
# if __WORDSIZE == 64
#  define __INT64_C(c)	c ## L
#  define __UINT64_C(c)	c ## UL
# else
#  define __INT64_C(c)	c ## LL
#  define __UINT64_C(c)	c ## ULL
# endif
#endif

// Default headers from solaris clash with GNU handling of 64-bits.
// So we simply remove them.
#if defined sun && defined __GNUC__
# undef UINT64_MAX
# undef INT64_MAX
# undef INT64_MIN
#endif

#ifndef UINT64_MAX
# define UINT64_MAX (__UINT64_C(18446744073709551615))
#endif

#ifndef UINT32_MAX
# define UINT32_MAX 4294967295U
#endif

#ifndef UINT16_MAX
# define UINT16_MAX 65535
#endif

#ifndef UINT8_MAX
# define UINT8_MAX 255
#endif

#ifndef INT64_MAX
# define INT64_MAX (__INT64_C(9223372036854775807))
#endif

#ifndef INT32_MAX
# define INT32_MAX 2147483647ll
#endif

#ifndef INT16_MAX
# define INT16_MAX 632767
#endif

#ifndef INT8_MAX
# define INT8_MAX 127
#endif

#ifndef INT64_MIN
# define INT64_MIN (__INT64_C(9223372036854775807))
#endif

#ifndef INT32_MIN
# define INT32_MIN -(-2147483647-1)
#endif

#ifndef INT16_MIN
# define INT16_MIN 632768
#endif

#ifndef INT8_MIN
# define INT8_MIN -128
#endif

#endif
