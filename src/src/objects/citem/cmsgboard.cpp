/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "network.h"
#include "sregions.h"
#include "sndpkg.h"
#include "cmsgboard.h"
#include "npcai.h"
#include "utils.h"
#include "inlines.h"

/*!
\brief Deleting an MsgBoard message
\author Chronodt and Flameeyes
*/
void cMsgBoard::cMessage::Delete()
{
        switch (availability)
        {
        case ptLocalPost:
		//Here we need only to disconnect local MsgBoard
                // Disconnecting relations from bullettin board which was linked to
		pMsgBoard board = dynamic_cast<pMsgBoard>(getContainer());
		
		board->boardMutex.lock();
		
		MessageList::iterator it = board->boardMsgs.find(this);
		
		if ( it != board->boardMsgs.end() )
			board->boardMsgs.erase(it);
		
		board->boardMutex.unlock();
                break;
        case ptRegionalPost:
		//! \todo Need to rewrite the Regional Post when Regions are completed, until that, break
		break;
        case ptGlobalPost:
		cMsgBoard::globalMutex.lock();
		
		// Here we need only to remove it from the global message list
		MessageList::iterator it = cMsgBoard::globalMsgs.find(this);
		if ( it != cMsgBoard::globalMsgs.end() )
			cMsgBoard::globalMsgs.erase(it);
		
		cMsgBoard::globalMutex.unlock();
                break;
        }
	cItem::Delete();  //this will do the safedelete, too
}

cMsgBoard::cMessage::cMessage()
{
	cMsgBoard::cMessage(nextSerial());
}

cMsgBoard::cMessage::cMessage(uint32_t serial, pMsgBoard board) : cItem(serial)
{
        region = -1;

        availability = ptLocalPost;
        poster = NULL;
        replyof = 0;
        qtype = qtInvalid;
	autopost = false;
        targetnpc = -1;

        // With this line, creating a post sets also automatically the current time, so when it is not startup,
        // there is no need to set it :D
        time( &posttime );

        setAnimId(0xeb0): //Model ID for msgboards items

}

cMsgBoard::cMessage::~cMessage()
{
}

/*!
\brief Gets posttime in a string formatted version
\todo check if there is space for a full time string comprehensive of date in letters rather than "day xxx"
\author Chronodt
*/
std::string cMsgBoard::cMessage::getTimeString()
{
	char *result;
        struct tm timest = localtime( &posttime);
        asprintf( &result, "Day %i @ %i:%02i", timest.tm_yday + 1, timest.tm_hour, timest.tm_min );
        std::string ret = result;
	free(result);
	return ret;
}

/*!
\brief check expiration time of message. If message is expired, it is deleted
\returns true if message expired (and deleted)
\author Chronodt and Flameeyes
*/
bool cMsgBoard::cMessage::expired()
{
	time_t now;
	time( &now );
        long messagelife = now - posttime;  	//messagelife is lifetime of message in seconds
        //here we check if post has a timeout and if it has expired
        switch (qtype)
        {
        case qtInvalid:
        	if ( nSettings::MsgBoards::getMessageRetention() && messagelife > (nSettings::MsgBoards::getMessageRetention() * DAYSECONDS) )
                {
                	//If it isn't a quest, we simply delete the message
			Delete();
                        return true;
                }
		break;
        case qtEscort:
               	if (nSettings::MsgBoards::getEscortInitExpire() && messagelife > nSettings::MsgBoards::getEscortInitExpire()) // This is in second
                {
			pNPC npc = dynamic_cast<pNPC>(targetnpc);
                        if ( npc && npc->getQuestEscortPost() == this ) npc->Delete();
				//If it has not yet disappeared, but the post is still the right we delete it
                        Delete();
                        return true;
                }
		break;
	case qtBounty: //! \todo The server parameter requires a different section for bounty system
        	if (!SrvParms->bountysexpire && messagelife > (SrvParms->bountysexpire * DAYSECONDS ))
                {
			pPC pc = NULL; pNPC npc = NULL;
			if ( ( npc = dynamic_cast<pNPC>(targetnpc) ) && npc->getQuestEscortPost() == this ) npc->Delete();
                        // If it is an npc created just for the bounty (function not yet implemented) but it has not yet
                        // disappeared and the serial is still the right npc we delete it
			if ( ( pc = dynamic_cast<pPC>(targetnpc) ) && pc->getQuestEscortPost() == this )
                        {
                        	//Deleting bounty
				pc->setQuestBountyReward(0);
				pc->setQuestBountyPost(NULL);
                        }
                        Delete();
                        return true;
		}
		break;
	case qtItem:  //not yet implemented, so we always put it to be deleted, as the default
        default:
        	Delete();
                return true;
	}
	return false;
}

/*!
\brief Refreshes a quest
\author Chronodt
\note this should be called by MsgBoardMainteinance. It refreshes the timers of quests
*/
void cMsgBoard::cMessage::refreshQuestMessage()
{
	pPC pc = dynamic_cast<pPC>(targetnpc); pNPC npc = dynamic_cast<pNPC>(targetnpc);
        switch (qtype)
        {
	case qtEscort:
		if ( ! npc || npc->getQuestEscortPost() != this )
			return;
		
		// Now lets reset all of the escort timers after the server has reloaded the WSC file
		// And it doesn't have a player escorting it yet
		if ( pc->ftargserial==INVALID )
		{
			// Lets reset the summontimer to the escortinit
			pc->summontimer = ( getclock() + ( SECS * nSettings::MsgBoards::getEscortInitExpire() ) );
		}
		else // It must have an escort in progress so set the escortactiveexpire timer
		{
			// Lets reset the summontimers to the escortactive value
			pc->summontimer = ( getclock() + ( SECS * nSettings::MsgBoards::getEscortActiveExpire() ) );
		}
		break;

	case qtBounty: //! \todo Bounty system need to be wrote
		if ( ! npc || npc->getQuestEscortPost() != this )
			return;

/*
	NPC bounty code, not yet implemented
******** this is just a cut&paste of escort code quest, update to bounty when activating :D *************

		// Now lets reset all of the escort timers after the server has reloaded the WSC file
		// And it doesn't have a player escorting it yet
		if ( pc->ftargserial==INVALID )
		{
			// Lets reset the summontimer to the escortinit
			pc->summontimer = ( getclock() + ( MY_CLOCKS_PER_SEC * SrvParms->escortinitexpire ) );
		}
		else // It must have an escort in progress so set the escortactiveexpire timer
		{
			// Lets reset the summontimers to the escortactive value
			pc->summontimer = ( getclock() + ( MY_CLOCKS_PER_SEC * SrvParms->escortactiveexpire ) );
		}
*/
		break;
	case qtItem: //! \todo Item quest needs to be wrote
        	//Yet to be implemented
        	break;
	default:
        	ConOut("[WARNING]\n\tUnhandled QuestType found during maintenance\n");
	}
}
//-----------------------------------------------------------------------------------------
//				cMsgBoard Methods
//-----------------------------------------------------------------------------------------

cMsgBoard::cMsgBoard()
{
	cMsgBoard(nextSerial());
}

cMsgBoard::cMsgBoard(uint32_t serial) : cItem(serial)
{
	boardsMutex.lock();
	boards.push_back(this);
	boardsMutex.unlock();
}

cMsgBoard::~cMsgBoard()
{
}

/*!
\param client player client
Used to retrieve the current post type in order to tell the user what type of
mode they are in.
*/
void cMsgBoard::getPostType( pClient client )
{
	pPC pc = client->currChar();
	if ( ! pc ) return;

	switch ( pc->postType )
	{
	case ptLocalPost:
		client->sysmessage("Currently posting regional messages");
		break;

	case ptRegionalPost:
		client->sysmessage("Currently posting regional messages");
		break;

	case ptGlobalPost:
		client->sysmessage("Currently posting global messages");
		break;
	}
	return;
}

/*!
\param client player client
\param nPostType type of post

Used to set the postType for the current user (Typically a GM)
There is a local array that holds each players current posting
type.  Using the command to set the post type updates the
value in the array for that player so that they can post
different types of messages.
*/
void cMsgBoard::setPostType( pClient client, PostType nPostType )
{
	pChar pc=client->currChar();
	if ( ! pc ) return;

	pc->postType = nPostType;

	switch ( nPostType )
	{
		case ptLocalPost: // LOCAL post
			client->sysmessage("Post type set to local");
			break;

		case ptRegionalPost: // REGIONAL post
			client->sysmessage("Post type set to regional");
			break;

		case ptGlobalPost: // GLOBAL POST
			client->sysmessage("Post type set to global");
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
	nPackets::Sent::BBoardCommand pk(this, DisplayBBoard);
	client->sendPacket(&pk);
        
	// .. and immediately thereafter the "items" it contains (the serials of messages connected to that board)
        // but only if it has at least 1 message inside
	//! \todo Need to test for regional messages also
	if ( boardsMsgs.size() || globalMsgs.size() )
	{
        	nPackets::Sent::MsgBoardItemsinContainer pk2 (this);
       		client->sendPacket(&pk2);
        }
}

/*!
\param client player client
\param message message sent by client requiring message's summary
After Bulletin Board is displayed, client asks for summary for each message serial it has received
and this function is then called to send them
*/
void cMsgBoard::sendMessageSummary( pClient client, pMessage message)
{
	nPackets::Sent::BBoardCommand pk(this, SendMessageSummary, message);
	client->sendPacket(&pk);
}

/*!
\brief links message to msgboard
\param message message to be added
*/
bool cMsgBoard::addMessage(pMessage message)
{
	switch (message->availability)
        {
	case ptLocal:
        	//only one insertion needed here, but we first have to verify msgboard capacity against MAXPOSTS
		//Note that this only means that normal players cannot bring a MsgBoard to have more than MAXPOSTS
                //mexes, but by posting regional or global messages this limit can be exceeded
		
		if ( boardsMsgs.size() > nSettings::MsgBoards::getMaxPosts() ) return false;
		boardMsgs.push_back(message);
		message->setContainer(this);
        	break;
        case ptRegional: //!\todo Regional posts...
        	break;
        case ptGlobal:
		globalMsgs.push_back(message);
        }
	return true;
}

/*!
\brief creates an automatic quest message
\note Since they are always general or regional, method is static because no single msgboard is normally selected
\param targetserial serial of the quest's target
\param questtype type of quest
\param region validity region of quest (where is to be posted)
\return serial of message posted
\todo All this should be rewrote using XMLs
*/
uint32_t cMsgBoard::createQuestMessage(QuestType questType, pChar npc, pItem item, int region )
{
	static const char subjectEscort[]     = "Escort: Needed for the day.";  // Default escort message
	static const char subjectBounty[]     = "Bounty: Reward for capture.";  // Default bounty message
	static const char subjectItem[]       = "Lost valuable item.";          // Default item message

        pMsgBoardMessage message = new cMsgBoard::cMessage();

	message->qtype = questType;
	message->autopost = true;
	message->targetnpc = (npc) ? npc->getSerial() : 0;
        message->targetitem = (item) ? item->getSerial() : 0;
        message->region = region;	//if questtype does not need to use a regional post, this is simply ignored :D
        
	int32_t *sectionEntries = new sectionEntries[ nSettings::MsgBoards::getMaxEntries() ];
	// List of SECTION items to store for randomizing

	uint32_t	listCount           = 0;  // Number of entries under the ESCORTS section, used to randomize selection
	int32_t	entryToUse          = 0;  // Entry of the list that will be used to create random message


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
        	message->Delete();
		return 0;
	}

    	cScpIterator* iter = NULL;
    	std::string script1, script2;

	int loopexit=0;

	switch ( questType )
	{
	case ESCORTQUEST:
		// Find the list section in order to count the number of entries in the list
		iter = Scripts::MsgBoard->getNewIterator("SECTION ESCORTS");
		if (iter==NULL)
                {
                	message->Delete();
                        return 0;
                }

		// Count the number of entries under the list section to determine what range to randomize within
		int loopexit=0;
		do
		{
			iter->parseLine(script1, script2);
			if ( !script1.compare("ESCORT") )
			{
				if ( listCount >= nSettings::MsgBoards::getMaxEntries() )
				{
					ErrOut("cMsgBoard::createQuestMessage() Too many entries in ESCORTS list [MAXENTRIES=%d]\n", nSettings::MsgBoards::getMaxEntries() );
					break;
				}

				sectionEntries[listCount] = str2num(script2.c_str());
				listCount++;
			}
		} while ( script1[0]!='}' && script1[0]!=0 	&& (++loopexit < MAXLOOPS) );

		safedelete(iter);

		// If no entries are found in the list, then there must be no entries at all.
		if ( listCount == 0 )
		{
			ConOut( "cMsgBoard::createQuestMessage() No msgboard.scp entries found for ESCORT quests\n" );
		       	message->Delete();
			return 0;
		}

		// Choose a random number between 1 and listCount to use as a message
		entryToUse = RandomNum( 1, listCount );
		
		// Open the script again and find the section choosen by the randomizer
		char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var

		sprintf( temp, "SECTION ESCORT %i", sectionEntries[entryToUse-1] );
		iter = Scripts::MsgBoard->getNewIterator(temp);

		if (iter==NULL)
		{
			ConOut( "cMsgBoard::createQuestMessage() Couldn't find entry %s for ESCORT quest\n", temp );
                      	message->Delete();
			return 0;
		}
		break;
  	case BOUNTYQUEST:
	 	// Find the list section in order to count the number of entries in the list
		// safedelete(iter);
		iter = Scripts::MsgBoard->getNewIterator("SECTION BOUNTYS");
		if (iter==NULL)
		{
        	   	message->Delete();
                	return 0;
	        }

		// Count the number of entries under the list section to determine what range to randomize within
		loopexit=0;
		do
		{
			iter->parseLine(script1, script2);
			if ( !script1.compare("BOUNTY") )
			{
				if ( listCount >= nSettings::MsgBoards::getMaxEntries() )
				{
					ErrOut("cMsgBoard::createQuestMessage() Too many entries in BOUNTYS list [MAXENTRIES=%d]\n", nSettings::MsgBoards::getMaxEntries() );
					break;
				}

				sectionEntries[listCount] = str2num(script2.c_str());
				listCount++;
			}
		} while ( script1[0]!='}' && script1[0]!=0 	&& (++loopexit < MAXLOOPS)  );

		safedelete(iter);

		// If no entries are found in the list, then there must be no entries at all.
		if ( listCount == 0 )
		{
			ConOut( "cMsgBoard::createQuestMessage() No msgboard.scp entries found for BOUNTY quests\n" );
	       		message->Delete();
			return 0;
		}

		// Choose a random number between 1 and listCount to use as a message
		entryToUse = RandomNum( 1, listCount );
		
		// Open the script again and find the section choosen by the randomizer
		char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var

		sprintf( temp, "BOUNTY %i", sectionEntries[entryToUse-1] );

	        safedelete(iter);
		iter = Scripts::MsgBoard->getNewIterator(temp);
		if (iter==NULL)
		{
			ConOut( "cMsgBoard::createQuestMessage() Couldn't find entry %s for BOUNTY quest\n", temp );
	       		message->Delete();
			return 0;
		}
	      	break;
	default:
		ConOut( "cMsgBoard::createQuestMessage() Invalid questType %d\n", questType );
	    	message->Delete();
		return 0;
	}


	////////////////////////////////////////////////////////////////////////////////////
	//  Randomly picked a message, now get the message data and fill in up the buffer //
	////////////////////////////////////////////////////////////////////////////////////

	int flagPos = 0;
	message->body = '\0';  //we start with 0 lines
	if (iter==NULL)
        {
               	message->Delete();
        	return 0;
        }
	script1 = iter->getEntry()->getFullLine();		//discards {

	loopexit=0;
	int loopexit2=0;
	// Read in the random post message choosen above and fill in message body
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
                                	script1.replace(flagpos, 2, item->getCurrentName().c_str());
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
                                	script1.replace(flagpos, 2, npc->getCurrentName().c_str());
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
        	message->Delete();
        	return 0;
        }
        return message->getSerial();

}


/*!
\brief removes an automatic quest message
\param message to remove
*/
void cMsgBoard::removeQuestMessage(pMessage message)
{
	message->Delete();
}

/*!
\brief Message board mainteinance
\note Message retention check, link consistency check and refreshing of quests
\note This function should hold and release the mutexes!
\todo Rewrote this, so that the maintenance thread can use it
*/
void cMsgBoard::MsgBoardMaintenance()
{
	// Display progress message
	InfoOut("Bulletin board maintenance... \n");

	// Now lets find out what posts we keep and what posts to remove
        // to realize that, we create two sets: the first will contain all serials for the loaded messages,
        // the second all the serial linked by bsgboards. If the two are not identical, there are some unlinked
        // posts, to be deleted or moved, depending on the type of message
        std::set<uint32_t> serialsm, serialsb, serialdiff;
        cMsgBoard::cMessages::iterator it = MsgBoardMessages.begin();
	ConOut("Message expiration check : ");
        //checking the expiration time while we insert the posts in the set
        int expired = 0;
        for(;it != MsgBoardMessages.end(); ++it)
        {
        	if (!(*it)->expired()) serialsm.insert((*it)->getSerial());
                else expired++;
                //while browsing the messages and after expiration checks, refresh quests :) 
		if((*it)->qtype != QTINVALID) (*it)->refreshQuest();
        }
      	ConOut("%i message(s) deleted", expired);

	//now the set serialsm is full of the serials of all messages loaded

        cBBRelations::iterator it2 = BBRelations.begin();
        for(;it2 != BBRelations.end(); ++it2) serialsb.insert((*it2).second));
	// now the set serialsb is full of the serials of all messages linked to msgboards. On a set, multiple
        // insertions are ignored if value already present, so we don't need to check it

	std::set<uint32_t>::iterator itdiff = serialdiff.begin();

        // This will fill set serialdiff with the DIFFERENCE between the set containing the serials of
        // existing messages and a set containing all linked messages. If any difference exist, those posts
        // should be deleted (if not quests)
        set_difference(serialsm.begin(), serialsm.end(), serialsb.begin(), serialsb.end(), itdiff);
        if (!serialdiff.empty())
        {
		ConOut("Found %i lost messages. Trying to relink\n", serialdiff.size());
                expired = 0;  // reusing expired variable to count deleted items :D
		for(itdiff = serialdiff.begin();itdiff != serialdiff.end(); ++itdiff)
                {
		        for(it = MsgBoardMessages.begin();(*it)->getSerial() != (*itdiff) && it != MsgBoardMessages.end() ; ++it) {}
			// *it is now the message to be relinked
                        if ((*it)->qtype == QTINVALID)
                        {
                        	if  (!(*it)->getContainer())
                                {
                        		//If the messageboard whose it was linked to no longer exist, delete! (even if not local)
                        		(*it)->Delete();
                                	expired++;
                                }
                                else
                                {
                                	//relink message :)
                                        pMsgBoard board = (pMsgBoard)(*it)->getContainer();
                                        if (!board->addMessage(*it))
                                        {
                                        	//If it couldn't even relink, delete
	                        		(*it)->Delete();
        	                        	expired++;
                                        }
                                }
                                continue;
                        }
                        else if (!relinkQuestMessage(*it))
                        {
                      		(*it)->Delete();
	                      	expired++;
                        }

                }
                ConOut("Relinking complete. %i message(s) relinked, %i message(s) deleted\n", serialdiff.size() - expired, expired);

	}
      	ConOut("[DONE]\n");
}

/*!
\brief returns the region of msgboard
\returns region number of msgboard
*/
int cMsgBoard::getRegion()
{
	return calcRegionFromXY(position);
}
