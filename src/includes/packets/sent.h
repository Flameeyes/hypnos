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

	uint32_t serial;
	uint16_t id;
	uint16_t amount;
	uint16_t x, y;
	uint16_t color;
};

/*!
\author Flameeyes
\brief Packet to be sent
*/
class cPacketSend
{
protected:
	uint8_t *buffer;	//!< Pointer to the buffer
	uint16_t length;	//!< Length of the buffer

public:
	inline virtual ~cPacketSend
	{ if ( buffer ) safedeletearray(buffer); }

	//! Prepare the buffer to be sent
	virtual void prepare() = 0;

	//! Prepare the packet if not prepared and return the buffer
	inline const uint8_t *getBuffer()
	{
		if ( ! length )
			prepare();

		return buffer;
	}

	//! Prepare the packet if not prepared and return the length of the buffer
	inline const uint16_t getLength()
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
	const uint32_t serial;	//!< Serial of the char which do the action
	const uint16_t action;	//!< id of the action to execute

public:
	/*!
	\param s serial of the char which do the action
	\param a id of the action to execute
	*/
	inline cPacketSendAction(uint32_t s, uint16_t a) :
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
	const uint32_t serial;	//!< Serial of the container to draw
	const uint16_t gump;	//!< Gump of the container to draw

public:
	/*!
	\param s serial of the container
	\param g gump of the container
	*/
	inline cPacketSendDrawContainer(uint32_t s, uint16_t g) :
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
	inline cPacketSendContainerItem(uint32_t s)

	/*!
	\brief add an item to the list of items in the container
	\param item item to add to the container
	*/
	inline void addItem(pItem item)
	{ list.push_back(sContainerItem(item)); }

	void prepare();
};

//! Add item to container
//! \note packet has to be sent AFTER the real moving of the item. This only tells the client where it has gone to
class cPacketSendAddItemtoContainer : cPacketSend
{
protected:
	pItem item;
public:
	/*!
	\param itm item to add
	\param cont serial of container item
	*/
	inline cPacketSendAddItemtoContainer(pItem itm) :
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
	inline cPacketSendWornItem(pItem itm) :
		item(itm)
	{ }

	void prepare();
};

//! Sound FX
class cPacketSendSoundFX : cPacketSend
{
protected:
	uint16_t model;	//!< Sound model
	Location loc;	//!< Location where the sound is played
public:
	/*!
	\param m sound model
	\param l where the sound will be played
	*/
	inline cPacketSendSoundFX(uint16_t m, Location l) :
		model(m), loc(l)
	{ }

	void prepare();
};

//! Delete object
class cPacketSendDeleteObj : cPacketSend
{
protected:
	uint32_t serial;
public:
	/*!
	\param s serial of the object to remove
	*/
	inline cPacketSendDeleteObj(uint32_t s) :
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
	uint16_t skill;
public:
	/*!
	\param c Character to send the skill of
	\param sk Skill to update
	*/
	inline cPacketSendUpdateSkill(pChar c, uint16_t sk) :
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
	uint16_t id;
public:
	/*!
	\param midi Midi file id
	*/
	inline cPacketSendPlayMidi(uint16_t midi) :
		id(midi)
	{ }

	void prepare();
};

//! Overall Light Level
class cPacketSendOverallLight : cPacketSend
{
protected:
	uint8_t level;
public:
	/*!
	\param l Light level
	*/
	inline cPacketSendOverallLight(uint8_t l) :
		level(l)
	{ }

	void prepare();
};

//! Status window
class cPacketSendStatus : cPacketSend
{
protected:
	pChar pc;	//!< Character
	uint8_t type;	//!< Type
	bool canrename;	//!< Can be renamed
public:
	/*!
	\param p Character
      	\param t type of window
        \param canrename client who receives this packet can rename char p
	*/
	inline cPacketSendStatus(pChar p, uint8_t t, bool r) :
		pc(p), type(t), canrename(r)
	{ }

	void prepare();
	void prepare1();
	void prepare3();
	void prepare4();
};

/*!
\brief Packet to confirm processing of buy (or sell) window
\author Chronodt
*/

class cPacketSendClearBuyWindow : cPacketSend
{
protected:
	const pNpc npc;	        //!< Vendor

public:
	/*!
	\param n npc vendor
	*/
	inline cPacketSendClearBuyWindow(pNpc n) :
		npc(n),
		buffer(NULL), length(NULL)
	{ }

	void prepare();
};

/*!
\brief Packet to warn client that something has been added to pc's clothing
\author Chronodt
*/

class cPacketSendPaperdollClothingUpdated : cPacketSend
{
public:
	inline cPacketSendPaperdollClothingUpdated() :
		buffer(NULL), length(NULL)
	{ }

	void prepare();
};

/*!
\brief Opens map gump with data from map
\author Chronodt
*/

class cPacketSendOpenMapGump : cPacketSend
{
protected:

	const pMap map;
public:
	/*!
	\param m map
	*/
	inline cPacketSendOpenMapGump(pMap m) :
        	map (m),
		buffer(NULL), length(NULL)
	{ }

	void prepare();
};



/*!
command:
1 = add map point
2 = add new pin with pin number. (insertion. other pins after the number are pushed back.)
3 = change pin
4 = remove pin
5 = remove all pins on the map
6 = toggle the 'editable' state of the map.
7 = return msg from the server to the request 6 of the client.
pin:
if command is 7, it is plotting state (1=on, 0=off)
if command is 2, it is the pin number that the new pin must have (all others must be "pushed onward" :D

Apparently you can have no more than 50 pins in a map, and it appears a client side limitation (and it appears really messy if you put them all :D)
*/


//!the following enum is used for both incoming and outgoing 0x56 packet

enum PlotCourseCommands (AddPin = 1, InsertPin, ChangePin, RemovePin, ClearAllPins, ToggleWritable, WriteableStatus);

class cPacketSendMapPlotCourse : cPacketSend
{
protected:

	const pMap map;
        const PlotCourseCommands command;
        const short int pin;
        const int x;
        const int y;
public:
	/*!
	\param m map used
	\param comm command to be sent to client
        \param p pin modified (or writeability status if comm == 6)
        \param xx x position of pin (map relative)
        \param yy y position of pin (map relative)
	*/
	inline cPacketSendMapPlotCourse(pMap m, PlotCourseCommands comm, short int p = 0, int xx = 0, int yy = 0) :
        	map (m), command (comm), pin (p),  x (xx), y (yy),
		buffer(NULL), length(NULL)
	{ }

	void prepare();
};

enum BBoardCommands (DisplayBBoard = 0, SendMessageSummary = 1, SendMessageBody = 2);
class cPacketSendBBoardCommand : cPacketSend
{
protected:

	const pMsgBoard msgboard;
        const BBoardCommands command;
        const pMsgBoardMessage message;

public:
	/*!
	\param m msgboard used
	\param com command for the msgboard
        \param mess message to be sent. May be omitted if command is DisplayBBoard
	*/
	inline cPacketSendMapPlotCourse(pMsgBoard m, BBoardCommands com, pMsgBoardMessage mess = NULL) :
        	msgboard (m), command(com), message(mess),
		buffer(NULL), length(NULL)
	{ }

	void prepare();
};


class cPacketSendMsgBoardItemsinContainer : cPacketSend
{
protected:

	const pMsgBoard msgboard;

public:
	/*!
        \note constructor for msgboards
	\param m msgboard used. This is called only on msgboard first opening
	*/
	inline cPacketSendMsgBoardItemsinContainer(pMsgBoard m) :
        	msgboard (m),
		buffer(NULL), length(NULL)
	{ }

	void prepare();
};

class cPacketSendSecureTradingStatus : cPacketSend
{
protected:

        const uint8_t action;
        const uint32_t id1,id2,id3;

public:
	inline cPacketSendSecureTradingStatus(uint8_t act, uint32_t i1, uint32_t i2, uint32_t i3) :
        	action(act), id1(i1), id2(i2), id3(i3),
		buffer(NULL), length(NULL)
	{ }

	void prepare();
};


class cPacketSendUpdatePlayer : cPacketSend
{
protected:

	const pChar chr;
        const uint8_t dir, flag, hi_color;
        const uint32_t id1,id2,id3;

public:
	inline cPacketSendUpdatePlayer(pChar pc, uint8_t newdir, uint8_t newflag, uint8_t newhi_color) :
        	chr(pc), dir(newdir), flag(newflag), hi_color(newhi_color),
		buffer(NULL), length(NULL)
	{ }

	void prepare();
};

class cPacketSendWarModeStatus : cPacketSend
{
protected:

        uint8_t buf[5];

public:
	inline cPacketSendWarModeStatus(uint8_t* buffer) :
		buffer(NULL), length(NULL)
	{ memcpy(buf, buffer, 5);}

	void prepare();
};


class cPacketSendPingReply : cPacketSend
{
protected:

        uint8_t buf[2];

public:
	inline cPacketSendPingReply(uint8_t* buffer) :
		buffer(NULL), length(NULL)
	{ memcpy(buf, buffer, 2);}

	void prepare();
};


/*!
\brief Packet received
\author Flameeyes
*/
class cPacketReceive
{
protected:

        uint8_t *buffer;           // needed in all derived classes
        uint16_t length;
public:
//	cPacketReceive();
      	inline cPacketReceive(uint8_t *buf, uint16_t len) :
		buffer(buf), length(len)
	{ } 
//	~cPacketReceive();
	static pPacketReceive fromBuffer(uint8_t *buffer, uint16_t length);
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
class cPacketReceiveTalkRequest         : public cPacketReceive;
class cPacketReceiveAttackRequest       : public cPacketReceive;
class cPacketReceiveDoubleclick         : public cPacketReceive;
class cPacketReceivePickUp              : public cPacketReceive;
class cPacketReceiveDropItem            : public cPacketReceive;
class cPacketReceiveSingleclick         : public cPacketReceive;
class cPacketReceiveActionRequest       : public cPacketReceive;
class cPacketReceiveWearItem            : public cPacketReceive;
class cPacketReceiveResyncRequest       : public cPacketReceive;
class cPacketReceiveRessChoice          : public cPacketReceive;
class cPacketReceiveStatusRequest       : public cPacketReceive;
class cPacketReceiveSetSkillLock        : public cPacketReceive;
class cPacketReceiveBuyItems            : public cPacketReceive;
class cPacketReceiveMapPlotCourse       : public cPacketReceive;
class cPacketReceiveLoginChar       	: public cPacketReceive;
class cPacketReceiveBookPage		: public cPacketReceive;
class cPacketReceiveTargetSelected      : public cPacketReceive;
class cPacketReceiveBBoardMessage       : public cPacketReceive;
class cPacketReceiveSecureTrade         : public cPacketReceive;
class cPacketReceiveWarModeChange       : public cPacketReceive;
class cPacketReceivePing	        : public cPacketReceive;
class cPacketReceiveRenameCharacter     : public cPacketReceive;
class cPacketReceiveDialogResponse      : public cPacketReceive;
class cPacketReceiveLoginRequest        : public cPacketReceive;
class cPacketReceiveDeleteCharacter     : public cPacketReceive;
class        : public cPacketReceive;
class        : public cPacketReceive;
class        : public cPacketReceive;
class        : public cPacketReceive;
class        : public cPacketReceive;
class        : public cPacketReceive;
class        : public cPacketReceive;
class        : public cPacketReceive;
#endif
