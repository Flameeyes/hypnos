/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __PACKETS_RECEIVED_H__
#define __PACKETS_RECEIVED_H__

#include "common_libs.hpp"
#include "enums.hpp"
#include "structs.hpp"
#include "speech.hpp"

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
	inline cPacketReceive(uint8_t *buf, uint16_t len, uint8_t *&newbuf, uint16_t &newlen) throw eBufferIncomplete; :
		buffer(buf), length(len)
	{ } 
	
	virtual ~cPacketReceive() = 0;
	
	static pPacketReceive fromBuffer(uint8_t *buffer, uint16_t length, uint8_t *&newbuf, uint16_t &newlen) throw eBufferIncomplete;
	virtual bool execute(pClient client) = 0;
};

namespace nPackets {
	/*!
	\brief Received packet
	\author Chronodt
	
	In this namespace we have all the packets received handling classes
	*/
	namespace Received {
		#define RECEIVE_PACKET(A, len) \
		class A : public cPacketReceive { \
			bool execute(pClient client); \
			uint16_t getLength() \
			{ return len; } \
			inline A(uint8_t *buf, uint16_t len, uint8_t *&newbuf, uint16_t &newlen) throw eBufferIncomplete : \
				cPacketReceive(buf, len, newbuf, newlen) \
			{ } \
		};
		
		RECEIVE_PACKET(CreateChar, 104)		//!< Character creation
		RECEIVE_PACKET(DisconnectNotify, 5)	//!< "software disconnect"
		RECEIVE_PACKET(MoveRequest, 7)		//!< move (or run) 1 step
		RECEIVE_PACKET(TalkRequest, 0)		//!< character speech (NOT UNICODE!!)
		RECEIVE_PACKET(AttackRequest, 5)	//!< Clients requests an armed/unarmed attack request against someone
		RECEIVE_PACKET(Doubleclick, 5)		//!< Use of item or info on char
		RECEIVE_PACKET(PickUp, 7)		//!< trying to pick up an item
		RECEIVE_PACKET(DropItem, 14)		//!< releasing mouse button and dropping item dragged. Position check too
		RECEIVE_PACKET(Singleclick, 5)		//!< info/name request for item/char
		RECEIVE_PACKET(ActionRequest, 0)	//!< skill/magic use (spells from macros i believe) and generic action, like salute or bow
		RECEIVE_PACKET(WearItem, 10)		//!< drag of item on paperdoll. Check if equippable too
		RECEIVE_PACKET(MoveACK_ResyncReq, 3)	//!< move acknowdlege or if it sends a 0x220000, resends data to client
		RECEIVE_PACKET(RessChoice, 2)		//!< once it was used for a ress choice(ghost or revive with skill penalties?) now only used for bounty placement
		RECEIVE_PACKET(StatusRequest, 10)	//!< statusbar request about a char
		RECEIVE_PACKET(SetSkillLock, 0)		//!< skill lock change (up, down, lock)
		RECEIVE_PACKET(BuyItems, 0)		//!< receive list of items selected from a buy list of vendor
		RECEIVE_PACKET(MapPlotCourse, 11)	//!< pin change for a course on map. Also all pin-related action on map
		RECEIVE_PACKET(LoginChar, 73)
		RECEIVE_PACKET(BookPage, 0)		//!< receives a changed book page
		RECEIVE_PACKET(TargetSelected, 19)	//!< target selected (for whatever asked for it :D)
		RECEIVE_PACKET(SecureTrade, 0)		//!< secure trade conclusion (change in the secure trade window's checkmark)
		RECEIVE_PACKET(BBoardMessage, 0)	//!< almost all msgboard activity except opening the board itself comes here
		RECEIVE_PACKET(WarModeChange, 5)	//!< warmode change request
		RECEIVE_PACKET(Ping, 2)			//!< Ping of client. Obiously server answers :D
		RECEIVE_PACKET(RenameCharacter, 35)	//!< client tries to rename a character
		RECEIVE_PACKET(DialogResponse, 13)
		RECEIVE_PACKET(LoginRequest, 62)
		RECEIVE_PACKET(DeleteCharacter, 39)	//!< client tries to delete a char from login window
		RECEIVE_PACKET(GameServerLogin, 65)
		RECEIVE_PACKET(BookUpdateTitle, 99)	//!< change title of book
		RECEIVE_PACKET(DyeItem, 9)		//!< dye item (returns color selected in dye menu)
		RECEIVE_PACKET(RequestHelp, 258)	//!< gm/conselor page request
		RECEIVE_PACKET(SellItems, 0)		//!< receive list of items selected from a sell list of vendor
		RECEIVE_PACKET(SelectServer, 3)
		RECEIVE_PACKET(TipsRequest, 4)
		RECEIVE_PACKET(GumpTextDialogReply, 0)
		RECEIVE_PACKET(UnicodeSpeechReq, 0)	//!< unicode speech request from client
		RECEIVE_PACKET(GumpResponse, 0)
		RECEIVE_PACKET(ChatMessage, 0)
		RECEIVE_PACKET(ChatWindowOpen, 64)
		RECEIVE_PACKET(PopupHelpRequest, 9)
		RECEIVE_PACKET(CharProfileRequest, 0)	//!< update or asks for character profile and title (char profile is the one on the paperdoll's scroll)
		RECEIVE_PACKET(ClientVersion, 0)	//!< during login, client sends its version
		RECEIVE_PACKET(AssistVersion, 0)
		RECEIVE_PACKET(MiscCommand, 0)		//!< multipurpouse packet. It ranges from party commands to fastwalk prevention, gumps and menu
		RECEIVE_PACKET(TextEntryUnicode, 0)
		RECEIVE_PACKET(ClientViewRange, 2)	//!< Client sends its (wanted) view area in "squares" (from 5 to 18). If server replyes with same packet, it gets activated
		RECEIVE_PACKET(LogoutStatus, 2)		//!< if client receives a certain flag on packet 0xa9 on login, sends this packet when logging out and expects a reply before disconnecting
		RECEIVE_PACKET(NewBookHeader, 0)	//!< more up-to-date book header communication (only difference found: variable-length title and author)
		RECEIVE_PACKET(FightBookSelection, 0)	//!< fightbook icon selection
		
		#undef RECEIVE_PACKET
	}
} // nPackets

#endif
