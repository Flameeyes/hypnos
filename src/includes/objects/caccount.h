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

class cAccount;
typedef cAccount *pAccount;				//!< Pointer to an account

typedef std::hash_map<std::list, cAccount> cAccounts;	//!< Hashed map of accounts

#include "common_libs.h"
#include "objecs/cpc.h"

/*!
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

	static cAccounts accounts;
		//!< All the accounts [Hash-map]

public:
	static void saveAll(std::string filename);
		//!< Globally save accounts
	static void loadAll(std::string filename);
		//!< Globally load accounts


	//! Type of crypto-algorithms
	enum CryptoType {
		cryptoPlain,	//! Plain text
		cryptoMD5,	//! MD5 hash
		cryptoSHA1	//! SHA1 hash
	};

protected:
	ZThread::FastMutex local_mt;
		//!< Local mutex, used for logins and other things
	std::string name; 		//!< Account name
	std::string password;		//!< Account password (crypted)
	CryptoType ctype;		//!< Type of crypted password
	uint8_t privlevel;		//!< Priviledge level
	int32_t creationdate;		//!< Epoch of creation date
	pPC banAuthor;			//!< Ban Author (if banned)
	int32_t banReleaseTime;		//!< Epoch of release time of ban
	int32_t jailtime;			//!< Epoch of jail's release time
	uint32_t lastConnIP;		//!< Last connection IP
	int32_t lastConnTime;		//!< Last connection epoch

	PCList chars;			//!< Characters of the account
	pPC lastchar;			//!< Last character used

	pPC currentChar;		//!< Current char used ingame
	
	uint8_t flags;			//!< Flags of the account
	
	inline void setFlag(uint8_t flag, bool on)
	{
		if ( on )
			flags |= flag;
		else
			flags &= ~flag;
	}
public:
//@{
/*!
\name Account Flags
\brief Flags present on an account
*/
	static const uint8_t flagSeeGMPages	= 0x01;
	static const uint8_t flagSeeConsPages	= 0x02;
	
	//! Gets if the account can see GM Pages
	inline const bool seeGMPages() const
	{ return flags & flagSeeGMPages; }
	
	//! (Un)sets the capability to see GM Pages
	inline void setSeeGMPages(bool on = true)
	{ setFlag(flagSeeGMPages, on); }
	
	//! Gets if the account can see Cons pages
	inline const bool seeConsPages const
	{ return flags & flagSeeConsPages; }
	
	//! (Un)sets the capability to see Cons Pages
	inline void setSeeConsPages(bool on = true)
	{ setFlag(flagSeeConPages, on); }
//@}

	cAccount();
		//!< Default constructor
	cAccount(cSQLite::cSQLiteQuery::tRow row);
		//!< Constructor with database row
	~cAccount();
		//!< Default destructor

        inline const uint8_t getCharsNumber() const                //!< Returns number of characters in account
        { return chars.size() }

        inline void addChartoAccount(pPC pc) const              //!< Adds pc to account
        { chars.push_back(pc) }

	void save();

	inline const bool inGame() const			//!< Returns if an account is in game
	{ return currentChar; }

        pPC getChar(int index);		//!< gets char number "index" in player account

};

#endif
