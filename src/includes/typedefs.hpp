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

#ifndef __COMMON_LIBS_H__
	#error "You should never include typedefs.hpp by itself. It should always be " \
		"included by common_lib.hpp"
#endif

//@{
/*!
\name Generic types
*/
typedef int32_t				ACCOUNT;
typedef int32_t				FUNCIDX;
//@}

typedef class cSocket *pSocket;			//!< Pointer to a cSocket
typedef class cPacketSend *pPacketSend;		//!< Pointer to a cPacketSend
typedef class cPacketReceive *pPacketReceive;	//!< Pointer to a cPacketReceive

typedef class cCommand* pCommand;		//!< Pointer to a cCommand

typedef class cCreatureInfo* pCreatureInfo;

typedef class cGMPage *pGMPage;			//!< Pointer to a GM Page

typedef class cClient *pClient;			//!< Pointer to a cClient

typedef class cMenu *pMenu;			//!< Pointer to a cMenu

typedef class cAccount *pAccount;		//!< Pointer to a cAccount

typedef class cParty *pParty;			//!< Pointer to a cParty instances

typedef void (*processTarget)( pClient, const struct sTarget& );	//! Target callback

//@{
/*!
\name Objects and serializables
*/

typedef class cObject *pObject;			//!< Pointer to a cObject
typedef class cSerializable *pSerializable;	//!< Pointer to cSerializable

typedef class cBody* pBody;			//!< Pointer to a cBody

typedef class cChar *pChar;			//!< Pointer to a cChar (character)
typedef class cPC *pPC;				//!< Pointer to a pPC (playing character)
typedef class cNPC *pNPC;			//!< Pointer to a pNPC (non playing character)

//@}

//@{
/*!
\name Items and subclasses
*/
typedef class cItem *pItem;			//!< Pointer to a cItem

typedef class cEquippable *pEquippable;		//!< Pointer to a cEquippable

typedef class cBook *pBook;			//!< Pointer to a cBook
typedef class cWeapon *pWeapon;			//!< Pointer to a cWeapon
typedef class cContainer *pContainer;		//!< Pointer to a cContainer
typedef class cMap *pMap;			//!< Pointer to a cMap
typedef class cEquippableContainer *pEquippableContainer;
						//!< Pointer to a cEquippableContainer (backpacks, bank boxes)
typedef class cMulti *pMulti;			//!< Pointer to a cMulti
typedef class cBoat *pBoat;			//!< Pointer to a cBoat
typedef class cHouse *pHouse;			//!< Pointer to a cHouse

typedef class cMsgBoard *pMsgBoard;		//!< Pointer to a cMsgBoard
typedef class cMessage* pMessage;		//!< Pointer to a message board message
//@}
