  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*!
\file
\brief Implementation of functions declared on common_libs.h
\note Not all functions declared on common_libs.h are implemented here: many
	of them are system-dependent, so are implemented in the archs.
*/

#define USE_THREADS
#include "common_libs.h"

OSVersion g_OSVer = OSVER_UNKNOWN;

OSVersion getOSVersion()
{
    if (g_OSVer==OSVER_UNKNOWN) {
        getOSVersionString();
    }
    return g_OSVer;
}

/*!
\author Akron
\brief return the start of the line passed
\param line string that represent the line
\return the pointer to the start of the line, chopping out all initial space characters
\since 0.82r3
*/
char *linestart(char *line)
{
	char*t = line;
	while (isspace(*t)) t++;
	return t;
}

/*!
\author Akron
\brief capitalize a c++ string
\param str the string to capitalize
\since 0.82r3
*/
void strupr(std::string &str)
{
	for(std::string::iterator it = str.begin(); it != str.end(); it++)
		if ( islower(*it) )
			*it -= 0x20;
}

/*!
\author Akron
\brief lowerize a c++ string
\param str the string to lowerize
\since 0.82r3
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
stringVector splitline(std::string str)
{
	stringVector ret;
	char *tmp = new char[str.size() +1];
	strncpy(tmp, str.c_str(), str.size()+1);
		
	char *s = strtok(tmp, " ");
	
	int loopexit=0;
	while ( (s!=NULL) && (++loopexit < MAXLOOPS) )
	{
		ret.push_back( std::string(s) );
		s=strtok(NULL, " ");
	}
}

