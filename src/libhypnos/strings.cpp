/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "libhypnos/strings.h"
#include "libhypnos/exceptions.h"

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
	char *tmp;
	asprintf(&tmp, "%u.%u.%u.%u", a, b, c, d);
	
	std::string ret(tmp);
	free(tmp);
	return ret;
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

}

#ifndef HAVE_ASPRINTF

#include "wefts_mutex.h"

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
