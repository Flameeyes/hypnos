  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

/*!
\file
\brief Message board functions
\author Dupois and Akron (rewrite and cleanup)
\note using namespaces and c++ style
*/

#ifndef __CMSGBOARDS
#define __CMSGBOARDS

#ifdef __unix__
#include <dirent.h>
#endif
class cMsgBoard;
typedef cMsgBoard *pMsgBoard;

class cMsgBoardMessage;
typedef cMsgBoardMessage *pMsgBoardMessage;

enum PostType { LOCALPOST = 0, REGIONALPOST, GLOBALPOST };
enum QuestType { ESCORTQUEST = 0xFF, BOUNTYQUEST = 0xFE, ITEMQUEST = 0xFD, QTINVALID = 0x0 };


/*!
\brief Message board message
\author Chronodt
Since each message has a serial number similar to an item, but with far less detail needed,
it needs only to be derived from cObject
*/
class cMsgBoardMessage : public cObject
{
protected:

public:

      	SERIAL poster;		// Serial of poster pg. if -1 autopost
        std::string subject;	// Subject of message (title)
        std::string time;	// time of posting
        std::string body;	// body of message
        PostType availability;  // local/regional/general post
	QuestType type;		// type of quest
        pMsgBoard board;	// board in which post has been posted. if general autopost it will be NULL

        cMsgBoardMessage();
	static UI32 nextSerial();
       	virtual void Delete();
}

/*!
\brief Message boards handling
\author Akron
\since 0.82r3
\todo rewrite functions
*/
class cMsgBoard : public cItem
{
public:
	/*!
	\brief Maximum number of posts per board

        (obsolete calculation)
        --------------------------------------------------------------------------------
	Buffer Size = 2560<br/>
	Therefore 0x3c max size = 2560 - 5 (0x3c header info) = 2550<br/>
	2550 / 19 (item segment size per msg) = 134<br/>
	Round down to 128 messages allowable on a message board (better safe than sorry)
        --------------------------------------------------------------------------------

        Outbound packets now can have any length, so the maxpost is only used to have a REASONABLE sized packet to send (Chronodt 10/3/04)

	*/
	static const UI32 MAXPOSTS = 128;

	static const UI32 MAXENTRIES = 256; //!< maximum number of entries in a ESCORTS list in the MSGBOARD.sSCP file

	/*!
	\brief different types of user posts
	*/


	/*!
	\brief different types of QUESTS
	Added for Quests (ESCORTS)<br/>
	Reason for starting high and counting down, is that I store the entire user
	posted message as is (which includes the message type).  The message type
	for a user post is 0x05 and I also use this field to determine whether the
	post is marked for deletion (0x00).  In order to allow for the maximum number
	of different quest types, I opted to start high and count down.
	*/

	void 	MsgBoardList( pClient client )
	void	MsgBoardSetPostType( pClient client, PostType nPostType );
	void	MsgBoardGetPostType( pClient client );
	int	MsgBoardPostQuest( pClient client, QuestType nQuestType );
	void	MsgBoardQuestEscortCreate( pClient client );
	void	MsgBoardQuestEscortArrive( P_CHAR pc, P_CHAR pc_k );
	void	MsgBoardQuestEscortDelete( int nNPCIndex );
	void	MsgBoardQuestEscortRemovePost( int nNPCIndex );
	void	MsgBoardMaintenance();
	bool	MsgBoardRemoveGlobalPostBySerial( int nPostSerial );
	#if defined(__unix__)
	std::vector<std::string> MsgBoardGetFile( char* pattern, char* path) ;
	#endif
};

#endif
