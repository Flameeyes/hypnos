/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
| str(n)casecmp (c) 1996 Alexandre Julliard - Wine Project                 |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "libhypnos/strings.h"
#include "libhypnos/exceptions.h"
#include <sstream>

namespace nLibhypnos {

/*!
\brief Converts an ip-string to a long value
\author Flameeyes
\param ip String representing the IP to convert (in dotted decimal form)
\return The long value of the IP passed (in host endian)
\throw eInvalidIP If the \c ip string is not a valid dotted decimal IP
*/
uint32_t ip2long(std::string ip)
{
	char buffer[16], *a = NULL, *b = NULL, *c = NULL;
	strncpy(buffer, ip.c_str(), 15);
	
	a = strchr(buffer, '.');
	if ( ! a ) throw eInvalidIP(ip);
	*(a++) = 0;
	
	b = strchr(a, '.');
	if ( ! b ) throw eInvalidIP(ip);
	*(b++) = 0;
	
	c = strchr(b, '.');
	if ( ! c ) throw eInvalidIP(ip);
	*(c++) = 0;

	return ( atoi(buffer) << 24 ) + ( atoi(a) << 16 ) +
		( atoi(b) << 8 ) + atoi(c);
}

/*!
\brief Converts a long value into an ip-string
\author Flameeyes
\param l long value
\return The String representing the given IP address value.
*/
std::string long2ip(int l)
{
	uint8_t a = l >> 24, b = ( l >> 16 ) & 0xFF,
		c = ( l >> 8 ) & 0xFF, d = l & 0xFF;
	
	std::ostringstream sout;
	
	sout << a << "." << b << "." << c << "." << d;
	return sout.str();
}

/*!
\author Flameeyes
\brief capitalize a c++ string
\param str the string to capitalize
*/
void strupr(std::string &str)
{
	for(std::string::iterator it = str.begin(); it != str.end(); it++)
		if ( islower(*it) )
			*it -= 0x20;
}

/*!
\author Flameeyes
\brief lowerize a c++ string
\param str the string to lowerize
*/
void strlwr(std::string &str)
{
	for(std::string::iterator it = str.begin(); it != str.end(); it++)
		if ( isupper(*it) )
			*it += 0x20;
}

/*!
\brief Split a single line in a vector by spaces
\author Flameeyes
\param str String to split
\return a stringVector with the tokens in it
*/
stringVector tokenize(std::string str)
{
	stringVector ret;
	char *tmp = new char[str.size() +1];
	strncpy(tmp, str.c_str(), str.size()+1);
		
	char *s = strtok(tmp, " ");
	
	while ( s != NULL )
	{
		ret.push_back( std::string(s) );
		s=strtok(NULL, " ");
	}
	
	delete tmp;
	
	return ret;
}

/*!
\brief Gets a string representing the current date and time for the log
*/
std::string getDateString()
{
	time_t TIME;
	tm* T;
	time(&TIME);
	T = localtime(&TIME);
	
	std::ostringstream sout;
	
	sout 	<< "["	<< std::setw(2) << std::setfill(0) << T->tm_mday
		<< "/"	<< std::setw(2) << std::setfill(0) << T->tm_mon + 1
		<< "/"	<< std::setw(4) << std::setfill(0) << T->tm_year + 1900
		<< " "	<< std::setw(2) << std::setfill(0) << T->tm_hour 
		<< ":"	<< std::setw(2) << std::setfill(0) << T->tm_min
		<< ":"	<< std::setw(2) << std::setfill(0) << T->tm_sec
		<< "]";

	return sout.str();
}

}

#ifndef HAVE_ASPRINTF

#include <wefts_mutex.h>

//@
/*!
\name asprintf and vasprintf stuff
\author Flameeyes
\brief Implementation of asprintf and vasprintf function which aren't present
	in Windows' library
*/

static char as_buffer[4096];
static Wefts::Mutex as_mutex;

int asprintf(char **strp, const char *fmt, ...)
{
	as_mutex.lock();
	va_list argptr;
	va_start( argptr, fmt );
        int retval = vsnprintf( as_buffer, 4096, fmt, argptr );
	va_end( argptr );
	
	if ( retval == -1 )
	{
		as_mutex.unlock();
		return -1;
	}
	if ( retval >= 4096 )
	{
		*strp = (char*)malloc(retval+1);
		va_start( argptr, fmt );
		int retval2 = vsnprintf( *strp, retval+1, fmt, argptr );
		va_end( argptr );
		as_mutex.unlock();
		return retval2;
	}
	
	*strp = (char*)malloc(retval+1);
	strncpy( *strp, as_buffer, retval+1 );
	as_mutex.unlock();
	return retval;
}

int vasprintf(char **strp, const char *fmt, va_list ap)
{
	
	mutex.lock();
        int retval = vsnprintf( as_buffer, 4096, fmt, ap );
	
	if ( retval == -1 )
	{
		as_mutex.unlock();
		return -1;
	}
	if ( retval >= 4096 )
	{
		va_end( ap );
		*strp = (char*)malloc(retval+1);
		int retval2 = vsnprintf( *strp, retval+1, fmt, ap );
		as_mutex.unlock();
		return retval2;
	}
	
	*strp = (char*)malloc(retval+1);
	strncpy( *strp, as_buffer, retval+1 );
	as_mutex.unlock();
	return retval;
}
//@}
#endif

#ifndef HAVE_STRLWR
char *strlwr(char *str) {
  for (unsigned int i=0;i<strlen(str);i++)
    str[i]=tolower(str[i]);
  return str;
}
#endif

#ifndef HAVE_STRUPR
char *strupr(char *str) {
  for (unsigned int i=0;i<strlen(str);i++)
    str[i]=toupper(str[i]);
  return str;
}
#endif

#ifndef HAVE_STRNCASECMP
int strncasecmp( const char *str1, const char *str2, size_t n )
{
    const unsigned char *ustr1 = (const unsigned char *)str1;
    const unsigned char *ustr2 = (const unsigned char *)str2;
    int res;

    if (!n) return 0;
    while ((--n > 0) && *ustr1)
    {
        if ((res = toupper(*ustr1) - toupper(*ustr2))) return res;
        ustr1++;
        ustr2++;
    }
    return toupper(*ustr1) - toupper(*ustr2);
}
#endif // HAVE_STRNCASECMP

#ifndef HAVE_STRCASECMP
int strcasecmp( const char *str1, const char *str2 )
{
    const unsigned char *ustr1 = (const unsigned char *)str1;
    const unsigned char *ustr2 = (const unsigned char *)str2;

    while (*ustr1 && toupper(*ustr1) == toupper(*ustr2)) {
        ustr1++;
        ustr2++;
    }
    return toupper(*ustr1) - toupper(*ustr2);
}
#endif // HAVE_STRCASECMP

#ifndef HAVE_BASENAME
char *basename(char *path)
{
	// ret= end of string path
	char *ret= path+strlen(path);

	// stop on the first '/' or '\' encountered
	while( (*ret!='\\') && (*ret!='/') ) ret--;
	return ++ret;
}
#endif
