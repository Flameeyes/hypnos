/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Declaration of cAccount class
*/

#ifndef __CACCOUNT_H__
#define __CACCOUNT_H__

#include "common_libs.h"
#include "extras/jails.h"
#include "enums.h"
#include "inlines.h"

#include <wefts_mutex.h>

/*!
\class cAccount caccount.h "objects/caccount.h"
\brief This class represent an account for game
*/
class cAccount
{
protected:
	static Wefts::Mutex global_mt;
		//!< Global mutex used while saving or loading

	static cAccounts accounts;		//!< All the accounts [Hash-map]

public:
	static void saveAll();			//!< Globally save accounts
	static void loadAll();			//!< Globally load accounts

	static pAccount findAccount(std::string name);

protected:
	Wefts::Mutex local_mt;			//!< Local mutex, used for logins and other things
	std::string name; 			//!< Account name
	std::string password;			//!< Account password (crypted)
	CryptoType ctype;			//!< Type of crypted password
	uint8_t privlevel;			//!< Priviledge level
	int32_t creationdate;			//!< Epoch of creation date
	pPC banAuthor;				//!< Ban Author (if banned)
	int32_t banReleaseTime;			//!< Epoch of release time of ban
	uint32_t lastConnIP;			//!< Last connection IP
	int32_t lastConnTime;			//!< Last connection epoch

	PCVector chars;				//!< Characters of the account
	pPC lastchar;				//!< Last character used

	pClient client;				//!< Client in use
public:
	const uint8_t getCharsNumber() const;

	inline uint8_t addCharToAccount(pPC pc);
	void save(uint32_t id);
        pPC getChar(uint8_t index);

	//! Returns the current client connected or NULL if not connected
	inline pClient currClient() const
	{ return client; }

	//! Gets the account name
	inline const std::string getName() const
	{ return name; }
	
	//! Gets the account priviledge level
	inline const int8_t getLevel() const
	{ return privlevel; }
	
//@{
/*!
\name Account Flags
*/
protected:
	uint8_t flags;					//!< Flags of the account
	
	static const uint8_t flagSeeGMPages	= 0x01;
	static const uint8_t flagSeeConsPages	= 0x02;
public:
	//! Gets if the account can see GM Pages
	inline const bool seeGMPages() const
	{ return flags & flagSeeGMPages; }
	
	//! (Un)sets the capability to see GM Pages
	inline void setSeeGMPages(bool on = true)
	{ setFlag(flags, flagSeeGMPages, on); }
	
	//! Gets if the account can see Cons pages
	inline const bool seeConsPages() const
	{ return flags & flagSeeConsPages; }
	
	//! (Un)sets the capability to see Cons Pages
	inline void setSeeConsPages(bool on = true)
	{ setFlag(flags, flagSeeConsPages, on); }
//@}

	cAccount(std::string aName, std::string aPassword);
		//!< Default constructor
	~cAccount();
		//!< Default destructor

//@{
/*!
\name Jail stuff
\see nJails namespace
*/
protected:
	nJails::cJailInfo *jailInfo;		//!< Pointer to the instance which describes the jail
public:
	//! Gets the pointer to the jail info
	inline nJails::cJailInfo *getJailInfo() const
	{ return jailInfo; }
	
	//! Sets the pointer to the jail info
	inline void setJailInfo(nJails::cJailInfo *info)
	{ jailInfo = info; }
//@}

};

#endif
