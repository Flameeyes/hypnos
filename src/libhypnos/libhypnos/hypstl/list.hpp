/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator (Utility Library)                              |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in LICENSE file.               |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __LIBHYPNOS_HYPSTL_LIST_HPP__
#define __LIBHYPNOS_HYPSTL_LIST_HPP__

#ifdef HAVE_LIST
	#include <list>
	using std::list;
#elif defined HAVE_LIST_H
	#include <list.h>
#endif

#ifdef HYPNOS_SOURCES
typedef list<class cGMPage *> GMPageList;		//!< List of GM Pages
typedef list<class cAccount *> AccountList;		//!< List of accounts
typedef list<struct sLocation> LocationList;		//!< List of locations (used for jails)
typedef list<class cChar *> CharList;			//!< List of characters
typedef list<class cPC *> PCList;			//!< List of playinc characters
typedef list<class cItem *> ItemList;		//!< List of items
typedef list<class cMessage> MessageList;	//!< List of messages
typedef list<class cMsgBoard> BoardsList;	//!< List of message boards
#endif

#endif
