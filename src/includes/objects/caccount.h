/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Declaration of cAccount class
*/

#ifndef __CACCOUNT_H__
#define __CACCOUNT_H__

#include "common_libs.h"
#include "backend/sqlite.h"
#include "enums.h"
#include <zthread/FastMutex.h>

/*!
\class cAccount caccount.h "objects/caccount.h"
\brief This class represent an account for game

In database we should store:
name password cryptotype privlevel creationdate ban_author banReleaseTime
jailtime lastconn_ip lastconn_time char1 char2 char3 char4 char5 lastchar
*/
class cAccount
{
protected:
	static ZThread::FastMutex global_mt;
		//!< Global mutex used while saving or loading

	static cAccounts accounts;			//!< All the accounts [Hash-map]

public:
	static void saveAll();				//!< Globally save accounts
	static void loadAll();				//!< Globally load accounts

	static pAccount findAccount(std::string name);

protected:
	ZThread::FastMutex local_mt;			//!< Local mutex, used for logins and other things
	std::string name; 				//!< Account name
	std::string password;				//!< Account password (crypted)
	CryptoType ctype;				//!< Type of crypted password
	uint8_t privlevel;				//!< Priviledge level
	int32_t creationdate;				//!< Epoch of creation date
	pPC banAuthor;					//!< Ban Author (if banned)
	int32_t banReleaseTime;				//!< Epoch of release time of ban
	int32_t jailtime;				//!< Epoch of jail's release time
	uint32_t lastConnIP;				//!< Last connection IP
	int32_t lastConnTime;				//!< Last connection epoch

	PCVector chars;					//!< Characters of the account
	pPC lastchar;					//!< Last character used

	pClient client;					//!< Client in use
public:
	inline const uint8_t getCharsNumber() const;

	inline uint8_t addCharToAccount(pPC pc);
	void save();
        pPC getChar(uint8_t index);

	//! Returns the current client connected or NULL if not connected
	inline pClient currClient() const
	{ return client; }

	//! Gets the account name
	inline const std::string getName() const
	{ return name; }
	
//@{
/*!
\name Account Flags
*/
protected:
	uint8_t flags;					//!< Flags of the account
	
	static const uint8_t flagSeeGMPages	= 0x01;
	static const uint8_t flagSeeConsPages	= 0x02;
	
	inline void setFlag(uint8_t flag, bool on)
	{
		if ( on )
			flags |= flag;
		else
			flags &= ~flag;
	}
public:
	//! Gets if the account can see GM Pages
	inline const bool seeGMPages() const
	{ return flags & flagSeeGMPages; }
	
	//! (Un)sets the capability to see GM Pages
	inline void setSeeGMPages(bool on = true)
	{ setFlag(flagSeeGMPages, on); }
	
	//! Gets if the account can see Cons pages
	inline const bool seeConsPages() const
	{ return flags & flagSeeConsPages; }
	
	//! (Un)sets the capability to see Cons Pages
	inline void setSeeConsPages(bool on = true)
	{ setFlag(flagSeeConsPages, on); }
//@}

	cAccount(std::string aName, std::string aPassword);
		//!< Default constructor
	cAccount(cSQLite::cSQLiteQuery::tRow row);
		//!< Constructor with database row
	~cAccount();
		//!< Default destructor
};

#endif
