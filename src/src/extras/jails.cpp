/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "extras/jails.h"
#include "abstraction/tvariant.h"
#include "logsystem.h"
#include "inlines.h"
#include <mxml.h>
#include <wefts_mutex.h>

namespace nJails {
	LocationList jails;		//!< List of locations usable for jails
	LocationList::iterator currentJail = jails.end();
					//!< Current jail used
	Wefts::Mutex mutex;		//!< Mutex for the load of jails
};

//! Loads the jails data from the jails.xml file
void nJails::loadJails()
{
	mutex.lock();
	bool motdLoaded = false;

	ConOut("Loading jails data...\t\t");
	
	std::ifstream xmlfile("config/jails.xml");
	try {
		MXML::Document doc(xmlfile);
		
		MXML::Node *n = doc.main()->child();
		if ( ! n ) return;
		do {
			if ( n->name() != "jail" )
			{
				LogWarning("Unknown element in jails.xml: %s", n->name().c_str());
				continue;
			}
			
			try {
				sLocation loc;
				loc.x = tVariant( n->getAttribute("x") ).toUInt16();
				loc.y = tVariant( n->getAttribute("y") ).toUInt16();
				loc.z = tVariant( n->getAttribute("z") ).toSInt8();
				
				if ( loc == sLocation(0,0,0) || ! isValidCoord(loc) )
				{
					LogWarning("Invalid jail location %u, %u, %s", loc.x, loc.y, loc.z);
					continue;
				}
				
				jails.push_back(loc);
			} catch ( MXML::NotFoundError e) {
				LogWarning("Incomplete node in jails.xml");
			}
			
		} while( (n = n->next()) );
		
		if ( ! jails.size() )
		{
			ConOut("[ Failed ]\n");
			LogCritical("No valid jails found on jails.xml");
			mutex.unlock();
			return;
		}
		
		currentJail = jails.begin();
		
		ConOut("[   OK   ]\n");
	} catch ( MXML::MalformedError e) {
		ConOut("[ Failed ]\n");
		LogCritical("jails.xml file not well formed.");
	}
	mutex.unlock();
}

const sLocation nJails::getCurrentJail()
{
	mutex.lock();
	
	if ( currentJail == jails.end() && ( ( currentJail = jails.begin() ) == jails.end() ) )
	{
		LogCritical("Trying to jail someone without jails present on jails.xml.. moving it to 10, 10, 0");
		mutex.unlock();
		return sLocation(10, 10, 0);
	}
	
	sLocation ret = *(++currentJail);
	mutex.unlock();
	
	return ret;
}
