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
typedef cAccount *pAccount;

typedef std::hash_map<std::list, cAccount> cAccounts;

#include <zthread/FastMutex.h>
#include <string>
#include <xercesc/parsers/XercesDOMParser.hpp>

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
	UI08 privlevel;			//!< Priviledge level
	SI32 creationdate;		//!< Epoch of creation date
	pChar banAuthor;		//!< Ban Author (if banned)
	SI32 banReleaseTime;		//!< Epoch of release time of ban
	SI32 jailtime;			//!< Epoch of jail's release time
	UI32 lastConnIP;		//!< Last connection IP
	SI32 lastConnTime;		//!< Last connection epoch

	std::list<pChar> chars;		//!< Characters of the account
	pChar lastchar;			//!< Last character used

	pChar currentChar;		//!< Current char used ingame

public:
	cAccount();
		//!< Default constructor
	cAccount(cSQLite::cSQLiteQuery::tRow row);
		//!< Constructor with database row
	~cAccount();
		//!< Default destructor

        inline const UI08 getCharsNumber() const                //!< Returns number of characters in account
        { return chars.size() }

        inline void addChartoAccount(pPC pc) const              //!< Adds pc to account
        { chars.push_back(pc) }

	void save();

	inline const bool inGame() const			//!< Returns if an accout is in game
	{ return currentChar; }

};

#endif
