/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __EXTRAS_JAILS_H__
#define __EXTRAS_JAILS_H__

#include "common_libs.h"
#include "structs.h"

/*!
\brief Jails access

This namespace contains functions and variables to get jails locations read
from the jails.xml document.
*/
namespace nJails {
	void loadJails();
	
	const sLocation getCurrentJail();
	bool isJailed(pPC pc);
	
	void checkJailed();
	
	void release(pClient client);
	
	/*!
	\brief Info about a jail for an account or a player
	*/
	class cJailInfo
	{
	protected:
		pPC author;
		string reason;
		uint32_t releasetime;
	public:
		//! Gets the jail's author
		inline pPC getAuthor() const
		{ return author; }
		
		//! Gets the jail's reason
		inline const string &getReason() const
		{ return reason; }
		
		//! Gets the jail's release time
		inline uint32_t getReleaseTime() const
		{ return releasetime; }
		
		cJailInfo(pPC auth, string &reas, uint32_t release) :
			author(auth), reason(reas), releasetime(release)
		{ }
	};
	
	//! Info about the position of a jailed character
	struct sJailPositions
	{
		sLocation original;	//!< Location where the character should be released
		sLocation jailed;	//!< Location where the character is jailed
	};
}

#endif
