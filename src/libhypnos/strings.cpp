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
	
	int loopexit=0;
	while ( s != NULL )
	{
		ret.push_back( std::string(s) );
		s=strtok(NULL, " ");
	}
	
	delete tmp;
	
	return ret;
}

}
