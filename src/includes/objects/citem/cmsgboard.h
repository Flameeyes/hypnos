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
\note rewrite by Chronodt (march 2004)
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

struct RegionSort //Used for sorting MsgBoards by region # and as secondary sort serial #. We need to employ a < comparator
{
	inline bool operator()(const pMsgBoard s1, const pMsgBoard s2) const
	{
        	if (s1->getRegion() == s2->getRegion()) return s1->getSerial32() < s2->getSerial32();
                else return s1->getRegion() < s2->getRegion();
	}
}
struct SerialSort //Used for sorting Messages by serial #
{
	inline bool operator()(const pMsgBoardMessage s1, const pMsgBoardMessage s2) const
	{
        	return s1->getSerial32() < s2->getSerial32();
	}
}
typedef std::set<pMsgBoardMessage, SerialSort> cMsgBoardMessages; //With SerialSort the messages are inserted in ascending serial order
typedef std::set<pMsgBoard, RegionSort> cMsgBoards; //the set and the RegionSort comparator guarantees a set sorted by region number (and as secondary sort serial #)
typedef std::multimap<UI32, UI32> cBBRelations;
/*!
In the multimap cBBrelations:
first  UI32 - Key:  using bulletin board serial as key, allows us to "collect" the serials of messages contained therein
second UI32 - Data: it will be the serial numbers of the messages within it. Since it is a MULTImap, to each
		    bulletin board (key) can be associated multiple messages (Data)

the relation map has to be saved in worldfile, so it needs serials and not pointers
*/

enum PostType { LOCALPOST = 0, REGIONALPOST, GLOBALPOST };
enum QuestType { ESCORTQUEST = 0xFF, BOUNTYQUEST = 0xFE, ITEMQUEST = 0xFD, QTINVALID = 0x0 };


/*!
\brief Message board message
\author Chronodt
*/
class cMsgBoardMessage : public cItem
{
protected:

public:

      	UI32 poster;		// Serial of poster pg. if -1 autopost
        std::string subject;	// Subject of message (title)
        std::string body;	// body of message
        time_t posttime;	// time of posting
        PostType availability;  // local/regional/general post
        int region;		// if REGIONAL avalaibility, region contains a region number based on worlddata (see sregions.cpp/h)
	QuestType qtype;     	// type of quest
        UI32 replyof;		// serial of post of whom this is reply of. If 0 this is a new post
        bool autopost;		// true if autoposted by server
        UI32 targetnpc;      	// if LOCAL post it is unused, if quest contains the serial of the target of the quest
        UI32 targetitem		//

        static cMsgBoardMessages MsgBoardMessages; //This will contain all messages
        cMsgBoardMessage();
        cMsgBoardMessage(UI32 serial);
        ~cMsgBoardMessage();
	static UI32 nextSerial();
       	virtual void Delete();
        std::string getTimeString();
        bool expired();		// expiration time check & delete. Returns true if post has been deleted for reaching expiration time
       	void refreshQuestMessage();

	inline const UI32 rtti() const
	{ return rtti::cMsgBoardMessage; }
}

/*!
\brief Message boards handling
\author Akron
\since 0.82r3
\todo rewrite functions
*/
class cMsgBoard : public cItem
{

protected:
        //These two static functions are used to find a msgboard where to post automatic messages
        //Or where to move global and regional messages when a msgboard is deleted with such messages inside
        static pMsgBoard findRegionalBoard(int region);
	static pMsgBoard findGlobalBoard();

public:
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
	static const UI32 MAXPOSTS = 128;

	static const UI32 MAXENTRIES = 256; //!< maximum number of entries in a ESCORTS list in the MSGBOARD.sSCP file

	static cBBRelations BBRelations; //This will associate a bb serial to the serial of all mesages therein contained
	static cMsgBoards MsgBoards;

        cMsgBoard();
        cMsgBoard(UI32 serial);
        ~cMsgBoard();


      	static void	getPostType( pClient client );
	static void 	setPostType( pClient client, PostType nPostType );
        void		openBoard(pClient client);
	void 		sendMessageSummary( pClient client, pMsgBoardMessage message );
        bool    	addMessage( pMsgBoardMessage message );
	static UI32 	createQuestMessage(QuestType questType, pChar npc = NULL, pItem item = NULL, int region = -1);
	static void	removeQuestMessage(UI32 messageserial);


        static bool     relinkQuestMessage(pMsgBoardMessage message);

        
      	static void	MsgBoardMaintenance();

        int getRegion();

        
        void Delete();


        static pair<cMsgBoards::iterator, cMsgBoards::iterator> getBoardsinRegion(int region);

        inline const UI32 rtti() const
	{ return rtti::cMsgBoard; }

};

#endif
