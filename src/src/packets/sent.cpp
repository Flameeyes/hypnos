/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Implementation of packets classes
*/

#include "cpacket.h"

void cPacketSendAction::prepare()
{
	static const uint8_t templ[14] = {
		0x6E, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02,
		0x00, 0x05, 0x00, 0x01, 0x0, 0x00, 0x01
		};

	buffer = new uint8_t[14];
	length = 14;
	memcpy(buffer, templ, 14);
	LongToCharPtr(serial, buffer+1);
	ShortToCharPtr(action, buffer+5);
}

void cPacketDrawContainer::prepare()
{
	static const uint8_t templ[7] = {
		0x24, 0x40, 0x0B, 0x00, 0x1A, 0x00, 0x3C
		};

	buffer = new uint8_t[7];
	length = 7;
	memcpy(buffer, templ, 7);

	LongToCharPtr(serial, buffer+1);
	ShortToCharPtr(gump, buffer+5);
}

void cPacketSendContainerItem::prepare()
{
	static const uint8_t templ1[5] = {
		0x3C, 0x00, 0x05, 0x00, 0x00
		};

	static const uint8_t templ2[19] = {
		0x40, 0x0D, 0x98, 0xF7, 0x0F, 0x4F, 0x00,
		0x00, 0x09, 0x00, 0x30, 0x00, 0x52, 0x40,
		0x0B, 0x00, 0x1A, 0x00, 0x00
		};

	uint16_t count = items.count();
	uint16_t length = 5 + count*19;

	buffer = new uint8_t[length];
	memcpy(buffer, templ1, 5);

	ShortToCharPtr(length, buffer+1);
	ShortToCharPtr(count, buffer+3);

	uint8_t *ptrItem = buffer+5;

	for(std::list<sContainerItem>::iterator it = items.begin(); it != items.end(); it++)
	{
		memcpy(ptrItem, templ2, 19);
		LongToCharPtr((*it).serial, ptrItem);
		ShortToCharPtr((*it).id, ptrItem+4);
		ShortToCharPtr((*it).amount, ptrItem+7);
		ShortToCharPtr((*it).x, ptrItem+9);
		ShortToCharPtr((*it).y, ptrItem+11);
		LongToCharPtr(serial, ptrItem+13);
		ShortToCharPtr((*it).color, ptrItem+17);

		ptrItem += 19;
	}
}

void cPacketSendAddItemtoContainer::prepare()
{
	length = 20;
	buffer = new uint8_t[20];

	buffer[0] = 0x25;
	buffer[7] = 0x00;

	LongToCharPtr(item->getSerial(), buffer+1);

       	//AntiChrist - world light sources stuff
	//if player is a gm, this item
	//is shown like a candle (so that he can move it),
	//....if not, the item is a normal
	//invisible light source!
        if(pc->IsGM() && item.id == 0x1647)
        {
        	ShortToCharPtr(0x0A0F, ptrItem+5);
                ShortToCharPtr(0x00C6, ptrItem+18);
        }
        else
        {
		ShortToCharPtr(item->getAnimId(), ptrItem+5);
                ShortToCharPtr(item->getColor(), ptrItem+18);
        }
	ShortToCharPtr(item->getAmount(), ptrItem+8);
	ShortToCharPtr(item->getLocation().x, ptrItem+10);
	ShortToCharPtr(item->getLocation().y, ptrItem+12);
	LongToCharPtr(item->getContainer()->getSerial(), ptrItem+14);

}

void cPacketSendWornItem::prepare()
{
	length = 15;
	buffer = new uint8_t[15];

	buffer[0] = 0x2E;
	buffer[7] = 0x00;

	LongToCharPtr(item->getSerial(), buffer+1);
	ShortToCharPtr(item->getAnimId(), buffer+5);
	buffer[8] = item->getLayer();
	LongToCharPtr(item->getContainer()->getSerial(), buffer+9);
	ShortToCharPtr(item->getColor(), buffer+13);
}

void cPacketSendSoundFX::prepare()
{
	static const uint8_t templ[12] = {
		0x54, 0x01, 0x12, 0x34, 0x00, 0x00,
		0x06, 0x40, 0x05, 0x9A, 0x00, 0x00
		};

	buffer = new uint8_t[12];
	length = 12;
	memcpy(buffer, templ, 12);

	int16_t z = loc.z;

	ShortToCharPtr(model, buffer +2);
	ShortToCharPtr(loc.x, buffer +6);
	ShortToCharPtr(loc.y, buffer +8);
	ShortToCharPtr(z, buffer +10);
}

void cPacketSendDeleteObj::prepare()
{
	length = 5;
	buffer = new uint8_t[5];

	buffer[0] = 0x1D;
	LongToCharPtr(serial, buffer+1);
}

void cPacketSendSkillState::prepare()
{
	length = 4 + TRUESKILLS*7 + 2;
	buffer = new uint8_t[length];

	buffer[0] = 0x3A;
	ShortToCharPtr(length, buffer+1);
	buffer[3] = 0x00;

	uint8_t *skill = buffer+4;
	for (int i=0; i<TRUESKILLS; i++)
	{
		Skills::updateSkillLevel(pc,i);

		ShortToCharPtr(i+1, skill);
		ShortToCharPtr(pc->getSkill(i), skill +2);
		ShortToCharPtr(pc->getSkillBase(i), skill +4);

		skill[6] = pc->getSkillLock(i);

		skill += 7;
	}
	ShorToCharPtr(0, skill);
}

void cPacketSendUpdateSkill::prepare()
{
	length = 11;
	buffer = new uint8_t[11];

	buffer[0] = 0x3A;
	ShortToCharPtr(11, buffer+1);
	buffer[3] = 0xFF;

	Skills::updateSkillLevel(pc, skill);
	ShortToCharPtr(skill+1, buffer +4);
	ShortToCharPtr(pc->getSkill(skill), buffer +6);
	ShortToCharPtr(pc->getSkillBase(skill), buffer +8);

	buffer[10] = pc->getSkillLock(skill);
}

void cPacketSendOpenBrowser::prepare()
{
	length = url.size() + 3;
	buffer = new uint8_t[length];

	buffer[0] = 0xA5;
	ShortToCharPtr(length, buffer+1);

	memcpy(buffer+3, url.c_str(), length-3);
}

void cPacketSendPlayMidi::prepare()
{
	length = 3;
	buffer = new uint8_t[3];

	buffer[0] = 0x6D;
	ShortToCharPtr(id, buffer+1);
}

void cPacketSendOverallLight::prepare()
{
	length = 2;
	buffer = new uint8_t[2];

	buffer[0] = 0x4F;
	buffer[1] = level;
}

void cPacketSendStatus::prepare()
{
	length = 0;
	if ( ! pc ) return;

	pBody body = pc->getBody();
	if ( ! body ) return;

	switch(type)
	{
		case 0: length = 43; break;
		case 1: length = 66; break;
		case 3: length = 70; break;
		case 4: length = 88; break;
		default: return;
	}

	buffer = new uint8_t[length];
	buffer[0] = 0x11;
	ShortToCharPtr(length, buffer+1);
	ShortToCharPtr(pc->getSerial(), buffer+3);

	memset(buffer+7, 0, 30);
	strncpy(buffer+7, body->getName().c_str(), 30);


        ShortToCharPtr(body->getHitPoints(), buffer+37);
        ShortToCharPtr(body->getMaxHitPoints(), buffer+39);

	buffer[41] = canrename ? 0xFF : 0x00;
	buffer[42] = type;

	switch(type)
	{
		case 4: prepare4();
		case 3: prepare3();
		case 1: prepare1();
	}
}

void cPacketSendStatus::prepare1()
{
	pBody body = pc->getBody();
	buffer[43] = body->getGender();

	ShortToCharPtr(body->getStrength(), buffer+44);
	ShortToCharPtr(body->getDexterity(), buffer+46);
	ShortToCharPtr(body->getIntelligence(), buffer+48);
	ShortToCharPtr(body->getStamina(), buffer+50);
	ShortToCharPtr(body->getMaxStamina(), buffer+52);
	ShortToCharPtr(body->getMana(), buffer+54);
	ShortToCharPtr(body->getMaxMana(), buffer+56);

	uint32_t gold = pc->rtti() == rtti::rttiPC ? (reinterpret_cast<pPC*>pc)->getGold : 0;
	LongToCharPtr(gold, buffer+58);

	ShortToCharPtr(body->getArmor(), buffer+62);
	ShortToCharPtr(body->getWeight(), buffer+64);
}

void cPacketSendStatus::prepare3()
{
	pBody body = pc->getBody();

	ShortToCharPtr(body->getStatCap(), buffer+66);
	buffer[68] = body->getFollowers();
	buffer[69] = body->getMaxFollowers();

}

void cPacketSendStatus::prepare4()
{
	pBody body = pc->getBody();

	ShortToCharPtr(body->getResistFire(), buffer+70);
	ShortToCharPtr(body->getResistCold(), buffer+72);
	ShortToCharPtr(body->getResistPoison(), buffer+74);
	ShortToCharPtr(body->getResistEnergy(), buffer+76);

	ShortToCharPtr(body->getLuck(), buffer+78);
	ShortToCharPtr(body->getDamageMin(), buffer+80);
	ShortToCharPtr(body->getDamageMax(), buffer+82);

	memset(buffer+84, 0, 4);
}


void cPacketSendClearBuyWindow::prepare()
{
	length = 8;
	buffer = new uint8_t[8];
	buffer[0] = 0x3B;
	ShortToCharPtr(0x08, buffer +1);			// Packet len
	LongToCharPtr( npc->getSerial(), buffer + 3);	        // vendorID
	buffer[7]=0x00;						// Flag:  0 => no more items  0x02 items following ...
}


void cPacketSendPaperdollClothingUpdated::prepare()
{
	length = 1;
	buffer = new uint8_t[1];
	buffer[0] = 0x29;
}

void cPacketSendOpenMapGump::prepare()
{
	length = 19;
	buffer = new uint8_t[19];
        buffer[0] = 0x90;
	LongToCharPtr(map->getSerial(), buffer +1);
        ShortToCharPtr(0x139D, buffer + 5);
	buffer[7]  = map->more1;	// Assign topleft x
	buffer[8]  = map->more2;
	buffer[9]  = map->more3;	// Assign topleft y
	buffer[10] = map->more4;
	buffer[11] = map->moreb1;	// Assign lowright x
	buffer[12] = map->moreb2;
	buffer[13] = map->moreb3;	// Assign lowright y
	buffer[14] = map->moreb4;
	int width, height;		// Temporary storage for w and h;
	width = 134 + (134 * morez);	// Calculate new w and h
	height = 134 + (134 * morez);
	ShortToCharPtr(width, map1 +15);
	ShortToCharPtr(height, map1 +17);
}

void cPacketSendMapPlotCourse::prepare()
{
	length = 11;
	buffer = new uint8_t[11];
        buffer[0] = 0x56;
	LongToCharPtr(map->getSerial(), buffer +1);
	buffer[5]  = command;
	if (command == WriteableStatus) buffer[6] = (pin) ? 1 : 0;  // not sure if the values needed by client are exactly 1 and 0, but better safe than sorry
        else buffer[6] = pin;
        ShortToCharPtr(x, buffer + 7);
        ShortToCharPtr(y, buffer + 9);
}

void cPacketSendBBoardCommand::prepare()
{
	switch (command)
        {
        case DisplayBBoard: //Display bulletin board
		length = 38;
	        buffer = new uint8_t[38];
	        buffer[0] = 0x71;
	        ShortToCharPtr(38, buffer +1); 	//message length
	        buffer[3] = 0; 			//subcommand 0
	        LongToCharPtr(msgboard->getSerial(), buffer + 4);	//board serial
	        memset(buffer + 8, 0, 22);				//filling boardname with 22 zeroes
	        LongToCharPtr(0x402000FF, buffer + 30);			//gump id, i believe
	        LongToCharPtr(0x0, buffer + 34);			//zero

		// If the name the item (Bulletin Board) has been defined, display it
		// instead of the default "Bulletin Board" title.
		if ( strncmp(msgboard->getCurrentNameC(), "#", 1) )
			strncpy( buffer + 8, msgboard->getCurrentNameC(), 20);  //Copying just the first 20 chars or we go out of bounds in the gump
	        else    strcpy( buffer + 8, "Bulletin Board");
                break;
        case SendMessageSummary:
        	//calculating length of packet  before building buffer
        	length = 16; //BASE length, the length of fixed-length components
                pChar poster = pointers::findCharBySerPtr(message->poster);
                std::string timestring = message->getTimeString();
                length += poster->getCurrentNameC().size() + 2;
                length += message->subject.size() + 2;
                length += timestring.size() + 2;
                buffer = new uint8_t[length];
	        buffer[0] = 0x71;
	        ShortToCharPtr(length, buffer +1); 	//message length
	        buffer[3] = 1; 				//subcommand 1
	        LongToCharPtr(msgboard->getSerial(), buffer + 4);	//board serial
 	        LongToCharPtr(message->getSerial(), buffer + 8);	//message serial
                LongToCharPtr(message->replyof, buffer + 12);		//parent message serial
                int offset = 16;
                buffer[16] = poster->getCurrentNameC().size() + 1;	//size() does not count the endstring 0
		strcpy( buffer + 17, poster->getCurrentNameC().c_str());
                offset += poster->getCurrentNameC().size() + 2;
                buffer[offset] = message->subject.size() + 1;
                strcpy( buffer + offset + 1, message->subject.c_str());
                offset += message->subject.size() + 2;
                buffer[offset] = timestring.size() + 1;
                strcpy( buffer + offset + 1, timestring.c_str());
                break;
        case SendMessageBody:
        	static const char pattern[29] = { 0x01, 0x90, 0x83, 0xea, 0x06,
                				  0x15, 0x2e, 0x07, 0x1d, 0x17,
                                                  0x0f, 0x07, 0x37, 0x1f, 0x7b,
                                                  0x05, 0xeb, 0x20, 0x3d, 0x04,
                                                  0x66, 0x20, 0x4d, 0x04, 0x66,
                                                  0x0e, 0x75, 0x00, 0x00};
                //calculating length of packet  before building buffer
        	length = 12; //BASE length, the length of fixed-length components
                pChar poster = pointers::findCharBySerPtr(message->poster);
                std::string timestring = message->getTimeString();
                length += poster->getCurrentNameC().size() + 2;
                length += message->subject.size() + 2;
                length += timestring.size() + 2;
                buffer = new uint8_t[length];
	        buffer[0] = 0x71;
	        ShortToCharPtr(length, buffer +1); 	//message length
	        buffer[3] = 2; 				//subcommand 2
	        LongToCharPtr(msgboard->getSerial(), buffer + 4);	//board serial
 	        LongToCharPtr(message->getSerial(), buffer + 8);	//message serial
                int offset = 12;
                buffer[12] = poster->getCurrentNameC().size() + 1;	//size() does not count the endstring \0 :)
		strcpy( buffer + 17, poster->getCurrentNameC().c_str());
                offset += poster->getCurrentNameC().size() + 2;
                buffer[offset] = message->subject.size() + 1;
                strcpy( buffer + offset + 1, message->subject.c_str());
                offset += message->subject.size() + 2;
                buffer[offset] = timestring.size() + 1;
                strcpy( buffer + offset + 1, timestring.c_str());
                offset += message->timestring.size() + 2;
		strncpy( buffer + offset + 1, pattern, 29);
                offset +=29;
                strncpy( buffer + offset + 1, message->body->data(),message->body.size());
        }
}

void cPacketSendMsgBoardItemsinContainer::prepare()
{
 	static const uint8_t templ1[5] = {
		0x3C, 0x00, 0x05, 0x00, 0x00
		};

	static const uint8_t templ2[19] = {
		0x00, 0x00, 0x00, 0x00, 0x0E, 0xB0, 0x00,
		0x00, 0x00, 0x00, 0x3A, 0x00, 0x3A, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00
		};

	pair<cBBRelations::iterator, cBBRelations::iterator> it = cMsgBoard::BBRelations.equal_range(msgboard->getSerial());
        // Now the *(it.first.second) is the serial of a message to be sent. and incrementing it.first
        // until it reaches it.second we obtain all the serials

	uint16_t count = distance (it.first, it.second); //It HAS at least 1 message or else this packet would not be asked for
	uint16_t length = 5 + count*19;

	buffer = new uint8_t[length];
	memcpy(buffer, templ1, 5);

	ShortToCharPtr(length, buffer+1);
	ShortToCharPtr(count, buffer+3);

	uint8_t *ptrItem = buffer+5;

	for(;it.first != it.second; ++(it.first))
	{
		memcpy(ptrItem, templ2, 19);
		LongToCharPtr((*it.first)->getSerial(), ptrItem);
		LongToCharPtr(msgboard->getSerial(), ptrItem+13);
                //all other parts are invaried in msgboards messages and are defaulted in templ2
		ptrItem += 19;
	}

}


void cPacketSendSecureTradingStatus::prepare()
{
        buffer = new uint8_t[17];
        length = 17;
	buffer[0] = 0x6F;
        ShortToCharPtr(17, buffer+1); 	//Size - no name in this message -  so length is fixed
	buffer[3]=action;	      	//State
	LongToCharPtr(id1, buffer +4);
	LongToCharPtr(id2, buffer +8);
	LongToCharPtr(id3, buffer +12);
	buffer[16]=0; 			// No name in this message
}

void cPacketSendUpdatePlayer::prepare()
{
	buffer = new uint8_t[17];
        length = 17;
	buffer[0]=0x77;
	Location pos = chr->getPosition();
	LongToCharPtr(chr->getSerial(), buffer +1);
	ShortToCharPtr(chr->getId(), buffer +5);
	ShortToCharPtr(pos.x, buffer +7);
	ShortToCharPtr(pos.y, buffer +9);
	buffer[11]=pos.dispz;
	buffer[12]=dir;
	ShortToCharPtr(chr->getColor(), buffer +13);
	buffer[15]=flag;
	buffer[16]=hi_color;
}

void cPacketSendWarModeStatus::prepare()
{
	buffer = new uint8_t[5];
        length = 5;
	memcpy(buffer, buf, 5);
}

void cPacketSendPingReply::prepare()
{
	buffer = new uint8_t[2];
        length = 2;
	memcpy(buffer, buf, 2);
}




/* PkG 0x85: Char Delete Error
 *	reason:
 *      0x00 => That character password is invalid.
 *      0x01 => That character doesn't exist.
 *      0x02 => That character is being played right now.
 *      0x03 => That charater is not old enough to delete.
                The character must be 7days old before it can be deleted.
 *      0x04 => That character is currently queued for backup and cannot be
 *              deleted.
 *      0x05 => Couldn't carry out your request.
 */

void cPacketSendCharAfterDelete::prepare()
{
	buffer = new uint8_t[2];
        length = 2;
        buffer[0] = 0x85;
	buffer[1] = reason;
}


void cPacketSendCharAfterDelete::prepare()
{
	buffer = new uint8_t[304];
        length = 304;
        memset(buffer, 304,0); //filling the buffer with zeroes
        buffer[0] = 0x86;
        ShortToCharPtr(304, buffer + 1);
        buffer[3] = account->getCharsNumber();
        for(int i = 0;i<5; ++i)
                if (i<= buffer[3])
                	strcpy(buffer + (i*60) + 4, account->getChar(i)->getCurrentNameC());
}


static pPacketReceive cPacketReceive::fromBuffer(uint8_t *buffer, uint16_t length)
{
       switch(buffer[0])
       {
                case 0x00: return new cPacketReceiveCreateChar(buffer, length);         // Create Character
                case 0x01: return new cPacketReceiveDisconnectNotify(buffer, length);   // Disconnect Notification
                case 0x02: return new cPacketReceiveMoveRequest(buffer, length);        // Move Request
                case 0x03: return new cPacketReceiveTalkRequest(buffer, length);        // Talk Request
                case 0x04: return NULL;							// God mode toggle
                case 0x05: return new cPacketReceiveAttackRequest(buffer, length);      // Attack Request
                case 0x06: return new cPacketReceiveDoubleclick(buffer, length);        // Double click
                case 0x07: return new cPacketReceivePickUp(buffer, length);             // Pick Up Item(s)
                case 0x08: return new cPacketReceiveDropItem(buffer, length);           // Drop Item(s)
                case 0x09: return new cPacketReceiveSingleclick(buffer, length);        // Single click
                case 0x12: return new cPacketReceiveActionRequest(buffer, length);      // Request Skill/Action/Magic Usage
                case 0x13: return new cPacketReceiveWearItem(buffer, length);           // Drop - Wear Item
                case 0x22: return new cPacketReceiveResyncRequest(buffer, length);      // when received, this packet is a Resync Request
                case 0x2c: return new cPacketReceiveRessChoice(buffer, length);         // (Obsolete) Resurrection Menu Choice
                case 0x34: return new cPacketReceiveStatusRequest(buffer, length);      // Get Player Status

                // packet 0x38 (pathfinding) is received or sent???

                case 0x3a: return new cPacketReceiveSetSkillLock(buffer, length);       // Set Skill Lock (receive version of packet 0x3a)
                case 0x3b: return new cPacketReceiveBuyItems(buffer, length);           // Buy Item(s)
                case 0x56: return new cPacketReceiveMapPlotCourse(buffer, length);      // Map Related
                case 0x5d: return new cPacketReceiveLoginChar(buffer, length); 		// Login Character
                case 0x66: return new cPacketReceiveBookPage(buffer, length); 	 	// Books - Page (receive version)
                case 0x69: return NULL; 						// (Obsolete) Change Text/Emote Color
                case 0x6c: return new cPacketReceiveTargetSelected(buffer, length);	// Targeting Cursor Commands
                case 0x6f: return new cPacketReceiveSecureTrade(buffer,length);		// Secure Trading
                case 0x71: return new cPacketReceiveBBoardMessage(buffer, length); 	// Bulletin Board Message
                case 0x72: return new cPacketReceiveWarModeChange(buffer, length); 	// Request War Mode Change/Send War Mode status
                case 0x73: return new cPacketReceivePing(buffer, length); 		// Ping message
               	case 0x75: return new cPacketReceiveRenameCharacter(buffer, length); 	// New name for a character
                case 0x7d: return new cPacketReceiveDialogResponse(buffer, length); 	// Client Response To Dialog
                case 0x80: return new cPacketReceiveLoginRequest(buffer, length); 	// Login Request
                case 0x83: return new cPacketReceiveDeleteCharacter(buffer, length); 	// Delete Character
                case 0x91: return new cPacketReceiveGameServerLogin(buffer, length);	// Game Server Login (Server to play selected)
                case 0x93: return new cPacketReceiveBookUpdateTitle(buffer, length);	// Books – Update Title Page (receive version of packet 0x93)
                case 0x95: return new cPacketReceiveDyeItem(buffer, length); 		// Dye item

                //Does this have to be implemented?
                case 0x98: return NULL; 						// All-names “3D” (3d clients only packet, receive version 7 bytes long)

                // in old nox is not implemented. Do we need it?
                case 0x9a: return NULL; 						// Console Entry Prompt

                case 0x9b: return new cPacketRequestHelp(buffer, length); 	      	// Request Help
                case 0x9f: length = ???; break; // Sell Reply
                case 0xa0: length =   3; break; // Select Server
                case 0xa4: length =   5; break; // Client Machine info (Apparently was a sort of lame spyware command .. unknown if it still works)
                case 0xa7: length =   4; break; // Request Tips/Notice
                case 0xaa: length =   5; break; // Attack Request Reply
                case 0xac: length = ???; break; // Gump Text Entry Dialog Reply
                case 0xad: length = ???; break; // Unicode speech request
                case 0xb1: length = ???; break; // Gump Menu Selection

                //unsure about this message if received, sent or both
                case 0xb2: length = ???; break; // Chat Message

                case 0xb5: length =  64; break; // Open Chat window
                case 0xb6: length =   9; break; // Send Help/Tip Request
                case 0xb8: length = ???; break; // Request Char Profile
                case 0xbb: length =   9; break; // Ultima Messenger (do we need this?)
                case 0xbd: length = ???; break; // Client Version Message
                case 0xbe: length = ???; break; // Assist Version
                case 0xbf: length = ???; break; // Misc. Commands Packet
                case 0xc2: length = ???; break; // Textentry Unicode
                case 0xc8: length =   2; break; // Client view range
//                case 0xc9: length =   6; break; // Get area server ping (OSI GM clients only)
//                case 0xca: length =   6; break; // Get user server ping (OSI GM clients only)
                case 0xcc: length = ???; break; // Localized Message Affix
                case 0xd1: length =   2; break; // Logout Status
                case 0xd4: length = ???; break; // new Book Header
                default: return NULL;	// Discard received packet
       }
}

/*!
\brief Character creation packet
\author Chronodt
\param client client who sent the packet

Mostly taken from old noxwizard.cpp and (vastly :) ) modified to pyuo object system
*/


bool cPacketReceiveCreateChar::execute(pClient client)
{
        // Disconnect-level encryption or transfer error check
        if ((length !=104) ||                                           // packet length check
           (LongFromCharPtr(buffer+1) != 0xedededed) ||                 // pattern checking
           (LongFromCharPtr(buffer+5) != 0xffffffff) ||
           //(buffer[9] != 0x00) ||                                     // this pattern is different for some clients, so commented out (for now)
           (buffer[10] != 0x00) ||                                      // at least one "letter" in character name
           (buffer[40] != 0x00))                                        // at least one "letter" in character password
        {
                client->disconnect();
	        return false;
	}

        uint8_t sex                = buffer[70];
        uint16_t strength           = buffer[71];
        uint16_t dexterity          = buffer[72];
        uint16_t intelligence       = buffer[73];
        uint16_t skill1             = buffer[74];
        uint16_t skill1value1       = buffer[75];
        uint16_t skill12            = buffer[76];
        uint16_t skill1value2       = buffer[77];
        uint16_t skill3             = buffer[78];
        uint16_t skill1value3       = buffer[79];
        uint16_t SkinColor          = ShortFromCharPtr(buffer + 80) | 0x8000;
        uint16_t HairStyle          = ShortFromCharPtr(buffer + 82);
        uint16_t HairColor          = ShortFromCharPtr(buffer + 84);
        uint16_t FacialHair         = ShortFromCharPtr(buffer + 86);
        uint16_t FacialHairColor    = ShortFromCharPtr(buffer + 88);
        uint16_t StartingLocation   = ShortFromCharPtr(buffer + 90);        // from starting list
        // uint16_t unknown         = ShortFromCharPtr(buffer + 92);
        uint16_t slot               = ShortFromCharPtr(buffer + 94);
        uint32_t clientIP           = LongFromCharPtr (buffer + 96);
        uint16_t shirt_color        = ShortFromCharPtr(buffer + 100);
        uint16_t pants_color        = ShortFromCharPtr(buffer + 102);


        // Disconnect-level protocol error check (possible client hack or too many chars already present in account)
        if (
                !(client->currAccount()->comparePassword(buffer+40)) ||                  //!< Password check
                (client->currAccount()->getCharsNumber() >= ServerScp::g_nLimitRoleNumbers) ||  //!< Max PCs per account check
                ((sex !=1) && (sex != 0)) ||                                                    //!< Sex validity check
                (strength + dexterity + intelligence > 80) ||                                   //!< Stat check: stat sum must be <=80
                (strength < 10)     || (strength > 60)     ||                                   //!< each stat must be >= 10 and <= 60
                (dexterity < 10)    || (dexterity > 60)    ||
                (intelligence < 10) || (intelligence > 60) ||
                (skillvalue1 + skillvalue2 + skillvalue3 != 100) ||                             //!< Skill check : sum of skills selected must be 100
                (skillvalue1 > 50)  || (skillvalue2 > 50)  || (skillvalue3 > 50) ||             //!< each skill must be >= 0 and <= 50. Since the 3 variables are uint8_t, if they are negative they will be seen as a number surely bigger than 127 :)
                (skill1 == skill2)  || (skill2 == skill3)  || (skill3 == skill1)                //!< 3 different skills must be selected
           )
        {
                client->disconnect();
	        return false;
	}


        // Correctable-level protocol error check (mainly out of bonds color or hair/beard style)
        //! Color & hairstyle boundary check
        if ((SkinColor <= 0x83EA) || (SkinColor >= 0x8422)) SkinColor = 0x83EA;
        if ((FacialHairColor <= 0x44E) || (FacialHairColor >= 0x4AD)) FacialHairColor = 0x044E;
        if ((HairColor <= 0x44E) || (HairColor >= 0x4AD)) HairColor = 0x044E;

        if (!((HairStyle >= 0x203b && HairStyle <= 0x203d ) || ( HairStyle >= 0x2044 && HairStyle <= 0x204a ))) HairStyle = 0;
        if (!(((FacialHair >= 0x203e && FacialHair <= 0x2041) || ( FacialHair >= 0x204b && FacialHair <= 0x204d )) || sex )) FacialHair = 0; //if female the beard check is always valid :P So FacialHair is put to 0

        // From here building a cBody
        pBody charbody = new cBody();
        charbody->setSerial(charbody->getNextSerial());

        if (sex) charbody->setId(bodyFemale) else charbody->setId(bodyMale);     // 0 = male 1 = female

        charbody->setStrength(strength);
        charbody->setHitPoints(strength);
        charbody->setMaxHitPoints(strength);

        charbody->setDexterity(dexterity);
        charbody->setStamina(dexterity);
        charbody->setMaxStamina(dexterity);

        charbody->setIntelligence(intelligence);
        charbody->setMana(intelligence);
        charbody->setMaxMana(intelligence);

        // all skills are set to 0 in the constructor, so the only skills needing to be set are the 3 selected
        charbody->SetSkill(skill1, skillvalue1 * 10);
        charbody->SetSkill(skill2, skillvalue2 * 10);
        charbody->SetSkill(skill3, skillvalue3 * 10);

        charbody->setSkinColor(SkinColor);
      	charbody->setName((const char*)&buffer[10]);
	charbody->setTitle("");                         //TODO check if this is enough to have a clean title

        //Now with the body completed and data verified, building cPC
        pPC pc = new cPC( cPC::nextSerial() );

        pc->setBody(charbody);
        pc->npc=false;
      	pc->SetPriv(defaultpriv1);
	pc->SetPriv2(defaultpriv2);

	Location charpos;
	charpos.x= str2num(start[StartingLocation][2]);
	charpos.y= str2num(start[StartingLocation][3]);
	charpos.dispz= charpos.z= str2num(start[StartingLocation][4]);
	pc->MoveTo( charpos );

	pc->dir=4;
	pc->namedeedserial=INVALID;
        for (int ii = 0; ii < TRUESKILLS; i++) Skills::updateSkillLevel(pc, ii);  //updating skill levels for pc

	pItem pi;

      	if (HairStyle)  // If HairStyle was invalid, is now 0, so baldy pg :D
	{
		pi = item::CreateFromScript( "$item_short_hair" );
		VALIDATEPIR(pi, false);
		pi->setId(HairStyle);
		pi->setColor(HairColor);
		pi->setContainer(pc);
		pi->layer=LAYER_HAIR;
                charbody->setLayerItem(layHair, pi);
	}

	if (FacialHair) // if FacialHair was invalid (or unselected) or pg is female, no beard is added
	{
		pi = item::CreateFromScript( "$item_short_beard" );
		VALIDATEPIR(pi, false);
		pi->setId(FacialHair);
		pi->setColor(FacialHairColor);
		pi->setContainer(pc);
		pi->layer=LAYER_BEARD;
                charbody->setLayerItem(layBeard, pi);
	}

        // - create the backpack
	pi= item::CreateFromScript( "$item_backpack");
	VALIDATEPIR(pi, false);
	pc->packitemserial= pi->getSerial();
        pi->setContainer(pc);

        // has to be set to its layer???


        // - create pants
	if( !RandomNum(0, 1) )
	{
		if(!sex)
			pi= item::CreateFromScript( "$item_long_pants");
		else
			pi= item::CreateFromScript( "$item_a_skirt");
	}
	else
	{
		if(!sex)
			pi= item::CreateFromScript( "$item_short_pants");
		else
			pi= item::CreateFromScript( "$item_a_kilt");
	}

	VALIDATEPIR(pi, false);

	// pant/skirt color -> old client code, random color
	pi->setColor(pants_color);
	pi->setContainer(pc);

	if( !(rand()%2) )
		pi= item::CreateFromScript( "$item_fancy_shirt");
	else
		pi= item::CreateFromScript( "$item_shirt");

	VALIDATEPIR(pi, false);
	pi->setColor(shirt_color);
	pi->setContainer(pc);

	// Give the character some gold
	if ( goldamount > 0 )
	{
		pi = item::CreateFromScript( "$item_gold_coin", pc->getBackpack(), goldamount );
	}


        // Assignation of new Char to user account
        client->currAccount()->addChartoAccount( pc );


        newbieitems(pc);
        client->startchar(); //TODO: move startchar from network to cClient
      	//clientInfo[s]->ingame=true;
        return true;
}

/*!
\brief Disconnection Request packet
\param client client who sent the packet
\author Chronodt
*/

bool cPacketReceiveDisconnectNotify::execute(pClient client)
{
        if ((length != 5) || (LongFromCharPtr(buffer+1) != 0xffffffff)) return false;
        client->disconnect();
        return true;
}

/*!
\brief Move Request packet: client trying to move PC
\author Chronodt
\param client client who sent the packet

Mostly taken from old network.cpp and modified to pyuo object system
*/

bool cPacketReceiveMoveRequest::execute (pClient client)
{
        if( (length == 7) && (client->currChar()!= NULL ))
        {
	        walking(client->currChar(), buffer[1], buffer[2]); // buffer[1] = direction, buffer[2] = sequence number
	        client->currChar()->disturbMed();
                return true;
        } else return false;
}

/*!
\brief Talk Request packet (NOT unicode!)
\author Chronodt
\param client client who sent the packet

Mostly taken from old network.cpp and modified to pyuo object system
*/

bool cPacketReceiveTalkRequest::execute (pClient client)
{
	if( (client->currChar()!=NULL) && (length != ShortFromCharPtr(buffer + 1)))
        {
        // the buffer should be invalid only on the next packet reception by the same client....
        // ... but is a copy really needed?
        	unsigned char nonuni[512];
		client->currChar()->unicode = false;
	        strcpy((char*)nonuni, (char*)&buffer[8]);
		talking(client, (char*)nonuni);
                return true;
	} else return false;
}

/*!
\brief Attack Request Packet
\author Chronodt
\param client client who sent the packet

Mostly taken from old network.cpp and rcvpkg.cpp and modified to pyuo object system
*/
bool cPacketReceiveAttackRequest::execute (pClient client)
{
        if (length != 5) return false;
	pPC pc = client->currChar();
	VALIDATEPCR( pc, false );
	pChar victim = pointers::findCharBySerPtr(buffer + 1);  //victim may be an npc too, so it is a cChar
	VALIDATEPCR( victim, false );

	if( pc->dead ) pc->deadAttack(victim);
	else    if( pc->jailed ) sysmessage(s,TRANSLATE("There is no fighting in the jail cells!"));
	        else pc->attackStuff(victim);
        return true;
}

/*!
\brief Doubleclick Packet
\author Chronodt
\param client client who sent the packet
*/

bool cPacketReceiveDoubleclick::execute(pClient client)
{
        if (length != 5) return false;
	pPC pc = client->currChar();
	VALIDATEPCR( pc, false );

	// the 0x80 bit in the first byte is used later for "keyboard" and should be ignored
	uint32_t serial = LongFromCharPtr(buffer +1) & 0x7FFFFFFF;

	if (isCharSerial(serial))
	{
		pChar pd = pointers::findCharBySerial(serial);
		if (pd)
                {
                        pd->doubleClick(client, buffer[1] & 0x80);
		        return true;
                }
	}
	pItem pi = pointers::findItemBySerial(serial);
	VALIDATEPIR(pi, false);  //If it's neither a char nor an item, then it's invalid
        pi->doubleClick(client);
        return true;
}

/*!
\brief Pickup Item Packet
\author Chronodt
\param client client who sent the packet
*/

bool cPacketReceivePickUp::execute(pClient client)
{
        if (length != 7) return false;
	pItem pi = pointers::findItemBySerPtr(LongFromCharPtr(buffer+1));
        uint16_t amount = ShortFromCharPtr(buffer+5);
      	VALIDATEPIR(pi, false);
        client->get_item(pi, amount);  //!< if refused, the get_item automatically bounces the item back
        return true;

}

/*!
\brief Drop Item Packet
\author Chronodt
\param client client who sent the packet
*/

bool cPacketReceiveDropItem::execute(pClient client)
{
        if (length != 14) return false;
	pItem pi = pointers::findItemBySerPtr(LongFromCharPtr(buffer+1));
        Location drop_at = Location(ShortFromCharPtr(buffer+5), ShortFromCharPtr(buffer+7), buffer[9]);
        pItem container = pointers::findItemBySerial(LongFromCharPtr(buffer+10));
        client->drop_item(pi, drop_at, container);  //!< if refused, the drop_item automatically bounces the item back
        return true;
}


/*!
\brief Singleclick Packet
\author Chronodt
\param client client who sent the packet
*/
bool cPacketReceiveSingleclick::execute(pClient client)
{
        if (length != 5) return false;
        uint32_t serial = LongFromCharPtr(buffer + 1);
	if ( isCharSerial( serial ) )
        {
                pChar pc = pointers::findCharBySerial(serial);
                VALIDATEPCR(pc, false);
		pc->SingleClick( client);
        }
	else
        {
        	pItem pi = pointers::findItemBySerial(serial);
                VALIDATEPIR(pi, false);
		pi->SingleClick( client );
        }
        return true;
}

/*!
\brief Action Request Packet
\author Chronodt
\param client client who sent the packet
\todo check the last if.... why does it do that check?????? it has no sense...
*/

bool cPacketReceiveActionRequest::execute(pClient client)
{
        uint16_t size = ShortFromCharPtr(buffer + 1);
        if (length != size) return false;

        uint8_t type = buffer[3];
        pChar pc = client->currChar();
        VALIDATEPCR(pc, false);
	if (type==0xC7) // Action
	{
		if (pc->isMounting()) return true;
		if (!(strcmp((char*)&buffer[4],"bow"))) pc->playAction(0x20);
		if (!(strcmp((char*)&buffer[4],"salute"))) pc->playAction(0x21);
	        return true; // Morrolan
	}
	else if (type) // Skill
	{
		int i=4;
		while ( (buffer[i]!=' ') && (i < size) ) i++;
		buffer[i]=0;
		Skills::SkillUse(client, str2num((char*)&buffer[4]));
		return true;
	}
	else if ((type==0x27)||(type==0x56))  // Spell
	{
		pItem p_j = NULL;
		pItem pack= pc->getBackpack();
                int book = 0;
		if(pack) //lb
		{
			NxwItemWrapper gri;
			gri.fillItemsInContainer( pack, false );
			gri.fillItemWeared( pc_currchar, true, true, false );
			for( gri.rewind(); !gri.isEmpty(); gri++ )
			{
				pItem pj=gri.getItem();
				if (pj)
					if (pj->type==ITYPE_SPELLBOOK)
					{
						p_j=pj;
						break;
					}
			}
		}
		if (p_j!=NULL)
		{
			book=buffer[4]-0x30;
			if (buffer[5]>0x20)
			{
				book=(book*10)+(buffer[5]-0x30);
			}
			if (pc->dead)
                        {
			        pc->sysmsg(TRANSLATE("Ethereal souls really can't cast spells"));
       			}
                        else
                        {
			        if (pc->isFrozen())
                                {
				        if (pc->casting) client->sysmsg(TRANSLATE("You are already casting a spell."));
				        else client->sysmsg(TRANSLATE("You cannot cast spells while frozen."));
        		        }
                                else
                                {
	        		        if (!pc->knowsSpell(static_cast<magic::SpellId>(book-1)))
                                        {
					        client->sysmsg("You don't know that spell yet.");
				        }
                                        else
                                        {
					        magic::beginCasting(static_cast<magic::SpellId>(book-1),client,magic::CASTINGTYPE_SPELL);
				        } // if don't knows spell
			        } // if frozen
        	       	} // if alive
		} // if has spellbook
	}  // if spell
	else
	{
		if (ShortFromCharPtr(buffer + 2) == 0x0543)  // Open spell book  //Chronodt: ?????? what is this?
		{
			client->sendSpellBook(NULL);
		}
		return true;
	}
        return false; //If flow arrives here, maybe invalid data in packet
}


/*!

\brief Wear Item Packet
\author Chronodt
\param client client who sent the packet
*/


bool cPacketReceiveWearItem::execute(pClient client)

{

        if (length != 10) return false;

	pChar pc = pointers::findCharBySerPtr(buffer+6);

	VALIDATEPCR(pck, false);
	pItem pi = pointers::findItemBySerPtr(buffer+1);
	VALIDATEPIR(pi, false);

        client->wear_item(pc, pi);

        return true;

}


/*!

\brief Resync Request Packet
\author Chronodt
\param client client who sent the packet
*/



bool cPacketReceiveResyncRequest::execute(pClient client)

{
        if (length != 3) return false;
	if( client->currChar()!=NULL ) {
        	client->currChar()->teleport();
	}
        return true;
}

/*!
\brief (probably very much obsolete) Resurrection Choice Menu Packet
\author Chronodt
\param client client who sent the packet
*/

bool cPacketReceiveRessChoice::execute(pClient client)
{
        if (length != 2) return false;
        if(buffer[1]==0x02)
	{
		pChar murderer=pointers::findCharBySerial(client->currChar()->murdererSer);
		if( murderer && SrvParms->bountysactive )
		{
			client->sysmessage(TRANSLATE("To place a bounty on %s, use the command BOUNTY <Amount>."), murderer->getCurrentNameC() );
		}
		client->sysmessage(TRANSLATE("You are now a ghost."));
	}
	if(buffer[1]==0x01)
	client->sysmessage(TRANSLATE("The connection between your spirit and the world is too weak."));
        return true;
}

/*!
\brief Receive Status Request Packet: client asks for status or skilllist
\author Chronodt
\param client client who sent the packet
*/

bool cPacketReceiveStatusRequest::execute(pClient client)
{
        if (length != 10) return false;
        if ( client->currChar() != NULL ) {
	        if (buffer[5]==4) client->statusWindow(pointers::findCharBySerPtr(buffer + 6), false); //!< NOTE: packet description states sending basic stats, so second argument is false. Correct if necessary 
                if (buffer[5]==5) client->skillWindow();
	}
        return true
}

/*!
\brief Set Skill Lock Packet
\author Chronodt
\param client client who sent the packet
*/

bool cPacketReceiveSetSkillLock::execute(pClient client)
{
        uint16_t size = ShortFromCharPtr(buffer + 1);
        if (length != size) return false;
      	// client 1.26.2b+ skill managment packet
        // -> 0,1,2,3 -> ignore them
	// -> 4 = skill number
	// -> 5 = 0 raising (up), 1 falling=candidate for atrophy, 2 = locked
        if ( client->currChar() ) client->currChar()->lockSkill[buffer[4]] = buffer[5]; // save skill managment changes
        return true;
}

/*!
\brief Buy Items Packet
\author Chronodt
\param client client who sent the packet
\note this packet contains all items purchased by player in buy gump
\note with current buffer implementation in csocket.cpp (1024 bytes) we can handle up to 135 different stacks of purchased items in a single go. Should be enough :D
*/

bool cPacketReceiveBuyItems::execute(pClient client)
{
        uint16_t size = ShortFromCharPtr(buffer + 1);
        if (length != size) return false;

        std::vector< boughtitem > allitemsbought;

	pNpc npc = (pNpc)pointers::findCharBySerPtr(buffer + 3);
	VALIDATEPCR(npc, false);

	int itemtotal=(size - 8)/7;
	if (itemtotal>256) return false;

        for(i=0;i<itemtotal;i++)
	{
		int pos=8+(7*i);

		boughtitem b;

		b.layer=buffer[pos];
		b.item=pointers::findItemBySerPtr(buffer + pos + 1);
		if(!b.item)
			continue;
		b.amount=ShortFromCharPtr(buffer + pos + 5);
		allitemsbought.push_back( b );
	}
        client->buyaction(npc, allitemsbought);
        return true;
}

/*!
\brief Receive map pins commands (add, remove, etc.)
\author Chronodt
\param client client who sent the packet
\todo to be completed when treasure maps done
*/

bool cPacketReceiveMapPlotCourse::execute(pClient client)
{
        if (length != 10) return false;

        pMap map = (pMap)pointers::findItemBySerPtr(LongFromCharPtr(buffer + 1));
        VALIDATEPIR( map, false);

        PlotCourseCommands command     	= buffer[5];
        int pin 	 		= buffer[6];
        uint16_t x 				= ShortFromCharPtr(buffer + 7);
        uint16_t y 				= ShortFromCharPtr(buffer + 9);

        switch(command)
        {
        	case AddPin:
                        return map->addPin(x, y);
                case InsertPin:
                        return map->insertPin(x,y,pin);
                case ChangePin:
                	return map->changePin(x,y,pin);
                case RemovePin:
                	return map->removePin(pin);
                case ClearAllPins:
                	return map->clearAllPins();
                case ToggleWritable:
                	return map->toggleWritable();
                case WriteableStatus:  //this message is only sent, so if it is received, continues out of the switch and returns false :D
                default:
        }
        return false;
}


bool cPacketReceiveLoginChar::execute(pClient client)
{
        if (length != 73) return false;
        if (LongFromCharPtr(buffer+1) != 0xedededed) return false;	//pattern check


        //TODO revise this


	loginchars[s] = NULL;

	pChar pc_k=NULL;

	if (acctno[s]>INVALID)
	{
		int j=0;
		Accounts->SetOffline(acctno[s]);
		NxwCharWrapper sc;
		Accounts->GetAllChars( acctno[s], sc );
		for( sc.rewind(); !sc.isEmpty(); sc++ ) {
			pChar pc_i=sc.getChar();
			if(!pc_i)
				continue;
			if (j==buffer[s][0x44]) {
				pc_k=pc_i;
				break;
			}
			j++;
		}

		if (pc_k)
		{
			pc_k->setClient(NULL);
			int32_t nSer = pc_k->getSerial();
			for ( int32_t idx = 0; idx < now; idx++ ) {
				if ( pc_k == loginchars[idx] ) {
					// TODO We need to fix this!!!
					uint8_t msg2[2]={ 0x53, 0x05 };
					Xsend(s, msg2, 2);
//AoS/					Network->FlushBuffer(s);
					Disconnect(s);
					Disconnect(idx);
					return;
				}
			}

			Accounts->SetOnline(acctno[s], pc_k);
			pc_k->logout=INVALID;

			loginchars[s] = pc_k;

			pc_k->setClient(new cNxwClientObj(s));
			startchar(s);
		}
		else
		{
			uint8_t msg[2]={ 0x53, 0x05 };
			Xsend(s, msg, 2);
//AoS/			Network->FlushBuffer(s);
			Disconnect(s);
		}
	}
}

/*!
\brief Receive changed book page
\author Akron & Chronodt
\param client client who sent the packet
*/


bool cPacketReceiveBookPage::execute(pClient client)
{
        uint16_t size = ShortFromCharPtr(buffer + 1);
        if (length != size) return false;
	pItem book=pointers::findItemBySerPtr(buffer+3);
	if(book)
	{
		if (book->morez == 0)	Books::addNewBook(book);
		if (book->morex!=666 && book->morex!=999)
			book->morex = 666;
		if (book->morex==666) // writeable book -> copy page data send by client to the class-page buffer
			Books::books[book->morez].ChangePages(buffer + 13, ShortFromCharPtr(buffer + 9), ShortFromCharPtr(buffer + 11), size - 13 );
		else if (pBook->morex==999)
			Books::books[book->morez].SendPageReadOnly(client, book, ShortFromCharPtr(buffer + 9));
	}
        return true;
}



bool cPacketReceiveTargetSelected::execute(pClient client)
{
        if (length != 19) return false;
	pTarget target = client->getTarget();
        if( target==NULL ) return true;


        //! \todo finish to update this when targets redone
        
	target->receive( ps );

					if( !target->isValid() )
						target->error( ps );
					else
						target->code_callback( ps, target );
}

/*!
\brief Receive secure trading message
\param client client who sent the packet
*/

bool cPacketReceiveSecureTrade::execute(pClient client)
{
        uint16_t size = ShortFromCharPtr(buffer + 1);
        if (length != size) return false;

	pContainer cont1, cont2;

	switch(buffer+3) //Buffer + 3 = Action byte
	{
	case 0://Start trade - Never happens, sent out by the server only.
		break;
	case 2://Change check marks. Possibly conclude trade
		cont1 = pointers::findItemBySerPtr(buffer + 4);
                //cont1 is now this client's secure trading container (a temporary container wich holds the trade items)

		if (cont1) cont2 = pointers::findItemBySerial(calcserial(cont1->moreb1, cont1->moreb2, cont1->moreb3, cont1->moreb4));
		else cont2=NULL;

		if (cont2)
		{
			cont1->morez=buffer[11];
			client->sendtradestatus(cont1, cont2);
			if (cont1->morez && cont2->morez)
			{
				client->dotrade(cont1, cont2);
				client->endtrade( LongFromCharPtr(buffer + 4) );
			}
		}
		break;
	case 1://Cancel trade. Send each person cancel messages, move items.
		client->endtrade( LongFromCharPtr(buffer +4) );
		break;
	default:
		ErrOut("Switch fallout. trade.cpp, trademsg()\n"); //Morrolan
                return false;
	}
        return true;
}


/*!
\brief Receive bullettin board message
\author Chronodt
\param client client who sent the packet
\note Packets flow is:
1) client doubleclicks the board (received 0x06 packets and doubleclick routine works with that)
2) server sends 0x71 command 0 to tell client to open the bulletin board gump and await message data
3) server sends 0x25 (add items to container) to tell to the client the serial numbers of all messages in the board
4) the client replies to 0x25 sending a 0x71 message 4 for each post in the board to get the details of each
   (poster, date and topic)
5) server replies to each 0x71 message 4 sent with a 0x71 message 1 with the required details

if the clients doubleclicks on a message, it sends 0x71 command 3 and the server replies with 0x71 command 2 with
the body of the message
*/


bool cPacketReceiveBBoardMessage::execute(pClient client)
{
        uint16_t size = ShortFromCharPtr(buffer + 1);
        if (length != size) return false;

	// Message \x71 has numerous uses for the Bulletin Board
	// so we need to get the type of message from the client first.

	int msgType = buffer[3];
        pMsgBoard msgboard = (pMsgBoard)pointers::findItemBySerial(LongFromCharPtr(buffer + 4));
        pMsgBoardMessage message = (pMsgBoardMessage)pointers::findItemBySerial(LongFromCharPtr(buffer + 8));
        //for msgtypes 3, 4, 6 message is the message on which operate, on message 5 it is parent message (message reply)
        VALIDATEPIR( msgboard, false);
	// message is validated inside the switch because subcommand 5 does not need it and may me invalid without consequences
        // in addition an unvalid message means that it has just been deleted by someone else

	switch (msgType)
	{
		case 3:  // Client->Server: Client has dbl-clicked on subject, requesting body of message
		{
                        if (message == NULL)
                        {
	                        // Possible situation: two users are browsing a msg board with the same message
                                // and the first user deletes that message. The second still sees it in the summary
                                // and can stil TRY to open it. We must tell the second player that the message is
                                // no longer available and remove it from his/her list.
                                cPacketSendDeleteObj pk(LongFromCharPtr(buffer + 8));
                                client->sendPacket(&pk);
                                client->sysmsg(tr("This message has just been deleted by someone else")); //TODO add this to translation files
                        }
                        else
                        {
	                        cPacketSendBBoardCommand pk(msgboard, SendMessageBody, message);
        	                client->sendPacket(&pk);
                        }
			break;
		}

		case 4:  // Client->Server: Client has ACK'ed servers download of posting serial numbers
               		 // and requires the summary of (buffer + 8) message (topic, poster id and date)
		{
		        VALIDATEPIR( message, false);
			// Server needs to handle ACK from client that contains the posting serial numbers
			msgboard->sendMessageSummary( client, message );
			break;
		}

		case 5:  // Client->Server: Client clicked on Post button (either from the main board or after pressing the Reply)
		{        //                 Reply just switches to the Post item.

			pChar pc = client->currChar();
			VALIDATEPCR(pc, false);

			// Check privledge level against server.cfg msgpostaccess
	                if ( !(pc->IsGM()) && !(SrvParms->msgpostaccess) )
                        {
				client->sysmsg(TRANSLATE("Thou art not allowed to post messages."));
                                return false;
                        }

                        uint32_t msgSN = LongFromCharPtr(buffer + 8);

                	// If this is a reply to anything other than a LOCAL post, abort
			if ( msgSN>0))
			{
				VALIDATEPIR( message, false);
                                if ( (message->availability != LOCALPOST) && !global::canReplytoGlobalMsgBoardPosts() )
                                {
					#ifdef DEBUG
					ErrOut("MsgBoard: Attempted reply to a global or regional post\n");
					#endif
					client->sysmessage( TRANSLATE("You can not reply to global or regional posts") );
					return false;
                                }
			}


       	                int subjectlen = buffer[9];
               	        pMsgBoardMessage newmessage = new cMsgBoardMessage(); 	//creating new message

                        //filling new message with data

                        newmessage->poster = pc->getSerial();
                        newmessage->availability = pc->postType;
                        if (pc->postType == REGIONALPOST) newmessage->region = msgboard->getRegion();
                       	newmessage->subject = string(buffer + 10);
                        newmessage->body = string(buffer + 10 + subjectlen, length - 11 - subjectlen);
                        // We need to use this string constructor because there may be many null-terminated strings, and since no
               	        // process is needed by the server, it is much easier to copy the bulk of the body in a string and just
                       	// give it back to a client that asks for it :D
                        newmessage->setContainer(msgboard);
			newmessage->replyof = msgSN;

			if (!msgboard->addMessage( newmessage ))
                        {
                                if (pc->postType == LOCALPOST) client->sysmessage( tr("This Message Board has too many messages!");
	                   	newmessage->Delete(); //if could not link, message should be deleted
                        }
                        else
                        {
				// Send "Add Item to Container" message to client
		                cPacketSendAddItemtoContainer pk(newmessage);
		                client->sendPacket(&pk);
                	}

			break;
		}

	case 6:  // Remove post from Bulletin board
		{
                        if (message == NULL)
                        {
	                        // Possible situation: two users are browsing a msg board with the same message
                                // and the first user deletes that message. The second still sees it in the summary.
                                // Since both are trying to delete it, the result is the same, only we do not have
                                // to delete the same item again :D
                                cPacketSendDeleteObj pk(LongFromCharPtr(buffer + 8));
                                client->sendPacket(&pk);
                        }
                        else
                        {
	                        VALIDATEPIR( message, false);
				pChar pc= client->currChar();
				VALIDATEPCR(pc, false);
				//             |p#|s1|s2|mt|b1|b2|b3|b4|m1|m2|m3|m4|
				// Client sends 71  0  c  6 40  0  0 18  1  0  0  4
				if ( (pc->IsGM()) || (SrvParms->msgpostremove) )
	                        {
                                	if ( global::onlyPosterCanDeleteMsgBoardMessage() && (pc->getSerial() != message->poster && !pc->IsGM() && (pc->getSerial32() != msgboard->getOwner() || message->availability != LOCALPOST )))
                                        	client->sysmessage( tr("You are not allowed to delete this message") );
                                        else
                                        {
                                        	// if onlyPosterCanDeleteMsgBoardMessage() is true, the only ones
                                                // who can delete a post are: a gm, the poster and the owner of the
                                                // msgboard, but the latter only if the message is local
	                                	cPacketSendDeleteObj pk(LongFromCharPtr(buffer + 8));
        	                        	client->sendPacket(&pk);
                                        	message->Delete();
                                        }
	                        }

                        }
			break;
		}


	default:
		return false;
	}
        return true;
}


bool cPacketReceiveWarModeChange::execute(pClient client)
{
        if (length != 5) return false;
        pPC pc = client->currChar();
        if( pc )
        {
        	if( (pc->inWarMode() && !buffer[1]) || (!pc->inWarMode() && buffer[1])  //Translation: if warmode has to change
                {
			pc->toggleWarMode();
			pc->targserial=INVALID;
                }
		// Now we send the packet back :D
		cPacketSendWarModeStatus pk(buffer);
        	client->sendPacket(&pk);

		if (pc->dead && pc->war) // Invisible ghost, resend.
			pc->teleport( TELEFLAG_NONE );
		client->playMidi();
		pc->disturbMed();
                return true;
	}
        return false;
}


bool cPacketReceivePing::execute(pClient client)
{
	if (length != 2) return false;
	//All this function has to do is send back the packet to the client :D (well if it isn't a ping! :D)
        cPacketSendPingReply pk(buffer);
       	client->sendPacket(&pk);
        return true;
}


bool cPacketReceiveRenameCharacter::execute(pClient client)
{
	if (length != 35) return false;
	pChar pc = pointers::findCharBySerPtr(buffer + 1);
        if(pc && ( client->currChar()->IsGMorCounselor() || client->currChar()->isOwnerOf( pc ) ) )
        {
        	pc->setCurrentName( buffer + 5 );
                return true;
        }
        return false;
}


bool cPacketReceiveDialogResponse::execute(pClient client)
{
	if (length != 13) return false;

        //TODO check menus

	Menus.handleMenu( client );

        return true;
}

bool cPacketReceiveLoginRequest::execute(pClient client)
{
	//!This is the first packet a client sends to the server in the login sequence
	if (length != 62) return false;

        //! \todo crypting code handling here
/*

	if ( clientCrypter[s] != NULL )
	clientCrypter[s]->setCryptMode(CRYPT_LOGIN) ;
	clientInfo[s]->firstpacket=false;
	LoginMain(s);
*/
	//TODO: Add code from loginmain in network.cpp and update it
        return true;
}

bool cPacketReceiveDeleteCharacter::execute(pClient client)
{
	if (length != 39) return false;
        uint32_t index    = LongFromCharPtr (buffer + 31); // index of character to delete
	uint32_t clientip = LongFromCharPtr (buffer + 35);

/* PkG 0x85,
 *      0x00 => That character password is invalid.
 *      0x01 => That character doesn't exist.
 *      0x02 => That character is being played right now.
 *      0x03 => That charater is not old enough to delete.
                The character must be 7days old before it can be deleted.
 *      0x04 => That character is currently queued for backup and cannot be
 *              deleted.
 *      0x05 => Couldn't carry out your request.
 */

        pAccount account = client->currAccount();
        if(!account->verifyPassword(buffer+1))
       	{
             	// Password invalid
                cPacketSendCharDeleteError pk(0x0);
	       	client->sendPacket(&pk);
        	return true;
        }

	pPC TrashMeUp = account->getChar(index);	//PC to delete. if index is too large or invalid, it returns NULL

	if (ServerScp::g_nPlayersCanDeleteRoles)
        {
		if(!TrashMeUp)
                {
			// Character does not exist
                	cPacketSendCharDeleteError pk(0x1);
	       		client->sendPacket(&pk);
                        return true;
		}

		if (TrashMeUp)
		{
			if( SrvParms->checkcharage && (getclockday() < TrashMeUp->getCreationDay() + 7) )
                        {
                        	// Character is too young to die :D
                                cPacketSendCharDeleteError pk(0x3);
	       			client->sendPacket(&pk);
				return true;
			}

			if(TrashMeUp->isOnline())
                        {
                        	// Character is being played right now!
                                cPacketSendCharDeleteError pk(0x2);
	       			client->sendPacket(&pk);
				return true;
			}
          		// Character Deletion
			TrashMeUp->Delete();

			cPacketSendCharAfterDelete pk(account);
		       	client->sendPacket(&pk);

			return true; // All done ;]
		}
	}
        // sending message "0x05 => Couldn't carry out your request" ???????
 	cPacketSendCharDeleteError pk(0x5);
	client->sendPacket(&pk);
        return true;
}



bool cPacketReceiveGameServerLogin::execute(pClient client)
{
	if (length != 65) return false;

        //! \todo another login packet. Revise when encryption done
        /*
	clientInfo[s]->firstpacket=false;
	clientInfo[s]->compressOut=true;
	if ( clientCrypter[s] != NULL )	clientCrypter[s]->setEntering(false);
	CharList(s);  */
        return true;
}




bool cPacketReceiveBookUpdateTitle::execute(pClient client)
{
	if (length != 99) return false;
	int j= 9;
	char author[31],title[61];
        pItem pBook=pointers::findItemBySerPtr(buffer+1);
  	if(!ISVALIDPI(pBook)) return false;
        strncpy(title, buffer + 9, 60);
        strncpy(author, buffer + 69, 30);
        //so if clients somehow does not send a null terminated string, we zero the last character to be sure
        title[60] = 0;
        author[30] = 0;
	Books::books[pBook->morez].ChangeAuthor(author);
	Books::books[pBook->morez].ChangeTitle(title);
        return true;
}



bool cPacketReceiveDyeItem::execute(pClient client)
{
	if (length != 9) return false;
        //TODO: moving dyeitem from commands to cclient??
        Commands::DyeItem(client);
        return true;
}


bool cPacketRequestHelp::execute(pClient client)
{
	if (length != 258) return false;
        //! \todo whem menu's are revised, look at this...
	gmmenu(s, 1);
        return true;
}
