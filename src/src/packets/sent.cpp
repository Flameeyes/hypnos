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

	UI32 gold = pc->rtti() == rttiPC ? (reinterpret_cast<pPC*>pc)->getGold : 0;
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

static pPacketReceive cPacketReceive::fromBuffer(UI08 *buffer, UI16 length)
{

       switch(buffer[0])
       {
                case 0x00: return new cPacketReceiveCreateChar(buffer, length);         // Create Character
                case 0x01: return new cPacketReceiveDisconnectNotify(buffer, length);   // Disconnect Notification
                case 0x02: return new cPacketReceiveMoveRequest(buffer, length);        // Move Request
                case 0x03: return new cPacketReceiveTalkRequest(buffer, length);        // Talk Request
                case 0x05: return new cPacketReceiveAttackRequest(buffer, length);      // Attack Request
                case 0x06: return new cPacketReceiveDoubleclick(buffer, length);        // Double click
                case 0x07: length =   7; break; // Pick Up Item(s)
                case 0x08: length =  14; break; // Drop Item(s)
                case 0x09: length =   5; break; // Single click
                case 0x12: length = ???; break; // Request Skill/Action/Magic Usage
                case 0x13: length =  10; break; // Drop - Wear Item
                case 0x1a: length = ???; break; // Object Information
                case 0x1b: length =  37; break; // Char Location and body type (Login confirmation)
                case 0x1c: length = ???; break; // Send Speech
                case 0x22: length =   3; break; // when received, this packet is a Resync Request
                case 0x23: length =  26; break; // Dragging of Items
//                case 0x2c: length =   2; break; // (Obsolete) Resurrection Menu Choice
                case 0x34: length =  10; break; // Get Player Status

                // packet 0x38 (pathfinding) is received or sent???

                case 0x3a: length = ???; break; // Set Skill Lock (receive version of packet 0x3a)
                case 0x3b: length = ???; break; // Buy Item(s)
                case 0x56: length =  11; break; // Map Related
                case 0x5d: length =  73; break; // Login Character
                case 0x66: length = ???; break; // Books - Page
//                case 0x69: length =   5; break; // (Obsolete) Change Text/Emote Color
                case 0x6c: length =  19; break; // Targeting Cursor Commands

                // packet 0x6f (secure trading) seems send-only package, but not sure

                case 0x71: length = ???; break; // Bulletin Board Message
                case 0x72: length =   5; break; // Request War Mode Change/Send War Mode status
                case 0x73: length =   2; break; // Ping message
                case 0x7d: length =  13; break; // Client Response To Dialog
                case 0x80: length =  62; break; // Login Request
                case 0x83: length =  39; break; // Delete Character
                case 0x86: length = 304; break; // Resend Characters After Delete

                //what is this???
                case 0x91: length =  65; break; // Game Server Login

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


virtual bool cPacketReceiveCreateChar::execute(pClient client)
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

        // From here building a cBody to pass to cClient::charcreate()
        // no serial number required because we use it only as a "container" to pass parameters to charcreate
        pBody charbody = new cBody();

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

virtual bool cPacketReceiveDisconnectNotify::execute(pClient client)
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

virtual bool cPacketReceiveMoveRequest::execute (pClient client)
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

virtual bool cPacketReceiveTalkRequest::execute (pClient client)
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
virtual bool cPacketReceiveAttackRequest::execute (pClient client)
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

virtual bool cPacketReceiveDoubleclick::execute(cClient client)
{
        if (length != 5) return false;
	pPC pc = client->currChar();
	VALIDATEPCR( pc, false );

	// the 0x80 bit in the first byte is used later for "keyboard" and should be ignored
	SERIAL serial = LongFromCharPtr(buffer +1) & 0x7FFFFFFF;

	if (isCharSerial(serial))
	{
		pChar pd = pointers::findCharBySerial(serial);
		ISVALIDPCR(pd, false)
		pd->doubleClick(client);
		return true;
	}
	pItem pi = pointers::findItemBySerial(serial);
	VALIDATEPIR(pi, false);  //If it's neither a char nor an item, then it's invalid
        pi->doubleClick(client);
}
