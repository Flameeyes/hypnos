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

//! Item in a container
struct sContainerItem
{
	sContainerItem() { }
	sContainerItem(pItem item)
	{
		it.serial	= item->getSerial();
		it.id		= item->getAnimId();
		it.amount	= item->getAmount();
		it.x		= item->getPosition().x;
		it.y		= item->getPosition().y;
		it.color	= item->getColor();
	};

	UI32 serial;
	UI16 id;
	UI16 amount;
	UI16 x, y;
	UI16 color;
};

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
class cPacketSendAction : cPacketSend
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
class cPacketSendDrawContainer : cPacketSend
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
class cPacketSendContainerItem : cPacketSend
{
protected:
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
	inline void addItem(pItem item)
	{ list.push_back(sContainerItem(item)); }

	void prepare();
};

//! Add item to container
class cPacketSendAddContainerItem : cPacketSend
{
protected:
	pItem item;
public:
	/*!
	\param itm item to add
	\param cont serial of container item
	*/
	inline cPacketAddContainerItem(pItem itm) :
		item(itm)
	{ }

	void prepare();
};

//! Work item
class cPacketSendWornItem : cPacketSend
{
protected:
	pItem item;
public:
	inline cPacketSendAddContainerItem(pItem itm) :
		item(itm)
	{ }

	void prepare();
};

//! Sound FX
class cPacketSendSoundFX : cPacketSend
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
};

//! Delete object
class cPacketSendDeleteObj : cPacketSend
{
protected:
	UI32 serial;
public:
	/*!
	\param s serial of the object to remove
	*/
	inline cPacketSendDeleteObj(UI32 s) :
		serial(s)
	{ }

	void prepare();
};

//! Send skill status
class cPacketSendSkillState : cPacketSend
{
protected:
	pChar pc;
public:
	/*!
	\param c Character to send the skill of
	*/
	inline cPacketSendSkillState(pChar c) :
		pc(c)
	{ }

	void prepare();
};

//! Update a skill
class cPacketSendUpdateSkill : cPacketSend
{
protected:
	pChar pc;
	UI16 skill;
public:
	/*!
	\param c Character to send the skill of
	\param sk Skill to update
	*/
	inline cPacketSendUpdateSkill(pChar c, UI16 sk) :
		pc(c), skill(sk)
	{ }

	void prepare();
};

//! Open Web Browser
class cPacketSendOpenBrowser : cPacketSend
{
protected:
	std::string url;
public:
	/*!
	\param url Url to open the browser to
	*/
	inline cPacketSendOpenBrowser(std::string str) :
		url(str)
	{ }

	void prepare();
};

//! Play midi file
class cPacketSendPlayMidi : cPacketSend
{
protected:
	UI16 id;
public:
	/*!
	\param midi Midi file id
	*/
	inline cPacketSendPlayMidi(UI16 midi) :
		id(midi)
	{ }

	void prepare();
};

//! Overall Light Level
class cPacketSendOverallLight : cPacketSend
{
protected:
	UI08 level;
public:
	/*!
	\param l Light level
	*/
	inline cPacketSendOverallLight(UI08 l) :
		level(l)
	{ }

	void prepare();
};

//! Status window
class cPacketSendStatus : cPacketSend
{
protected:
	pChar pc;	//!< Character
	UI08 type;	//!< Type
	bool canrename;	//!< Can be renamed
public:
	/*!
	\brief p Character
	*/
	inline cPacketSendStatus(pChar p, UI08 t, bool r) :
		pc(p), type(t), canrename(r)
	{ }

	void prepare();
	void prepare1();
	void prepare3();
	void prepare4();
};

/*!
\brief Packet received
\author Flameeyes
*/
class cPacketReceive
{
protected:

        UI08 *buffer;           // needed in derived classes 
        UI16 length;
public:
	cPacketReceive();
      	inline cPacketReceive(UI08 *buf, UI16 len) :
		buffer(buf), length(len)
	{ } 
	~cPacketReceive();
	static pPacketReceive fromBuffer(UI08 *buffer, UI16 length);
	inline virtual bool execute(pClient client)
	{ }
};


/*!
\brief packet reception classes declarations
\author Chronodt
*/

class cPacketReceiveCreateChar          : public cPacketReceive;
class cPacketReceiveDisconnectNotify    : public cPacketReceive;
class cPacketReceiveMoveRequest         : public cPacketReceive;
#endif
