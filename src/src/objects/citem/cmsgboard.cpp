tare  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
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

void cMsgBoardMessage::Delete() : cItem::Delete()
{
	cPacketSendDeleteObject pk(serial);

        //! \todo replace this when sets redone
        //TODO: if regional or global post it may be necessary to send packet to other pgs too,
        // 	other people near other msgboards that may have been modified
/*
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
                if (it.first == cMsgBoards::MsgBoards.end()) break;	//If no msgboards in region, bail out (but if post exist.. msgboard should exist too :D)
                /*
                We now have a range it.first-it.second of msgBoards that are in the same region as the message to be deleted. (they may even be
                the same)
                Since they are all in the same region, due to RegionSort, this range is sorted by serial number now, and we use this fact:
                BBrelations key are the serials and sorted in the same way, so the find algorithm will NOT need to research from the beginning
                each time. In addition, find algorithm looks for the first (and should be the only) occurrence of the pair, so the search is much
                more time efficient than a whole search from the beginning for each msgboard in region
                */
                cBBRelations::iterator bbit = cMsgBoards::BBRelations.begin();
                for(; (distance(it.first, it.second) >= 0) || (bbit == cMsgBoards::BBRelations.end()); ++it.first)
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
	safedelete(this);
}

cMsgBoardMessage::cMsgBoardMessage()
{
	cMsgBoardMessage(nextSerial());
}

cMsgBoardMessage::cMsgBoardMessage(UI32 serial) : cItem(serial)
{
//	setSerial(serial);
        region = 0;

        availability = LOCALPOST;
        poster = -1;
        replyof = 0;
        qtype = QTINVALID;
	autopost = false;

        time_t now;
        time( &now );
	posttime = *localtime( &now );
        //With these last 3 lines, creating a post sets also automatically the current time, so when it is not startup, there is no need to set it :D

        MsgBoardMessages.push_back(this)



        setAnimId(0xeb0): //Model ID for msgboards items
}

cMsgBoardMessage::~cMsgBoardMessage()
{
        MsgBoardMessages.erase(find(MsgBoardMessages.begin(), MsgBoardMessages.end(), this));
}

std::string cMsgBoardMessage::getTimeString()
{
	char result[25];
        sprintf( result, "Day %i @ %i:%02i", posttime.tm_yday + 1, posttime.tm_hour, posttime.tm_min );
        return std::string(result);
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
void cMsgBoard::getPostType( pClient client )
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
void cMsgBoard::setPostType( pClient client, PostType nPostType )
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
        cPacketSendMsgBoardItemsinContainer pk2 (this);
       	client->sendPacket(&pk2);
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
                if (distance(it.first, it.second) >= MAXPOSTS) return false;

        	BBRelations.insert(cBBRelations::pair(getSerial32(), message->getSerial32()));
        	break;
        case REGIONALPOST;
               	pair<cMsgBoards::iterator, cMsgBoards::iterator> it = getBoardsinRegion(region);
                //if no msgboard in region, it should not post. Only significant if autopost... or else some very weird things are floating around :D
                if (it.first == MsgBoards.end()) return false;
                // We now have a range it.first-it.second of msgBoards that are in the same region as the message. (they may even be the same)
		for (;distance(it.first,it.second) >= 0; ++it.first)
                	BBRelations.insert(cBBRelations::pair((*(it.first))->getSerial32(), message->getSerial32()));
        	break;
        case GLOBALPOST:
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
int MsgBoardPostQuest( int serial, QuestType questType )
{
	TEXT	subjectEscort[]     = "Escort: Needed for the day.";  // Default escort message
	TEXT	subjectBounty[]     = "Bounty: Reward for capture.";  // Default bounty message
	TEXT	subjectItem[]       = "Lost valuable item.";          // Default item message
	TEXT	subject[50]         = "";                             // String that will hold the default subject
	SI32	sectionEntrys[MAXENTRIES];                            // List of SECTION items to store for randomizing

	UI32	listCount           = 0;  // Number of entries under the ESCORTS section, used to randomize selection
	SI32	entryToUse          = 0;  // Entry of the list that will be used to create random message

	SI32	linesInBody         = 0;  // Count of number of lines in body of post
	SI32	lineLength          = 0;  // Length of the line just read in including terminating NULL
	SI32	offset              = 0;  // Offset to next line in buffer
	SI32	numLinesOffset      = 0;  // Offset to the number of lines in body field

	// msg2Post[] Buffer initialization
	msg2Post[0]   = 0x71;   // Packet ID
	msg2Post[1]   = 0x00;   // Size of packet (High byte)
	msg2Post[2]   = 0x00;   // Size of packet (Low byte)

	// This is the type of quest being posted:
	// The value will start arbitrarily at 0xFF and count down
	//    ESCORT = 0xFF (defined in msgboard.h)
	//    BOUNTY = 0xFE
	//    ITEM   = 0xFD
	switch ( questType )
	{
		case ESCORTQUEST:
			msg2Post[3]   = ESCORTQUEST;
			break;

		case BOUNTYQUEST:
			msg2Post[3]   = BOUNTYQUEST;
			break;

		case ITEMQUEST:
			msg2Post[3]   = ITEMQUEST;
			break;
	}

	// Since quest posts can only be regional or global, can use the BullBoard SN fields as CHAR or ITEM fields
	LongToCharPtr(serial, msg2Post +4);  // Normally Bulletin Board SN but used for quests as CHAR or ITEM SN
	LongToCharPtr(0x0000, msg2Post +8);  // Reply to message serial number ( 00 00 00 00 for base post )

    cScpIterator* iter = NULL;
    char script1[1024];
    char script2[1024];

	int loopexit=0;

	safedelete(iter);
	switch ( questType )
	{
	case ESCORTQUEST:
		{
			// Find the list section in order to count the number of entries in the list
			iter = Scripts::MsgBoard->getNewIterator("SECTION ESCORTS");
			if (iter==NULL) return 0;

			// Count the number of entries under the list section to determine what range to randomize within
			int loopexit=0;
			do
			{
				iter->parseLine(script1, script2);
				if ( !(strcmp("ESCORT", script1)) )
				{
					if ( listCount >= MAXENTRIES )
					{
						ErrOut("MsgBoardPostQuest() Too many entries in ESCORTS list [MAXENTRIES=%d]\n", MAXENTRIES );
						break;
					}

					sectionEntrys[listCount] = str2num(script2);
					listCount++;
				}
			} while ( script1[0]!='}' && script1[0]!=0 	&& (++loopexit < MAXLOOPS) );

			safedelete(iter);

			// If no entries are found in the list, then there must be no entries at all.
			if ( listCount == 0 )
			{
				ConOut( "NoX-Wizard: MsgBoardPostQuest() No msgboard.scp entries found for ESCORT quests\n" );
				return 0;
			}

			// Choose a random number between 1 and listCount to use as a message
			entryToUse = RandomNum( 1, listCount );
#ifdef DEBUG
			ErrOut("MsgBoardPostQuest() listCount=%d  entryToUse=%d\n", listCount, entryToUse );
#endif
			// Open the script again and find the section choosen by the randomizer
			char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var

			sprintf( temp, "SECTION ESCORT %i", sectionEntrys[entryToUse-1] );
			iter = Scripts::MsgBoard->getNewIterator(temp);

			if (iter==NULL)
			{
				ConOut( "NoX-Wizard: MsgBoardPostQuest() Couldn't find entry %s for ESCORT quest\n", temp );
				return 0;
			}
			break;
		}

  case BOUNTYQUEST:
    {
			// Find the list section in order to count the number of entries in the list
			safedelete(iter);
			iter = Scripts::MsgBoard->getNewIterator("SECTION BOUNTYS");
			if (iter==NULL) return 0;

			// Count the number of entries under the list section to determine what range to randomize within
			loopexit=0;
			do
			{
				iter->parseLine(script1, script2);
				if ( !(strcmp("BOUNTY", script1)) )
				{
					if ( listCount >= MAXENTRIES )
					{
						ErrOut("MsgBoardPostQuest() Too many entries in BOUNTYS list [MAXENTRIES=%d]\n", MAXENTRIES );
						break;
					}

					sectionEntrys[listCount] = str2num(script2);
					listCount++;
				}
			} while ( script1[0]!='}' && script1[0]!=0 	&& (++loopexit < MAXLOOPS)  );

			safedelete(iter);

			// If no entries are found in the list, then there must be no entries at all.
			if ( listCount == 0 )
			{
				ConOut( "NoX-Wizard: MsgBoardPostQuest() No msgboard.scp entries found for BOUNTY quests\n" );
				return 0;
			}

			// Choose a random number between 1 and listCount to use as a message
			entryToUse = RandomNum( 1, listCount );
#ifdef DEBUG
			ErrOut("MsgBoardPostQuest() listCount=%d  entryToUse=%d\n", listCount, entryToUse );
#endif
			// Open the script again and find the section choosen by the randomizer
 			char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var

			sprintf( temp, "BOUNTY %i", sectionEntrys[entryToUse-1] );

			safedelete(iter);
			iter = Scripts::MsgBoard->getNewIterator(temp);
			if (iter==NULL)
			{
				ConOut( "NoX-Wizard: MsgBoardPostQuest() Couldn't find entry %s for BOUNTY quest\n", temp );
				return 0;
			}
      break;
    }

	default:
		{
			ConOut( "NoX-Wizard: MsgBoardPostQuest() Invalid questType %d\n", questType );
			return 0;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////
	//  Randomly picked a message, now get the message data and fill in up the buffer //
	////////////////////////////////////////////////////////////////////////////////////

	char  *flagPos = NULL;
	char  flag;
	char  tempString[64];

	if (iter==NULL) return 0;
	strcpy(script1, iter->getEntry()->getFullLine().c_str());		//discards {

	// Insert the default subject line depending on the type of quest selected
	switch ( questType )
	{
	case ESCORTQUEST:
		// Copy the default subject to the generic subject string
		strncpy( subject, subjectEscort, sizeof(subject) );
		break;

	case BOUNTYQUEST:
		// Copy the default subject to the generic subject string
		strncpy( subject, subjectBounty, sizeof(subject) );
		break;

	case ITEMQUEST:
		// Copy the default subject to the generic subject string
		strncpy( subject, subjectItem, sizeof(subject) );
		break;

	default:
		ErrOut("MsgBoardPostQuest() invalid quest type\n");
		return 0;
	}

	// Set the SizeOfSubject field in the buffer and copy the subject string to the buffer
	msg2Post[12] = strlen(subject) + 1;
	strncpy( (char*)&msg2Post[13], subject, msg2Post[12] );

	// Set the offset to one past linesInBody count value of the buffer
	// Point to the Size of the line segment
	offset += ( msg2Post[12] + 13 + 1 );

	// Set the subject
	numLinesOffset = offset - 1;

	loopexit=0;
	int loopexit2=0;
	// Read in the random post message choosen above and fill in buffer body for posting
	char *temp = script1; //xan -> holy shit those globals :(
	while ( (++loopexit < MAXLOOPS)  )
	{
		strcpy(script1, iter->getEntry()->getFullLine().c_str());
		// If we reached the ending curly brace, exit the loop
		if ( !strcmp(script1, "}") ) break;

		flagPos = strchr( script1, '%' );

		// Loop until we don't find anymore replaceable parameters
		loopexit2=0;
		while ( flagPos && (++loopexit2 < MAXLOOPS)  )
		{
			if ( flagPos )
			{
				// Move the the letter indicating what text to insert
				flag = *(flagPos + 1);

				// Save the remainder of the original string temporarily
				strcpy( tempString, (flagPos+2) );

				// Replace the flag with the requested text
				P_CHAR pc_s=pointers::findCharBySerial(serial);
				VALIDATEPCR(pc_s,0);
				switch ( flag )
				{
					// NPC Name
				case 'n':
					{
						strcpy( flagPos, pc_s->getCurrentNameC() );
						strcat( temp, tempString );
						break;
					}

					// LOCATION in X, Y coords
				case 'l':
					{
						Location charpos= pc_s->getPosition();
						sprintf( flagPos, "%d, %d", charpos.x, charpos.y );
						strcat( temp, tempString );
						break;
					}

					// NPC title
				case 't':
					{
						strcpy( flagPos, pc_s->title.c_str() );
						strcat( temp, tempString );
						break;
					}

					// Destination Region Name
				case 'r':
					{
						strcpy( flagPos, region[pc_s->questDestRegion].name );
						strcat( temp, tempString );
						break;
					}

					// Region Name
				case 'R':
					{
						strcpy( flagPos, region[pc_s->region].name );
						strcat( temp, tempString );
						break;
					}

					// Gold amount
				case 'g':
					{


						char szBounty[32] = "";

						sprintf(szBounty,"%d",pc_s->questBountyReward) ;
						strcpy( flagPos, szBounty );
						strcat( temp, tempString );
						break;
					}

				default:
					{
						break;
					}
				}

				// Look for another replaceable parameter
				flagPos = strchr( flagPos, '%' );
			}
		}
		// Get the length of the line read into 'temp'
		// after being modified with any extra info due to flags (plus one for the terminating NULL)
		lineLength = ( strlen(temp) + 1 );

		msg2Post[offset] = lineLength;
		offset++;

		// Append the string in the msg2Post buffer
		memcpy( &msg2Post[offset], temp, (lineLength+1) );
		offset += lineLength;

		// Increment the total number of lines read in
		linesInBody++;
	}

	safedelete(iter);

	ShortToCharPtr(offset, msg2Post +1);
	msg2Post[numLinesOffset] = linesInBody;

	// If the message is posted to the message board successfully
	// RETURN 1 otherwise RETURN 0 to indicate a failure of some sort
	// Insert the default subject line depending on the type of quest selected
  switch ( questType )
	{
	case ESCORTQUEST:
    // return the value of the new message serial number ( 0 = failed post )
		return addMessage( 0, REGIONALPOST, 1 );
	case BOUNTYQUEST:
    // return the value of the new message serial number ( 0 = failed post )
		return addMessage( 0, GLOBALPOST, 1 );
	default:
		ErrOut("MsgBoardPostQuest() invalid quest type or quest not implemented\n");
	}

	// Post failed
	return 0;

}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:    MsgBoardQuestEscortCreate( int npcIndex )
//
// PURPOSE:     Used to generate an escort quest based on the currently
//              NPC's serial number
//
// PARAMETERS:  npcIndex     NPC index value in chars[] array
//
// RETURNS:     void
//////////////////////////////////////////////////////////////////////////////
void MsgBoardQuestEscortCreate( int npcIndex )
{
	P_CHAR npc=MAKE_CHAR_REF(npcIndex);
	VALIDATEPC(npc);

	// Choose a random region as a destination for the escort quest (except for the same region as the NPC was spawned in)
	int loopexit=0;
	do
	{
		if ( escortRegions )
		{
			// If the number of escort regions is 1, check to make sure that the only
			// valid escort region is not the NPC's current location - if it is Abort
			if ( (escortRegions==1) && (validEscortRegion[0]==npc->region) )
			{
				npc->questDestRegion = 0;
				break;
			}

			npc->questDestRegion = validEscortRegion[RandomNum(0, (escortRegions-1))];
		}
		else
		{
			npc->questDestRegion = 0;  // If no escort regions have been defined in REGIONS.SCP then we can't do it!!
			break;
		}
	} while ( (npc->questDestRegion == npc->region) 	&& (++loopexit < MAXLOOPS)  );

	// Set quest type to escort
	npc->questType = ESCORTQUEST;

	// Make sure they don't move until an player accepts the quest
	npc->npcWander       = WANDER_NOMOVE;                // Don't want our escort quest object to wander off.
	npc->npcaitype = NPCAI_GOOD;                // Remove any AI from the escort (should be dumb, just follow please :)
	npc->questOrigRegion = npc->region;  // Store this in order to remeber where the original message was posted

	// Set the expirey time on the NPC if no body accepts the quest
	if ( SrvParms->escortinitexpire )
		npc->summontimer = ( uiCurrentTime + ( MY_CLOCKS_PER_SEC * SrvParms->escortinitexpire ) );

	// Make sure the questDest is valid otherwise don't post and delete the NPC
	if ( !npc->questDestRegion )
	{
		ErrOut("MsgBoardQuestEscortCreate() No valid regions defined for escort quests\n");
		npc->Delete();
		//deletechar( npcIndex );
		return;
	}

	// Post the message to the message board in the same REGION as the NPC
	if ( !MsgBoardPostQuest(npc->getSerial32(), ESCORTQUEST) )
	{
		ConOut( "NoX-Wizard: MsgBoardQuestEscortCreate() Failed to add quest post for %s\n", npc->getCurrentNameC() );
		ConOut( "NoX-Wizard: MsgBoardQuestEscortCreate() Deleting NPC %s\n", npc->getCurrentNameC() );
		npc->Delete();
		//deletechar( npcIndex );
		return;
	}

	// Debugging messages
#ifdef DEBUG
	ErrOut("MsgBoardQuestEscortCreate() Escort quest for:\n       %s to be escorted to %s\n", npc->name, region[npc->questDestRegion].name );
#endif

}


//////////////////////////////////////////////////////////////////////////////
// FUNCTION:    MsgBoardQuestEscortArrive( int npcIndex, int pcIndex )
//
// PURPOSE:     Called when escorted NPC reaches its destination
//
// PARAMETERS:  npcIndex   Index number of the NPC in the chars[] array
//              pcIndex    Index number of the player in the chars[] array
//
// RETURNS:     void
//////////////////////////////////////////////////////////////////////////////
void MsgBoardQuestEscortArrive( P_CHAR pc, P_CHAR pc_k)
{



 	char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var
	// Calculate payment for services rendered
	int servicePay = ( RandomNum(0, 20) * RandomNum(1, 30) );  // Equals a range of 0 to 600 possible gold with a 5% chance of getting 0 gold

	// If they have no money, well, oops!
	if ( servicePay == 0 )
	{
		sprintf( temp, TRANSLATE("Thank you %s for thy service. We have made it safely to %s. Alas, I seem to be a little short on gold. I have nothing to pay you with."), pc_k->getCurrentNameC(), region[pc->questDestRegion].name );
		pc->talk( pc_k->getSocket(), temp, 0 );
	}
	else // Otherwise pay the poor sod for his time
	{
		// Less than 75 gold for a escort is pretty cheesey, so if its between 1 and 75, add a randum amount of between 75 to 100 gold
		if ( servicePay < 75 ) servicePay += RandomNum(75, 100);
		pc_k->addGold(servicePay);
		pc_k->playSFX( goldsfx(servicePay) );
		sprintf( temp, TRANSLATE("Thank you %s for thy service. We have made it safely to %s. Here is thy pay as promised."), pc_k->getCurrentNameC(), region[pc->questDestRegion].name );
		pc->talk( pc_k->getSocket(), temp, 0 );
	}

	// Inform the PC of what he has just been given as payment
	pc_k->sysmsg(TRANSLATE("You have just received %d gold coins from %s %s"), servicePay, pc->getCurrentNameC(), pc->title.c_str() );

	// Take the NPC out of quest mode
	pc->npcWander = WANDER_FREELY_CIRCLE;         // Wander freely
	pc->ftargserial = INVALID;            // Reset follow target
	pc->questType = QTINVALID;         // Reset quest type
	pc->questDestRegion = 0;   // Reset quest destination region

	// Set a timer to automatically delete the NPC
	pc->summontimer = ( uiCurrentTime + ( MY_CLOCKS_PER_SEC * SrvParms->escortdoneexpire ) );

	//removefromptr(&cownsp[chars[k].ownserial%HASHMAX], k);
    /*
	pc->own1=255;
	pc->own2=255;
	pc->own3=255;
	pc->own4=255;
	pc->ownserial=-1;
    */
    pc->setOwnerSerial32Only(-1);

}


//////////////////////////////////////////////////////////////////////////////
// FUNCTION:    MsgBoardQuestEscortDelete( int npcIndex )
//
// PURPOSE:     Called when escorted NPC needs to be deleted from the world
//              (just a wrapper in case some additional logic needs to be added)
//
// PARAMETERS:  npcIndex   Index number of the NPC in the chars[] array
//
// RETURNS:     void
//////////////////////////////////////////////////////////////////////////////
void MsgBoardQuestEscortDelete( int npcIndex )
{
	P_CHAR npc=MAKE_CHAR_REF(npcIndex);
	VALIDATEPC(npc);
	npc->Kill();
	npc->Delete();

}


//////////////////////////////////////////////////////////////////////////////
// FUNCTION:    MsgBoardQuestEscortRemovePost( int npcIndex )
//
// PURPOSE:     Marks the posting for a specific NPC for deletion, thereby
//              removing it from the bulletin boards viewable list.
//
// PARAMETERS:  npcIndex   Index number of the NPC in the chars[] array
//
// RETURNS:     void
//////////////////////////////////////////////////////////////////////////////
void MsgBoardQuestEscortRemovePost( int npcIndex )
{
	// Read bbi file to determine messages on boards list
	// Find the post related to this NPC's quest and mark it for deletion
	// thereby removing it from the bulletin boards list

	P_CHAR npc=MAKE_CHAR_REF(npcIndex);
	VALIDATEPC(npc);

	SERIAL npc_serial = npc->getSerial32();

	int loopexit=0;

	FILE *file = NULL;
	// 50 chars for prefix and 4 for the extension plus the ending NULL
	char fileName[256] = "";
 	char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var

	// REGIONAL post file
	sprintf( temp, "region%d.bbi", npc->questOrigRegion );

	// If a MSBBOARDPATH has been define in the SERVER.cfg file, then use it
	if (SrvParms->msgboardpath)
		strcpy( fileName, SrvParms->msgboardpath );

	// Set fileName to REGIONAL.bbi
	//sysmessage( s, "Opening REGIONAL.bbi messages");
	strcat( fileName, temp );
	file = fopen( fileName, "rb+" );

	// If the file exists continue, othewise abort with an error
	if ( file != NULL )
	{
		// Ignore first 4 bytes of bbi file as this is reserverd for the current max message serial number being used
		if ( fseek( file, 4, SEEK_SET ) )
		{
			ErrOut("MsgBoardQuestEscortRemovePost() failed to seek first message seg in bbi\n");
			return;
		}

		// Loop until we have reached the end of the file
		while ( !feof(file) 	&& (++loopexit < MAXLOOPS)  )
		{
			//  | 0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18     NS = NPC Serial
			//  |mg1|mg2|mg3|mg4|mo1|mo2|DEL|sg1|sg2|xx1|xx2|yy1|yy2|NP1|NS2|NS3|NS4|co1|co2|
			// "\x40\x1c\x53\xeb\x0e\xb0\x00\x00\x00\x00\x3a\x00\x3a\x40\x00\x00\x19\x00\x00";

			// Fill up the msg with data from the bbi file
			if ( fread( msg, sizeof(char), 19, file ) != 19 )
			{
				ErrOut("MsgBoardQuestEscortRemovePost() Could not find message to mark deleted\n");
				if ( feof(file) ) break;
			}

			if ( LongFromCharPtr(msg +13)  == npc_serial )
			{
				// Jump back to the DEL segment in order to mark the post for deletion
				fseek( file, -13, SEEK_CUR );

				// Write out the mark for deletion value (0x00)
				fputc( 0, file );

				// We found the message we wanted so break out and close the file
				break;
			}

		}
	}

	// Close bbi file
	if ( file ) fclose( file );

	return;

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
	char                  filePath[256]   = "";
	char                  fileName[256]   = "";
	char                  fileBBITmp[256] = "";
	char                  fileBBPTmp[256] = "";
	UI08                  msg2[MAXBUFFER];

	FILE                  *pBBINew        = NULL;
	FILE                  *pBBIOld        = NULL;

	FILE                  *pBBPNew        = NULL;
	FILE                  *pBBPOld        = NULL;

	// WINDOWS OS structure to be passed to _findfirst() and _findnext()
	// too make this work with LINUX some #ifdef'ing will have to happen.
#if defined(__unix__)
	std::vector<std::string> vecFiles ;
#else
	struct _finddata_t    BBIFile;
	struct _finddata_t    BBPFile;
	long                  hBBIFile = 0;
	long                  hBBPFile = 0;
#endif


	struct tm             currentDate;
	time_t                now;
	int                   dayOfYear;
	int                   postDay;
	int                   postAge;
	int                   count;

	SERIAL                newPostSN  = 0;
	SERIAL                basePostSN = 0;
	unsigned int          sizeOfBBP  = 0;

	UI32		      index = 0 ;

	// Display progress message
	//ErrOut("Bulletin Board Maintenace - Cleaning and compacting BBI & BBP files.\nNoX-Wizard: Progress");
	InfoOut("Bulletin board maintenance... ");

	// Load the MSGBOARDPATH into an array
	// If a MSBBOARDPATH has been define in the SERVER.cfg file, then use it
	if (SrvParms->msgboardpath)
		strcpy( filePath, SrvParms->msgboardpath );
	ConOut("1\n");
	// Set the Tmp file names
	strcpy( fileBBITmp, filePath  );
	strcat( fileBBITmp, "bbi.tmp" );

	strcpy( fileBBPTmp, filePath  );
	strcat( fileBBPTmp, "bbp.tmp" );
	ConOut("2\n");
	// Setup for the starting findfirst() call
#if defined(__unix__)
	vecFiles = MsgBoardGetFile(".bbi",filePath) ;
#else
	strcpy( fileName, filePath );
	strcat( fileName, "*.bbi"  );
#endif
	ConOut("3\n");
	// Calculate current time and date to check if post retention period has expired
	time( &now );
	currentDate = *localtime( &now );
	dayOfYear   = currentDate.tm_yday+1;
	ConOut("4\n");
	// Find a *.BBI file, if none exist then no bulletin boards exist
#if defined(__unix__)
	if(vecFiles.empty())
#else
	if( (hBBIFile = _findfirst( fileName, &BBIFile )) == -1L )
#endif
	{
		//ConOut( "\n\tNo BBI files found." );
		// Setup for the BBP findfirst() call
		ConOut("5\n");
#if defined(__unix__)
		ConOut("Before\n");
		vecFiles = MsgBoardGetFile(".bbp",filePath) ;
		ConOut("After\n");
		if ( vecFiles.empty() )
#else
		strcpy( fileName, filePath );
		strcat( fileName, "*.bbp"  );
		if( (hBBPFile = _findfirst( fileName, &BBPFile )) == -1L )
#endif
			;//ConOut( "\n\tNo BBP files found.\n\tNo Bulletin Board files found to process.\n" );
		else
			ConOut( "[ERROR]\n\tBBP files found with no matching BBI files.\n\t!!! Clean your bulletin board directory !!!\n", filePath );

		// Close the BBI & BBP file handles and exit routine
#ifndef __unix__
		_findclose( hBBIFile );
		_findclose( hBBPFile );
#endif
		ConOut("[DONE]\n");
		return;
	}

	ConOut("FUCK\n");
	// If we made it through the first check then we found a BBI file.
	do
	{
		// Set the number of messages compressed back to 0
		count = 0;

		// Add the file path first then the name of the current BBI file
		strcpy( fileName, filePath     );
	#if defined(__unix__)
		strcat( fileName, vecFiles[index].c_str() );
	#else
		strcat( fileName, BBIFile.name );
	#endif

		// Setting up BBI file for cleaning and compression
		// Rename the BBI file to the temporary file
		rename( fileName, fileBBITmp );

		// Open the new file with the same name as the original BBI file
		pBBINew = fopen( fileName, "wb" );

		// Open the old BBI.TMP file
		pBBIOld = fopen( fileBBITmp, "rb" );

		// Make sure ALL files opened ok
		if ( feof(pBBINew) || feof(pBBIOld) )
		{
			fclose( pBBINew );
			fclose( pBBIOld );

			// Delete the new file if it exists
			remove ( fileName );

			// Put the old file name back
			rename( fileBBITmp, fileName );

			ConOut("[ERROR]\n\t  couldn't open all the BBI files needed, aborting!\n");
			break;
		}

		// Set the fileName to the proper extension for the BBP file
		fileName[strlen(fileName)-1] = 'p';

		// Rename the BBP file to the temporary file
		rename( fileName, fileBBPTmp );

		// Open the new file with the same name as the original BBP file
		if ( ( pBBPNew = fopen( fileName, "a+b" ) )==NULL )
		{
			ConOut("[ERROR]\n\t  couldn't open all the BBP files needed, aborting!\n");
			return;
		}

		// Open the old BBI.TMP file
		if ( ( pBBPOld = fopen( fileBBPTmp, "rb" ) )==NULL )
		{
			ConOut("[ERROR]\n\t  couldn't open all the BBP files needed, aborting!\n");
			fclose( pBBPNew );
			return;
		}

		// Make sure ALL files opened ok
		if ( feof(pBBPNew) || feof(pBBPOld) )
		{
			fclose( pBBPNew );
			fclose( pBBPOld );

			// Delete the new file if it exists
			remove ( fileName );

			// Put the old file name back
			rename( fileBBPTmp, fileName );

			ConOut("[ERROR]\n\t  couldn't open all the BBP files needed, aborting!\n");
			break;
		}

		// Determine what type of file this is and initialize its starting post serial number accordingly
		switch ( fileName[strlen(filePath)] )
		{
			// global.bbp
			case 'g':
				newPostSN = 0x01000000;
				break;

			// region.bbp
			case 'r':
				newPostSN = 0x02000000;
				break;

				// local.bbp ( ie 40000000.bbp )
			default:
				newPostSN = 0x03000000;
				break;
		}

		// Save the newPostSN for the BBP file
		basePostSN = newPostSN;

		// Write out the new base SN to the new BBI file
		LongToCharPtr(newPostSN, msg +0);

		if ( fwrite( msg, sizeof(char), 4, pBBINew ) != 4 )
			ConOut("[FAIL]\n\tMsgBoardMaintenance() Failed to write out newPostSN to pBBINew\n");

		// Now lets find out what posts we keep and what posts to remove

		// Fill post2Keep array with all posts that are not marked for deletion or past the retention period
		// Ignore first 4 bytes of bbi file as this is reserverd for the current max message serial number being used
		if ( fseek( pBBIOld, 4, SEEK_SET ) )
		{
			ConOut("[FAIL]\n\tMsgBoardMaintenance() failed to seek to first message segment in pBBIOld\n");
			return;
		}

		// Loop until we have reached the end of the BBI file
		while ( !feof(pBBIOld) 	&& (loopexit < MAXLOOPS) )
		{
			//Increment progress dots
			ConOut(".");

			// Fill up msg with data from the bbi file
			if ( fread( msg, sizeof(char), 19, pBBIOld ) != 19 )
				if ( feof(pBBIOld) ) break;

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



#if defined(__unix__)

std::vector<std::string> MsgBoardGetFile( char* pattern, char* path)
{

	// Vector of matching files
	std::vector<std::string>   vecFile ;

#ifndef __BEOS__
		long  count ;

		std::string sFilename ;
		std::string sPattern(pattern) ;
		std::string sPath(path) ;

		dirent  **stDirectory ;
		count = scandir(path,&stDirectory,0,alphasort) ;
		if (count > -1)
		{
			while (count--)
			{
				sFilename = sFilename.assign(stDirectory[count]->d_name) ;
				// Was the pattern found?
				if (sFilename.rfind(sPattern) != string::npos)
				{
			 	    // add the file path to it (NO, windows version doesn't either
				    //sFilename = sPath + sFilename ;
				    vecFile.push_back(sFilename) ;
				}
				free(stDirectory[count]) ;
			}
			free(stDirectory) ;
	}

#endif //__BEOS__


	return vecFile ;

}


/*!
\brief gets all MsgBoards in region. NOTE: iterators returned in pair are inclusive!
\param region region to scan for msgboards
\returns if any msgboard is found, returns range (even if both iterators are the same), else both iterators point to MsgBoards.end()
*/

static pair<cMsgBoards::iterator, cMsgBoards::iterator> cMsgBoard::getBoardsinRegion(int region)
{
        	cMsgBoards::iterator it = MsgBoards.begin();
                for(;((*it)->getRegion() != region) && (it != MsgBoards.end()); ++it) {} //With this we find the first board in whick region number is the same as the message to be deleted
                //check if the last msgboard is of the right region, else break out
                if ((it == MsgBoards.end()) return pair<cMsgBoards::iterator, cMsgBoards::iterator>(it, it);
                cMsgBoards::iterator itbegin = it;
       	        for(;((it+1) != MsgBoards.end() && ((*(it+1))->getRegion() == region)); ++it) {} //it at the end of the cycle contains the LAST Msgboard in region
                return pair<cMsgBoards::iterator, cMsgBoards::iterator> (itbegin, it);
}
#endif


