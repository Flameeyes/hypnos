/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __CPACKET_H__
#define __CPACKET_H__

#include "common_libs.h"
#include "enums.h"
#include "structs.h"
#include "speech.h"

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
	{ delete[] buffer; }

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

	virtual void fixForClient(ClientType ct)
	{ }
};

/*!
\brief Base class for a received packet
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

namespace nPackets {
	namespace Sent {	
		/*!
		\brief Status window
		\author Flameeyes
		\note packet 0x11
		*/
		class Status : public cPacketSend
		{
		protected:
			pChar pc;	//!< Character
			uint8_t type;	//!< Type
			bool canrename;	//!< Can the characeter be renamed?
		public:
			/*!
			\param p Character
			\param t type of window
			\param r client who receives this packet can rename char p
			*/
			inline Status(pChar p, uint8_t t, bool r) :
				cPacketSend(NULL, 0), pc(p), type(t), canrename(r)
			{ }
		
			void prepare();
		};
		
		
		/*!
		\brief Sends to client info on item on the ground
		\author Chronodt
		\note packet 0x1a
		*/
		
		
		class ObjectInformation : public cPacketSend
		{
		protected:
			pItem pi;	//!< Item to send
			pPC pc;		//!< Player who sees the item
		public:
			/*!
			\param i item
			\param p pc who sees the item
			*/
			inline ObjectInformation(pItem i, pPC p) :
				cPacketSend(NULL, 0), pi(i), pc(p)
			{ }
			void prepare();
		};
		
		
		/*!
		\brief lsd - alters item already shown to client. Real item position & color are not altered, just the shown ones
		\author Chronodt
		\note packet 0x1a
		*/
		class LSDObject : public cPacketSend
		{
		protected:
			pItem pi;		//!< Item
			pPC pc;			//!< Player who sees the item
			uint16_t color;		//!< new color of item
			sLocation position;	//!< new position of item
		public:
			/*!
			\param i item
			\param p pc who sees the item
			\param c new color of item
			\param pos new position of item
			*/
			inline LSDObject(pItem i, pPC p, uint16_t c, sLocation pos) :
				cPacketSend(NULL, 0), pi(i), pc(p), color(c), position(pos)
			{ }
			void prepare();
		};
		
		/*!
		\brief Char sLocation and body type (Login confirmation)
		\author Chronodt
		\note packet 0x1b
		*/
		
		
		class LoginConfirmation : public cPacketSend
		{
		protected:
			pPC pc;			//!< Player who is logging in
		public:
			/*!
			\param p pc who is logging in
			*/
			inline LoginConfirmation(pPC p) :
				cPacketSend(NULL, 0), pc(p)
			{ }
			void prepare();
		};
		
		/*!
		\brief Send character speech (not unicode) to listener
		\author Chronodt
		\note packet 0x1c
		*/
		
		class Speech : public cPacketSend
		{
		protected:
			cSpeech speech;				//!< what ps is saying
			pSerializable ps;			//!< everything with a serial can talk... but please use some logic :P
			bool ghost;				//!< "ghostize" message (OOOoOOOoOOO :) )
		public:
			/*!
			\param s what is being told
			\param p talker. NOTE: if p is NULL, it is considered a system message
			\param ghostize speech has to be mutated to ghost speech
			*/
			inline Speech(cSpeech &s, pSerializable p = NULL, bool ghostize = false) :
				cPacketSend(NULL, 0), speech(s), ps(p), ghost(ghostize)
			{ }
			void prepare();
		};
		
		
		/*!
		\brief Delete object
		\author Flameeyes & Chronodt
		\note packet 0x1d
		*/
		class DeleteObj : public cPacketSend
		{
		protected:
			pSerializable pser;
		public:
			/*!
			\param s serializable object to remove
			*/
			inline DeleteObj(pSerializable s) :
				cPacketSend(NULL, 0), pser(s)
			{ }
		
			void prepare();
		};
		
		/*!
		\brief Draws game player (used only for client-played char)
		\author Chronodt
		\note packet 0x20
		*/
		
		class DrawGamePlayer : public cPacketSend
		{
		protected:
			pPC pc;		//! Current player
		public:
			/*!
			\param player current player
			*/
			inline DrawGamePlayer(pPC player) :
				cPacketSend(NULL, 0), pc(player)
			{ }
		
			void prepare();
		};
		
		/*!
		\brief Move reject
		\author Chronodt
		\note packet 0x21
		*/
		
		class MoveReject : public cPacketSend
		{
		protected:
			pPC pc;			//! Current player
			uint8_t sequence;       //! Sequence number of move rejected
		public:
			/*!
			\param player current player
			\param seq Sequence number to reject
			*/
			inline MoveReject(pPC player, uint8_t seq) :
				cPacketSend(NULL, 0), pc(player), sequence(seq)
			{ }
		
			void prepare();
		};
		
		/*!
		\brief Move accepted
		\author Chronodt
		\note packet 0x22
		*/
		
		class MoveAcknowdledge : public cPacketSend
		{
		protected:
			uint8_t sequence;	//! walk sequence number
			uint8_t notoriety;	//! could be how the player has to be seen (blue innocent, grey criminal, etc) not sure about it
		public:
			/*!
			\param s sequence #
			\param n notoriety (????)
			*/
		
			inline MoveAcknowdledge(uint8_t s, uint8_t n) :
				cPacketSend(NULL, 0), sequence(s), notoriety(n)
			{ }
		
			void prepare();
		};
		
		/*!
		\brief Item dragging
		\author Chronodt
		\note packet 0x23
		*/
		
		class DragItem : public cPacketSend
		{
		protected:
			pItem item; 		//!< dragged item
			sLocation destination; 	//!< where the item is dragged to
			uint16_t amount;	//!< how many items in stack
		public:
			inline DragItem(pItem aItem, sLocation aDestination, uint16_t aAmount) :
				cPacketSend(NULL, 0), item(aItem), destination(aDestination), amount(aAmount)
			{ }

			void prepare();
		};

                /*!
		\brief Open gump
		\author Chronodt
		\note packet 0x24
		*/
		class OpenGump : public cPacketSend
		{
		protected:
			uint32_t serial; 		//!< serial of gump or gump-related item/char
			uint16_t gump;			//!< gump id
		public:
			inline OpenGump(uint32_t aSerial, uint16_t aGump) :
				cPacketSend(NULL, 0), serial(aSerial), gump(aGump)
			{ }

			void prepare();
		};

		/*!
		\brief tells to the client the serial and all other data about an item in the given container. usually preceded by another package
		\author Flameeyes & Chronodt
		\note packet 0x25

		\note packet has to be sent AFTER the real moving of the item. This only tells the client where it has gone to
		*/

		class ShowItemInContainer : public cPacketSend
		{
		protected:
			pItem item;	//!< Item to add
		public:
			/*!
			\param aItem item to add
			*/
			inline ShowItemInContainer(pItem aItem) :
				cPacketSend(NULL, 0), item(aItem)
			{ }

			void prepare();
		};

		/*!
		\brief Kick client
		\author Chronodt
		\note packet 0x26

		\todo use this packet where it is needed
		*/

		class Kick : public cPacketSend
		{
		protected:
			pPC kicker;	//!< gm who performs the kick (not sure if really needed)
		public:
			/*!
			\param aKicker gm who kick this client'pg
			*/
			inline Kick(pPC aKicker) :
				cPacketSend(NULL, 0), kicker(aKicker)
			{ }

			void prepare();
		};


		/*!
		\brief Bounce item
		\author Chronodt
		\note packet 0x27
		*/

		class BounceItem : public cPacketSend
		{
		protected:
			uint8_t mode; 		//!< I dont' really know. Sometimes you send a 5, sometimes a 0....
		public:
			inline BounceItem(uint8_t aMode = 0) :
				cPacketSend(NULL, 0), mode(aMode)
			{ }

			void prepare();
		};



		/*!
		\brief Clear Square
		\author Chronodt
		\note packet 0x28
		*/
		class ClearSquare : public cPacketSend
		{
		protected:
		uint16_t x;	//!< x coordinate
		uint16_t y;	//!< y coordinate
		public:
			inline ClearSquare(uint16_t aX, uint16_t aY) :
				cPacketSend(NULL, 0), x(aX), y(aY)
			{ }

			void prepare();
		};

		/*!
		\brief Packet to warn client that something has been added to pc's clothing
		\author Chronodt
		\note packet 0x29
		*/

		class PaperdollClothingUpdated : public cPacketSend
		{
		public:
			inline PaperdollClothingUpdated() :
				cPacketSend(NULL, 0)
			{ }

			void prepare();
		};

		/*!
		\brief Mobile attributes
		\author Chronodt
		\note packet 0x2d
		*/
		class MobileAttributes : public cPacketSend
		{
		protected:
		pChar chr;		//!< character
		public:
			inline MobileAttributes(pChar aChr) :
				cPacketSend(NULL, 0), chr(aChr)
			{ }

			void prepare();
		};

		/*!
		\brief Adds item to client's paperdoll
		\author Chronodt
		\note packet 0x2e
		*/

		class WornItem : public cPacketSend
		{
		protected:
			pEquippable item;
		public:
			inline WornItem(pEquippable aItem) :
				cPacketSend(NULL, 0), item(aItem)
			{ }

			void prepare();
		};

		/*!
		\brief There is a fight going on somewhere on screen
		\author Chronodt
		\note 0x2F
		*/
		class FightOnScreen : public cPacketSend
		{
		protected:
			pChar attacker;		// attacking character
			pChar defender;		// attacked character

		public:
			inline FightOnScreen(pChar aAttacker, pChar aDefender) :
				cPacketSend(NULL, 0), attacker(aAttacker), defender(aDefender)
			{ }

			void prepare();
		};


		/*!
		\brief pause/resume client
		\author Chronodt
		\note packet 0x33
		*/

		class PauseClient : public cPacketSend
		{
		protected:
			uint8_t pausestatus;	//!< 0 = pause, 1 = resume

		public:
			inline PauseClient(uint8_t aPauseStatus) :
				cPacketSend(NULL, 0), pausestatus(aPauseStatus)
			{ }

			void prepare();
		};

		/*!
		\brief pathfinding
		\author Chronodt
		\note packet 0x38

		\note this packet should be sent at least 19 times consecutively to have any effect (unverified)
		*/

		class Pathfinding : public cPacketSend
		{
		protected:
			sLocation loc;		//!< destination
		public:
			inline Pathfinding(sLocation aLoc) :
				cPacketSend(NULL, 0), loc(aLoc)
			{ }

			void prepare();
		};


		/*!
		\brief Send skill list or updates a single skill
		\author Chronodt
		\note packet 0x3a
		*/

		class SendSkills : public cPacketSend
		{
		protected:
			pChar pc;
			uint8_t skill;
		public:
			/*!
			\param aPc Character to send the skill of
			\param aSkill skill to send. if INVALID (or omitted) sends all the skills
			*/
			inline SendSkills(pChar aPc, uint8_t aSkill = UINVALID16) :
				cPacketSend(NULL, 0), pc(aPc), skill(aSkill)
			{ }

			void prepare();
		};

		/*!
		\brief empties buy window (i.e. just like pressing the clear button on client, but this is from server side)
		\author Chronodt
		\note packet 0x3b
		*/
		class ClearBuyWindow : public cPacketSend
		{
		protected:
			const pNPC npc;	        //!< Vendor

		public:
			/*!
			\param n npc vendor
			*/
			inline ClearBuyWindow(pNPC n) :
				cPacketSend(NULL, 0), npc(n)
			{ }
			void prepare();
		};


		/*!
		\brief tells client which items are in the given container
		\author Flameeyes
		\note packet 0x3c
		*/

		class ContainerItem : public cPacketSend
		{
		protected:
			std::list<sContainerItem> items;
		public:
			/*!
			\param aContainer the container (for all items)
			*/
			inline ContainerItem() :
				cPacketSend(NULL, 0)
			{ }

			/*!
			\brief add an item to the list of items in the container
			\param item item to add to the container
			*/
			inline void addItem(pItem item)
			{ items.push_back(sContainerItem(item)); }

			void prepare();
		};

		/*!
		\brief tells client which items are in the given container (msgboard version)
		\author Flameeyes & Chronodt
		\note packet 0x3c

		\note This functions holds and releases the cMsgBoard::globalMutex and cMsgBloard::boardMutex mutexes
		*/
		class MsgBoardItemsinContainer : public cPacketSend
		{
		protected:
		
			const pMsgBoard msgboard;
		
		public:
			/*!
			\note constructor for msgboards
			\param aMsgboard msgboard used. This is called only on msgboard first opening
			*/
			inline MsgBoardItemsinContainer(pMsgBoard aMsgboard) :
				cPacketSend(NULL, 0), msgboard (aMsgboard)
			{ }
		
			void prepare();
		};


		/*!
		\brief Personal Light Level
		\author Chronodt
		\note packet 0x4e
		*/
		class PersonalLight : public cPacketSend
		{
		protected:
			pChar pc;	//!< Character
			uint8_t light;	//!< light level
		public:
			/*!
			\param aPc Character to send the light level of
			\param aLight light level ( 0x00=day, 0x09=OSI night, 0x1F=Black )
			*/
			inline PersonalLight(pChar aPc, uint8_t aLight) :
				cPacketSend(NULL, 0), pc(aPc), light(aLight)
			{ }

			void prepare();
		};

		/*!
		\brief Overall Light Level
		\author Flameeyes
		\note packet 0x4f
		*/
		class OverallLight : public cPacketSend
		{
		protected:
			uint8_t level;	//!< Light level to send
		public:
			/*!
			\param l Light level
			*/
			inline OverallLight(uint8_t l) :
				cPacketSend(NULL, 0), level(l)
			{ }
		
			void prepare();
		};

		/*!
		\brief Idle warning
		\author Chronodt
		\note packet 0x53
		*/

		class IdleWarning : public cPacketSend
		{
		protected:
		/* type is:

			1=no character
			2=char exists
			3=can't connect
			4=can't connect
			5=character already in world
			6=login problem
			7=idle
			8=can't connect
		*/
			uint8_t type;
		public:
			/*!
			\param aType Warning type
			*/
			inline IdleWarning(uint8_t aType) :
				cPacketSend(NULL, 0), type(aType)
			{ }

			void prepare();
		};

		/*!
		\brief Sound effect
		\author Flameeyes
		\note packet 0x54
		*/

		class SoundFX : public cPacketSend
		{
		protected:
			uint16_t model;	//!< Sound model
			sLocation loc;	//!< sLocation where the sound is played
			bool rep;	//!< If true, the sound will be repeated
		public:
			/*!
			\param m sound model
			\param l where the sound will be played
			\param r should the sound be repeated?
			*/
			inline SoundFX(uint16_t m, sLocation l, bool r) :
				cPacketSend(NULL, 0), model(m), loc(l), rep(r)
			{ }

			void prepare();
		};

		/*!
		\brief StartGame
		\author Chronodt
		\note packet 0x55
		*/

		class StartGame : public cPacketSend
		{
		protected:
		public:
			inline StartGame() :
				cPacketSend(NULL, 0)
			{ }

			void prepare();
		};

		/*!
		\brief Sets/resets pins in a map
		\author Chronodt
		\note packet 0x56
		*/

		class MapPlotCourse : public cPacketSend
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
			\param aMap map used
			\param aCommand command to be sent to client
			\param aPin pin modified (or writeability status if command == 6)
			\param xx x position of pin (map relative, pixels from topleft corner)
			\param yy y position of pin (map relative, pixels from topleft corner)
			*/
			inline MapPlotCourse(pMap aMap, PlotCourseCommands aCommand, uint16_t aPin = 0, uint32_t xx = 0, uint32_t yy = 0) :
				cPacketSend(NULL, 0), map (aMap), command (aCommand), pin (aPin),  x (xx), y (yy)
			{ }

			void prepare();
		};
		/*!
		\brief Local time of server (or game time?)
		\author Chronodt
		\note packet 0x5b
		*/

		class GameTime : public cPacketSend
		{
		protected:
		public:
			inline GameTime() :
				cPacketSend(NULL, 0)
			{ }
			void prepare();
		};

		/*!
		\brief Send Weather
		\author Chronodt
		\note packet 0x65

		\remarks maximum number of weather effects on screen is 70
		\remarks If it is raining, you can add snow by setting the num
		to the num of rain currently going, plus the number of snow you want
		\attention Weather messages are only displayed when weather starts
		\note Weather will end automatically after 6 minutes without any weather change packets
		\remarks You can totally end weather (to display a new message) by teleporting

		weather type:
		0x00="It starts to rain"
		0x01="A fierce storm approaches."
		0x02="It begins to snow"
		0x03="A storm is brewing.",
		0xFF=None (turns off sound effects),
		0xFE=(no effect?? Set temperature?)
		0x78,0x20,0x4F,0x4E = reset, never tested
		*/
		class Weather : public cPacketSend
		{
		protected:
			uint8_t weather;	//!< weather effect (0x00 - rain, 0x02- snow, 0xff - nothing)
			uint8_t intensity;	//!< number of simultaneous weather effects on screen
		public:
			inline Weather(uint8_t aWeather, uint8_t aIntensity) :
				cPacketSend(NULL, 0), weather(aWeather), intensity(aIntensity)
			{ }
			void prepare();
		};

		/*!
		\brief Send Book Page (Read-Write book version)
		\author Flameeyes
		\note packet 0x66

		\note Sends the pages of a writable book
		*/
		class BookPagesReadWrite : public cPacketSend
		{
		protected:
			pBook book;
			
			void preparePagesReadOnly();
			void preparePagesReadWrite();
		public:
			inline BookPagesReadWrite(pBook abook) :
				cPacketSend(NULL, 0), book(abook)
			{ }
		
			void prepare();
		};

		/*!
		\brief Send Book Page (Read-only book version)
		\author Flameeyes
		\note packet 0x66

		Sends the requested page of a readonly book
		*/
		class BookPageReadOnly : public cPacketSend
		{
		protected:
			pBook book;
			uint16_t p;	//!< Index of the page to send
			
			void preparePagesReadOnly();
			void preparePagesReadWrite();
		public:
			inline BookPageReadOnly(pBook abook, uint16_t page) :
				cPacketSend(NULL, 0), book(abook), p(page)
			{ }
		
			void prepare();
		};


		/*!
		\brief Send targeting cursor to client
		\author Chronodt
		\note packet 0x6C

		\todo targeting has to be remade almost completely -_-
		*/
		class TargetingCursor : public cPacketSend
		{
		protected:
			uint8_t type;		//!< 0 = creature/item target, 1 = xyz(location) target
			uint32_t cursorid;	//!< I SUPPOSE it is something related to the animation id of the targeting cursor
		public:
			inline TargetingCursor(uint8_t aType, uint32_t aCursorId) :
				cPacketSend(NULL, 0), type(aType), cursorid(aCursorId)
			{ }
			void prepare();
		};



		/*!
		\brief cChar::action() packet
		\author Flameeyes
		*/
		class Action : public cPacketSend
		{
		protected:
			const uint32_t serial;	//!< Serial of the char which do the action
			const uint16_t action;	//!< id of the action to execute
		
		public:
			/*!
			\param s serial of the char which do the action
			\param a id of the action to execute
			*/
			inline Action(uint32_t s, uint16_t a) :
				cPacketSend(NULL, 0), serial(s), action(a)
			{ }

			void prepare();
		};

		/*!
		\brief Login Denied
		\author Kheru
		\note packet 0x82

		0x00 = unknown user
		0x01 = account already in use
		0x02 = account disabled
		0x03 = password bad
		0x04 and higher = communications failed 
		*/

		class LoginDenied : public cPacketSend
		{
		protected:
			uint8_t reason;

		public:
			inline LoginDenied(uint8_t aReason) :
				cPacketSend(NULL, 0), reason(aReason)
			{ }

			void prepare();
		};

		//! Open Web Browser
		class OpenBrowser : public cPacketSend
		{
		protected:
			std::string url;	//!< Url to open the browser to
		public:
			/*!
			\param str Url to open the browser to
			*/
			inline OpenBrowser(std::string str) :
				cPacketSend(NULL, 0), url(str)
			{ }

			void prepare();
		};

		/*!
		\brief Send Tips or MOTD/Server notice window
		\note Packet 0xA6
		*/
		class TipsWindow : public cPacketSend
		{
		protected:
			uint8_t	type;		//!< Type: 0x00 tip, 0x02 MOTD
			uint16_t tip_num;	//!< #Tip number (no use if MOTD)
			std::string message;	//!< Tip (or MOTD) to send

		public:
			inline TipsWindow(std::string aMessage, uint8_t TipType = 0x02, uint16_t TipNum = 0x0000) :
				cPacketSend(NULL, 0), type(TipType), tip_num(TipNum), message(aMessage)
			{ }

			void prepare();
		};
		
		/*!
		\brief Sends the OK/Not OK for an attack
		\note Packet 0xAA
		*/
		class AttackAck : public cPacketSend
		{
		protected:
			pChar victim;	//!< Character attacked
		public:
			/*!
			\param v Character attacked (should be NULL to undo an attack)
			*/
			AttackAck(pChar v) :
				cPacketSend(NULL, 0), victim(v)
			{ }
			
			void prepare();
		};
		

		//! Play midi file
		class PlayMidi : public cPacketSend
		{
		protected:
			uint16_t id;	//!< ID of the MIDI to play
		public:
			/*!
			\param midi Midi file id
			*/
			inline PlayMidi(uint16_t midi) :
				cPacketSend(NULL, 0), id(midi)
			{ }

			void prepare();
		};


		/*!
		\brief Opens map gump with data from map
		\author Chronodt
		*/
		
		class OpenMapGump : public cPacketSend
		{
		protected:
		
			const pMap map;	//!< Map's gump
		public:
			/*!
			\param m map
			*/
			inline OpenMapGump(pMap m) :
				cPacketSend(NULL, 0), map (m)
			{ }
		
			void prepare();
		};



		enum BBoardCommands {bbcDisplayBBoard, bbcSendMessageSummary, bbcSendMessageBody};
		class BBoardCommand : public cPacketSend
		{
		protected:
		
			const pMsgBoard msgboard;
			const BBoardCommands command;
			const pMessage message;
		
		public:
			/*!
			\param m msgboard used
			\param com command for the msgboard
			\param mess message to be sent. May be omitted if command is DisplayBBoard
			*/
			inline BBoardCommand(pMsgBoard m, BBoardCommands com, pMessage mess = NULL) :
				cPacketSend(NULL, 0), msgboard (m), command(com), message(mess)
			{ }
		
			void prepare();
		};
		
		

		
		class SecureTrading : public cPacketSend
		{
		protected:
		
			const uint8_t action;		//!< 0 = initiate, 1 = close window (end trade), 2 = change status (the button)
			const pPC tradePartner;		//!< trade partner of receiving client
			const uint32_t id1;		//!< if trade opening, must contains serial of trade container for this client, else its tradestatus (the button status: 1 checked 0 unchecked)
			const uint32_t id2;		//!< if trade opening, must contains serial of trade container for partner client, else its tradestatus (the button status: 1 checked 0 unchecked)

		public:
			inline SecureTrading(uint8_t aAction, pPC aTradePartner, uint32_t aId1, uint32_t aId2) :
				cPacketSend(NULL, 0), action(aAction), tradePartner(aTradePartner), id1(aId1), id2(aId2)
			{ }
		
			void prepare();
		};
		
		
		class UpdatePlayer : public cPacketSend
		{
		protected:
			pChar chr;
			uint8_t dir, flag, hi_color;
			uint32_t id1,id2,id3;
		
		public:
			inline UpdatePlayer(pChar pc, uint8_t newdir, uint8_t newflag, uint8_t newhi_color) :
				cPacketSend(NULL, 0), chr(pc), dir(newdir), flag(newflag), hi_color(newhi_color)
			{ }
		
			void prepare();
		};
		
		class WarModeStatus : public cPacketSend
		{
		protected:
		
			uint8_t buf[5];
		
		public:
			inline WarModeStatus(uint8_t* buffer) :
				cPacketSend(NULL, 0)
			{ memcpy(buf, buffer, 5);}
		
			void prepare();
		};
		
		
		class PingReply : public cPacketSend
		{
		protected:
		
			uint8_t buf[2];
		
		public:
			inline PingReply(uint8_t* buffer) :
				cPacketSend(NULL, 0)
			{ memcpy(buf, buffer, 2);}
		
			void prepare();
		};
		
		class CharDeleteError : public cPacketSend
		{
		protected:
			uint8_t reason;
		
		public:
			inline CharDeleteError(uint8_t r) :
				cPacketSend(NULL, 0), reason(r)
			{ }
		
			void prepare();
		};
		
		class CharAfterDelete : public cPacketSend
		{
		protected:
			pAccount account;
		
		public:
			inline CharAfterDelete(pAccount a) :
				cPacketSend(NULL, 0), account(a)
			{ }
		
			void prepare();
		};
		
		class CharProfile : public cPacketSend
		{
		protected:
			uint32_t serial;
			pChar who;
		
		public:
			inline CharProfile(uint32_t s, pChar w) :
				cPacketSend(NULL, 0), serial(s), who(w)
			{ }
		
			void prepare();
		};
		
		
		class LogoutStatus : public cPacketSend
		{
		protected:
		
		public:
			inline LogoutStatus() :
				cPacketSend(NULL, 0)
			{ }
		
			void prepare();
		};
		
		class ClientViewRange : public cPacketSend
		{
		protected:
			uint8_t range;
		public:
			inline ClientViewRange(uint8_t r) :
				cPacketSend(NULL, 0), range(r)
			{ }
		
			void prepare();
		};
		
		//! Sends a book's header
		class BookHeader : public cPacketSend
		{
		protected:
			pBook book;	//!< Book to send header of
			bool readonly;	//!< Is the book read only?
		public:
			inline BookHeader(pBook abook, bool ro) :
				cPacketSend(NULL, 0), book(abook), readonly(ro)
			{ }
		
			void prepare();
		};
		

	} // Sent
	
	/*!
	\brief Received packet
	\author Chronodt
	
	In this namespace we have all the packets received handling classes
	*/
	namespace Received {
		#define RECEIVE_PACKET(A) \
		class A : public cPacketReceive { \
			bool execute(pClient client); \
			inline A(uint8_t *buf, uint16_t len) : \
				cPacketReceive(buf, len) \
			{ } \
		};
		
		RECEIVE_PACKET(CreateChar)	//!< Character creation
		RECEIVE_PACKET(DisconnectNotify)	//!< "software disconnect"
		RECEIVE_PACKET(MoveRequest)	//!< move (or run) 1 step
		RECEIVE_PACKET(TalkRequest)	//!< character speech (NOT UNICODE!!)
		RECEIVE_PACKET(AttackRequest)	//!< Clients requests an armed/unarmed attack request against someone
		RECEIVE_PACKET(Doubleclick)	//!< Use of item or info on char
		RECEIVE_PACKET(PickUp)		//!< trying to pick up an item
		RECEIVE_PACKET(DropItem)		//!< releasing mouse button and dropping item dragged. Position check too
		RECEIVE_PACKET(Singleclick)	//!< info/name request for item/char
		RECEIVE_PACKET(ActionRequest)	//!< skill/magic use (spells from macros i believe) and generic action, like salute or bow
		RECEIVE_PACKET(WearItem)		//!< drag of item on paperdoll. Check if equippable too
		RECEIVE_PACKET(MoveACK_ResyncReq)	//!< move acknowdlege or if it sends a 0x220000, resends data to client
		RECEIVE_PACKET(RessChoice)	//!< once it was used for a ress choice(ghost or revive with skill penalties?) now only used for bounty placement
		RECEIVE_PACKET(StatusRequest)	//!< statusbar request about a char
		RECEIVE_PACKET(SetSkillLock)	//!< skill lock change (up, down, lock)
		RECEIVE_PACKET(BuyItems)		//!< receive list of items selected from a buy list of vendor
		RECEIVE_PACKET(MapPlotCourse)	//!< pin change for a course on map. Also all pin-related action on map
		RECEIVE_PACKET(LoginChar)
		RECEIVE_PACKET(BookPage)		//!< receives a changed book page
		RECEIVE_PACKET(TargetSelected)	//!< target selected (for whatever asked for it :D)
		RECEIVE_PACKET(SecureTrade)	//!< secure trade conclusion (change in the secure trade window's checkmark)
		RECEIVE_PACKET(BBoardMessage)	//!< almost all msgboard activity except opening the board itself comes here
		RECEIVE_PACKET(WarModeChange)	//!< warmode change request
		RECEIVE_PACKET(Ping)		//!< Ping of client. Obiously server answers :D
		RECEIVE_PACKET(RenameCharacter)	//!< client tries to rename a character
		RECEIVE_PACKET(DialogResponse)
		RECEIVE_PACKET(LoginRequest)
		RECEIVE_PACKET(DeleteCharacter)	//!< client tries to delete a char from login window
		RECEIVE_PACKET(GameServerLogin)
		RECEIVE_PACKET(BookUpdateTitle)	//!< change title of book
		RECEIVE_PACKET(DyeItem)		//!< dye item (returns color selected in dye menu)
		RECEIVE_PACKET(RequestHelp)	//!< gm/conselor page request
		RECEIVE_PACKET(SellItems)		//!< receive list of items selected from a sell list of vendor
		RECEIVE_PACKET(SelectServer)
		RECEIVE_PACKET(TipsRequest)
		RECEIVE_PACKET(GumpTextDialogReply)
		RECEIVE_PACKET(UnicodeSpeechReq)	//!< unicode speech request from client
		RECEIVE_PACKET(GumpResponse)
		RECEIVE_PACKET(ChatMessage)
		RECEIVE_PACKET(ChatWindowOpen)
		RECEIVE_PACKET(PopupHelpRequest)
		RECEIVE_PACKET(CharProfileRequest)//!< update or asks for character profile and title (char profile is the one on the paperdoll's scroll)
		RECEIVE_PACKET(ClientVersion)	//!< during login, client sends its version
		RECEIVE_PACKET(AssistVersion)
		RECEIVE_PACKET(MiscCommand)	//!< multipurpouse packet. It ranges from party commands to fastwalk prevention, gumps and menu
		RECEIVE_PACKET(TextEntryUnicode)
		RECEIVE_PACKET(ClientViewRange)	//!< Client sends its (wanted) view area in "squares" (from 5 to 18). If server replyes with same packet, it gets activated
		RECEIVE_PACKET(LogoutStatus)	//!< if client receives a certain flag on packet 0xa9 on login, sends this packet when logging out and expects a reply before disconnecting
		RECEIVE_PACKET(NewBookHeader)	//!< more up-to-date book header communication (only difference found: variable-length title and author)
		RECEIVE_PACKET(FightBookSelection)//!< fightbook icon selection
		
		#undef RECEIVE_PACKET
	}
} // nPackets

#endif

