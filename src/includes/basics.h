/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Header for conversion functions
\note documentation ported to doxygen by Akron
*/
#ifndef __BASICS_H__
#define __BASICS_H__

#include "common_libs.h"
#include "structs.h"

//@{
/*!
\author Lord Binary
\name Wrappers for stdlib num-2-str functions
*/

/*!
\brief convert the integer into a string in decimal base
\param i integer to convert
\param ourstring string to write the number to
*/
inline void numtostr(uint32_t i, char *ourstring)
{
	sprintf(ourstring,"%d",i);
}

/*!
\brief convert the integer into a string in hexadecimal base
\param i integer to convert
\param ourstring string to write the number to
*/
inline void hextostr(uint32_t i, char *ourstring)
{
	sprintf (ourstring, "%x",i);
}
//@}

//@{
/*!
\name Bases
\brief bases for conversion from string to number
*/
static const int baseInArray = -1;
static const int baseAuto = 0;
static const int baseBin = 2;
static const int baseOct = 8;
static const int baseDec = 10;
static const int baseHex = 16;
//@}

//@{
/*!
\name Wrappers
\author Sparhawk
\brief Wrappers for c++ strings
*/

/*!
\author Sparhawk
\brief wrapper to str2num function
\param s string that represent the number
\param base base in which the number is (see Bases)
\return the number represented by the string
*/
inline int str2num ( std::string& s, int base = baseAuto )
{
	return strtol( const_cast< char* >( s.c_str() ), NULL, base );
}

/*!
\brief Wrapper to str2num function
\author Endymion
\param s wstring that represent the number
\param base base in which the number is (see Bases)
\return the number represented by the string
*/
inline int str2num ( std::wstring& s, int base = baseAuto )
{
	return wcstol( const_cast< wchar_t* >(s.c_str()), NULL, base );
}

/*!
\brief convert a char* into a number with the specified base
\author Xanathar
\return int the number or 0 if no conversion possible
\param sz the string
\param base number's base
\deprecated After removed the dummy pointer, replace in the sources the
            str2num call with a strtol direct call...
*/
inline int str2num( char* sz, int base = baseAuto )
{
	return strtol(sz, NULL, base );
}

/*!
\brief Convert a wchar_t* into a number with the specified base
\author Endymion
\return int the number or 0 if no conversion possible
\param sz the string
\param base number's base
\deprecated After removed the dummy pointer, replace in the sources the
            str2num call with a wcstol direct call...
*/
inline int str2num( wchar_t* sz, int base )
{
	return wcstol(sz, NULL, base );
}

/*!
\author Xanathar
\brief New style hexstring to number
\param sz the hexstring
\return the number represented by the string
*/
inline int hex2num (char *sz)
{
	return strtol(sz, NULL, baseHex);
}

/*!
\author Sparhawk
\brief wrapper to str2num function
\param s the hexstring
\return the number represented by the string
*/
inline int hex2num ( std::string& s )
{
	return strtol( const_cast< char* >( s.c_str() ), NULL, baseHex );
}
//@}

// double dist( sLocation a, sLocation b, bool countZ = !server_data.disable_z_checking);
//!\todo Reinsert the configurable option
const double dist( sLocation a, sLocation b, bool countZ = true);

/*!
\brief Check if a location is in the range of another one
\param a Base location to check
\param b sLocation to check if in range
\param range range to use
\deprecated No one is using it, maybe should be removed...
*/
inline const bool inRange(const sLocation a, const sLocation b, const uint16_t range = VISRANGE)
{
	return (dist(a,b)<= range);
}

int fillIntArray(char* str, int *array, int maxsize, int defval = -1, int base = baseAuto);
void readSplitted(FILE* F, char* script1, char* script2);
int RandomNum(int nLowNum, int nHighNum);
char *RealTime(char *time_str);

#endif //__BASICS_H__
