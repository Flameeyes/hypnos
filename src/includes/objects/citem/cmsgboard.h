/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Message board functions
\author Dupois and Flameeyes (rewrite and cleanup)
\note rewrite by Chronodt (march 2004) and by Flameeyes (august 2004)
*/

#ifndef __CMSGBOARD_H__
#define __CMSGBOARD_H__

#include "common_libs.h"
#include "enums.h"
#include "objects/citem.h"
#include "packets/sent.h"
#include <wefts_mutex.h>


/*!
\brief Message board's message
\author Chronodt & Flameeyes
*/
class cMessage : public cItem
{
friend class nPackets::Sent::BBoardCommand;
protected:
	pPC poster;		//!< Poster pc, NULL if autoposted
	std::string subject;	//!< Subject of message (title)
	std::string body;	//!< body of message
	time_t posttime;	//!< time of posting
	PostType availability;  //!< local/regional/general post
	QuestType qtype;	//!< type of quest
	uint32_t replyof;	//!< serial of post of whom this is reply of. If 0 this is a new post
	bool autopost;		//!< true if autoposted by server \todo Move this as a flag
	pChar targetnpc;	//!< if LOCAL post it is unused, if quest contains the serial of the target of the quest

public:
	cMessage();
	cMessage(uint32_t serial);
	~cMessage();
	
	std::string getTimeString();
	
	bool isExpired();	//!< expiration time check & delete. Returns true if post has been deleted for reaching expiration time
	void refreshQuestMessage();
};


/*!
\brief Message boards handling
\author Flameeyes
\since 0.82r3
*/
class cMsgBoard : public cItem
{
friend class cMessage;
protected:
	static BoardsList boards;	//!< All the boards (used for maintenance)
	static Wefts::Mutex boardsMutex;//!< Mutex for the access of cMsgBoard::boards variable
	static MessageList globalMsgs;	//!< Global messages
	static Wefts::Mutex globalMutex;//!< Mutex for the access of cMsgBoard::globalMsgs variable
	MessageList boardMsgs;		//!< Board messages
	Wefts::Mutex boardMutex;	//!< Mutes for the access of cMsgBoard::boardMsgs attribute
public:
//@{
/*!
\name Constructors and operators
*/
	cMsgBoard();
	cMsgBoard(uint32_t serial);
	~cMsgBoard();
//@}

	static void getPostType( pClient client );
	static void setPostType( pClient client, PostType nPostType );
	void openBoard(pClient client);
	void sendMessageSummary( pClient client, pMessage message );
	bool addMessage( pMessage message );
	static uint32_t createQuestMessage(QuestType questType, pChar npc = NULL, pItem item = NULL, int region = -1);
	static void removeQuestMessage(uint32_t messageserial);
	static bool relinkQuestMessage(pMessage message);
	static void MsgBoardMaintenance();

	int getRegion();
	
	void Delete();
};

#endif
