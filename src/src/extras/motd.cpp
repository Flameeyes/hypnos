/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "extras/motd.h"

namespace nMOTD {
	std::string motd;	//!< Message of the day
	stringVector tips;	//!< Tips for the players
};

/*!
\brief Loads MOTD and Tips data from motd.xml configuration file.
*/
void loadMOTD()
{
	bool motdLoaded = false;

	ConOut("Loading MOTD and Tips data...\t\t");
	
	std::ifstream xmlfile("config/motd.xml");
	try {
		MXML::Document doc(xmlfile);
		
		MXML::Node *n = doc.main()->child();
		if ( ! n ) return;
		do {
			if ( n->name() == "motd" )
			{
				if ( !motdLoaded )
					motd = n->data();
				else
				{
					LogWarning("Double motd element in motd.xml, ignoring...");
					continue;
				}
			} else if ( n->name() == "tip" )
				tip.push_back(n->data());
			else
			{
				LogWarning("Unknown element in motd.xml: %s", n->name().c_str());
				continue;
			}
			
		} while( (n = n->next()) );
		ConOut("[   OK   ]\n");
	} catch ( MXML::MalformedError e) {
		ConOut("[ Failed ]\n");
		LogCritical("motd.xml file not well formed.");
	}
}

/*!
\brief Gets the Message of the Day for the server
\return The string load from motd.xml
*/
const std::string &nMOTD::getMOTD()
{
	return motd;
}

/*!
\brief Gets the given tip
\param index Index of the tip to get
\return The index-tip load from motd.xml
*/
const std::string &nMOTD::getTip(uint16_t index)
{
	return tips[index];
}

//! Gets the number of tips presents
const uint16_t nMOTD::getTipsCount()
{
}
