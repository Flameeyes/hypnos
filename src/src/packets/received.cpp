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
	LongToCharPtr(item->getContainr()->getSerial(), ptrItem+14);
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
	LongToCharPtr(item->getContainr()->getSerial(), buffer+9);
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
	ShortTOCharPtr(id, buffer+1);
}

void cPacketSendOverallLight::prepare()
{
	length = 2;
	buffer = new UI08[2];

	buffer[0] = 0x4F;
	buffer[1] = level;
}
