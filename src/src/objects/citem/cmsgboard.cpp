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

#include "nxwcommn.h"
#include "network.h"
#include "sregions.h"
#include "sndpkg.h"
#include "debug.h"
#include "cmsgboard.h"
#include "npcai.h"
#include "scp_parser.h"
#include "items.h"
#include "chars.h"
#include "utils.h"
#include "inlines.h"
#include "nox-wizard.h"
#include "scripts.h"

/*!
\note xan -> do not move in headers, plz
*/
extern char g_strScriptTemp[TEMP_STR_SIZE];

/*
static UI32 cMsgBoardMessage::nextSerial()
{
	//! Since the clients treats a message almost like an item, we use an item serial :D
	return cItem::nextSerial();
}
*/

/*!
\brief Deleting an MsgBoard message
\author Chronodt
*/

void cMsgBoardMessage::Delete()
{
/*
	cPacketSendDeleteObject pk(serial);

        //! \todo replace this when sets redone



        NxwSocketWrapper sc;
        sc.fillOnline( this );
	for ( sc.rewind(); !sc.isEmpty(); sc++ ) {
		NXWCLIENT ps = sc.getClient();
		if ( ps != NULL )
			ps->sendPacket(&pk);
	}
*/
        switch (availability)
        {
        case LOCALPOST: //Here we need only to disconnect local MsgBoard
                // Disconnecting relations from bullettin board which was linked to
        	cBBRelations::iterator it = find(cMsgBoards::BBRelations.begin(),cMsgBoards::BBRelations.end(), cBBRelations::pair(getContainer()->getSerial32(), serial));
	        if ((it != cMsgBoards::BBRelations.end()) || (*it == cBBRelations::pair(getContainer()->getSerial32(), serial)) cMsgBoards::BBRelations.erase(it);
                break;
        case REGIONALPOST: //Here, instead, we have to disconnect ALL msgBoard in the same region as the board in which post was originally posted

        	pair<cMsgBoards::iterator, cMsgBoards::iterator> it = cMsgBoard::getBoardsinRegion(region);
                if (it.first == cMsgBoards::MsgBoards.end()) break;	//If no msgboards in region, bail out
                /*
                We now have a range it.first-it.second of msgBoards that are in the same region as the message to be deleted. (they may even be
                the same)
                Since they are all in the same region, due to RegionSort, this range is sorted by serial number now, and we use this fact:
                BBrelations key are the serials and sorted in the same way, so the find algorithm will NOT need to research from the beginning
                each time. In addition, find algorithm looks for the first (and should be the only) occurrence of the pair, so the search is much
                more time efficient than a whole search from the beginning for each msgboard in region
                */
                cBBRelations::iterator bbit = cMsgBoards::BBRelations.begin();
                for(; (it.first != it.second) || (bbit == cMsgBoards::BBRelations.end()); ++it.first)
                {
                        //finding the current serial and set it for next search, since it will be only on following serials
			bbit = find(bbit,cMsgBoards::BBRelations.end(), cBBRelations::pair((*itbegin)->getSerial32(), serial));
               	        //bbit now contains the iterator that points to the pair msgboard serial - message serial to delete
                       	if (bbit == cMsgBoards::BBRelations.end() && ((*bbit) != cBBRelations::pair((*itbegin)->getSerial32(), serial)) )
                        {
                        	// if for any chance bbit reaches the end of the set (meaning no pair has been found) we skip deletion of that
                                // message and begin again from the beginning with next mex. It really is a really bad case scenario because
                                // it SHOULD never get to this, but this way we could avoid deleting last connection uselessly if something goes
                                // wrong :D
                        	bbit = cMsgBoards::BBRelations.begin();
                                continue;
                        }
                        BBRelations.erase(bbit++);
       	        }
                break;
        case GLOBALPOST:  //Obiously, we need to disconnect this from ALL MsgBoards
                for(cMsgBoards::iterator it = cMsgBoards::MsgBoards.begin(); (it != cMsgBoards::MsgBoards.end()); ++it)
                {
                        //Removing a global post is not as efficient as removing a regional post.... :(
                        //but since most autopost for quests will be regional, it is best to get THAT as the most efficient :P
                        cBBRelations::iterator it2 = find(cMsgBoards::BBRelations.begin(),cMsgBoards::BBRelations.end(), cBBRelations::pair((*it)->getSerial32(), serial));
                        if (it2 != cMsgBoards::BBRelations.end()) cMsgBoards::BBRelations.erase(it2);
                }
                break;
        }
	cItem::Delete();  //this will do the safedelete, too
}

cMsgBoardMessage::cMsgBoardMessage()
{
	cMsgBoardMessage(nextSerial());
}

cMsgBoardMessage::cMsgBoardMessage(UI32 serial) : cItem(serial)
{
//	setSerial(serial);
        region = -1;

        availability = LOCALPOST;
        poster = -1;
        replyof = 0;
        qtype = QTINVALID;
	autopost = false;
        targetserial = -1;

        // With this line, creating a post sets also automatically the current time, so when it is not startup,
        // there is no need to set it :D
        time( &posttime );


        setAnimId(0xeb0): //Model ID for msgboards items

        MsgBoardMessages.push_back(this)

}

cMsgBoardMessage::~cMsgBoardMessage()
{
        MsgBoardMessages.erase(find(MsgBoardMessages.begin(), MsgBoardMessages.end(), this));
}

std::string cMsgBoardMessage::getTimeString()
{
	char result[25];
        struct tm timest = localtime( &posttime);
        sprintf( result, "Day %i @ %i:%02i", timest.tm_yday + 1, timest.tm_hour, timest.tm_min );
        return std::string(result);
}

bool cMsgBoardMessage::expired()
{
	time_t now;
	time( &now );
        long messagelife = now - posttime;  	//messagelife is lifetime of message in seconds
        //here we check if post has a timeout and if it has expired
        switch (qtype)
        {
        case QTINVALID:
        	if (!SrvParms->msgretention && messagelife > (SrvParms->msgretention * 86400))  //86400 = 24 * 60 * 60 -> msgretention is expressed in days while messagelife in seconds
                {
                	//If it isn't a quest, we simply delete the message
			Delete();
                        return true;
                }
		break;
        case ESCORTQUEST:
               	if (!SrvParms->escortinitexpire && messagelife > SrvParms->escortinitexpire)	//escortinitexpire is expressed in seconds
                {
                	pNPC npc = pointers::findCharBySerial(targetnpc);
                        if (!npc && npc->questEscortPostSerial == getSerial32()) npc->Delete();	//If it has not yet disappeared, but the serial is still the right escort npc (the serial may have been reused!!) we delete it
                        Delete();
                        return true;
                }
		break;
	case BOUNTYQUEST:
        	if (!SrvParms->bountysexpire && messagelife > (SrvParms->bountysexpire * 86400))	//86400 = 24 * 60 * 60 -> bountysexpire is expressed in days while messagelife in seconds
                {
                        pChar pc = pointers::findCharBySerial(targetnpc);
                        // If it is an npc created just for the bounty (function not yet implemented) but it has not yet
                        // disappeared, but the serial is still the right npc (the serial may have been reused!!) we delete it
                        if (!pc && pc->rtti() == rtti::cNPC && pc->questBountyPostSerial == getSerial32()) pc->Delete();
                        Delete();
                        return true;
		}
		break;
	case ITEMQUEST:  //not yet implemented, so we always put it to be deleted, as the default
        default:
        	Delete();
                return true;
	}
	return false;
}

//-----------------------------------------------------------------------------------------
//				cMsgBoard Methods
//-----------------------------------------------------------------------------------------

/*!
\brief Char array for messages to client.
Message body (when entering body of post) can hold a maximum of 1975 chars (approx)
*/
//UI08 msg[MAXBUFFER];

//! Buffer to be used when posting messages
//UI08 msg2Post[MAXBUFFER] = "\x71\xFF\xFF\x05\x40\x00\x00\x19\x00\x00\x00\x00";
//                                     |Pid|sz1|sz2|mTy|b1 |b2 |b3 |b4 |m1 |m2 |m3 |m4 |

cMsgBoard::cMsgBoard()
{
	cMsgBoard(nextSerial());
}

cMsgBoard::cMsgBoard(UI32 serial) : cItem(serial)
{
        MsgBoards.push_back(this)
}

cMsgBoard::~cMsgBoard()
{
	MsgBoards.erase(find(MsgBoards.begin(), MsgBoards.end(), this));
}


/*!
\param client player client
Used to retrieve the current post type in order to tell the user what type of
mode they are in.
*/
static void cMsgBoard::getPostType( pClient client )
{
	pPC pc = client->currChar();
	VALIDATEPC(pc);

	PostType type = pc->postType;

	switch ( type )
	{
		case LOCALPOST: // LOCAL post
			client->sysmessage( TRANSLATE("Currently posting LOCAL messages") );
			break;

		case REGIONALPOST: // REGIONAL post
			client->sysmessage( TRANSLATE("Currently posting REGIONAL messages") );
			break;

		case GLOBALPOST: // GLOBAL POST
			client->sysmessage( TRANSLATE("Currently posting GLOBAL messages" ));
			break;
	}
	return;
}

/*!
\param client player client
\param nPostType type of post

Used to set the postType for the current user (Typically a GM)
There is a local array that holds each players curreny posting
type.  Using the command to set the post type updates the
value in the array for that player so that they can post
different types of messages.
*/
static void cMsgBoard::setPostType( pClient client, PostType nPostType )
{
	pChar pc=client->currChar();
	VALIDATEPC(pc);

	pc->postType = nPostType;

	switch ( nPostType )
	{
		case LOCALPOST: // LOCAL post
			client->sysmessage( TRANSLATE("Post type set to LOCAL" ));
			break;

		case REGIONALPOST: // REGIONAL post
			client->sysmessage( TRANSLATE("Post type set to REGIONAL") );
			break;

		case GLOBALPOST: // GLOBAL POST
			client->sysmessage( TRANSLATE("Post type set to GLOBAL") );
			break;
	}
	return;

}

/*!
\param client player client
Called when player dbl-clicks on a Message Board thereby
requesting a list of messages posted on the board.
*/
void cMsgBoard::openBoard(pClient client)
{
	// Send Message Board open to client...
	cPacketSendBBoardCommand pk(this, DisplayBBoard);
	client->sendPacket(&pk);
        // .. and immediately thereafter the "items" it contains (the serials of messages connected to that board)
        // but only if it has at least 1 message inside
      	if (BBRelations.find(getSerial32()) != BBRelations.end())
	{
        	cPacketSendMsgBoardItemsinContainer pk2 (this);
       		client->sendPacket(&pk2);
        }
}

/*!
\param client player client
\param messageserial serial of the message sent by client requiring message's summary
After Bulletin Board is displayed, client asks for summary for each message serial it has received
and this function is then called to send them
*/
void cMsgBoard::sendMessageSummary( pClient client, pMsgBoardMessage message)
{
/*
        cMsgBoardMessages::iterator it = cMsgBoardMessage::MsgBoardMessages.begin();
	for(;(it != cMsgBoardMessage::MsgBoardMessages.end()) && ((*it)->getSerial32() != messageserial), ++it) {}
	if ((*it)->getSerial32() != messageserial) return; //if not found, something is wrong, so we do nothing else
*/
	cPacketSendBBoardCommand pk(this, SendMessageSummary, message);
	client->sendPacket(&pk);
}

/*!
\brief links message to msgboard
\param message message to be added
*/

bool cMsgBoard::addMessage(pMsgBoardMessage message)
{
	switch (message->availability)
        {
	case LOCALPOST:
        	//only one insertion needed here, but we first have to verify msgboard capacity against MAXPOSTS
		//Note that this only means that normal players cannot bring a MsgBoard to have more than MAXPOSTS
                //mexes, but by posting regional or global messages this limit can be exceeded
                
               	pair<cBBRelations::iterator, cBBRelations::iterator> it = BBRelations.equal_range(getSerial32());
                if (distance(it.first, it.second) > MAXPOSTS) return false;

        	BBRelations.insert(cBBRelations::pair(getSerial32(), message->getSerial32()));
        	break;
        case REGIONALPOST;
               	pair<cMsgBoards::iterator, cMsgBoards::iterator> it = getBoardsinRegion(region);
                //if no msgboard in region, it should not post. Only significant if autopost... or else some very weird things are floating around :D
                if (it.first == MsgBoards.end()) return false;
                // We now have a range it.first-(it.second - 1) of msgBoards that are in the same region as the message.
		for (;it.first != it.second; ++it.first)
                	BBRelations.insert(cBBRelations::pair((*(it.first))->getSerial32(), message->getSerial32()));
        	break;
        case GLOBALPOST:
        	if (MsgBoards.empty()) return false; //Obiously, even general posts cannot be done when NO msgboards are present at all....
        	for(cMsgBoards::iterator it = MsgBoards.begin(), it != MsgBoards.end(), ++it)
                      	BBRelations.insert(cBBRelations::pair((*it)->getSerial32(), message->getSerial32()));
        }
	return true;
}



//////////////////////////////////////////////////////////////////////////////
// FUNCTION:    MsgBoardPostQuest( int serial, int questType )
//
// PURPOSE:     Used to read in the template for displaying a typical quest
//              message based on the type of quest and the serial number of
//              the NPC or Item.  Certain parameters can be used as variables
//              to replace certain NPC such as %n for NPC name, %t NPC title
//              etc.  See the MSGBOARD.SCP file for an example.
//
// PARAMETERS:  serial      NPC or Item serial number
//              questType   Type of quest being posted (used to determine
//                          if the item_st or char_st should be used to
//                          for replacing paramters in the script.
//
// RETURNS:     0           Failed to post message
//              PostSerial  Serial number of the post if successfull
//
// NOTES:       Currently only escort quests work so this function us still
//              in its early stages in regards to the questType parameter.
//////////////////////////////////////////////////////////////////////////////

/*!
\brief creates an automatic quest
/note Since they are always general or regional, method is static because no single msgboard is normally selected
\param targetserial serial of the quest's target
\param questtype type of quest
\param region validity region of quest (where is to be posted)
\return serial of message posted 
*/

static UI32 cMsgBoard::createQuestMessage(QuestType questType, pNPC npc, pItem item, int region )
{
	static const char subjectEscort[]     = "Escort: Needed for the day.";  // Default escort message
	static const char subjectBounty[]     = "Bounty: Reward for capture.";  // Default bounty message
	static const char subjectItem[]       = "Lost valuable item.";          // Default item message


        pMsgBoardMessage message = new cMsgBoardMessage();

	message->qtype = questType;
	message->autopost = true;
	message->targetnpc = (npc) ? npc->getSerial32() : 0;
        message->targetitem = (item) ? item->getSerial32() : 0;
        message->region = region;	//if questtype does not need to use a regional post, this is simply ignored :D
        
	SI32	sectionEntrys[MAXENTRIES];                            // List of SECTION items to store for randomizing

	UI32	listCount           = 0;  // Number of entries under the ESCORTS section, used to randomize selection
	SI32	entryToUse          = 0;  // Entry of the list that will be used to create random message


	switch ( questType )
	{
		case ITEMQUEST:
                	message->subject = string(subjectItem);
			message->availability = REGIONALPOST;
                        if (!npc || !item)
                        {
	                        ErrOut("cMsgBoard::createQuestMessage() missing valid npc or item for ITEMQUEST\n");
              	        	message->Delete;
				return 0;
                        }
			break;

		case BOUNTYQUEST:
	                message->subject = string(subjectBounty);
			message->availability = GLOBALPOST;
			if (!npc)
                        {
				ErrOut("cMsgBoard::createQuestMessage() missing valid npc for BOUNTYQUEST\n");
              	        	message->Delete;
				return 0;
                        }

                        break;

		case ESCORTQUEST:
	                message->subject = string(subjectEscort);
			message->availability = REGIONALPOST;
                        if (!npc)
                        {
				ErrOut("cMsgBoard::createQuestMessage() missing valid npc for ESCORTQUEST\n");
              	        	message->Delete;
				return 0;
                        }

			break;
		default:
			ErrOut("cMsgBoard::createQuestMessage() invalid quest type\n");
	        	message->Delete;
			return 0;
	}

        pMsgBoard MsgBoard = NULL;

        switch (message->availability)
        {
    	case REGIONALPOST:
        	MsgBoard = findRegionalBoard(region); break;
        case GLOBALPOST:
               	MsgBoard = findGlobalBoard(); break;
        }

	if (MsgBoard == NULL)
        {
        	message->Delete;
		return 0;
	}

    	cScpIterator* iter = NULL;
    	std::string script1, script2;

	int loopexit=0;

//	safedelete(iter);
	switch ( questType )
	{
	case ESCORTQUEST:
		{
			// Find the list section in order to count the number of entries in the list
			iter = Scripts::MsgBoard->getNewIterator("SECTION ESCORTS");
			if (iter==NULL)
                        {
                              	message->Delete;
                        	return 0;
                        }

			// Count the number of entries under the list section to determine what range to randomize within
			int loopexit=0;
			do
			{
				iter->parseLine(script1, script2);
				if ( !script1.compare("ESCORT") )
				{
					if ( listCount >= MAXENTRIES )
					{
						ErrOut("cMsgBoard::createQuestMessage() Too many entries in ESCORTS list [MAXENTRIES=%d]\n", MAXENTRIES );
						break;
					}

					sectionEntrys[listCount] = str2num(script2.c_str());
					listCount++;
				}
			} while ( script1[0]!='}' && script1[0]!=0 	&& (++loopexit < MAXLOOPS) );

			safedelete(iter);

			// If no entries are found in the list, then there must be no entries at all.
			if ( listCount == 0 )
			{
				ConOut( "cMsgBoard::createQuestMessage() No msgboard.scp entries found for ESCORT quests\n" );
		        	message->Delete;
				return 0;
			}

			// Choose a random number between 1 and listCount to use as a message
			entryToUse = RandomNum( 1, listCount );
#ifdef DEBUG
			ErrOut("cMsgBoard::createQuestMessage() listCount=%d  entryToUse=%d\n", listCount, entryToUse );
#endif
			// Open the script again and find the section choosen by the randomizer
			char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var

			sprintf( temp, "SECTION ESCORT %i", sectionEntrys[entryToUse-1] );
			iter = Scripts::MsgBoard->getNewIterator(temp);

			if (iter==NULL)
			{
				ConOut( "cMsgBoard::createQuestMessage() Couldn't find entry %s for ESCORT quest\n", temp );
                               	message->Delete;
				return 0;
			}
			break;
		}

  case BOUNTYQUEST:
    {
			// Find the list section in order to count the number of entries in the list
			// safedelete(iter);
			iter = Scripts::MsgBoard->getNewIterator("SECTION BOUNTYS");
			if (iter==NULL)
                        {
                               	message->Delete;
                        	return 0;
                        }

			// Count the number of entries under the list section to determine what range to randomize within
			loopexit=0;
			do
			{
				iter->parseLine(script1, script2);
				if ( !script1.compare("BOUNTY") )
				{
					if ( listCount >= MAXENTRIES )
					{
						ErrOut("cMsgBoard::createQuestMessage() Too many entries in BOUNTYS list [MAXENTRIES=%d]\n", MAXENTRIES );
						break;
					}

					sectionEntrys[listCount] = str2num(script2.c_str());
					listCount++;
				}
			} while ( script1[0]!='}' && script1[0]!=0 	&& (++loopexit < MAXLOOPS)  );

			safedelete(iter);

			// If no entries are found in the list, then there must be no entries at all.
			if ( listCount == 0 )
			{
				ConOut( "cMsgBoard::createQuestMessage() No msgboard.scp entries found for BOUNTY quests\n" );
		        	message->Delete;
				return 0;
			}

			// Choose a random number between 1 and listCount to use as a message
			entryToUse = RandomNum( 1, listCount );
#ifdef DEBUG
			ErrOut("cMsgBoard::createQuestMessage() listCount=%d  entryToUse=%d\n", listCount, entryToUse );
#endif
			// Open the script again and find the section choosen by the randomizer
 			char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var

			sprintf( temp, "BOUNTY %i", sectionEntrys[entryToUse-1] );

			safedelete(iter);
			iter = Scripts::MsgBoard->getNewIterator(temp);
			if (iter==NULL)
			{
				ConOut( "cMsgBoard::createQuestMessage() Couldn't find entry %s for BOUNTY quest\n", temp );
		        	message->Delete;
				return 0;
			}
      break;
    }

	default:
		{
			ConOut( "cMsgBoard::createQuestMessage() Invalid questType %d\n", questType );
		    	message->Delete;
			return 0;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////
	//  Randomly picked a message, now get the message data and fill in up the buffer //
	////////////////////////////////////////////////////////////////////////////////////

	int flagPos = 0;
	message->body = '\0';  //we start with 0 lines
	if (iter==NULL)
        {
               	message->Delete;
        	return 0;
        }
	script1 = iter->getEntry()->getFullLine();		//discards {

	loopexit=0;
	int loopexit2=0;
	// Read in the random post message choosen above and fill in buffer body for posting
	while ( (++loopexit < MAXLOOPS)  )
	{
		script1 = iter->getEntry()->getFullLine();
		// If we reached the ending curly brace, exit the loop
		if ( !script1.compare("}") ) break;

		flagPos = script1.find( '%' );


		// Loop until we don't find anymore replaceable parameters
		loopexit2=0;
		while ( flagPos < script1.size() && (++loopexit2 < MAXLOOPS)  )
		{
			// Replace the flag with the requested text

			switch ( script1[flagPos + 1] )
			{
                        	       	// Item Name
			case 'i':
				{
                                	script1.replace(flagpos, 2, item->getCurrentNameC());
					break;
				}
                                	// Item region (actual)
			case 'I':
				{
					script1.replace(flagpos, 2, region[item->region].name);
					break;
				}
					// NPC Name
			case 'n':
				{
                                	script1.replace(flagpos, 2, npc->getCurrentNameC());
					break;
				}
					// LOCATION in X, Y coords
			case 'l':
				{
                                	char tempString[32] = "";
					Location charpos= npc->getPosition();
					sprintf( tempString, "%d, %d", charpos.x, charpos.y );
					script1.replace(flagpos, 2, tempString);
					break;
				}
					// NPC title
			case 't':
				{
					script1.replace(flagpos, 2, npc->title);
					break;
				}
					// Destination Region Name
			case 'r':
				{
					script1.replace(flagpos, 2, region[npc->questDestRegion].name);
					break;
				}
					// Region Name
			case 'R':
				{
					script1.replace(flagpos, 2, region[npc->region].name);
					break;
				}
					// Gold amount
			case 'g':
				{

					char tempString[32] = "";
					sprintf(tempString,"%d",npc->questBountyReward) ;
					script1.replace(flagpos, 2, tempString);
					break;
				}
			default:
				{
					break;
				}
			}
			// Look for another replaceable parameter
			flagPos = script1.find( '%' );
		}

                (message->body[0])++;	//increasing number of lines in message

		//Adding line size and line body in body of message
                message->body += script1.size() +1 ; // the +1 is added to include \0 terminator
                message->body += script1;
                message->body += '\0';
	}

	safedelete(iter);


	if (!MsgBoard->addMessage( message ))
        {
        	message->Delete;
        	return 0;
        }
        return message->getSerial32();

}

static void cMsgBoard::removeQuestMessage(UI32 messageserial)
{
        for (cMsgBoardMessages::iterator it = MsgBoardMessages.begin(); (*it)->getSerial32() != messageserial && it != MsgBoardMessages.end(); ++it) {}
        if (it != MsgBoardMessages.end()) (*it)->Delete();
}



//////////////////////////////////////////////////////////////////////////////
// FUNCTION:    MsgBoardMaintenance( void )
//
// PURPOSE:     Cleans out old posts which are older than the MSGRETENTION
//              period set in SERVER.cfg and any posts that have been marked
//              for deletion (such as escort quests after they have been
//              accepted or posts that have been "removed" through the user
//              interface.  This is called as a cleanup routine on server
//              startup.  Compacts and reassigns message serial numbers.
//
// PARAMETERS:  void
//
// RETURNS:     void
//
// NOTES:       This function uses the _findfirst() and _findnext() functions
//              which are OS specific.  There will definetly be an issue in
//              compiling this on LINUX because I have no idea what the
//              structure to be passed to the functions needs to be for LINUX.
//              This will definetly have to be #ifdef'd to handle this.
//              Anyone with LINUX experience please feel free to fix it up.
//////////////////////////////////////////////////////////////////////////////
void MsgBoardMaintenance( void )
{
	int loopexit=0, loopexit2=0;
	UI08                  msg2[MAXBUFFER];

	struct tm             currentDate;
	time_t                now;
	int                   dayOfYear;
	int                   postDay;
	int                   postAge;
	int                   count;

	// Display progress message
	InfoOut("Bulletin board maintenance... ");


	ConOut("1\n");
	ConOut("2\n");
	ConOut("3\n");
	// Calculate current time and date to check if post retention period has expired

	time( &now );
        // "now" now contains time elapsed in seconds from 1 jan 1970, but since the messages also
        // keep the time that way, we only need to make a subtraction to get seconds since posting

	ConOut("4\n");

	ConOut("[DONE]\n");

	// Now lets find out what posts we keep and what posts to remove
        // to realize that, we create two sets: the first will contain all serials for the loaded messages,
        // the second all the serial linked by bsgboards. If the two are not identical, there are some unlinked
        // posts, to be deleted or moved, depending on the type of message
        std::set<UI32> serialsm, serialsb;
        cMsgBoardMessages::iterator it = MsgBoardMessages.begin();

        //checking the expiration time while we insert the posts in the set
        for(;it != MsgBoardMessages.end(); ++it) if (!(*it)->expired()) serialsm.insert((*it)->getSerial32());




	// Loop until we have reached the end of the BBI file
	while ( !feof(pBBIOld) 	&& (loopexit < MAXLOOPS) )
	{
		//Increment progress dots
		ConOut(".");

		// Day that post was created
		postDay = ShortFromCharPtr(msg +7);

			// Calculate the age of this post;
			postAge = dayOfYear - postDay;

			// If postAge is negative, then we are wrapping around the end of the year so add 365 to
			// make it positive
			if ( postAge < 0 )
				postAge += 365;

			//  |Off| 1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18
			//  |mg1|mg2|mg3|mg4|mo1|mo2|???|sg1|sg2|xx1|xx2|yy1|yy2|cS1|cS2|cS3|cS4|co1|co2|
			// "\x40\x1c\x53\xeb\x0e\xb0\x00\x00\x00\x00\x3a\x00\x3a\x40\x00\x00\x19\x00\x00";
			// cS = Charater SN ( only has a value when an NPC posted the message )

			// Check to see whether the post is a dangling quest posting.  Can occur if a quest was
			// generate and posted and then the server crashed without saving the world file.
			// You would then have a post with no quest object related to it.  So we have to
			// scan through the WSC file to figure out if the quest posted has a related object
			// in the world.
			// Message type > 0x05 is a quest && every quest must have an object associated with it
			// So if this is true we must have a quest post with a valid quest object
			if (  (msg[6]>0x05) && ( msg[13] || msg[14] || msg[15] || msg[16]) )
			{
				// Convert the post objects serial number to an int.
				SERIAL postObjectSN  = LongFromCharPtr(msg +13);
				int postQuestType = msg[6];
				int foundMatch    = 0;

				P_CHAR pc_z=pointers::findCharBySerial(postObjectSN);

				switch ( postQuestType )
				{
					case ESCORTQUEST:
						{
							if (ISVALIDPC(pc_z))
							{
								if ( pc_z->npc && ( pc_z->questType>0 ) )
								{
									// Now lets reset all of the escort timers after the server has reloaded the WSC file
									// If this is an Escor Quest NPC
									if ( (pc_z->questType==ESCORTQUEST) )
									{
										// And it doesn't have a player escorting it yet
										if ( pc_z->ftargserial==INVALID )
										{
											// Lets reset the summontimer to the escortinit
											pc_z->summontimer = ( uiCurrentTime + ( MY_CLOCKS_PER_SEC * SrvParms->escortinitexpire ) );
										}
										else // It must have an escort in progress so set the escortactiveexpire timer
										{
											// Lets reset the summontimers to the escortactive value
											pc_z->summontimer = ( uiCurrentTime + ( MY_CLOCKS_PER_SEC * SrvParms->escortactiveexpire ) );
										}
										// Found a matching NPC for this posted quest so flag the post for compression
										foundMatch = 1;
										break;
									}
								}
							}
						}
						break;
					case BOUNTYQUEST:
						{
							if (ISVALIDPC(pc_z))
							{
								if ( (pc_z->npc == 0) && (pc_z->questBountyReward  >  0) )
								{
									// Check that if this is a BOUNTYQUEST that should be removed first!
									if( ( postAge>=SrvParms->bountysexpire ) && ( SrvParms->bountysexpire!=0 ) )
									{
										// Reset the Player so they have no bounty on them
										pc_z->questBountyReward     = 0;
										pc_z->questBountyPostSerial = 0;
									}
									else
									{
										// Found a matching PC for this posted quest and the post
										// has not expired so flag the post for compression
													foundMatch = 1;
									}
									break;
								}
							}
						}
						break;
					default:
						{
							ConOut("[WARNING]\n\tUnhandled QuestType found during maintenance\n");
						}
				}

				// After looking through the char_st for a matching SN for the object that posted the message
				// If we found a match , then everything is ok, other wise there is a dangling post with no
				// related object owning it in the world.
				if ( !foundMatch )
				{
					// Show the operator a message indicating that a dangling post has been removed
					ConOut("[WARNING]\n\tDangling Post found (SN = %08x, questType = %02x) REMOVING!\n", postObjectSN, msg[6] );
					// Set the flag to delete the dangling post
					msg[6]=0x00;
				}
			}

			// If the segment 6 is 0x00 OR the postAge is greater than the MSGRETENTION period
			// then the message is marked for deletion so don't add it to the post2Keep array
			if ( (msg[6]!=0x00 || msg[6]==BOUNTYQUEST) && (postAge<=SrvParms->msgretention) )
			{
				// We found a message to be saved and compressed so lets find the matching
				// message in the BBP file and compress it
				// Loop until we have reached the end of the BBP file
				loopexit2=0;
				while ( !feof(pBBPOld) 	&& (++loopexit2 < MAXLOOPS) )
				{
					//Increment progress dots
					ConOut(".");
					// Fill up msg2 with the first 12 bytes of data from the bbp file
					if ( fread( msg2, sizeof(char), 12, pBBPOld ) != 12 )
						break;
					// Calculate the size of the remainder of this BBP segment ( -12 because we just read the first 12 bytes)
					sizeOfBBP = ShortFromCharPtr(msg2 +1) - 12;
					// Fill up the rest of the msg2 with data from the BBP file
					if ( fread( &msg2[12], sizeof(char), sizeOfBBP, pBBPOld ) != sizeOfBBP )
						if ( feof(pBBPOld) ) break;
					// Check to see that the post SN of the message just read matches the SN in the BBI file
					if ( LongFromCharPtr(msg2 +8) == LongFromCharPtr(msg +0) )
					{
						// This is a match so write the message out to the new BBP file
						// First set the serial number of this post to the newPostSN
						LongToCharPtr(newPostSN, msg2 +8);

						// If this is a BOUNTYQUEST, then make sure you update the
						// PC that references this bounty with the new BountyPostSerial#
						if( msg[6] == BOUNTYQUEST )
						{
        						SERIAL postObjectSN  = LongFromCharPtr(msg +13);
							P_CHAR pc_z=pointers::findCharBySerial(postObjectSN);
							if (ISVALIDPC(pc_z))
							{
								if ( /*(pc_z->getSerial32()== postObjectSN) &&*/ (pc_z->npc== 0) && (pc_z->questBountyReward  >  0) )
								{
									pc_z->questBountyPostSerial = newPostSN;
								}
							}
						}

						// Write out the entire message
						if ( fwrite( msg2, sizeof(char), (sizeOfBBP+12), pBBPNew ) != (sizeOfBBP+12) )
							ConOut("[FAIL]\n\tMsgBoardMaintenance() Failed to write out BBP segment to pBBPNew\n");

						// We found the message we are looking for so exit the loop leaving the file
						// pointer where it is (messages must be in the same order in both files).
						// Update msg[] with newPostSN value
						LongToCharPtr(newPostSN, msg +0);

						// Write out new BBI segment to pBBINew
						if ( fwrite( msg, sizeof(char), 19, pBBINew ) != 19)
							ConOut("[FAIL]\n\t MsgBoardMaintenance() Failed to write out BBI segment to pBBINew\n");
						// Increment the newPostSN
						newPostSN++;
						// Increment the count of the number of times we compressed a message
						count++;
						// We found the message we wanted so break out of this BBP loop
						break;
					}
				}
			}
		}

		// Finished iterating through the BBI & BBP file so set the new max message SN in the BBI file
		// and clean up in order to get ready for the next set of BBI & BBP files

		// Jump to the start of the pBBINew file
		if ( fseek( pBBINew, 0, SEEK_SET ) )
				ConOut("[FAIL]\n\tMsgBoardMaintenance() failed to seek to start of pBBINew file\n");

		// If we the number of times through the loop is 0 then we need to increment the newPostSN
		if ( count == 0 ) newPostSN++;

		// Set the buffer to the newPostSN
		LongToCharPtr(newPostSN-1, msg +0);

		// Write out the newPostSN
		if ( fwrite( msg, sizeof(char), 4, pBBINew ) != 4)
				ConOut("[FAIL]\n\tMsgBoardMaintenance() Failed to write out newPostSN pBBINew\n");


		// Close both BBP files
		fclose( pBBPOld );
		fclose( pBBPNew );

		// Delete the BBP temp file
		remove( fileBBPTmp );

		// Close both BBI files
		fclose( pBBIOld );
		fclose( pBBINew );

		// Delete the BBI temp file
		remove( fileBBITmp );

		loopexit=0;

		index++ ;
#if defined(__unix__)
	}
	while ((vecFiles.size() < index)   && (++loopexit < MAXLOOPS)  );
#else
	}
	while ( (_findnext( hBBIFile, &BBIFile ) == 0) 	&& (++loopexit < MAXLOOPS)  );

	// Close the _findfirst handle
	_findclose( hBBIFile );
#endif
	ConOut("[ OK ]\n");
	return;
}



/*!
\brief gets all MsgBoards in region.
\note the first iterator in the pair is the first of the range, but the second is the first of NEXT board or end()
\returns if any msgboard is found, returns as range [first,second[ , else both iterators point to MsgBoards.end()
*/

static pair<cMsgBoards::iterator, cMsgBoards::iterator> cMsgBoard::getBoardsinRegion(int region)
{
        	cMsgBoards::iterator it = MsgBoards.begin();
                //With this we find the first board in which region number is the same as the message to be deleted
                for(;((*it)->getRegion() != region) && (it != MsgBoards.end()); ++it) {}
                //check if the last msgboard is of the right region, else break out
                if ((it == MsgBoards.end()) return pair<cMsgBoards::iterator, cMsgBoards::iterator>(it, it);
                cMsgBoards::iterator itbegin = it;
       	        for(;(it != MsgBoards.end() && ((*it)->getRegion() == region)); ++it) {} //it at the end of the cycle contains the iterator to the first Msgboard in next region or end() if it was already the last
                return pair<cMsgBoards::iterator, cMsgBoards::iterator> (itbegin, it);
}



