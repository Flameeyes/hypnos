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


#ifndef __UTILS_STRINGS_H__
#define __UTILS_STRINGS_H__

#include "common_libs.h"

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


bool ip2long(std::string ip, int &ris);
std::string long2ip(int l);

void strupr(std::string &str);
void strlwr(std::string &str);

stringVector tokenize(std::string str);

int strtonum(int countx, int base= 0);

#endif

