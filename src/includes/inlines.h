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

#include "basics.h"
#include "data.h"

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

inline bool chance(uint8_t percent) { return ( (rand()%100) < percent); }

inline void SetTimerSec( uint32_t *timer, const short seconds)
{
	*timer=seconds * SECS + getclock();
}

inline std::string toString(int value)
{
	char *s;
	asprintf(&s, "%d", value);
	std::string ret(s);
	free(s);
	
	return ret;
}

inline std::string toString(double value)
{
	char *s;
	asprintf(&s, "%f", value);
	std::string ret(s);
	free(s);
	
	return ret;
}

/*!
\brief Convert a string to a wstring
\author Endymion
\param from the source string
\param to the dest wstring
\deprecated This shouldn't be used.. endymion's code warning!
*/
inline void string2wstring( std::string from, std::wstring& to )
{
	to.erase();
	for( std::string::iterator iter = from.begin(); iter != from.end(); iter++ )
		to += static_cast<wchar_t>(*iter);
} DEPRECATED;

/*!
\brief Convert a wstring to a string
\author Endymion
\param from the source wstring
\param to the dest string
\deprecated This shouldn't be used.. endymion's code warning!
*/
inline void wstring2string( std::wstring from, std::string& to )
{
	to.erase();
	for( std::wstring::iterator iter = from.begin(); iter != from.end(); iter++ )
		to += static_cast<char>(*iter);
} DEPRECATED;

/*!
\brief Converts an std::string to a number
\param str string to convert
\return the number represented by the string or 0 if error
\author Flameeyes

This is a mere overload for atoi(char*) function
*/
inline int32_t atoi(std::string str)
{ return atoi(str.c_str()); }

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
inline const int numbitsset( int number )
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
\brief Converts an ip-string to a long value
\param ip ip-string
\param ris integer to store the ip to
\return true if the string is a valid ip, else false

\note this is not a "clean" function, because uses goto
	but it works great!

\author Flameeyes
*/
bool ip2long(char *ip, int &ris)
{
  char buffer[16], *a = NULL, *b = NULL, *c = NULL;
  bool ret = true;

  a = strchr(buffer, '.');
  if ( ! a ) { ret = false; goto end; }
  *(a++) = 0;

  b = strchr(a, '.');
  if ( ! b ) { ret = false; goto end; }
  *(b++) = 0;

  c = strchr(b, '.');
  if ( ! c ) { ret = false; goto end; }
  *(c++) = 0;

  ris = ( atoi(a) << 24 ) + ( atoi(b) << 16 ) + ( atoi(c) << 8 ) + atoi(c);

  end:
    if ( a ) *(--a) = '.';
    if ( b ) *(--b) = '.';
    if ( c ) *(--c) = '.';

    return ret;
}

/*!
\brief Converts a long value into an ip-string
\param l long value
\param out output string - must be prealloccated
\return pointer to the out string

\author Flameeyes
*/
char *long2ip(int l, char *out)
{
  char a = l >> 24, b = ( l >> 16 ) & 0xFF,
       c = ( l >> 8 ) & 0xFF, d = l & 0xFF;
  sprintf(out, "%u.%u.%u.%u", a, b, c, d);
  return out;
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
*/
bool isValidCoord( sPoint p )
{
	return p.x < map_width*8 && p.y < map_height*8;
}

/*!
\brief Calculates the adjacent direction (counterclockwise)
\param dir initial direction
\return The adjacent direction
*/
inline const uint8_t getLeftDir(uint8_t dir)
{
	dir &= 7;	// make sure it's valid
	return dir==0 ? 7 : dir-1;
}

/*!
\brief Calculates the adjacent direction (clockwise)
\param dir initial direction
\return The adjacent direction
*/
inline const uint8_t getRightDir(uint8_t dir)
{
	dir &= 7;
	return dir==7 ? 0 : dir+1;
}

#endif //__INLINES_H__
