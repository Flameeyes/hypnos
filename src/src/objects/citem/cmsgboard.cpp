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
\author Chronodt (almost total rewrite)
*/

#include "network.h"
#include "sregions.h"
#include "sndpkg.h"
#include "debug.h"
#include "cmsgboard.h"
#include "npcai.h"
#include "utils.h"
#include "inlines.h"

/*!
\brief Deleting an MsgBoard message
\author Chronodt
*/

void cMsgBoardMessage::Delete()
{
/*

All in this comment range is done by cItem::Delete() at the bottom of this method...

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
        	cBBRelations::iterator it = find(cMsgBoards::BBRelations.begin(),cMsgBoards::BBRelations.end(), cBBRelations::pair(getContainer()->getSerial(), serial));
	        if (it != cMsgBoards::BBRelations.end()) cMsgBoards::BBRelations.erase(it);
                break;
        case REGIONALPOST: //Here, instead, we have to disconnect ALL msgBoard in the same region as the board in which the message was originally posted to

        	pair<cMsgBoards::iterator, cMsgBoards::iterator> it = cMsgBoard::getBoardsinRegion(region);
                if (it.first == cMsgBoards::MsgBoards.end()) break;	//If no msgboards in region, bail out
                /*
                We now have a range it.first-it.second of msgBoards that are in the same region as the message to be deleted. (they are returned
                as first in region - first in next region, so they are never the same unless they are both end(), and it means no msgboards in region)
                Since they are all in the same region, due to RegionSort, this range is sorted by serial number now, and we use this fact:
                BBrelations key is the serial and sorted in the same way, so the find algorithm will NOT need to research from the beginning
                each time. In addition, find algorithm looks for the first (and should be the only) occurrence of the pair, so the search is much
                more time efficient than a whole search from the beginning for each msgboard in region
                */
                cBBRelations::iterator bbit = cMsgBoards::BBRelations.begin();
                for(; (it.first != it.second) && (bbit != cMsgBoards::BBRelations.end()); ++it.first)
                {
                        //finding the current serial and set it for next search, since it will be only on following serials
			bbit = find(bbit,cMsgBoards::BBRelations.end(), cBBRelations::pair((*itbegin)->getSerial(), serial));
               	        //bbit now contains the iterator that points to the pair msgboard serial - message serial to delete
                       	if (bbit == cMsgBoards::BBRelations.end() && ((*bbit) != cBBRelations::pair((*itbegin)->getSerial(), serial)) )
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
                        cBBRelations::iterator it2 = find(cMsgBoards::BBRelations.begin(),cMsgBoards::BBRelations.end(), cBBRelations::pair((*it)->getSerial(), serial));
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

cMsgBoardMessage::cMsgBoardMessage(uint32_t serial) : cItem(serial)
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

/*!
\brief Gets posttime in a string formatted version
\todo check if there is space for a full time string comprehensive of date in letters rather than "day xxx"
\author Chronodt
*/
std::string cMsgBoardMessage::getTimeString()
{
	char result[25];
        struct tm timest = localtime( &posttime);
        sprintf( result, "Day %i @ %i:%02i", timest.tm_yday + 1, timest.tm_hour, timest.tm_min );
        return std::string(result);
}

/*!
\brief check expiration time of message. If message is expired, it is deleted
\returns true if message expired (and deleted)
\author Chronodt
*/

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
                	pNPC npc = cSerializable::findCharBySerial(targetnpc);
                        if (!npc && npc->questEscortPostSerial == getSerial()) npc->Delete();	//If it has not yet disappeared, but the serial is still the right escort npc (the serial may have been reused!!) we delete it
                        Delete();
                        return true;
                }
		break;
	case BOUNTYQUEST:
        	if (!SrvParms->bountysexpire && messagelife > (SrvParms->bountysexpire * 86400))	//86400 = 24 * 60 * 60 -> bountysexpire is expressed in days while messagelife in seconds
                {
                        pChar pc = cSerializable::findCharBySerial(targetnpc);
                        // If it is an npc created just for the bounty (function not yet implemented) but it has not yet
                        // disappeared and the serial is still the right npc (the serial may have been reused!!) we delete it
                        if (!pc && pc->rtti() == rtti::cNPC && pc->questBountyPostSerial == getSerial()) pc->Delete();
			if (!pc && pc->rtti() == rtti::cPC && pc->questBountyPostSerial == getSerial())
                        {
                        	//Deleting bounty
				pc->questBountyReward     = 0;
				pc->questBountyPostSerial = 0;
                        }
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

/*!
\brief Refreshes a quest
\author Chronodt
\note this should be called by MsgBoardMainteinance. It refreshes the timers of quests
*/
void cMsgBoardMessage::refreshQuestMessage()
{
	pChar pc=cSerializable::findCharBySerial(targetnpc);
        pItem item = cSerializable::findItemBySerial(targetitem);
        switch (qtype)
        {
	case ESCORTQUEST:
		if (pc)
		{
			if ( pc->rtti() == rtti::cNPC &&  pc->questType == ESCORTQUEST  )
			{
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
			}
		}
		break;

	case BOUNTYQUEST:
		if (pc)
		{
			if ( pc->rtti() == rtti::cNPC &&  pc->questType == BOUNTYQUEST )
			{
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
			}
		}
		break;
	case ITEMQUEST:
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
void cMsgBoard::getPostType( pClient client )
{
	pPC pc = client->currChar();
	if ( ! pc ) return;

	PostType type = pc->postType;

	switch ( type )
	{
		case LOCALPOST: // LOCAL post
			client->sysmessage("Currently posting LOCAL messages");
			break;

		case REGIONALPOST: // REGIONAL post
			client->sysmessage("Currently posting REGIONAL messages");
			break;

		case GLOBALPOST: // GLOBAL POST
			client->sysmessage("Currently posting GLOBAL messages");
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
		case LOCALPOST: // LOCAL post
			client->sysmessage("Post type set to LOCAL");
			break;

		case REGIONALPOST: // REGIONAL post
			client->sysmessage("Post type set to REGIONAL");
			break;

		case GLOBALPOST: // GLOBAL POST
			client->sysmessage("Post type set to GLOBAL");
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
      	if (BBRelations.find(getSerial()) != BBRelations.end())
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
                
               	pair<cBBRelations::iterator, cBBRelations::iterator> it = BBRelations.equal_range(getSerial());
                if ( distance(it.first, it.second) > nSettings::MsgBoards::getMaxPosts() ) return false;

        	BBRelations.insert(cBBRelations::pair(getSerial(), message->getSerial()));
        	break;
        case REGIONALPOST;
               	pair<cMsgBoards::iterator, cMsgBoards::iterator> it = getBoardsinRegion(region);
                //if no msgboard in region, it should not post. Only significant if autopost... or else some very weird things are floating around :D
                if (it.first == MsgBoards.end()) return false;
                // We now have a range it.first-(it.second - 1) of msgBoards that are in the same region as the message.
		for (;it.first != it.second; ++it.first)
                	BBRelations.insert(cBBRelations::pair((*(it.first))->getSerial(), message->getSerial()));
        	break;
        case GLOBALPOST:
        	if (MsgBoards.empty()) return false; //Obiously, even general posts cannot be done when NO msgboards are present at all....
        	for(cMsgBoards::iterator it = MsgBoards.begin(), it != MsgBoards.end(), ++it)
                      	BBRelations.insert(cBBRelations::pair((*it)->getSerial(), message->getSerial()));
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
*/

uint32_t cMsgBoard::createQuestMessage(QuestType questType, pChar npc, pItem item, int region )
{
	static const char subjectEscort[]     = "Escort: Needed for the day.";  // Default escort message
	static const char subjectBounty[]     = "Bounty: Reward for capture.";  // Default bounty message
	static const char subjectItem[]       = "Lost valuable item.";          // Default item message


        pMsgBoardMessage message = new cMsgBoardMessage();

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
#ifdef DEBUG
		ErrOut("cMsgBoard::createQuestMessage() listCount=%d  entryToUse=%d\n", listCount, entryToUse );
#endif
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
#ifdef DEBUG
		ErrOut("cMsgBoard::createQuestMessage() listCount=%d  entryToUse=%d\n", listCount, entryToUse );
#endif
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
\param messageserial serial of the message to delete
*/

void cMsgBoard::removeQuestMessage(uint32_t messageserial)
{
	cMsgBoardMessages::iterator it = MsgBoardMessages.begin();
        for (; (*it)->getSerial() != messageserial && it != MsgBoardMessages.end(); ++it) {}
        if (it != MsgBoardMessages.end()) (*it)->Delete();
}




/*!
\brief Message board mainteinance
\note Message retention check, link consistency check and refreshing of quests
\todo check if moving msgboards with regional posts outside its region causes trouble, and if so do a region check and relink here 
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
        cMsgBoardMessages::iterator it = MsgBoardMessages.begin();
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
\brief gets all MsgBoards in region.
\note the first iterator in the pair is the first of the range, but the second is the first of NEXT board or end()
\returns if any msgboard is found, returns as range [first,second[ , else both iterators point to MsgBoards.end()
*/

pair<cMsgBoards::iterator, cMsgBoards::iterator> cMsgBoard::getBoardsinRegion(int region)
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

/*!
\brief returns the region of msgboard
\returns region number of msgboard
*/

int cMsgBoard::getRegion()
{
	return calcRegionFromXY(position);
}
