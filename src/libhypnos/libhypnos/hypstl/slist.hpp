/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator (Utility Library)                              |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in LICENSE file.               |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __LIBHYPNOS_HYPSTL_SLIST_HPP__
#define __LIBHYPNOS_HYPSTL_SLIST_HPP__

#ifdef HAVE_SLIST
	#include <slist>
	using std::slist;
#elif defined HAVE_EXT_SLIST
	#include <ext/slist>
	using __gnu_cxx::slist;
#elif defined HAVE_SLIST_H
	#include <slist.h>
#else
	// If we don't have available a singly-linked list, failback to
	// standard lists.
	template <typename T> typedef list<T> slist<T>;
#endif

typedef slist<uint32_t> uint32_slist;	//!< Singly-linked list of uint32_t

#ifdef HYPNOS_SOURCES
typedef slist<class cClient *> ClientSList;	//!< Singly-linked list of connected clients
typedef slist<class cChar *> CharSList;		//!< Singly-linked list of characters
typedef slist<class cPC *> PCSList;		//!< Singly-liked list of playinc characters
typedef slist<class cItem *> ItemSList;		//!< Singly-linked list of items
typedef slist<class cEquippable *> EquippableSList;//!< Singly-linked list of equippable items
typedef slist<class cHouse *> HouseSList;	//!< Singly-linked list of houses
#endif

#endif
