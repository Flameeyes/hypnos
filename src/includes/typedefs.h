/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file typedefs.h
\brief contains typedefs for integer values and common structures.
\todo complete the documentation of types
*/

#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

//@{
/*!
\name Generic types
*/
typedef uint32_t			TIMERVAL;
typedef int32_t				ACCOUNT;
typedef int32_t				FUNCIDX;

typedef class tVariant *pVariant;		//!< Pointer to a variant instance
typedef std::list<tVariant> tVariantList;	//!< List of variants
typedef std::vector<tVariant> tVariantVector;	//!< Vector of variants

typedef std::set<uint32_t> uint32_set;		//!< A set of uint32_t
typedef std::vector<uint32_t> uint32_vector;	//!< A vector of uint32_t
typedef std::slist<uint32_t> uint32_slist;	//!< A slist of uint32_t

typedef std::vector<std::string> stringVector;	//!< A vector of strings

typedef std::basic_string<uint16_t> unistring;	//!< Unicode string
//@}

typedef class cTarget* pTarget;			//!< Pointer to a cTarget
typedef class cSocket *pSocket;			//!< Pointer to a cSocket
typedef class cPacketSend *pPacketSend;		//!< Pointer to a cPacketSend
typedef class cPacketReceive *pPacketReceive;	//!< Pointer to a cPacketReceive

typedef class cCommand* pCommand;		//!< Pointer to a cCommand
typedef std::map< std::string, pCommand > CommandMap;
						//!< Map of commands

typedef class cCreatureInfo* pCreatureInfo;

typedef class cGMPage *pGMPage;			//!< Pointer to a GM Page
typedef std::list<pGMPage> GMPageList;		//!< List of GM Pages

typedef class cClient *pClient;			//!< Pointer to a cClient
typedef std::list<pClient> ClientList;		//!< List of connected clients

typedef class cMenu *pMenu;			//!< Pointer to a cMenu
typedef std::map<uint32_t, pMenu> MenuMap;	//!< Map of menus

typedef class cAccount *pAccount;		//!< Pointer to a cAccount
typedef std::map<std::string, pAccount> cAccounts;
						//!< Hashed-map of accounts

//@{
/*!
\name Objects and serializables
*/

typedef class cObject *pObject;			//!< Pointer to a cObject
typedef class cSerializable *pSerializable;	//!< Pointer to cSerializable
typedef std::map<uint32_t, pSerializable> SerializableMap;
						//!< Map of serializable objects (items, chars..)

typedef class cBody* pBody;			//!< Pointer to a cBody

typedef class cChar *pChar;			//!< Pointer to a cChar (character)
typedef std::list<pChar> CharList;		//!< List of characters
typedef class cPC *pPC;				//!< Pointer to a pPC (playing character)
typedef std::vector<pPC> PCVector;		//!< Vector of playing characters
typedef class cNPC *pNPC;			//!< Pointer to a pNPC (non playing character)

//@}

//@{
/*!
\name Items and subclasses
*/

typedef class cItem *pItem;			//!< Pointer to a cItem
typedef std::list<pItem> ItemList;		//!< List of items
typedef class cEquippable *pEquippable;		//!< Pointer to a cEquippable
typedef class cBook *pBook;			//!< Pointer to a cBook
typedef class cWeapon *pWeapon;			//!< Pointer to a cWeapon
typedef class cContainer *pContainer;		//!< Pointer to a cContainer
typedef class cMsgBoard *pMsgBoard;		//!< Pointer to a cMsgBoard
typedef class cMap *pMap;			//!< Pointer to a cMap
typedef class cEquippableContainer *pEquippableContainer;
						//!< Pointer to a cEquippableContainer (backpacks, bank boxes)
//@}

#endif //__TYPEDEFS_H__
