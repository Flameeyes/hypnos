/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Strings' utility functions

In this file we'll declare (and sometimes define) some utility functions used
for managing strings, like converting them and tokenize them.

I moved all that here because they were a big part of inlines.h and we needed
more of them.
*/

#ifndef __LIBHYPNOS_STRINGS_H__
#define __LIBHYPNOS_STRINGS_H__

#include "libhypnos/commons.h"

namespace nLibhypnos {

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

bool ip2long(std::string ip, int &ris);
std::string long2ip(int l);

void strupr(std::string &str);
void strlwr(std::string &str);

stringVector tokenize(std::string str);

int strtonum(int countx, int base= 0);

}

/*!
\brief Converts an std::string to a number
\param str string to convert
\return the number represented by the string or 0 if error
\author Flameeyes

This is a mere overload for atoi(char*) function
*/
inline int32_t atoi(std::string str)
{
	return atoi(str.c_str());
}

#ifdef WIN32
int asprintf(char **strp, const char *fmt, ...) PRINTF_LIKE(2,3)
int vasprintf(char **strp, const char *fmt, va_list ap);
#endif

#ifndef HAVE_STRLWR
char *strlwr(char *);
#endif

#ifndef HAVE_STRUPR
char *strupr(char *);
#endif

#endif
