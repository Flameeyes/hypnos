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
#include "objects/citem/cmsgboard.h"

namespace nPackets {

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
	
		virtual void fixForClient(ClientType ct) = 0;
	};
	

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
			pItem item;		//!< Item
			pPC pc;			//!< Player who sees the item
			uint16_t color;		//!< new color of item
			Location position;	//!< new position of item
		public:
			/*!
			\param i item
			\param p pc who sees the item
			\param c new color of item
			\param pos new position of item
			*/
			inline LSDObject(pItem i, pPC p, uint16_t c, Location pos) :
				cPacketSend(NULL, 0), item(i), pc(p), color(c), position(pos)
			{ }
			void prepare();
		};
		
		/*!
		\brief Char Location and body type (Login confirmation)
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
			Location destination; 	//!< where the item is dragged to
			uint16_t amount;	//!< how many items in stack
		public:
			inline DragItem(pItem aItem, Location aDestination, uint16_t aAmount) :
				cPacketSend(NULL, 0), item(aItem), destination(aDestination), amount(aAmount)
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
		\brief Draws a container on the user script
		\author Flameeyes
		*/
		class DrawContainer : public cPacketSend
		{
		protected:
			const uint32_t serial;	//!< Serial of the container to draw
			const uint16_t gump;	//!< Gump of the container to draw
		
		public:
			/*!
			\param s serial of the container
			\param g gump of the container
			*/
			inline DrawContainer(uint32_t s, uint16_t g) :
				cPacketSend(NULL, 0), serial(s), gump(g)
			{ }
		
			void prepare();
		};
		
		/*!
		\brief Send an item in container
		\author Flameeyes
		*/
		class ContainerItem : public cPacketSend
		{
		protected:
			std::list<sContainerItem> items;
			uint32_t contSerial;
		public:
			/*!
			\param s serial of the container (for all items)
			*/
			inline ContainerItem(uint32_t s) :
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
		class ShowItemInContainer : public cPacketSend
		{
		protected:
			pItem item;	//!< Item to add
		public:
			/*!
			\param itm item to add
			*/
			inline ShowItemInContainer(pItem itm) :
				cPacketSend(NULL, 0), item(itm)
			{ }
		
			void prepare();
		};
		
		//! Work item
		class WornItem : public cPacketSend
		{
		protected:
			pItem item;
		public:
			inline WornItem(pItem itm) :
				cPacketSend(NULL, 0), item(itm)
			{ }
		
			void prepare();
		};
		
		//! Sound FX
		class SoundFX : public cPacketSend
		{
		protected:
			uint16_t model;	//!< Sound model
			Location loc;	//!< Location where the sound is played
		public:
			/*!
			\param m sound model
			\param l where the sound will be played
			*/
			inline SoundFX(uint16_t m, Location l) :
				cPacketSend(NULL, 0), model(m), loc(l)
			{ }
		
			void prepare();
		};
		
		
		
		//! Send skill status
		class SkillState : public cPacketSend
		{
		protected:
			pChar pc;
		public:
			/*!
			\param c Character to send the skill of
			*/
			inline SkillState(pChar c) :
				cPacketSend(NULL, 0), pc(c)
			{ }
		
			void prepare();
		};
		
		//! Update a skill
		class UpdateSkill : public cPacketSend
		{
		protected:
			pChar pc;
			uint16_t skill;
		public:
			/*!
			\param c Character to send the skill of
			\param sk Skill to update
			*/
			inline UpdateSkill(pChar c, uint16_t sk) :
				cPacketSend(NULL, 0), pc(c), skill(sk)
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
		
		//! Overall Light Level
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
		\brief Packet to confirm processing of buy (or sell) window
		\author Chronodt
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
		\brief Packet to warn client that something has been added to pc's clothing
		\author Chronodt
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
		
		enum PlotCourseCommands {
			pccAddPin = 1,		//!< Add map poin
			pccInsertPin,		//!< Add new pin with pin number (insertion. other pins after the number are pushed back.)
			pccChangePin,		//!< Change pin
			pccRemovePin,		//!< Remove pin
			pccClearAllPins,	//!< Remove all pins on the map
			pccToggleWritable,	//!< Toggle the 'editable' state of the map
			pccWriteableStatus	//!< Return message from the server to request 6 of the client
		};
		
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
			\param m map used
			\param comm command to be sent to client
			\param p pin modified (or writeability status if comm == 6)
			\param xx x position of pin (map relative)
			\param yy y position of pin (map relative)
			*/
			inline MapPlotCourse(pMap m, PlotCourseCommands comm, uint16_t p = 0, uint32_t xx = 0, uint32_t yy = 0) :
				cPacketSend(NULL, 0), map (m), command (comm), pin (p),  x (xx), y (yy)
			{ }
		
			void prepare();
		};
		
		enum BBoardCommands {bbcDisplayBBoard, bbcSendMessageSummary, bbcSendMessageBody};
		class BBoardCommand : public cPacketSend
		{
		protected:
		
			const pMsgBoard msgboard;
			const BBoardCommands command;
			const cMsgBoard::cMessage *message;
		
		public:
			/*!
			\param m msgboard used
			\param com command for the msgboard
			\param mess message to be sent. May be omitted if command is DisplayBBoard
			*/
			inline BBoardCommand(pMsgBoard m, BBoardCommands com, cMsgBoard::cMessage *mess = NULL) :
				cPacketSend(NULL, 0), msgboard (m), command(com), message(mess)
			{ }
		
			void prepare();
		};
		
		
		class MsgBoardItemsinContainer : public cPacketSend
		{
		protected:
		
			const pMsgBoard msgboard;
		
		public:
			/*!
			\note constructor for msgboards
			\param m msgboard used. This is called only on msgboard first opening
			*/
			inline MsgBoardItemsinContainer(pMsgBoard m) :
				cPacketSend(NULL, 0), msgboard (m)
			{ }
		
			void prepare();
		};
		
		class SecureTradingStatus : public cPacketSend
		{
		protected:
		
			const uint8_t action;
			const uint32_t id1,id2,id3;
		
		public:
			inline SecureTradingStatus(uint8_t act, uint32_t i1, uint32_t i2, uint32_t i3) :
				cPacketSend(NULL, 0), action(act), id1(i1), id2(i2), id3(i3)
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
		
	} // Sent
	
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

