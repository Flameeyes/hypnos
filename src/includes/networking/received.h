/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __PACKETS_RECEIVED_H__
#define __PACKETS_RECEIVED_H__

#include "common_libs.h"
#include "enums.h"
#include "structs.h"
#include "speech.h"

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
		
		RECEIVE_PACKET(CreateChar)		//!< Character creation
		RECEIVE_PACKET(DisconnectNotify)	//!< "software disconnect"
		RECEIVE_PACKET(MoveRequest)		//!< move (or run) 1 step
		RECEIVE_PACKET(TalkRequest)		//!< character speech (NOT UNICODE!!)
		RECEIVE_PACKET(AttackRequest)		//!< Clients requests an armed/unarmed attack request against someone
		RECEIVE_PACKET(Doubleclick)		//!< Use of item or info on char
		RECEIVE_PACKET(PickUp)			//!< trying to pick up an item
		RECEIVE_PACKET(DropItem)		//!< releasing mouse button and dropping item dragged. Position check too
		RECEIVE_PACKET(Singleclick)		//!< info/name request for item/char
		RECEIVE_PACKET(ActionRequest)		//!< skill/magic use (spells from macros i believe) and generic action, like salute or bow
		RECEIVE_PACKET(WearItem)		//!< drag of item on paperdoll. Check if equippable too
		RECEIVE_PACKET(MoveACK_ResyncReq)	//!< move acknowdlege or if it sends a 0x220000, resends data to client
		RECEIVE_PACKET(RessChoice)		//!< once it was used for a ress choice(ghost or revive with skill penalties?) now only used for bounty placement
		RECEIVE_PACKET(StatusRequest)		//!< statusbar request about a char
		RECEIVE_PACKET(SetSkillLock)		//!< skill lock change (up, down, lock)
		RECEIVE_PACKET(BuyItems)		//!< receive list of items selected from a buy list of vendor
		RECEIVE_PACKET(MapPlotCourse)		//!< pin change for a course on map. Also all pin-related action on map
		RECEIVE_PACKET(LoginChar)
		RECEIVE_PACKET(BookPage)		//!< receives a changed book page
		RECEIVE_PACKET(TargetSelected)		//!< target selected (for whatever asked for it :D)
		RECEIVE_PACKET(SecureTrade)		//!< secure trade conclusion (change in the secure trade window's checkmark)
		RECEIVE_PACKET(BBoardMessage)		//!< almost all msgboard activity except opening the board itself comes here
		RECEIVE_PACKET(WarModeChange)		//!< warmode change request
		RECEIVE_PACKET(Ping)			//!< Ping of client. Obiously server answers :D
		RECEIVE_PACKET(RenameCharacter)		//!< client tries to rename a character
		RECEIVE_PACKET(DialogResponse)
		RECEIVE_PACKET(LoginRequest)
		RECEIVE_PACKET(DeleteCharacter)		//!< client tries to delete a char from login window
		RECEIVE_PACKET(GameServerLogin)
		RECEIVE_PACKET(BookUpdateTitle)		//!< change title of book
		RECEIVE_PACKET(DyeItem)			//!< dye item (returns color selected in dye menu)
		RECEIVE_PACKET(RequestHelp)		//!< gm/conselor page request
		RECEIVE_PACKET(SellItems)		//!< receive list of items selected from a sell list of vendor
		RECEIVE_PACKET(SelectServer)
		RECEIVE_PACKET(TipsRequest)
		RECEIVE_PACKET(GumpTextDialogReply)
		RECEIVE_PACKET(UnicodeSpeechReq)	//!< unicode speech request from client
		RECEIVE_PACKET(GumpResponse)
		RECEIVE_PACKET(ChatMessage)
		RECEIVE_PACKET(ChatWindowOpen)
		RECEIVE_PACKET(PopupHelpRequest)
		RECEIVE_PACKET(CharProfileRequest)	//!< update or asks for character profile and title (char profile is the one on the paperdoll's scroll)
		RECEIVE_PACKET(ClientVersion)		//!< during login, client sends its version
		RECEIVE_PACKET(AssistVersion)
		RECEIVE_PACKET(MiscCommand)		//!< multipurpouse packet. It ranges from party commands to fastwalk prevention, gumps and menu
		RECEIVE_PACKET(TextEntryUnicode)
		RECEIVE_PACKET(ClientViewRange)		//!< Client sends its (wanted) view area in "squares" (from 5 to 18). If server replyes with same packet, it gets activated
		RECEIVE_PACKET(LogoutStatus)		//!< if client receives a certain flag on packet 0xa9 on login, sends this packet when logging out and expects a reply before disconnecting
		RECEIVE_PACKET(NewBookHeader)		//!< more up-to-date book header communication (only difference found: variable-length title and author)
		RECEIVE_PACKET(FightBookSelection)	//!< fightbook icon selection
		
		#undef RECEIVE_PACKET
	}
} // nPackets

#endif
