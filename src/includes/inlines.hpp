/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Inline functions of misc utility
*/

#ifndef __INLINES_H__
#define __INLINES_H__

#include "common_libs.hpp"
#include "data.hpp"
#include "constants.hpp"

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#elif defined (HAVE_WINSOCK_H)
#include <winsock.h>
#endif

template<typename T> inline T qmax(T a, T b) { return a > b ? a : b; }
template<typename T> inline T qmin(T a, T b) { return a < b ? a : b; }

template<typename T> inline bool between(T val, T min, T max)
{
	if ( qmax(min, max) == min ) qswap(min, max); 
	return val >= min && val <= max;
}

template<typename T> inline void safedelete(T*& p) { delete p; p = NULL; }
template<typename T> inline void safedeletearray(T*& p) { delete[] p; p = NULL; }
template<typename T> inline void qswap(T& a, T& b) { T dummy; dummy = a; a = b; b = dummy; }

/*!
\brief Sets/unsets a flag in a flags variable
\param[out] flags Reference to the flags variable
\param flag To set/unset
\param on If true the flag will be set, else unset

This function is used by many classes to sets the internal flags. Original
implementation used to create an internal setFlag() function to use, but using\
this we will have less functions inside the classes.
*/
template<typename T> inline void setFlag(T &flags, T flag, bool on)
{
	if ( on )
		flags |= flag;
	else
		flags &= ~flag;
}

inline bool chance(uint8_t percent) { return ( (rand()%100) < percent); }

inline void SetTimerSec( uint32_t *timer, const short seconds)
{
	*timer=seconds * SECS + getClockmSecs();
}

/*!
\brief play sound
\param goldtotal Total of gold "moved"
\return soundsfx to play
*/
inline uint16_t goldsfx(uint16_t goldtotal)
{
	uint16_t sound;

	if (goldtotal==1)
		sound = 0x0035;
	else if (goldtotal<6)
		sound = 0x0036;
	else
		sound = 0x0037;

	return sound;
}

/*!
\brief count the number of bit set
\return int number of bit set
\param number the number
\todo is this really usefull ?
*/
inline int numbitsset( int number )
{
	int bitsset = 0;

	while( number )
	{
		bitsset += number&0x1;
		number >>= 1;
	}
	return bitsset;
}

/*!
\author Duke
\brief Calculates a long int from 4 subsequent bytes pointed to by p in network endian
\param p pointer to the 4 subsequent bytes
\return the value of the long found
*/
inline uint32_t LongFromCharPtr(const unsigned char *p)
{
	return ntohl( *(reinterpret_cast<const uint32_t *>(p)) );
}

/*!
\author Duke
\brief Calculates a short int from 2 subsequent bytes pointed to by p in network endian
\param p pointer to the 2 subsequent bytes
\return the value of the short found
*/
inline uint16_t ShortFromCharPtr(const unsigned char *p)
{
	return ntohs( *(reinterpret_cast<const uint16_t *>(p)) );
}

/*!
\author Duke
\brief Stores a long int into 4 subsequent bytes pointed to by p in network endian
\param i value to store
\param p pointer to the char array
*/
inline void LongToCharPtr(const uint32_t i, unsigned char *p)
{
	uint32_t *b = reinterpret_cast<uint32_t*>(p);
	*b = htonl(i);
}

/*!
\author Duke
\brief Stores a short int into 2 subsequent bytes pointed to by p in network endian
\param i value to store
\param p pointer to the char array
*/
inline void ShortToCharPtr(const uint16_t i, unsigned char *p)
{
	uint16_t *b = reinterpret_cast<uint16_t*>(p);
	*b = htons(i);
}

/*!
\brief Tells if a given point is a valid coordinate
\param p Point to test
\return true if the point is in the map, else false

\todo Fix this up with the new map.mul handler
*/
inline bool isValidCoord( sPoint p )
{
	return p.x < map_width*8 && p.y < map_height*8;
}

/*!
\brief Calculates the adjacent direction (counterclockwise)
\param dir initial direction
\return The adjacent direction
*/
inline uint8_t getLeftDir(uint8_t dir)
{
	dir &= 7;	// make sure it's valid
	return dir==0 ? 7 : dir-1;
}

/*!
\brief Calculates the adjacent direction (clockwise)
\param dir initial direction
\return The adjacent direction
*/
inline uint8_t getRightDir(uint8_t dir)
{
	dir &= 7;
	return dir==7 ? 0 : dir+1;
}

#endif //__INLINES_H__
