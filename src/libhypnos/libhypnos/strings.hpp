/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2003 - NoX-Wizard Project                                  |
| Copyright (c) 2004 - Hypnos Project                                      |
| str(n)casecmp (c) 1996 Alexandre Julliard - Wine Project                 |
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

#include "libhypnos/commons.hpp"
#include "libhypnos/hypstl/string.hpp"
#include "libhypnos/hypstl/vector.hpp"

#ifndef HAVE_ASPRINTF
int asprintf(char **strp, const char *fmt, ...) PRINTF_LIKE(2,3);
int vasprintf(char **strp, const char *fmt, va_list ap);
#endif

#ifndef HAVE_STRLWR
char *strlwr(char *);
#endif

#ifndef HAVE_STRUPR
char *strupr(char *);
#endif

#ifndef HAVE_STRNCASECMP
# ifdef HAVE_STRNICMP
#  define strncasecmp strnicmp
# else
int strncasecmp(const char *str1, const char *str2, size_t n);
# endif
#endif // !defined(HAVE_STRNCASECMP)

#ifndef HAVE_STRCASECMP
# ifdef HAVE_STRICMP
#  define strcasecmp stricmp
# else
int strcasecmp(const char *str1, const char *str2);
# endif
#endif // !defined(HAVE_STRCASECMP)

#ifndef HAVE_BASENAME
char *basename(char *path);
#endif

namespace nLibhypnos {

inline string toString(int value)
{
	char *s;
	asprintf(&s, "%d", value);
	string ret(s);
	free(s);
	
	return ret;
}

inline string toString(double value)
{
	char *s;
	asprintf(&s, "%f", value);
	string ret(s);
	free(s);
	
	return ret;
}

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
inline int str2num ( string& s, int base = baseAuto )
{
	return strtol( const_cast< char* >( s.c_str() ), NULL, base );
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
inline int hex2num ( string& s )
{
	return strtol( const_cast< char* >( s.c_str() ), NULL, baseHex );
}
//@}

/*!
\brief Converts an string to a number
\param str string to convert
\return the number represented by the string or 0 if error
\author Flameeyes

This is a mere overload for atoi(char*) function
*/
inline int32_t atoi(string str)
{
	return atoi(str.c_str());
}

uint32_t ip2long(string ip);
string long2ip(uint32_t l);

void strupr(string &str);
void strlwr(string &str);

vector<string> tokenize(string str);

int strtonum(int countx, int base= 0);

string getDateString();

}

#endif
