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
	static const UI08 templ[14] = {
		0x6E, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02,
		0x00, 0x05, 0x00, 0x01, 0x0, 0x00, 0x01
		};

	buffer = new UI08[14];
	length = 14;
	memcpy(buffer, templ, 14);
	LongToCharPtr(serial, buffer+1);
	ShortToCharPtr(action, buffer+5);
}

void cPacketDrawContainer::prepare()
{
	static const UI08 templ[7] = {
		0x24, 0x40, 0x0B, 0x00, 0x1A, 0x00, 0x3C
		};

	buffer = new UI08[7];
	length = 7;
	memcpy(buffer, templ, 7);

	LongToCharPtr(serial, buffer+1);
	ShortToCharPtr(gump, buffer+5);
}

void cPacketSendContainerItem::prepare()
{
	static const UI08 templ1[5] = {
		0x3C, 0x00, 0x05, 0x00, 0x00
		};

	static const UI08 templ2[19] = {
		0x40, 0x0D, 0x98, 0xF7, 0x0F, 0x4F, 0x00,
		0x00, 0x09, 0x00, 0x30, 0x00, 0x52, 0x40,
		0x0B, 0x00, 0x1A, 0x00, 0x00
		};

	UI16 count = items.count();
	UI16 length = 5 + count*19;

	buffer = new UI08[length];
	memcpy(buffer, templ1, 5);

	ShortToCharPtr(length, buffer+1);
	ShortToCharPtr(count, buffer+3);

	UI08 *ptrItem = buffer+5;

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

void cPacketSendAddContainerItem::prepare()
{
	length = 20;
	buffer = new UI08[20];

	buffer[0] = 0x25;
	buffer[7] = 0x00;

	//AntiChrist - world light sources stuff
	//if player is a gm, this item
	//is shown like a candle (so that he can move it),
	//....if not, the item is a normal
	//invisible light source!
	if(pc->IsGM() && item.id == 0x1647)
	{
		item.id = 0x0A0F;
		item.color = 0x00C6;
	}

	LongToCharPtr(item->getSerial(), buffer+1);
	ShortToCharPtr(item->getAnimId(), ptrItem+5);
	ShortToCharPtr(item->getAmount(), ptrItem+8);
	ShortToCharPtr(item->getLocation().x, ptrItem+10);
	ShortToCharPtr(item->getLocation().y, ptrItem+12);
	LongToCharPtr(item->getContainer()->getSerial(), ptrItem+14);
	ShortToCharPtr(item->getColor(), ptrItem+18);
}

void cPacketSendWornItem::prepare()
{
	length = 15;
	buffer = new UI08[15];

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
	static const UI08 templ[12] = {
		0x54, 0x01, 0x12, 0x34, 0x00, 0x00,
		0x06, 0x40, 0x05, 0x9A, 0x00, 0x00
		};

	buffer = new UI08[12];
	length = 12;
	memcpy(buffer, templ, 12);

	SI16 z = loc.z;

	ShortToCharPtr(model, buffer +2);
	ShortToCharPtr(loc.x, buffer +6);
	ShortToCharPtr(loc.y, buffer +8);
	ShortToCharPtr(z, buffer +10);
}

void cPacketSendDeleteObj::prepare()
{
	length = 5;
	buffer = new UI08[5];

	buffer[0] = 0x1D;
	LongToCharPtr(serial, buffer+1);
}

void cPacketSendSkillState::prepare()
{
	length = 4 + TRUESKILLS*7 + 2;
	buffer = new UI08[length];

	buffer[0] = 0x3A;
	ShortToCharPtr(length, buffer+1);
	buffer[3] = 0x00;

	UI08 *skill = buffer+4;
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
	buffer = new UI08[11];

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
	buffer = new UI08[length];

	buffer[0] = 0xA5;
	ShortToCharPtr(length, buffer+1);

	memcpy(buffer+3, url.c_str(), length-3);
}

void cPacketSendPlayMidi::prepare()
{
	length = 3;
	buffer = new UI08[3];

	buffer[0] = 0x6D;
	ShortToCharPtr(id, buffer+1);
}

void cPacketSendOverallLight::prepare()
{
	length = 2;
	buffer = new UI08[2];

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

	buffer = new UI08[length];
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

	UI32 gold = pc->rtti() == rtti::rttiPC ? (reinterpret_cast<pPC*>pc)->getGold : 0;
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
//	length = 8;    //redundant. Constructor already sets length to 8
	buffer = new UI08[8];
	buffer[0] = 0x3B;
	ShortToCharPtr(0x08, buffer +1);			// Packet len
	LongToCharPtr( npc->getSerial32(), buffer + 3);	        // vendorID
	buffer[7]=0x00;						// Flag:  0 => no more items  0x02 items following ...
}


void cPacketSendPaperdollClothingUpdated::prepare()
{
//	length = 1;    //redundant. Constructor already sets length to 1
	buffer = new UI08[1];
	buffer[0] = 0x29;
}

void cPacketSendOpenMapGump::prepare()
{
//	length = 19;
	buffer = new UI08[19];
        buffer[0] = 0x90;
	LongToCharPtr(map->getSerial32(), buffer +1);
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
//	length = 11;
	buffer = new UI08[11];
        buffer[0] = 0x56;
	LongToCharPtr(map->getSerial32(), buffer +1);
	buffer[5]  = command;
	if (command == WriteableStatus) buffer[6] = (pin) ? 1 : 0;  // not sure if the values needed by client are exactly 1 and 0, but better safe than sorry 	
        else buffer[6] = pin;
        ShortToCharPtr(x, buffer + 7);
        ShortToCharPtr(y, buffer + 9);
}



static pPacketReceive cPacketReceive::fromBuffer(UI08 *buffer, UI16 length)
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
                case 0x6f: 								// Secure Trading
                case 0x71: return new cPacketReceiveBBoardMessage(buffer, length); 	// Bulletin Board Message
                case 0x72: length =   5; break; // Request War Mode Change/Send War Mode status
                case 0x73: length =   2; break; // Ping message
                case 0x7d: length =  13; break; // Client Response To Dialog
                case 0x80: length =  62; break; // Login Request
                case 0x83: length =  39; break; // Delete Character
                case 0x86: length = 304; break; // Resend Characters After Delete
                case 0x91: length =  65; break; // Game Server Login (Server to play selected)
                case 0x93: length =  99; break; // Books – Update Title Page (receive version of packet 0x93)
                case 0x95: length =   9; break; // Dye Window
                case 0x98: length =   7; break; // All-names “3D” (3d clients only packet, receive version 7 bytes long)
                case 0x99: length =  26; break; // Bring Up House/Boat Placement View
                case 0x9a: length =  16; break; // Console Entry Prompt
                case 0x9b: length = 258; break; // Request Help
                case 0x9f: length = ???; break; // Sell Reply
                case 0xa0: length =   3; break; // Select Server
//                case 0xa4: length =   5; break; // Client Machine info (Apparently was a sort of lame spyware command .. unknown if it still works)
                case 0xa7: length =   4; break; // Request Tips/Notice
                case 0xaa: length =   5; break; // Attack Request Reply
                case 0xac: length = ???; break; // Gump Text Entry Dialog Reply
                case 0xad: length = ???; break; // Unicode speech request
                case 0xb1: length = ???; break; // Gump Menu Selection

                //unsure about this message if received, sent or both
                case 0xb2: length = ???; break; // Chat Message

                case 0xb5: length =  64; break; // Open Chat window
                case 0xb6: length =   9; break; // Send Help/Tip Request
                case 0xb8: length = ???; break; // Request Char Profil
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
                default: // Discard received packet
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

        UI08 sex                = buffer[70];
        UI16 strength           = buffer[71];
        UI16 dexterity          = buffer[72];
        UI16 intelligence       = buffer[73];
        UI16 skill1             = buffer[74];
        UI16 skill1value1       = buffer[75];
        UI16 skill12            = buffer[76];
        UI16 skill1value2       = buffer[77];
        UI16 skill3             = buffer[78];
        UI16 skill1value3       = buffer[79];
        UI16 SkinColor          = ShortFromCharPtr(buffer + 80) | 0x8000;
        UI16 HairStyle          = ShortFromCharPtr(buffer + 82);
        UI16 HairColor          = ShortFromCharPtr(buffer + 84);
        UI16 FacialHair         = ShortFromCharPtr(buffer + 86);
        UI16 FacialHairColor    = ShortFromCharPtr(buffer + 88);
        UI16 StartingLocation   = ShortFromCharPtr(buffer + 90);        // from starting list
        // UI16 unknown         = ShortFromCharPtr(buffer + 92);
        UI16 slot               = ShortFromCharPtr(buffer + 94);
        UI32 clientIP           = LongFromCharPtr (buffer + 96);
        UI16 shirt_color        = ShortFromCharPtr(buffer + 100);
        UI16 pants_color        = ShortFromCharPtr(buffer + 102);


        // Disconnect-level protocol error check (possible client hack or too many chars already present in account)
        if (
                !(client->currAccount()->getPassword()->compare(buffer+40)) ||                  //!< Password check
                (client->currAccount()->getCharsNumber() >= ServerScp::g_nLimitRoleNumbers) ||  //!< Max PCs per account check
                ((sex !=1) && (sex != 0)) ||                                                    //!< Sex validity check
                (strength + dexterity + intelligence > 80) ||                                   //!< Stat check: stat sum must be <=80
                (strength < 10)     || (strength > 60)     ||                                   //!< each stat must be >= 10 and <= 60
                (dexterity < 10)    || (dexterity > 60)    ||
                (intelligence < 10) || (intelligence > 60) ||
                (skillvalue1 + skillvalue2 + skillvalue3 != 100) ||                             //!< Skill check : sum of skills selected must be 100
                (skillvalue1 > 50)  || (skillvalue2 > 50)  || (skillvalue3 > 50) ||             //!< each skill must be >= 0 and <= 50. Since the 3 variables are UI08, if they are negative they will be seen as a number surely bigger than 127 :)
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

	P_ITEM pi;

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
        Network->startchar(s);
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
	SERIAL serial = LongFromCharPtr(buffer +1) & 0x7FFFFFFF;

	if (isCharSerial(serial))
	{
		pChar pd = pointers::findCharBySerial(serial);
		if (ISVALIDPC(pd))
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
        UI16 amount = ShortFromCharPtr(buffer+5);
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
        Location drop_at = Loc(ShortFromCharPtr(buffer+5), ShortFromCharPtr(buffer+7), buffer[9]);
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
        SERIAL serial = LongFromCharPtr(buffer + 1);
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
        UI16 size = ShortFromCharPtr(buffer + 1);
        if (length != size) return false;

        UI08 type = buffer[3];
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
		if(ISVALIDPI(pack)) //lb
		{
			NxwItemWrapper gri;
			gri.fillItemsInContainer( pack, false );
			gri.fillItemWeared( pc_currchar, true, true, false );
			for( gri.rewind(); !gri.isEmpty(); gri++ )
			{
				pItem pj=gri.getItem();
				if (ISVALIDPI(pj))
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
		if( ( ISVALIDPC(murderer) ) && SrvParms->bountysactive )
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
        UI16 size = ShortFromCharPtr(buffer + 1);
        if (length != size) return false;
      	// client 1.26.2b+ skill managment packet
        // -> 0,1,2,3 -> ignore them
	// -> 4 = skill number
	// -> 5 = 0 raising (up), 1 falling=candidate for atrophy, 2 = locked
        if ( ISVALIDPC( client->currChar()) ) client->currChar()->lockSkill[buffer[4]] = buffer[5]; // save skill managment changes
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
        UI16 size = ShortFromCharPtr(buffer + 1);
        if (length != size) return false;

        std::vector< buyeditem > allitemsbought;

	pNpc npc = (pNpc)pointers::findCharBySerPtr(buffer + 3);
	VALIDATEPCR(npc, false);

	int itemtotal=(size - 8)/7;
	if (itemtotal>256) return false;

        for(i=0;i<itemtotal;i++)
	{
		int pos=8+(7*i);

		buyeditem b;

		b.layer=buffer[pos];
		b.item=pointers::findItemBySerPtr(buffer + pos + 1);
		if(!ISVALIDPI(b.item))
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
        UI16 x 				= ShortFromCharPtr(buffer + 7);
        UI16 y 				= ShortFromCharPtr(buffer + 9);

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

	P_CHAR pc_k=NULL;

	if (acctno[s]>INVALID)
	{
		int j=0;
		Accounts->SetOffline(acctno[s]);
		NxwCharWrapper sc;
		Accounts->GetAllChars( acctno[s], sc );
		for( sc.rewind(); !sc.isEmpty(); sc++ ) {
			P_CHAR pc_i=sc.getChar();
			if(!ISVALIDPC(pc_i))
				continue;
			if (j==buffer[s][0x44]) {
				pc_k=pc_i;
				break;
			}
			j++;
		}

		if (ISVALIDPC(pc_k))
		{
			pc_k->setClient(NULL);
			SI32 nSer = pc_k->getSerial32();
			for ( SI32 idx = 0; idx < now; idx++ ) {
				if ( pc_k == loginchars[idx] ) {
					// TODO We need to fix this!!!
					UI08 msg2[2]={ 0x53, 0x05 };
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
			UI08 msg[2]={ 0x53, 0x05 };
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
        UI16 size = ShortFromCharPtr(buffer + 1);
        if (length != size) return false;
	pItem book=pointers::findItemBySerPtr(buffer+3);
	if(ISVALIDPI(book))
	{
		if (book->morez == 0)	Books::addNewBook(book);
		if (book->morex!=666 && book->morex!=999)
			book->morex = 666;
		if (book->morex==666) // writeable book -> copy page data send by client to the class-page buffer
			Books::books[book->morez].ChangePages((char*)(buffer + 13), ShortFromCharPtr(buffer + 9), ShortFromCharPtr(buffer + 11), size - 13 );
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


        //! \todo update this when targets redone
        
	target->receive( ps );

					if( !target->isValid() )
						target->error( ps );
					else
						target->code_callback( ps, target );
}



/*!
\brief Receive bullettin board message
\author Chronodt
\param client client who sent the packet
*/


bool cPacketReceiveBBoardMessage::execute(pClient client)
{
        UI16 size = ShortFromCharPtr(buffer + 1);
        if (length != size) return false;

	// Message \x71 has numerous uses for the Bulletin Board
	// so we need to get the type of message from the client first.

	int msgType = buffer[3];
        pMsgBoard msgboard = (pMsgBoard)pointers::findItemBySerial(LongFromCharPtr(buffer + 4));
        VALIDATEPIR( msgboard, false)

	switch (msgType)
	{
		case 3:  // Client->Server: Client has dbl-clicked on subject, requesting body of message
		{        // Example  [SEND:12] 71 00 0c 03 40 07 ba 3d 40 1c 53 eb
	                msgSN = LongFromCharPtr(buffer + 8);
			msgboard->MsgBoardOpenPost( client, msgSN );
			break;
		}

		case 4:  // Client->Server: Client has ACK'ed servers download of posting serial numbers
		{
      	                msgSN = LongFromCharPtr(buffer + 8);
			// Check to see whether client has ACK'd all of our message ID's before proceeding
			client->postAckCount++;
			//ConOut(" pstAckCont=%d        postCount=%d\n", postAckCount[s], postCount[s]);
			if ( postAckCount[s] != postCount[s] )
				return;

			// Server needs to handle ACK from client that contains the posting serial numbers
			MsgBoardList( s );
			break;
		}

	case 5:  // Client->Server: Client clicked on Post button (either from the main board or after pressing the Reply)
		{        //                 Reply just switches to the Post item.

			P_CHAR pc=MAKE_CHAR_REF(currchar[s]);
			VALIDATEPC(pc);
			// Check privledge level against server.cfg msgpostaccess

			if ( (pc->IsGM()) || (SrvParms->msgpostaccess) )
				MsgBoardPost( s, pc->postType, 0 );
			else
				pc->sysmsg(TRANSLATE("Thou art not allowed to post messages."));

			break;
		}

	case 6:  // Remove post from Bulletin board
		{
			P_CHAR pc=MAKE_CHAR_REF(currchar[s]);
			VALIDATEPC(pc);
			//             |p#|s1|s2|mt|b1|b2|b3|b4|m1|m2|m3|m4|
			// Client sends 71  0  c  6 40  0  0 18  1  0  0  4
			if ( (pc->IsGM()) || (SrvParms->msgpostremove) )
				MsgBoardRemovePost( s );
			break;
		}


	default:
		{
			ErrOut("MsgBoardEvent() Unknown msgType:%x for message: %x\n", buffer[s][3], buffer[s][0]);
			break;
		}
	}
}
