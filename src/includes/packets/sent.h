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

#include "common_libs.h"
#include "enums.h"
#include "structs.h"

/*!
\author Flameeyes
\brief Packet to be sent
*/
class cPacketSend
{
protected:
	uint8_t *buffer;	//!< Pointer to the buffer
	uint16_t length;	//!< Length of the buffer
	
	cPacketSend(uint8_t *aBuffer, uint16_t aLenght)
	{ buffer = aBuffer; length = aLenght; }

public:
	inline virtual ~cPacketSend()
	{ if ( buffer ) delete[] buffer; }

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

	virtual void fixForClient(ClientType ct) = 0;
};

/*!
\brief cChar::action() packet
\author Flameeyes
*/
class cPacketSendAction : public cPacketSend
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
		cPacketSend(NULL, 0), serial(s), action(a)
	{ }

	void prepare();
};

/*!
\brief Draws a container on the user script
\author Flameeyes
*/
class cPacketSendDrawContainer : public cPacketSend
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
		cPacketSend(NULL, 0), serial(s), gump(g)
	{ }

	void prepare();
};

/*!
\brief Send an item in container
\author Flameeyes
*/
class cPacketSendContainerItem : public cPacketSend
{
protected:
	std::list<sContainerItem> items;
	uint32_t contSerial;
public:
	/*!
	\param s serial of the container (for all items)
	*/
	inline cPacketSendContainerItem(uint32_t s) :
		cPacketSend(NULL, 0), contSerial(s)
	{ }

	/*!
	\brief add an item to the list of items in the container
	\param item item to add to the container
	*/
	inline void addItem(pItem item)
	{ items.push_back(sContainerItem(item)); }

	void prepare();
};

//! Add item to container
//! \note packet has to be sent AFTER the real moving of the item. This only tells the client where it has gone to
class cPacketSendAddItemtoContainer : public cPacketSend
{
protected:
	pItem item;
public:
	/*!
	\param itm item to add
	\param cont serial of container item
	*/
	inline cPacketSendAddItemtoContainer(pItem itm) :
		cPacketSend(NULL, 0), item(itm)
	{ }

	void prepare();
};

//! Work item
class cPacketSendWornItem : public cPacketSend
{
protected:
	pItem item;
public:
	inline cPacketSendWornItem(pItem itm) :
		cPacketSend(NULL, 0), item(itm)
	{ }

	void prepare();
};

//! Sound FX
class cPacketSendSoundFX : public cPacketSend
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
		cPacketSend(NULL, 0), model(m), loc(l)
	{ }

	void prepare();
};

//! Delete object
class cPacketSendDeleteObj : public cPacketSend
{
protected:
	uint32_t serial;
public:
	/*!
	\param s serial of the object to remove
	*/
	inline cPacketSendDeleteObj(uint32_t s) :
		cPacketSend(NULL, 0), serial(s)
	{ }

	void prepare();
};

//! Send skill status
class cPacketSendSkillState : public cPacketSend
{
protected:
	pChar pc;
public:
	/*!
	\param c Character to send the skill of
	*/
	inline cPacketSendSkillState(pChar c) :
		cPacketSend(NULL, 0), pc(c)
	{ }

	void prepare();
};

//! Update a skill
class cPacketSendUpdateSkill : public cPacketSend
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
		cPacketSend(NULL, 0), pc(c), skill(sk)
	{ }

	void prepare();
};

//! Open Web Browser
class cPacketSendOpenBrowser : public cPacketSend
{
protected:
	std::string url;
public:
	/*!
	\param url Url to open the browser to
	*/
	inline cPacketSendOpenBrowser(std::string str) :
		cPacketSend(NULL, 0), url(str)
	{ }

	void prepare();
};

//! Play midi file
class cPacketSendPlayMidi : public cPacketSend
{
protected:
	uint16_t id;	//!< ID of the MIDI to play
public:
	/*!
	\param midi Midi file id
	*/
	inline cPacketSendPlayMidi(uint16_t midi) :
		cPacketSend(NULL, 0), id(midi)
	{ }

	void prepare();
};

//! Overall Light Level
class cPacketSendOverallLight : public cPacketSend
{
protected:
	uint8_t level;	//!< Light level to send
public:
	/*!
	\param l Light level
	*/
	inline cPacketSendOverallLight(uint8_t l) :
		cPacketSend(NULL, 0), level(l)
	{ }

	void prepare();
};

//! Status window
class cPacketSendStatus : public cPacketSend
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
		cPacketSend(NULL, 0), pc(p), type(t), canrename(r)
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

class cPacketSendClearBuyWindow : public cPacketSend
{
protected:
	const pNPC npc;	        //!< Vendor

public:
	/*!
	\param n npc vendor
	*/
	inline cPacketSendClearBuyWindow(pNPC n) :
		cPacketSend(NULL, 0), npc(n)
	{ }

	void prepare();
};

/*!
\brief Packet to warn client that something has been added to pc's clothing
\author Chronodt
*/

class cPacketSendPaperdollClothingUpdated : public cPacketSend
{
public:
	inline cPacketSendPaperdollClothingUpdated() :
		cPacketSend(NULL, 0)
	{ }

	void prepare();
};

/*!
\brief Opens map gump with data from map
\author Chronodt
*/

class cPacketSendOpenMapGump : public cPacketSend
{
protected:

	const pMap map;	//!< Map's gump
public:
	/*!
	\param m map
	*/
	inline cPacketSendOpenMapGump(pMap m) :
        	cPacketSend(NULL, 0), map (m)
	{ }

	void prepare();
};

enum PlotCourseCommands {
	pccAddPin = 1,		//!< Add map poin
	pccInsertPin,		//!< Add new pin with pin number (insertion. other pins after the number are pushed back.)
	pccChangePin,		//!< Change pin
	pccRemovePin,		//!< Remove pin
	pccClearAllPins,	//!< Remove all pins on the map
	pccToggleWritable,	//!< Toggle the 'editable' state of the map
	pccWriteableStatus	//!< Return message from the server to request 6 of the client
};

class cPacketSendMapPlotCourse : public cPacketSend
{
protected:
	const pMap map;
        const PlotCourseCommands command;
	
	/*!
	if command is pccWriteableStatus, it is plotting state (1=on, 0=off)
	if command is pccInsertPin, it is the pin number that the new pin must have (all others must be "pushed onward")
	
	Apparently you can have no more than 50 pins in a map, and it appears a client side limitation (and it appears really messy if you put them all :D)
	*/
        const uint16_t pin;
        const uint32_t x;
        const uint32_t y;
public:
	/*!
	\param m map used
	\param comm command to be sent to client
        \param p pin modified (or writeability status if comm == 6)
        \param xx x position of pin (map relative)
        \param yy y position of pin (map relative)
	*/
	inline cPacketSendMapPlotCourse(pMap m, PlotCourseCommands comm, uint16_t p = 0, uint32_t xx = 0, uint32_t yy = 0) :
        	cPacketSend(NULL, 0), map (m), command (comm), pin (p),  x (xx), y (yy)
	{ }

	void prepare();
};

enum BBoardCommands {bbcDisplayBBoard, bbcSendMessageSummary, bbcSendMessageBody};
class cPacketSendBBoardCommand : public cPacketSend
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
        	cPacketSend(NULL, 0), msgboard (m), command(com), message(mess)
	{ }

	void prepare();
};


class cPacketSendMsgBoardItemsinContainer : public cPacketSend
{
protected:

	const pMsgBoard msgboard;

public:
	/*!
        \note constructor for msgboards
	\param m msgboard used. This is called only on msgboard first opening
	*/
	inline cPacketSendMsgBoardItemsinContainer(pMsgBoard m) :
        	cPacketSend(NULL, 0), msgboard (m)
	{ }

	void prepare();
};

class cPacketSendSecureTradingStatus : public cPacketSend
{
protected:

        const uint8_t action;
        const uint32_t id1,id2,id3;

public:
	inline cPacketSendSecureTradingStatus(uint8_t act, uint32_t i1, uint32_t i2, uint32_t i3) :
        	cPacketSend(NULL, 0), action(act), id1(i1), id2(i2), id3(i3)
	{ }

	void prepare();
};


class cPacketSendUpdatePlayer : public cPacketSend
{
protected:
	pChar chr;
	uint8_t dir, flag, hi_color;
	uint32_t id1,id2,id3;

public:
	inline cPacketSendUpdatePlayer(pChar pc, uint8_t newdir, uint8_t newflag, uint8_t newhi_color) :
        	cPacketSend(NULL, 0), chr(pc), dir(newdir), flag(newflag), hi_color(newhi_color)
	{ }

	void prepare();
};

class cPacketSendWarModeStatus : public cPacketSend
{
protected:

        uint8_t buf[5];

public:
	inline cPacketSendWarModeStatus(uint8_t* buffer) :
		cPacketSend(NULL, 0)
	{ memcpy(buf, buffer, 5);}

	void prepare();
};


class cPacketSendPingReply : public cPacketSend
{
protected:

        uint8_t buf[2];

public:
	inline cPacketSendPingReply(uint8_t* buffer) :
		cPacketSend(NULL, 0)
	{ memcpy(buf, buffer, 2);}

	void prepare();
};

class cPacketSendCharDeleteError : public cPacketSend
{
protected:
	uint8_t reason;

public:
	inline cPacketSendCharDeleteError(uint8_t r) :
        	cPacketSend(NULL, 0), reason(r)
	{ }

	void prepare();
};

class cPacketSendCharAfterDelete : public cPacketSend
{
protected:
	pAccount account;

public:
	inline cPacketSendCharAfterDelete(pAccount a) :
        	cPacketSend(NULL, 0), account(a)
	{ }

	void prepare();
};

class cPacketSendCharProfile : public cPacketSend
{
protected:
	uint32_t serial;
        pChar who;

public:
	inline cPacketSendCharProfile(uint32_t s, pChar w) :
        	serial(s), who(w),
		buffer(NULL), length(NULL)
	{ }

	void prepare();
};


class cPacketSendLogoutStatus : public cPacketSend
{
protected:

public:
	inline cPacketSendLogoutStatus() :
		buffer(NULL), length(NULL)
	{ }

	void prepare();
};

class cPacketSendClientViewRange : public cPacketSend
{
protected:
	uint8_t range;
public:
	inline cPacketSendClientViewRange(uint8_t r) :
        	range(r),
		buffer(NULL), length(NULL)
	{ }

	void prepare();
};

class cPacketSendMoveAcknowdledge : public cPacketSend
{
protected:
	uint8_t sequence;
	uint8_t notoriety;
public:
	inline cPacketSendMoveAcknowdledge(uint8_t s, uint8_t n) :
        	sequence(s), notoriety(n),
		buffer(NULL), length(NULL)
	{ }

	void prepare();
};


/*!
\brief Packet received
\author Flameeyes
*/
class cPacketReceive
{
protected:

        uint8_t *buffer;	//!< Pointer to the buffer (needed in all derived classes)
        uint16_t length;	//!< Length of the buffer
public:
      	inline cPacketReceive(uint8_t *buf, uint16_t len) :
		buffer(buf), length(len)
	{ } 
	
	virtual ~cPacketReceive() = 0;
	
	static pPacketReceive fromBuffer(uint8_t *buffer, uint16_t length);
	virtual bool execute(pClient client) = 0;
};

//@{
/*!
\name Packet Reception
\author Chronodt
*/
class cPacketReceiveCreateChar          : public cPacketReceive;	//!< Character creation
class cPacketReceiveDisconnectNotify    : public cPacketReceive;	//!< "software disconnect"
class cPacketReceiveMoveRequest         : public cPacketReceive;	//!< move (or run) 1 step
class cPacketReceiveTalkRequest         : public cPacketReceive;	//!< character speech (NOT UNICODE!!)
class cPacketReceiveAttackRequest       : public cPacketReceive;        //!< Clients requests an armed/unarmed attack request against someone
class cPacketReceiveDoubleclick         : public cPacketReceive;	//!< Use of item or info on char
class cPacketReceivePickUp              : public cPacketReceive;	//!< trying to pick up an item
class cPacketReceiveDropItem            : public cPacketReceive;	//!< releasing mouse button and dropping item dragged. Position check too
class cPacketReceiveSingleclick         : public cPacketReceive;	//!< info/name request for item/char
class cPacketReceiveActionRequest       : public cPacketReceive;	//!< skill/magic use (spells from macros i believe) and generic action, like salute or bow
class cPacketReceiveWearItem            : public cPacketReceive;	//!< drag of item on paperdoll. Check if equippable too
class cPacketReceiveMoveACK_ResyncReq   : public cPacketReceive;	//!< move acknowdlege or if it sends a 0x220000, resends data to client
class cPacketReceiveRessChoice          : public cPacketReceive;	//!< once it was used for a ress choice(ghost or revive with skill penalties?) now only used for bounty placement
class cPacketReceiveStatusRequest       : public cPacketReceive;	//!< statusbar request about a char
class cPacketReceiveSetSkillLock        : public cPacketReceive;	//!< skill lock change (up, down, lock)
class cPacketReceiveBuyItems            : public cPacketReceive;	//!< receive list of items selected from a buy list of vendor
class cPacketReceiveMapPlotCourse       : public cPacketReceive;	//!< pin change for a course on map. Also all pin-related action on map
class cPacketReceiveLoginChar       	: public cPacketReceive;
class cPacketReceiveBookPage		: public cPacketReceive;        //!< receives a changed book page
class cPacketReceiveTargetSelected      : public cPacketReceive;        //!< target selected (for whatever asked for it :D)
class cPacketReceiveSecureTrade         : public cPacketReceive;	//!< secure trade conclusion (change in the secure trade window's checkmark)
class cPacketReceiveBBoardMessage       : public cPacketReceive;	//!< almost all msgboard activity except opening the board itself comes here
class cPacketReceiveWarModeChange       : public cPacketReceive;        //!< warmode change request
class cPacketReceivePing	        : public cPacketReceive;        //!< Ping of client. Obiously server answers :D
class cPacketReceiveRenameCharacter     : public cPacketReceive;	//!< client tries to rename a character
class cPacketReceiveDialogResponse      : public cPacketReceive;
class cPacketReceiveLoginRequest        : public cPacketReceive;
class cPacketReceiveDeleteCharacter     : public cPacketReceive;        //!< client tries to delete a char from login window
class cPacketReceiveGameServerLogin     : public cPacketReceive;
class cPacketReceiveBookUpdateTitle     : public cPacketReceive;	//!< change title of book
class cPacketReceiveDyeItem             : public cPacketReceive;	//!< dye item (opens dye dialog)
class cPacketReceiveRequestHelp	        : public cPacketReceive;        //!< gm/conselor page request
class cPacketReceiveSellItems           : public cPacketReceive;        //!< receive list of items selected from a sell list of vendor
class cPacketReceiveSelectServer        : public cPacketReceive;
class cPacketReceiveTipsRequest         : public cPacketReceive;
class cPacketReceiveGumpTextDialogReply : public cPacketReceive;
class cPacketReceiveUnicodeSpeechReq    : public cPacketReceive;	//!< unicode speech request from client
class cPacketReceiveGumpResponse        : public cPacketReceive;
class cPacketReceiveChatMessage       	: public cPacketReceive;
class cPacketReceiveChatWindowOpen      : public cPacketReceive;
class cPacketReceivePopupHelpRequest    : public cPacketReceive;
class cPacketReceiveCharProfileRequest  : public cPacketReceive;	//!< update or asks for character profile and title (char profile is the one on the paperdoll's scroll)
class cPacketReceiveClientVersion       : public cPacketReceive;        //!< during login, client sends its version
class cPacketReceiveAssistVersion       : public cPacketReceive;
class cPacketReceiveMiscCommand         : public cPacketReceive;	//!< multipurpouse packet. It ranges from party commands to fastwalk prevention, gumps and menu
class cPacketReceiveTextEntryUnicode    : public cPacketReceive;
class cPacketReceiveClientViewRange     : public cPacketReceive;	//!< Client sends its (wanted) view area in "squares" (from 5 to 18). If server replyes with same packet, it gets activated
class cPacketReceiveLogoutStatus        : public cPacketReceive;	//!< if client receives a certain flag on packet 0xa9 on login, sends this packet when logging out and expects a reply before disconnecting
class cPacketReceiveNewBookHeader       : public cPacketReceive;	//!< more up-to-date book header communication (only difference found: variable-length title and author)
class cPacketReceiveFightBookSelection  : public cPacketReceive;	//!< fightbook icon selection
//@}

#endif

