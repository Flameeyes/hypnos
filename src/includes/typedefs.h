/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief contains typedefs for integer values and common structures.
\todo complete the documentation of types
*/

#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

//@{
/*!
\name Generic types
*/
typedef int32_t				ACCOUNT;
typedef int32_t				FUNCIDX;

typedef basic_string<uint16_t> unistring;	//!< Unicode string
//@}

typedef class cSocket *pSocket;			//!< Pointer to a cSocket
typedef class cPacketSend *pPacketSend;		//!< Pointer to a cPacketSend
typedef class cPacketReceive *pPacketReceive;	//!< Pointer to a cPacketReceive

typedef class cCommand* pCommand;		//!< Pointer to a cCommand
typedef map< string, pCommand > CommandMap;
						//!< Map of commands

typedef class cCreatureInfo* pCreatureInfo;

typedef class cGMPage *pGMPage;			//!< Pointer to a GM Page
typedef list<pGMPage> GMPageList;		//!< List of GM Pages

typedef class cClient *pClient;			//!< Pointer to a cClient
typedef slist<pClient> ClientSList;	//!< Singly-linked list of connected clients

typedef class cMenu *pMenu;			//!< Pointer to a cMenu
typedef map<uint32_t, pMenu> MenuMap;	//!< Map of menus

typedef class cAccount *pAccount;		//!< Pointer to a cAccount
typedef map<string, pAccount> cAccounts;
						//!< map of accounts
typedef list<pAccount> AccountList;	//!< List of accounts

typedef vector<struct sPoint> PointVector;	//!< Vector of (x,y) points (uint16_t coords)
typedef list<struct sLocation> LocationList;
						//!< List of locations (used for jails)

typedef class cParty *pParty;			//!< Pointer to a cParty instances

typedef void (*processTarget)( pClient, const struct sTarget& );	//! Target callback

//@{
/*!
\name Objects and serializables
*/

typedef class cObject *pObject;			//!< Pointer to a cObject
typedef class cSerializable *pSerializable;	//!< Pointer to cSerializable
typedef map<uint32_t, pSerializable> SerializableMap;
						//!< Map of serializable objects (items, chars..)

typedef class cBody* pBody;			//!< Pointer to a cBody

typedef class cChar *pChar;			//!< Pointer to a cChar (character)
typedef list<pChar> CharList;		//!< List of characters
typedef slist<pChar> CharSList;		//!< Singly-linked list of characters
typedef class cPC *pPC;				//!< Pointer to a pPC (playing character)
typedef vector<pPC> PCVector;		//!< Vector of playing characters
typedef list<pPC> PCList;			//!< List of playinc characters
typedef slist<pPC> PCSList;		//!< Singly-liked list of playinc characters
typedef class cNPC *pNPC;			//!< Pointer to a pNPC (non playing character)

//@}

//@{
/*!
\name Items and subclasses
*/
typedef class cItem *pItem;			//!< Pointer to a cItem
typedef list<pItem> ItemList;		//!< List of items
typedef slist<pItem> ItemSList;		//!< Singly-linked list of items

typedef class cEquippable *pEquippable;		//!< Pointer to a cEquippable
typedef slist<pEquippable> EquippableSList;//!< Singly-linked list of equippable items

typedef class cBook *pBook;			//!< Pointer to a cBook
typedef class cWeapon *pWeapon;			//!< Pointer to a cWeapon
typedef class cContainer *pContainer;		//!< Pointer to a cContainer
typedef class cMap *pMap;			//!< Pointer to a cMap
typedef class cEquippableContainer *pEquippableContainer;
						//!< Pointer to a cEquippableContainer (backpacks, bank boxes)
typedef class cMulti *pMulti;			//!< Pointer to a cMulti
typedef class cBoat *pBoat;			//!< Pointer to a cBoat
typedef class cHouse *pHouse;			//!< Pointer to a cHouse
typedef slist<pHouse> HouseSList;		//!< Singly-linked list of houses

typedef class cMsgBoard *pMsgBoard;		//!< Pointer to a cMsgBoard
typedef class cMessage* pMessage;		//!< Pointer to a message board message
typedef list<pMessage> MessageList;	//!< List of messages
typedef list<pMsgBoard> BoardsList;	//!< List of message boards
//@}

#endif //__TYPEDEFS_H__
