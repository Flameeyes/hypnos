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