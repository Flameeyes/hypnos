/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Implementation of packets classes
*/

#include "settings.h"
#include "inlines.h"
#include "packets/send.h"
#include "objects/cchar.h"
#include "objects/cpc.h"
#include "objects/cbody.h"
#include "objects/cclient.h"
#include "objects/citem/cbook.h"
#include "objects/citem/cmap.h"

/*!
\brief Status window
\author Flameeyes
\note packet 0x11
*/
void nPackets::Sent::Status::prepare()
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
	strncpy(buffer + 7, body->getName().c_str(), 30);


	ShortToCharPtr(body->getHitPoints(), buffer+37);
	ShortToCharPtr(body->getMaxHitPoints(), buffer+39);

	buffer[41] = canrename ? 0xFF : 0x00;
	buffer[42] = type;

	switch(type)
	{  //note :do not put breaks in this switch. they are cascaded so newer versions of this packet gets all data
	case 4:
		ShortToCharPtr(body->getResistFire(), buffer+70);
		ShortToCharPtr(body->getResistCold(), buffer+72);
		ShortToCharPtr(body->getResistPoison(), buffer+74);
		ShortToCharPtr(body->getResistEnergy(), buffer+76);
		ShortToCharPtr(body->getLuck(), buffer+78);
		ShortToCharPtr(body->getDamageMin(), buffer+80);
		ShortToCharPtr(body->getDamageMax(), buffer+82);
		memset(buffer+84, 0, 4);
	case 3:
        	ShortToCharPtr(body->getStatCap(), buffer+66);
		buffer[68] = body->getFollowers();
		buffer[69] = body->getMaxFollowers();
	case 1:
		buffer[43] = body->getGender();
		ShortToCharPtr(body->getStrength(), buffer+44);
		ShortToCharPtr(body->getDexterity(), buffer+46);
		ShortToCharPtr(body->getIntelligence(), buffer+48);
		ShortToCharPtr(body->getStamina(), buffer+50);
		ShortToCharPtr(body->getMaxStamina(), buffer+52);
		ShortToCharPtr(body->getMana(), buffer+54);
		ShortToCharPtr(body->getMaxMana(), buffer+56);

		pPC pctest = dynamic_cast<pPC>pc;
		uint32_t gold = pctest ? pctest->getGold() : 0;
		LongToCharPtr(gold, buffer+58);

		ShortToCharPtr(body->getArmor(), buffer+62);
		ShortToCharPtr(body->getWeight(), buffer+64);

	}
}

/*!
\brief show item to player (called when item first appears on char visualrange ... on the ground)
\author Chronodt
\note packet 0x1a
*/
void nPackets::Sent::ObjectInformation::prepare()
{
	buffer = new uint8_t[20]; 	//MAXIMUM packet length
	sLocation pos = pi->getPosition();

	buffer[0] = 0x1A;
	LongToCharPtr(pi->getSerial() | 0x80000000, buffer +3);

	//if item is a lightsource and player is a gm,
	//is shown like a candle (so that he can move it),
	//....if not, the item is a normal
	//invisible light source!
	if(pc->isGM() && pi->getId()==0x1647) ShortToCharPtr(0x0A0F, buffer +7);
	else if (pc->canViewHouseIcon() && pi->getId()>=0x4000 && pi->getId()<=0x40FF) ShortToCharPtr(0x14F0, buffer +7);
	// LB, 25-dec-1999 litle bugfix for treasure multis, ( == changed to >=)
	else ShortToCharPtr(pi->getAnimId(), buffer +7);
	
	ShortToCharPtr(pi->amount, buffer +9);
	ShortToCharPtr(pos.x, buffer +11);
	ShortToCharPtr(pos.y | 0xC000, buffer +13);

	uint8_t offset = 15;

	if (pi->dir)
	{
		++offset;
		buffer[11]|=0x80;
		buffer[15]=static_cast<unsigned char>(pi->dir);
	}

	buffer[offset]= pos.z;


	if(pc->isGM() && pi->getId()==0x1647) ShortToCharPtr(0x00C6, buffer + offset +1);	//let's show the lightsource like a blue item
	else ShortToCharPtr(pi->getColor(), buffer + offset + 1);

	buffer[offset +2]=0;
	If (pc->isGM() && (pi->visible ==1 || pi->visible==2)) buffer[offset +2]|=0x80;

	if (pi->magic==1 || pc->canAllMove()) buffer[offset +2]|=0x20; //item can be moved even if normally cannot

	if ((pi->magic==3 || pi->magic==4) && pc == pi->getOwner() ) buffer[offset +2]|=0x20; //Item can be moved by owner for those "magic levels"

	length = offset +4;
	ShortToCharPtr(length, buffer +1);
}

/*!
\brief alters item already shown to client (color & position). Item real position and color is not changed (used by lsd effect)
\author Chronodt
\note packet 0x1a
*/

void nPackets::Sent::LSDObject::prepare()
{
	buffer = new uint8_t[20]; 	//MAXIMUM packet length

	buffer[0] = 0x1A;
	LongToCharPtr(pi->getSerial() | 0x80000000, buffer +3);

	//if item is a lightsource and player is a gm,
	//is shown like a candle (so that he can move it),
	//....if not, the item is a normal
	//invisible light source!
	if(pc->isGM() && pi->getId()==0x1647) ShortToCharPtr(0x0A0F, buffer +7);
	else 	if (pc->canViewHouseIcon() && pi->getId()>=0x4000 && pi->getId()<=0x40FF) ShortToCharPtr(0x14F0, buffer +7);         // LB, 25-dec-1999 litle bugfix for treasure multis, ( == changed to >=)
		else ShortToCharPtr(pi->animid(), buffer +7);
	ShortToCharPtr(pi->amount, buffer +9);
	ShortToCharPtr(position.x, buffer +11);
	ShortToCharPtr(position.y | 0xC000, buffer +13);

	uint8_t offset = 15;

	if (pi->dir)
	{
		++offset;
		buffer[11]|=0x80;
		buffer[15]=static_cast<unsigned char>(pi->dir);
	}

	buffer[offset]= position.z;

	if(pc->isGM() && pi->getId()==0x1647) ShortToCharPtr(0x00C6, buffer + offset +1);	//let's show the lightsource like a blue item
        else ShortToCharPtr(color, buffer + offset + 1);

	buffer[offset +2]=0;
	if (pc->isGM() && (pi->visible ==1 || pi->visible==2)) buffer[offset +2]|=0x80;

	if (pi->magic==1 || pc->canAllMove()) buffer[offset +2]|=0x20; //item can be moved even if normally cannot

	if ((pi->magic==3 || pi->magic==4) && pc == pi->getOwner()) buffer[offset +2]|=0x20; //Item can be moved by owner for those "magic levels"

	length = offset +4;
	ShortToCharPtr(length, buffer +1);
}


/*!
\brief Char sLocation and body type (Login confirmation)
\author Chronodt
\note packet 0x1b
*/

void nPackets::Sent::LoginConfirmation::prepare()
{
	buffer = new uint8_t[37];
	length= 37;

	static const uint8_t templ[37] = {
	        0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 0x09,
	        0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		};
	memcpy(buffer, templ, 37);
	sLocation charpos= pc->getPosition();

	LongToCharPtr(pc->getSerial(), buffer +1);
	ShortToCharPtr(pc->getId(), buffer +9);
	ShortToCharPtr(charpos.x, buffer +11);
	ShortToCharPtr(charpos.y, buffer +13);
	buffer[16]= charpos.z;
	buffer[17]= pc->dir;
	if(pc->poisoned) buffer[28]=0x04; else buffer[28]=0x00; //AntiChrist -- thnx to SpaceDog
}

/*!
\brief Send Speech (not unicode)
\author Chronodt
\note packet 0x1c
*/

void nPackets::Sent::Speech::prepare()
{
	std::string text;
	if (ghost) text = speech.toGhost()
	else text = speech.toString();	// reducing unicode string to simple text
	length = 44 + text.size() + 1;
	buffer = new uint8_t[length];

	buffer[0] = 0x1c;
	ShortToCharPtr(length, buffer+1);
	LongToCharPtr(ps ? ps->getSerial() : 0xffffffff, buffer+3);
	ShortToCharPtr(ps ? ps->getId() : 0xffff, buffer+7);
	buffer[9] = speech.getMode();
	ShortToCharPtr(speech.getColor(), buffer+10);
	ShortToCharPtr(speech.getFont(), buffer+12);
	strncpy(buffer + 14, (ps) ? ps->getCurrentName().c_str() : "", 30); //this will write the name and fills the missing char in the 30 bytes buffer with \0
	strcpy(buffer + 44, text.c_str());
}

/*!
\brief removes item from client's view
\author Flameeyes
\note packet 0x1d
*/

void nPackets::Sent::DeleteObj::prepare()
{
	length = 5;
	buffer = new uint8_t[5];

	buffer[0] = 0x1D;
	LongToCharPtr(pser->getSerial(), buffer+1);
}

/*!
\brief Draws game player (used only for client-played char)
\author Chronodt
\note packet 0x20
*/

void nPackets::Sent::DrawGamePlayer::prepare()
{
	length = 19;
	buffer = new uint8_t[19];
	pBody body = pc->getBody();

	buffer[0] = 0x20;
	LongToCharPtr(pc->getSerial(), buffer+1);
	ShortToCharPtr(body->getId(), buffer +5);
	buffer[7] = unk1;
	ShortToCharPtr(body->getSkinColor(), buffer +8);

	uint8_t flag = 0;
	if ( pc->poisoned )   flag |= 0x04;
	if ( pc->isHidden() ) flag |= 0x80;

	buffer[10] = flag;

	sLocation pos = pc->getPosition();
	ShortToCharPtr(pos.x, buffer +11);
	ShortToCharPtr(pos.y, buffer +13);
	ShortToCharPtr(0, buffer +15);
	buffer[17]= pc->dir;   // old nox used this packet sending dir|0x80, but that wouldn't make the char always running?
	//!\todo verify if using pos.dispz or pos.z changes anything here
	buffer[18]= pos.dispz;
}

/*!
\brief Move reject
\author Chronodt
\note packet 0x21
*/

void nPackets::Sent::MoveReject::prepare()
{
	length = 8;
	buffer = new uint8_t[8];

	buffer[0] = 0x21;
	buffer[1] = sequence;
	ShortToCharPtr(pc->getPosition().x, buffer + 2);
	ShortToCharPtr(pc->getPosition().y, buffer + 4);
	buffer[6] = pc->direction;
	buffer[7] = pc->getPosition().z;
}

/*!
\brief Move accepted
\author Chronodt
\note packet 0x22
*/

void nPackets::Sent::MoveAcknowdledge::prepare()
{
	buffer = new uint8_t[3];
	length = 3;

	buffer[0] = 0x22;
	buffer[1] = sequence;
	buffer[2] = notoriety;
/*
notoriety:
0 = invalid/across server line
1 = innocent (blue)
2 = guilded/ally (green)
3 = attackable but not criminal (gray)
4 = criminal (gray)
5 = enemy (orange)
6 = murderer (red)
7 = unknown use (translucent (like 0x4000 hue))
*/
}

/*!
\brief Item dragging
\author Chronodt
\note packet 0x23
*/

void nPackets::Sent::DragItem::prepare()
{
	buffer = new uint8_t[26];
	length = 26;

	buffer[0] = 0x23;
	ShortToCharPtr(item->getId(), buffer + 1);
	memset(buffer + 3, 0, 3); //sets 3 unknown bytes to 0
	ShortToCharPtr(amount, buffer + 6);
	LongToCharPtr(item->getSerial(), buffer + 8);
	sLocation worldpos = item->getWorldPosition()
	ShortToCharPtr(worldpos.x, buffer + 12);
	ShortToCharPtr(worldpos.y, buffer + 14);
	buffer[16] = worldpos.z;
	LongToCharPtr(item->getSerial(), buffer + 17);
	ShortToCharPtr(destination.x, buffer + 21);
	ShortToCharPtr(destination.y, buffer + 23);
        buffer[25] = destination.z;
}

/*!
\brief Open gump
\author Chronodt
\note packet 0x24
*/

void nPackets::Sent::OpenGump::prepare()
{
	buffer = new uint8_t[7];
	length = 7;

	buffer[0] = 0x24;
	LongToCharPtr(serial, buffer + 1);
	ShortToCharPtr(gump, buffer + 5);
}

/*!
\brief tells to the client the serial and all other data about an item in the given container. usually preceded by another package
\author Flameeyes & Chronodt
\note packet 0x25
*/

void nPackets::Sent::ShowItemInContainer::prepare()
{
	length = 20;
	buffer = new uint8_t[20];

	buffer[0] = 0x25;
	buffer[7] = 0x00;

	LongToCharPtr(item->getSerial(), buffer+1);

	//if player is a gm, this item
	//is shown like a candle (so that he can move it),
	//....if not, the item is a normal
	//invisible light source!
	if(pc->isGM() && item.id == 0x1647)
	{
		ShortToCharPtr(0x0A0F, buffer + 5);
		ShortToCharPtr(0x00C6, buffer + 18);
	}
	else
	{
		ShortToCharPtr(item->getAnimId(), buffer + 5);
		ShortToCharPtr(item->getColor(), buffer + 18);
	}
	ShortToCharPtr(item->getAmount(), buffer + 8);
	ShortToCharPtr(item->getPosition().x, buffer + 10);
	ShortToCharPtr(item->getPosition().y, buffer + 12);
	LongToCharPtr(item->getContainer()->getSerial(), buffer + 14);
}

/*!
\brief Kick client
\author Chronodt
\note packet 0x26

\todo use this packet where it is needed
*/

void nPackets::Sent::Kick::prepare()
{
	buffer = new uint8_t[5];
	length = 2;
	buffer[0] = 0x26;
	LongToCharPtr(kicker->getSerial(), buffer + 1);
}


/*!
\brief Bounce item
\author Chronodt
\note packet 0x27
*/

void nPackets::Sent::BounceItem::prepare()
{
	buffer = new uint8_t[2];
	length = 2;
	buffer[0] = 0x27;
	buffer[1] = mode;
}

/*!
\brief Clear Square
\author Chronodt
\note packet 0x28

\todo this packet is not used anywhere, but may be useful when wiping
*/
void nPackets::Sent::ClearSquare::prepare()
{
	buffer = new uint8_t[5];
	length = 5;
	buffer[0] = 0x28;
	ShortToCharPtr(x, buffer + 1);
	ShortToCharPtr(y, buffer + 3);
}

/*!
\brief Tells the client paperdoll has been changed
\author Chronodt
\note packet 0x29

\todo verify if this packet has to be sent on equip() and unEquip() methods of cChar
*/

void nPackets::Sent::PaperdollClothingUpdated::prepare()
{
	length = 1;
	buffer = new uint8_t[1];
	buffer[0] = 0x29;
}

/*!
\brief Mobile attributes
\author Chronodt
\note packet 0x2d

\todo apparently this useful packet is not used -_-
*/
void nPackets::Sent::MobileAttributes::prepare()
{
	length = 17;
	buffer = new uint8_t[17];
	buffer[0] = 0x2d;
	LongToCharPtr(chr->getSerial(), buffer + 1);
	ShortToCharPtr(chr->getMaxHp(), buffer + 5);
	ShortToCharPtr(chr->getHp(), buffer + 7);
	ShortToCharPtr(chr->getMaxMana(), buffer + 9);
	ShortToCharPtr(chr->getMana(), buffer + 11);
	ShortToCharPtr(chr->getMaxStamina(), buffer + 13);
	ShortToCharPtr(chr->getStamina(), buffer + 15);
}

/*!
\brief Adds item to client's paperdoll
\author Chronodt
\note packet 0x2e
*/

void nPackets::Sent::WornItem::prepare()
{
	length = 15;
	buffer = new uint8_t[15];

	buffer[0] = 0x2E;
	buffer[7] = 0x00;

	LongToCharPtr(item->getSerial(), buffer+1);
	ShortToCharPtr(item->getAnimId(), buffer+5);
	buffer[8] = item->getLayer();
	pBody body = dynamic_cast<pBody> (item->getContainer());
	LongToCharPtr(body->getChar()->getSerial(), buffer+9);
	ShortToCharPtr(item->getColor(), buffer+13);
}

/*!
\brief There is a fight going on somewhere on screen
\author Chronodt
\note packet 0x2f

\todo awaiting new combat/status system for implementation
*/

void nPackets::Sent::FightOnScreen::prepare()
{
	length = 10;
	buffer = new uint8_t[10];

	buffer[0] = 0x2F;
	buffer[1] = 0x00;

	LongToCharPtr(attacker->getSerial(), buffer+2);
	LongToCharPtr(defender->getSerial(), buffer+6);
}

/*!
\brief pause/resume client
\author Chronodt
\note packet 0x33

\todo awaiting new save system to implement client pause
*/

void nPackets::Sent::PauseClient::prepare()
{
	length = 2;
	buffer = new uint8_t[2];

	buffer[0] = 0x33;
	buffer[1] = pausestatus;
}

/*!
\brief pathfinding
\author Chronodt
\note packet 0x38

\todo fastwalk prevention code (if needed)
\note this packet should be sent at least 19 times consecutively to have any effect (unverified)
*/

void nPackets::Sent::Pathfinding::prepare()
{
	length = 7;
	buffer = new uint8_t[7];

	buffer[0] = 0x38;
	ShortToCharPtr(loc.x, buffer+1);
	ShortToCharPtr(loc.y, buffer+3);
	ShortToCharPtr(loc.z, buffer+5);
}

/*!
\brief Send skill list or updates a single skill
\author Chronodt
\note packet 0x3a
*/

void nPackets::Sent::SendSkills::prepare()
{

	bool capdata = nSettings::Server::isEnabledSkillBySkillCap();
	bool singleskill = skill == UINVALID16;
	uint8_t mode = 0x00;

	if (singleskill)
	{
		if (capdata)
		{
			mode = 0xdf;
			length = 13;	//Adding space for skillcap
		}
		else
		{
			mode = 0xff;
			length = 11;
		}
	}
	else
	{
		if (capdata)
		{
			mode = 0x02;
			length = 4 + skTrueSkills*9 + 2;	//Adding space for skillcap
		}
		else length = 4 + skTrueSkills*7 + 2;
	}
	
	buffer = new uint8_t[length];

	buffer[0] = 0x3A;
	ShortToCharPtr(length, buffer+1);
	buffer[3] = mode;

	uint8_t *offset = buffer+4;
	uint8_t skillssent = singleskill ? 1 : skTrueSkills;
	uint8_t i= singleskill ? skill : 0;
	for (; i<skillssent; ++i)
	{
		Skills::updateSkillLevel(pc,i);

		ShortToCharPtr(i+1, offset);
		ShortToCharPtr(pc->getBody()->getSkill(i), offset +2);
		ShortToCharPtr(pc->getBody()->getSkillBase(i), offset +4);

		offset[6] = pc->getSkillLock(i);
		if (capdata)
		{
			ShortToCharPtr(pc->getBody()->getSkillCap(i), skill + 7);	//!\todo add a getSkillCap(uint8_t skill) with single skill cap
			offset+=9;
		}
		else offset += 7;
	}
	if (!mode || mode == 0x02) ShortToCharPtr(0, offset);
}

/*!
\brief empties buy window (i.e. just like pressing the clear button on client, but this is from server side)
\author Chronodt
\note packet 0x3b
*/

void nPackets::Sent::ClearBuyWindow::prepare()
{
	length = 8;
	buffer = new uint8_t[8];
	buffer[0] = 0x3B;
	ShortToCharPtr(0x08, buffer +1);			// Packet len
	LongToCharPtr( npc->getSerial(), buffer + 3);		// vendorID
	buffer[7]=0x00;						// Flag:  0 => no more items  0x02 items following ...
}


/*!
\brief tells client which items are in the given container
\author Flameeyes
\note packet 0x3c
*/

void nPackets::Sent::ContainerItem::prepare()
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


/*!
\brief tells client which items are in the given msgboard
\author Flameeyes & Chronodt
\note packet 0x3c

msgboard version of this packet requires special handling so it is separate from the other 0x3c above

\note This functions holds and releases the cMsgBoard::globalMutex and cMsgBloard::boardMutex mutexes
*/
void nPackets::Sent::MsgBoardItemsinContainer::prepare()
{
 	static const uint8_t templ1[5] = {
		0x3C, 0x00, 0x05, 0x00, 0x00
		};

	static const uint8_t templ2[19] = {
		0x00, 0x00, 0x00, 0x00, 0x0E, 0xB0, 0x00,
		0x00, 0x00, 0x00, 0x3A, 0x00, 0x3A, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00
		};

	cMsgBoard::globalMutex.lock();
	msgboard->boardMutex.lock();
	
	uint16_t count = msgboard->boardMsgs.size() + cMsgBoard::globalMsgs.size();
		//! \todo We need to add support for regional posts here!

	length = 5 + count*19;

	buffer = new uint8_t[length];
	memcpy(buffer, templ1, 5);

	ShortToCharPtr(length, buffer+1);
	ShortToCharPtr(count, buffer+3);

	uint8_t *ptrItem = buffer+5;

	for( cMsgBoard::MessageList iterator it = msgboard->boardMsgs.begin(); it != msgboard->boardMsgs.end(); it++ )
	{ // First of all, local messages
		memcpy(ptrItem, templ2, 19);
		LongToCharPtr( (*it)->getSerial(), ptrItem);
		LongToCharPtr( msgboard->getSerial(), ptrItem+13);
		//all other parts are invaried in msgboards messages and are defaulted in templ2
		ptrItem += 19;
	}
	
	//! \todo We need to add support for regional posts here!
	
	for( cMsgBoard::MessageList iterator it = cMsgBoard::glboalMsgs.begin(); it != cMsgBoard::glboalMsgs.end(); it++ )
	{ // At last, global messages
		memcpy(ptrItem, templ2, 19);
		LongToCharPtr( (*it)->getSerial(), ptrItem);
		LongToCharPtr( msgboard->getSerial(), ptrItem+13);
		//all other parts are invaried in msgboards messages and are defaulted in templ2
		ptrItem += 19;
	}
	
	cMsgBoard::globalMutex.unlock();
	msgboard->boardMutex.unlock();
}


/*!
\brief Personal Light Level
\author Chronodt
\note packet 0x4e
*/

void nPackets::Sent::Action::prepare()
{
	buffer = new uint8_t[6];
	length = 6;
	buffer[0] = 0x4E;
	LongToCharPtr(pc->getSerial(), buffer+1);
	buffer[5] = light;
}

/*!
\brief Overall Light Level
\author Flameeyes
\note packet 0x4f
*/

void nPackets::Sent::OverallLight::prepare()
{
	length = 2;
	buffer = new uint8_t[2];
	buffer[0] = 0x4F;
	buffer[1] = level;
}

/*!
\brief Idle warning
\author Chronodt
\note packet 0x53
*/

void nPackets::Sent::IdleWarning::prepare()
{
	length = 2;
	buffer = new uint8_t[2];
	buffer[0] = 0x53;
	buffer[1] = type;
}

/*!
\brief Sound effect
\author Flameeyes
\note packet 0x54
*/
void nPackets::Sent::SoundFX::prepare()
{
	static const uint8_t templ[12] = {
		0x54, 0x00, 0x12, 0x34, 0x00, 0x00,
		0x06, 0x40, 0x05, 0x9A, 0x00, 0x00
		};

	buffer = new uint8_t[12];
	length = 12;
	memcpy(buffer, templ, 12);
	
	buffer[1] = rep ? 0x00 : 0x01;
	ShortToCharPtr(model, buffer +2);
	ShortToCharPtr(loc.x, buffer +6);
	ShortToCharPtr(loc.y, buffer +8);
	ShortToCharPtr(loc.z, buffer +10);
}

/*!
\brief StartGame
\author Chronodt
\note packet 0x55
*/

void nPackets::Sent::StartGame::prepare()
{
	length = 1;
	buffer = new uint8_t[1];
	buffer[0] = 0x55;
}

/*!
\brief Sets/resets pins in a map
\author Chronodt
\note packet 0x56
*/

void nPackets::Sent::MapPlotCourse::prepare()
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

/*!
\brief Local time of server (or game time?)
\author Chronodt
\note packet 0x5b
*/

void nPackets::Sent::GameTime::prepare()
{
	length = 4;
	buffer = new uint8_t[4];
	buffer[0] = 0x5b;
	time_t now;
	time( &now );
	struct tm timest = localtime( &now);
	buffer[1] = timest.hour;
	buffer[2] = timest.min;
	buffer[3] = timest.sec;
}

/*!
\brief Send Weather
\author Chronodt
\note packet 0x65
*/

void nPackets::Sent::Weather::prepare()
{

	length = 4;
	buffer = new uint8_t[4];
	buffer[0] = 0x65;
	buffer[1] = weather;
	buffer[2] = intensity;
	buffer[3] = 0;		// maybe temperature, but it is unused for now
}

/*!
\brief Send Book Page (ReadWrite book version)
\author Flameeyes
\note packet 0x66
*/

void nPackets::Send::BookPagesReadWrite::prepare()
{
	cBook::tpages pages = book->getPages(pages);
	
	length = 9;
	for( cBook::tpages::iterator it = pages.begin(); it != pages.end(); it++ )
	{
		bytes += 4;
		uint16_t j = 0;
		for ( stringVector::iterator it2 = (*it).begin(); it2 != (*it).end(); it2++, j++)
			length += (*it2).size() + 1; // null-terminated string
		
		if ( j < 8 )
			length += 2 * (j-8)
	}
	
	buffer = new uint8_t[length];
	
	buffer[0] = 0x66;
	ShortToCharPtr(length, buffer+1);
	LongToCharPtr(book->getSerial(), buffer+3);
	ShortToCharPtr(page.size(), buffer+7);
	
	uint8_t *page = 9;
	uint16_t i = 1;
	for( cBook::tpages::iterator it = pages.begin(); it != pages.end(); it++ )
	{
		ShortToCharPtr(i, page);
		ShortToCharPtr(8, page+2);
		
		uint8_t *line = page+4;
		int j = 0;
		for( stringVector::iterator it2 = (*it).begin(); it2 != (*it).end(); it2++, j++)
		{
			strcpy( page, (*it2).c_str() );
			page += (*it2).size() +1;
		}
		while ( j++ < 8 )
		{
			strcpy( page, " " );
			page += 2;
		}
	}
}

/*!
\brief Send Book Page (Readonly book version)
\author Flameeyes
\note packet 0x66
*/

void nPackets::Send::BookPageReadOnly::prepare()
{
	stringVector page = book->getPage(p);
	
	length = 13;
	for ( stringVector::iterator it = page.begin(); it != page.end(); it++)
		length += (*it2).size() + 1; // null-terminated string
	
	buffer = new uint8_t[length];
	
	buffer[0] = 0x66;
	ShortToCharPtr(length, buffer+1);
	LongToCharPtr(book->getSerial(), buffer+3);
	ShortToCharPtr(page.size(), buffer+7);
	ShortToCharPtr(p, bookpage+9);
	ShortToCharPtr(page.size(), bookpage+11);
	
	uint8_t *line = buffer+13;
	
	for( stringVector::iterator it = page.begin(); it != page.end(); it++)
	{
		strcpy( line, (*it2).c_str() );
		line += (*it2).size() +1;
	}
}

/*!
\brief Send Targeting cursor to client
\author Chronodt
\note packet 0x6C

\todo targeting has to be remade almost completely -_-
*/

void nPackets::Sent::TargetingCursor::prepare()
{
	buffer = new uint8_t[19];
	length = 19;
	buffer[0] = 0x6c;
	buffer[1] = type;
	LongToCharPtr(cursorid, buffer+2);
	buffer[6] = 0;
	memset(buffer + 7, 0, 12);	//the remaining bytes are useless in sent message
}

/*!
\brief Send music code to client
\author Flameeyes
\note packet 0x6D
*/

void nPackets::Sent::PlayMidi::prepare()
{
	length = 3;
	buffer = new uint8_t[3];
	buffer[0] = 0x6D;
	ShortToCharPtr(id, buffer+1);
}

/*!
\brief Send action code to client (character performs the requested action)
\author Flameeyes
\note packet 0x6E
*/

void nPackets::Sent::Action::prepare()
{
	static const uint8_t templ[14] = {
		0x6E, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02,
		0x00, 0x05, 0x00, 0x01, 0x0, 0x00, 0x01
		};

	buffer = new uint8_t[14];
	length = 14;
	memcpy(buffer, templ, 14);
	LongToCharPtr(chr->getSerial(), buffer+1);
	ShortToCharPtr(action, buffer+5);
}

/*!
\brief Secure trade commands
\author Chronodt
\note packet 0x6f
*/
void nPackets::Sent::SecureTrading::prepare()
{
	if (action) length = 17;
	else length = 47;
	buffer = new uint8_t[length];
	buffer[0] = 0x6F;
	ShortToCharPtr(length, buffer+1);
	buffer[3]=action;
	LongToCharPtr(tradePartner->getSerial(), buffer +4);
	LongToCharPtr(id1, buffer +8);
	LongToCharPtr(id2, buffer +12);
	if (action) buffer[16]=0; 			// No name in this message
	else
	{
		buffer[16] = 1;
		strncpy(buffer + 17, tradePartner->getCurrentName().c_str(), 30);
	}
}

/*!
\brief Sends a visual effect
\author Chronodt
\note packet 0x70
*/

void nPackets::Sent::GraphicalEffect::prepare()
{
	length = 28;
	buffer = new uint8_t[28];
	buffer[0] = 0x70;
	buffer[1] = type;
	LongToCharPtr(src ? src->getSerial() : 0, buffer +2);
	LongToCharPtr(dst ? dst->getSerial() : 0, buffer +6);
	ShortToCharPtr(effect, buffer + 10);
	ShortToCharPtr(src_pos.x, buffer + 12);
	ShortToCharPtr(src_pos.y, buffer + 14);
	buffer[16]=src_pos.z;
	ShortToCharPtr(dst_pos.x, buffer + 17);
	ShortToCharPtr(dst_pos.y, buffer + 19);
	buffer[21]=dst_pos.z;
	buffer[22]=speed;
	buffer[23]=duration;
	ShortToCharPtr(0, buffer + 24); 	//unknown bytes, apparently 0
	buffer[26]=fixeddir ? 0x01 : 0x00;
	buffer[27]=explode ? 0x01 : 0x00;
}

/*!
\brief Sends bulletin boards commands
\author Chronodt
\note packet 0x71
*/

void nPackets::Sent::BBoardCommand::prepare()
{
	switch (command)
        {
		case bbcDisplayBBoard: //Display bulletin board
		{
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
			if ( msgboard->getCurrentName().length() )
				strncpy( buffer + 8, msgboard->getCurrentName(), 20);
					//Copying just the first 20 chars or we go out of bounds in the gump
			else
				strcpy( buffer + 8, "Bulletin Board");
			break;
		}
		case bbcSendMessageSummary:
		{
			//calculating length of packet  before building buffer
			length = 16; //BASE length, the length of fixed-length components
			pChar poster = message->getPoster();
			std::string timestring = message->getTimeString();
			length += poster->getCurrentName().size() + 2;
			length += message->subject.size() + 2;
			length += timestring.size() + 2;
			buffer = new uint8_t[length];
			buffer[0] = 0x71;
			ShortToCharPtr(length, buffer +1); 					//message length
			buffer[3] = 1; 								//subcommand 1
			LongToCharPtr(msgboard->getSerial(), buffer + 4);			//board serial
			LongToCharPtr(message->getSerial(), buffer + 8);			//message serial
			LongToCharPtr(message->replyof->getSerial(), buffer + 12);		//parent message serial
			int offset = 16;
			buffer[16] = poster->getCurrentName().size() + 1;		//size() does not count the endstring 0
			strcpy( buffer + 17, poster->getCurrentName().c_str() );
			offset += poster->getCurrentName().size() + 2;
			buffer[offset] = message->subject.size() + 1;
			strcpy( buffer + offset + 1, message->subject.c_str());
			offset += message->subject.size() + 2;
			buffer[offset] = timestring.size() + 1;
			strcpy( buffer + offset + 1, timestring.c_str());
			break;
		}
		case bbcSendMessageBody:
		{
			static const char pattern[29] = { 0x01, 0x90, 0x83, 0xea, 0x06,
							0x15, 0x2e, 0x07, 0x1d, 0x17,
							0x0f, 0x07, 0x37, 0x1f, 0x7b,
							0x05, 0xeb, 0x20, 0x3d, 0x04,
							0x66, 0x20, 0x4d, 0x04, 0x66,
							0x0e, 0x75, 0x00, 0x00};
			//calculating length of packet  before building buffer
			length = 12; //BASE length, the length of fixed-length components
			pChar poster = message->getPoster();
			std::string timestring = message->getTimeString();
			length += poster->getCurrentName().c_str().size() + 2;
			length += message->subject.size() + 2;
			length += timestring.size() + 2;
			buffer = new uint8_t[length];
			buffer[0] = 0x71;
			ShortToCharPtr(length, buffer +1); 	//message length
			buffer[3] = 2; 				//subcommand 2
			LongToCharPtr(msgboard->getSerial(), buffer + 4);	//board serial
			LongToCharPtr(message->getSerial(), buffer + 8);	//message serial
			int offset = 12;
			buffer[12] = poster->getCurrentName().c_str().size() + 1;	//size() does not count the endstring \0 :)
			strcpy( buffer + 17, poster->getCurrentName().c_str().c_str());
			offset += poster->getCurrentName().c_str().size() + 2;
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
}

/*!
\brief Sends war mode actual status to client [packet 0x72]
\author Chronodt
\note packet 0x72
*/
void nPackets::Sent::WarModeStatus::prepare()
{
	buffer = new uint8_t[5];
	length = 5;
	memcpy(buffer, buf, 5);
}

/*!
\brief Ping reply [packet 0x73]
\author Chronodt
\note packet 0x73
*/

void nPackets::Sent::PingReply::prepare()
{
	buffer = new uint8_t[2];
	length = 2;
	memcpy(buffer, buf, 2);
}

/*!
\brief Open Buy Window [packet 0x74]
\author Chronodt
\note packet 0x74
*/

void nPackets::Sent::BuyWindow::prepare()
{
	length = 8;	//length of constant part
	ItemSList items = container->getItems();

	for(ItemSList::iterator it = items.begin(); it != items.end();++it)
	{
		//! \todo use a description instead of currentname
		length += (*it)->getCurrentName().size() + 5;
	}

	//now length is the right length of packet. Now we can create the buffer ad fill it
	buffer = new uint8_t[length];
	buffer[0] = 0x74;
	ShortToCharPtr(length, buffer +1);			// message length
	LongToCharPtr(container->getSerial(), buffer+3); 	// Container serial number
	buffer[7]=items.size(); 				// Count of items;
	uint8_t* offset = buffer + 8;
	for(ItemSList::iterator it = items.begin(); it != items.end();++it)
	{
		uint32_t price = (*it)->value;
		//! \todo revise trade system
		/*
		price = pj->calcValue(price);

		if ( nSettings::Server::isEnabledTradeSystem() )
			value=calcGoodValue(pc, *it, price, 0); // by Magius(CHE)
		*/
		LongToCharPtr(price, offset); 	// price of item
		//! \todo use a description instead of currentname
		offset[4] = (*it)->getCurrentName().size();
		offset += 5 + offset[4];
	}
}

/*!
\brief Updates player [packet 0x77]
\author Chronodt
\note packet 0x77
*/

void nPackets::Sent::UpdatePlayer::prepare()
{
	buffer = new uint8_t[17];
	length = 17;
	buffer[0]=0x77;
	sLocation pos = chr->getPosition();
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

/*!
\brief Draws a player [packet 0x78]
\author Chronodt
\note packet 0x78
*/

void nPackets::Sent::DrawObject::prepare()
{
	length = 23;	// length of the fixed parts (19) + the 4 0-bytes used as terminator

	NxwItemWrapper si;
	si.fillItemWeared( pc, true, true, false );	//! \todo i suppose fillItemWeared will be updated correctly (with only the equipped items on pc's body))
	for( si.rewind(); !si.isEmpty(); ++si, length+=5 ) if (!si.getItem()->getColor()) length+=2;
	buffer = new uint8_t[length];
	buffer[0] = 0x78;
	ShortToCharPtr(length, buffer +1);
	LongToCharPtr(pc->getSerial(), buffer + 3);
	ShortToCharPtr(pc->getBody()->getId(), buffer + 7); 		// Character art id

	sLocation charpos = pc->getPosition();
	ShortToCharPtr(charpos.x, buffer + 9);
	ShortToCharPtr(charpos.y, buffer + 11);
	if (usedispz)
		buffer[13]= charpos.dispz; 				// Character z position
	else
		buffer[13]= charpos.z;
	buffer[14]= pc->dir; 						// Character direction
	ShortToCharPtr(pc->getBody()->getSkinColor(), buffer +15);	// Character skin color
	buffer[17]=0; 							// Character flags
	if (pc->IsHidden() || !(pc->IsOnline()||pc->npc))
		buffer[17]|=0x80; 					// .... show hidden state correctly
	if (pc->poisoned)
		buffer[17]|=0x04;

	//! \todo verify guilds
	int guild;
	guild=Guilds->Compare(client->currChar(),pc);
	if (guild==1)					//Same guild (Green)
		buffer[18] = 2;
	else if (guild==2) 				// Enemy guild.. set to orange
		buffer[18] = 5;
	else if (pc->IsGrey())
		buffer[18] = 3;
	else switch(pc->flag)
	{//1=blue 2=green 5=orange 6=Red 7=Transparent(Like skin 66 77a)
		case 0x01: buffer[18]=6; break;// If a bad, show as red.
		case 0x04: buffer[18]=1; break;// If a good, show as blue.
		case 0x08: buffer[18]=2; break; //green (guilds)
		case 0x10: buffer[18]=5; break;//orange (guilds)
		default: buffer[18]=3; break;//grey (Can be pretty much any number.. I like 3 :-)
	}

	uint8_t *offset = buffer + 19;

	for( si.rewind(); !si.isEmpty(); si++ )
	{
		//! \todo when sets remade, verify if all these checks are still necessary
		pEquippable pj=dynamic_cast<pEquippable> si.getItem();
		if (pj)
			if ( true /* pj->layers[pj->layer] == 0*/ )
			{
				LongToCharPtr(pj->getSerial(), offset);
				ShortToCharPtr(pj->getId(), offset+4);
				offset[6]=pj->getLayer();
				if (!pj->getColor())
				{
					offset[4]|=0x80;
					ShortToCharPtr(pj->getColor(), offset + 7);
					offset+= 2;
				}
				offset += 7;
				// layers[pj->layer] = 1;	<--- what was the use of this? :D
			}
	}

	LongToCharPtr(0, offset);	// adding the 4-bytes 0-terminator
}

/*!
\brief Opens dialog box [packet 0x7c]
\author Chronodt
\note packet 0x7c
*/

void nPackets::Sent::OpenDialogBox::prepare()
{
	//! \todo this function (awaiting gump remake)
}

/*!
\brief Login refused for some reason [packet 0x82]
\author Kheru
\note packet 0x82
*/
void nPackets::Sent::LoginDenied::prepare()
{
	buffer = new uint8_t[2];
	length = 2;

	buffer[0] = 0x82;
	buffer[1] = reason;
}

/*!
\brief Char deletion error [packet 0x85]
\author Chronodt
\note packet 0x85

reason:

0x00 => That character password is invalid.
0x01 => That character doesn't exist.
0x02 => That character is being played right now.
0x03 => That charater is not old enough to delete.
	The character must be 7 days old before it can be deleted.
0x04 => That character is currently queued for backup and cannot be
	deleted.
0x05 => Couldn't carry out your request.
*/
void nPackets::Sent::CharDeleteError::prepare()
{
	buffer = new uint8_t[2];
	length = 2;
	buffer[0] = 0x85;
	buffer[1] = reason;
}

/*!
\brief Resends character list to client after a succesful deletion [packet 0x86]
\author Chronodt
\note packet 0x86
*/
void nPackets::Sent::CharAfterDelete::prepare()
{
	buffer = new uint8_t[304];
	length = 304;
	memset(buffer, 304,0); //filling the buffer with zeroes
	buffer[0] = 0x86;
	ShortToCharPtr(304, buffer + 1);
	buffer[3] = account->getCharsNumber();
	for(int i = 0;i<5; ++i)
		if (i<= buffer[3])
			strcpy(buffer + (i*60) + 4, account->getChar(i)->getCurrentName().c_str());
}

/*!
\brief Opens Paperdoll [packet 0x88]
\author Chronodt
\note packet 0x88
*/
void nPackets::Sent::OpenPaperdoll::prepare()
{
	buffer = new uint8_t[66];
	length = 66;
	buffer[0] = 0x88;
	LongToCharPtr(pc->getSerial(), buffer + 1);
	strncpy(buffer + 5, pc->getCompleteTitle().c_str(), 60);
	// in documentation there is a flag byte here. i THINK it is a unicode/ascii flag (0 ascii, 1 unicode), but i'm not sure
	buffer[65] = 0x0
}

/*!
\brief Corpse Clothing [packet 0x89]
\author Chronodt
\note packet 0x89
*/
void nPackets::Sent::CorpseClothing::prepare()
{
	length = 7 + 5 * items.size() + 1; 		//7 header + 5 for each item and + 1 for terminator
	buffer = new uint8_t[length];
	buffer[0] = 0x89;
	ShortToCharPtr(length, buffer + 1);
	LongToCharPtr(corpse->getSerial(), buffer + 3);

	uint8_t* offset = buffer + 7;
	for( std::slist<pEquippable>::iterator it = items.begin(); it!= items.end(); ++it)
	{
		offset[0]= (*it)->getLayer();
		LongToCharPtr((*it)->getSerial(), offset + 1);
		offset+=5;
	}
	offset[0] = 0;	//terminator
}

/*!
\brief Connect to game server [packet 0x8c]
\author Chronodt
\note packet 0x8c
*/
void nPackets::Sent::ConnectToGameServer::prepare()
{
	length = 11;
	buffer = new uint8_t[11];
	buffer[0] = 0x8c;
	LongToCharPtr(ip, buffer + 1);
	ShortToCharPtr(port, buffer + 5);
	LongToCharPtr(newkey, buffer + 7);
}

/*!
\brief Opens map gump on client [packet 0x90]
\author Chronodt
\note packet 0x90
*/
void nPackets::Sent::OpenMapGump::prepare()
{
	length = 19;
	buffer = new uint8_t[19];
	buffer[0] = 0x90;
	LongToCharPtr(map->getSerial(), buffer +1);
	ShortToCharPtr(0x139D, buffer + 5);
	buffer[7]  = map->more1.moreb1;	// Assign topleft x
	buffer[8]  = map->more1.moreb2;
	buffer[9]  = map->more1.moreb3;	// Assign topleft y
	buffer[10] = map->more1.moreb4;
	buffer[11] = map->more2.moreb1;	// Assign lowright x
	buffer[12] = map->more2.moreb2;
	buffer[13] = map->more2.moreb3;	// Assign lowright y
	buffer[14] = map->more2.moreb4;
	int width, height;		// Temporary storage for w and h;
	width = 134 + (134 * morez);	// Calculate new w and h
	height = 134 + (134 * morez);
	ShortToCharPtr(width, buffer +15);
	ShortToCharPtr(height, buffer +17);
}

/*!
\brief Sends book header data [packet 0x93]
\author Flameeyes
\note packet 0x93
*/
void nPackets::Sent::BookHeader::prepare()
{
	buffer = new uint8_t[99];
	length = 99;
	buffer[0] = 0x93;
	LongToCharPtr( book->getSerial(), buffer+1 );
	buffer[5] = readonly ? 0x00 : 0x01;
	buffer[6] = 0x01; // Seems to be fixed
	ShortToCharPtr( book->getNumPages(), buffer+7 )
	strncpy( buffer+9, book->getTitle().c_str(), 60 );
	strncpy( buffer+69, book->getAuthor().c_str(), 30 );
}

/*!
\brief Sends Dye window [packet 0x95]
\author Chronodt
\note packet 0x95
*/
void nPackets::Sent::DyeWindow::prepare()
{
	length = 9;
	buffer = new uint8_t[9];
	buffer[0] =0x95;
	LongToCharPtr(object->getSerial(), buffer +1);
	ShortToCharPtr(0x0000, buffer +5);	// ignored on send ....
	pChar pc = dynamic_cast<pChar> object;
	pItem pi = dynamic_cast<pItem> object;
	if (pc) ShortToCharPtr(pc->getBody()->getId(), buffer + 7);	// def. on send 0x0FAB (the dyeing vat)
	if (pi) ShortToCharPtr(pi->getId(), buffer + 7);		// def. on send 0x0FAB (the dyeing vat)
}

/*!
\brief Sends Move player [packet 0x97]
\author Chronodt
\note packet 0x97
*/
void nPackets::Sent::MovePlayer::prepare()
{
	length = 2;
	buffer = new uint8_t[2];
	buffer[0] = 0x97;
	buffer[1] = direction;
}

/*!
\todo packet 0x98: all names 3d, if needed
*/

/*!
\brief This is sent to bring up a house-placing target [packet 0x99]
\author Kheru
\note packet 0x99
*/
void nPackets::Sent::TargetMulti::prepare()
{
	length = 26;
	buffer = new uint8_t[length];
	memset(buffer, 0, length);

	buffer[0] = 0x99;
	buffer[1] = 0x01; // Target type: (0x01 from server, 0x00 from client)
	LongToCharPtr(multi_serial, buffer +2);
	ShortToCharPtr(multi_id, buffer +18);
	LongToCharPtr(radius, buffer +20);
}

/*!
\todo packet 0x9a: console entry prompt, if needed
*/

/*!
\brief Sends Sell list [packet 0x9e]
\author Chronodt
\note packet 0x9e
*/
void nPackets::Sent::SellList::prepare()
{
	length = 9; 		// 9 header + (14+namelength) for each item 
	for( std::slist<pEquippable>::iterator it = items.begin(); it!= items.end(); ++it)
	{
		length+= 14 + (*it)->getCurrentName().size();
	}
	buffer = new uint8_t[length];
	buffer[0] = 0x9E;
	ShortToCharPtr(length, buffer + 1);
	LongToCharPtr(vendor->getSerial(), buffer + 3);
	ShortToCharPtr(items.size(), buffer + 7);	// number of items to be sent to gump
	uint8_t* offset = buffer + 9;
	for( std::slist<pEquippable>::iterator it = items.begin(); it!= items.end(); ++it)
	{
		LongToCharPtr((*it)->getSerial(), offset);
		ShortToCharPtr((*it)->getId(), offset + 4);
		ShortToCharPtr((*it)->getColor(), offset + 6);
		ShortToCharPtr((*it)->getAmount(), offset + 8);
		uint16_t value=(*it)->value;
		value = (*it)->calcValue(value);
		if ( nSettings::Server::isEnabledTradeSystem() )
			value=calcGoodValue(npc, *it,value,1); // by Magius(CHE)
		ShortToCharPtr(value, offset + 10);
		ShortToCharPtr((*it)->getCurrentName().size(), offset + 12);
		strcpy(buffer + 14, (*it)->getCurrentName().c_str());
		offset+=14 + (*it)->getCurrentName().size();
	}
}


/*!
\brief Sends new Hp value of a char to client [packet 0xa1]
\author Chronodt
\note packet 0xa1
*/
void nPackets::Sent::UpdateHp::prepare()
{
	length = 9;
	buffer = new uint8_t[9];
	buffer[0] = 0xA1;
	LongToCharPtr(pc->getSerial(), buffer + 1);
	ShortToCharPtr(pc->getMaxHp(), buffer + 5);
	ShortToCharPtr(pc->getHp(), buffer + 7);
}

/*!
\brief Sends new Mana value of a char to client [packet 0xa2]
\author Chronodt
\note packet 0xa2
*/
void nPackets::Sent::UpdateMana::prepare()
{
	length = 9;
	buffer = new uint8_t[9];
	buffer[0] = 0xA2;
	LongToCharPtr(pc->getSerial(), buffer + 1);
	ShortToCharPtr(pc->getMaxMana(), buffer + 5);
	ShortToCharPtr(pc->getMana(), buffer + 7);
}

/*!
\brief Sends new Stamina value of a char to client [packet 0xa3]
\author Chronodt
\note packet 0xa3
*/
void nPackets::Sent::UpdateStamina::prepare()
{
	length = 9;
	buffer = new uint8_t[9];
	buffer[0] = 0xA3;
	LongToCharPtr(pc->getSerial(), buffer + 1);
	ShortToCharPtr(pc->getMaxStamina(), buffer + 5);
	ShortToCharPtr(pc->getStamina(), buffer + 7);
}

/*!
\brief Opens web browser [packet 0xa5]
\author Flameeyes
\note packet 0xa5
*/
void nPackets::Sent::OpenBrowser::prepare()
{
	length = url.size() + 3;
	buffer = new uint8_t[length];
	buffer[0] = 0xA5;
	ShortToCharPtr(length, buffer+1);
	memcpy(buffer+3, url.c_str(), length-3);
}

/*!
\brief Send Tips or MOTD/Server notice window [packet 0xa6]
\author Kheru
\note Packet 0xA6
*/
void nPackets::Sent::TipsWindow::prepare()
{
	uint16_t msg_size = message.size();
	length = msg_size + 10;
	buffer = new uint8_t[length];

	buffer[0] = 0xA6;
	ShortToCharPtr(length, buffer +1);
	buffer[3] = type;
	ShortToCharPtr(0x0000, buffer +4);
	ShortToCharPtr(tip_num, buffer +6);
	ShortToCharPtr(msg_size, buffer +8);
	memcpy(buffer, message.c_str(), msg_size);
}

/*!
\todo packet 0xa8: Game Server List... login packet (strictly linked with Login2 in network.cpp and a global variable)
*/

/*!
\todo packet 0xa9: Characters / Starting Locations... login packet (strictly linked with GoodAuth in network.cpp and a global variable)
*/

/*!
\brief Sends attack request reply [packet 0xaa]
\author Flameeyes
\note packet 0xaa
*/
void nPackets::Sent::AttackAck::prepare()
{
	length = 5;
	buffer = new buffer[5];
	buffer[0] = 0xAA;
	if ( victim )
		LongToCharPtr(victim->getSerial(), buffer+1);
	else
		LongToCharPtr(0, buffer+1);
}

/*!
\brief Opens Gump Text Entry Dialog [packet 0xab]
\author Chronodt
\note packet 0xab
*/

void nPackets::Sent::OpenTextEntryDialog::prepare()
{
	//! \todo this function (awaiting gump remake)
}



void nPackets::Sent::CharProfile::prepare()
{
// packet documentation is sketchy at best. Expect many implemetation errors here -_-
	length = 7;
	length += who->getCurrentName().size() + 1;	//null terminator included
	length += who->getTitle().size() * 2;		//unicode title, so double characters of title
	length +=2; //unicode null terminator for title
	length += who->getProfile().size();
	length +=2; //unicode null terminator for profile
	buffer = new uint8_t[length];
	buffer[0] = 0xb8;
	ShortToCharPtr(length, buffer + 1);
	LongToCharPtr(serial, buffer + 3);
	char* offset = buffer +7;
	strcpy(offset, who->getCurrentName().c_str());	// Copy charname as a null-terminated string
	cSpeech title = cSpeech(who->getTitle());		// Using cSpeech string constructor
	title.setPacketByteOrder();				// "unicode" conversion of title
	offset += who->getCurrentName().size() + 1;
	// here we cannot use wchar version of strcpy (wcscpy) because wchar is not guaranteed to be the 16bit char that uo protocol requires
	// so we use memcpy (to copy even the "0" higher bytes)
	memcpy(offset, title.rawBytes(), title.size() * 2 + 2);		// the "+2" guarantees the copy of the 16 bit null terminator
	offset += title.size() * 2 + 2;
	memcpy(offset, who->getProfile().rawBytes(), who->getProfile().size() * 2 + 2);
}

void nPackets::Sent::Features::prepare()
{
	buffer = new uint8_t[3];
	length = 2;

	buffer[0] = 0xB9;
	ShortToCharPtr(features, buffer+1);
}


void nPackets::Sent::ClientViewRange::prepare()
{
	buffer = new uint8_t[2];
	length = 2;
	buffer[0] = 0xc8;
	buffer[1] = range;
}


void nPackets::Sent::LogoutStatus::prepare()
{
	buffer = new uint8_t[2];
	length = 2;
	buffer[0] = 0xd1;
	buffer[1] = 0x01;
}
