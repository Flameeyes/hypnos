/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Declaration of packets classes
*/

#ifndef __CPACKET_H__
#define __CPACKET_H__

class cPacketSend;
typedef cPacketSend *pPacketSend;

class cPacketReceive;
typedef cPacketReceive *pPacketReceive;

#include "cclient.h"

/*!
\author Flameeyes
\brief Packet to be sent
*/
class cPacketSend
{
protected:
	UI08 *buffer;	//!< Pointer to the buffer
	UI16 length;	//!< Length of the buffer

public:
	inline virtual ~cPacketSend
	{ if ( buffer ) delete buffer; }

	//! Prepare the buffer to be sent
	virtual void prepare() = 0;

	//! Prepare the packet if not prepared and return the buffer
	inline const UI08 *getBuffer()
	{
		if ( ! length )
			prepare();

		return buffer;
	}

	//! Prepare the packet if not prepared and return the length of the buffer
	inline const UI16 getLength()
	{
		if ( ! length )
			prepare();

		return length;
	}

	virtual void fixForClient(cClient::ClientType ct)
	{ }

private:
	cPacketSend();
};

/*!
\brief cChar::action() packet
\author Flameeyes
*/
class cPacketSendAction
{
protected:
	const UI32 serial;	//!< Serial of the char which do the action
	const UI16 action;	//!< id of the action to execute

public:
	/*!
	\param s serial of the char which do the action
	\param a id of the action to execute
	*/
	inline cPacketSendAction(UI32 s, UI16 a) :
		serial(s), action(a),
		buffer(NULL), length(NULL)
	{ }

	void prepare();
};

/*!
\brief Draws a container on the user script
\author Flameeyes
*/
class cPacketSendDrawContainer
{
protected:
	const UI32 serial;	//!< Serial of the container to draw
	const UI16 gump;	//!< Gump of the container to draw

public:
	/*!
	\param s serial of the container
	\param g gump of the container
	*/
	inline cPacketSendDrawContainer(UI32 s, UI16 g) :
		serial(s), gump(g),
		buffer(NULL), length(NULL)
	{ }

	void prepare();
};

/*!
\brief Send an item in container
\author Flameeyes
*/
class cPacketSendContainerItem
{
protected:
	struct sContainerItem
	{
		UI32 serial;
		UI16 id;
		UI16 amount;
		UI16 x, y;
		UI16 color;
	};
	std::list<sContainerItem> items;
public:
	/*!
	\param s serial of the container (for all items)
	*/
	inline cPacketSendContainerItem(UI32 s)

	/*!
	\brief add an item to the list of items in the container
	\param item item to add to the container
	*/
	inline void addItem(pItem *item)
	{
		sContainerItem it;
		it.serial	= item->getSerial();
		it.id		= item->getAnimId();
		it.amount	= item->getAmount();
		it.x		= item->getPosition().x;
		it.y		= item->getPosition().y;
		it.color	= item->getColor();
		list.push_back(it);
	}

	void prepare();
}

/*!
\brief Sound FX
\author Flameeyes
*/
class cPacketSendSoundFX
{
protected:
	UI16 model;	//!< Sound model
	Location loc;	//!< Location where the sound is played
public:
	/*!
	\param m sound model
	\param l where the sound will be played
	*/
	inline cPacketSendSoundFX(UI16 m, Location l) :
		model(m), loc(l)
	{ }

	void prepare();
}

/*!
\brief Packet received
\author Flameeyes
*/
class cPacketReceive
{
protected:
	cPacketReceive();

public:
	~cPacketReceive();
	static pPacketReceive fromBuffer(UI08 *buffer, UI16 length);
	inline virtual void execute(pClient client)
	{ }
};

#endif