/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Message board functions
\author Dupois and Akron (rewrite and cleanup)
\note rewrite by Chronodt (march 2004) and by Flameeyes (august 2004)
*/

#ifndef __CMSGBOARD_H__
#define __CMSGBOARD_H__

enum PostType { ptLocalPost = 0, ptRegionalPost, ptGlobalPost };
enum QuestType { qtEscort = 0xFF, qtBoundy = 0xFE, qtItem = 0xFD, qtInvalid = 0x0 };

#include "objects/citem.h"

/*!
\brief Message boards handling
\author Akron
\since 0.82r3
*/
class cMsgBoard : public cItem
{
protected:
	typedef class cMessage* pMessage;
	typedef std::list<pMessage> MessageList;
	
	/*!
	\brief Message board message
	\author Chronodt & Flameeyes
	*/
	class cMessage
	{
	protected:
		pPC poster;		//!< Poster pc, NULL if autoposted
		std::string subject;	//!< Subject of message (title)
		std::string body;	//!< body of message
		time_t posttime;	//!< time of posting
		PostType availability;  //!< local/regional/general post
		int region;		//!< if REGIONAL avalaibility, region contains a region number based on worlddata (see sregions.cpp/h)
		QuestType qtype;	//!< type of quest
		pMessage replyof;	//!< serial of post of whom this is reply of. If 0 this is a new post
		bool autopost;		//!< true if autoposted by server
		uint32_t targetnpc;	//!< if LOCAL post it is unused, if quest contains the serial of the target of the quest
		uint32_t targetite;	//!<
	
	public:
		cMessage();
		cMessage(uint32_t serial);
		~cMessage();
		
		std::string getTimeString();
		
		bool isExpired();	//!< expiration time check & delete. Returns true if post has been deleted for reaching expiration time
		void refreshQuestMessage();
	}
	
	static MessageList globalMsgs;
	static std::map<uint32_t, MessageList> regionalMsgs;
public:
//@{
/*!
\name Parameters
\todo Need to be moved as server parameters
*/
	
	/*!
	\brief Maximum number of posts per board

        (obsolete calculation)
        --------------------------------------------------------------------------------
	Buffer Size = 2560
	Therefore 0x3c max size = 2560 - 5 (0x3c header info) = 2550
	2550 / 19 (item segment size per msg) = 134
	Round down to 128 messages allowable on a message board (better safe than sorry)
        --------------------------------------------------------------------------------

        Outbound packets now can have any length, so the maxpost is only used to have a REASONABLE sized packet to send (Chronodt 10/3/04)

	*/
	static const uint32_t MAXPOSTS = 128;

	static const uint32_t MAXENTRIES = 256; //!< maximum number of entries in a ESCORTS list in the MSGBOARD.sSCP file
//@}

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
