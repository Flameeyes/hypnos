/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Declaration of cClient class
*/

#ifndef __CCLIENT_H__
#define __CCLIENT_H__

#include "common_libs.h"
#include "structs.h"
#include "speech.h"

//! Used in trading methods
struct sBoughtItem
{
	int layer;
	pItem item;
	int amount;
};

/*!
\author Flameeyes
\brief Client access class

This class represent the client connection and is an high-end socket class.
Sending packages to be sent here, will call cSocket raw-level socket for send
them to the client itself.
This is the class which provides client compression & encryption.
*/
class cClient
{
friend class cGMPage;
protected:
	static ClientSList clients;	//!< All the clients
	static ClientSList cGMs;		//!< GMs' clients \todo Need to be used

public:
	static const uint32_t clientHasCrypto		= 0x00000001;
	static const uint32_t clientIsT2A		= 0x00000002;
	static const uint32_t clientIsUO3D		= 0x00000004;
	static const uint32_t clientIsAoS		= 0x00000008;

protected:
	pPC pc;	        //!< Current char used by the client
	pAccount acc;	//!< Current account logged in by the client
	pSocket sock;	//!< Current socket used by the client

	/*!< holds number of "squares" clients can see. Default is 18, but it can be changed 
 	\see cPacketReceiveClientViewRange
  	*/	
	uint8_t visualRange;				   
                                   
 

	uint32_t flags;			//!< Flags of capabilities of the client
        short int clientDimension;	//!< 2d or 3d client? (must contain 2 or 3)
public:
	cClient(int32_t sd, struct sockaddr_in* addr);
	~cClient();
	
	void disconnect();

	void sendPacket(pPacketSend ps);

	//! Gets the current char owned by the client
	inline pPC currChar() const
	{ return pc; }

	inline pAccount currAccount() const
	{ return acc; }

	void encode(uint8_t *&);
	void compress(uint8_t *&, uint32_t&);
	void encrypt(uint8_t *&);

	//! Gets the online GMs
	static ClientSList getOnlineGMs()
	{ return cGMs; }
public:
	void get_item(pItem pi, uint16_t amount);                   	//!< Client grabs an item
	void drop_item(pItem pi, sLocation &loc, pSerializable dest);   	//!< Item is dropped on ground, char or item

protected:
	bool dragging; 	//!< true if is dragging
	bool evilDrag; 	//!< evil dragging, we need this for UO3D clients to save dragging history
	pItem dragItem;	//!< to use as safekeeping against client disconnection
	// internally used by drop_item:
	void pack_item(pItem pi, pItem dest);				//!< Item is dropped on another item
	void dump_item(pItem pi, sLocation &loc);			//!< Item is dropped on the ground
	void droppedOnChar(pItem pi, pChar dest);			//!< Item is dropped on a character
	// internally used by droppedOnChar
	void droppedOnPet(pItem pi, pNPC pet);				//!< Item is dropped on a pet
	void droppedOnGuard(pItem pi, pNPC npc);			//!< Item is dropped on a guard
	void droppedOnBeggar(pItem pi, pNPC npc);			//!< Item is dropped on a beggar
	void droppedOnTrainer(pItem pi, pNPC npc);			//!< Item is dropped on a trainer
	void droppedOnSelf(pItem pi);					//!< Item is dropped on self
	void wear_item(pChar pck, pItem pi);				//!< Item is dropped on paperdoll
	// bouncing
	void item_bounce3(const pItem pi);				//!< simple bouncing
	void item_bounce4(const pItem pi);				//!< bounce & checkid before resending item
	void item_bounce5(const pItem pi);				//!< bounce & resend item
	void item_bounce6(const pItem pi);				//!< advanced bouncing

public:
	inline bool isDragging() const
	{ return dragging; }

	inline void setDragging()
	{ dragging=true; }
        
	inline void resetDragging()
	{ dragging=false; }


//@{
/*!
\name Trading stuff
*/
protected:
	struct sSecureTradeSession
	{
		pClient tradepartner;   //there will be a copy of this structure in both clients, so only the other one is really needed
		pContainer container1;
		pContainer container2;
		bool status1;		//status of the secure trade: flagged or not
		bool status2;		//status of the secure trade: flagged or not

		inline bool operator==(const sSecureTradeSession& session2) const
		{
			return 	tradepartner == session2.tradepartner &&
				container1   == session2.container1 &&
				container2   == session2.container2 &&
				status1      == session2.status1 &&
				status2      == session2.status2;
		}
	};

	std::list<sSecureTradeSession> SecureTrade;	//!< Holds the secure trade session of this client (begun and received both)

public:
	//! Adds a session to this client's list of open secure trading sessions
	inline void addTradeSession(sSecureTradeSession &session)
	{ SecureTrade.push_back(session); }

	//! returns true if client has at least an open trade session
	inline bool hasOpenTradeSessions()
	{ return SecureTrade.size(); }

	//! removing trade session between this and another client
	inline void removeTradeSession(sSecureTradeSession &session)
	{ SecureTrade.erase(find(SecureTrade.begin(), SecureTrade.end(), session)); }

	sSecureTradeSession findTradeSession(pContainer tradecontainer);	//!< Finds the trade session between "this" and another client knowing "this" tradecontainer
	sSecureTradeSession findTradeSession(pClient tradeclient);		//!< Finds the trade session between "this" and another client knowing the other client
	pContainer tradestart(pClient targetClient);				//!< Opens a secure trade windows between this and targetClient. returns this client's ctrade container
	void sendtradestatus(sSecureTradeSession &session);			//!< updates secure trade window
	void dotrade(sSecureTradeSession &session);				//!< concludes trade (either swapping items or returning them)
	void endtrade(sSecureTradeSession &session);				//!< closing trade window : called when one client ends the transaction (closing the trade window) or one client crashes (client destruction)
	bool buyShop(pNPC vendor);						//!< sends list of buyable items to client (opens buy gump)
	void buyaction(pNPC npc, std::list< sBoughtItem > &allitemsbought);	//!< Getting purchased item and gold/availability check
	void sellaction(pNPC npc, std::list< sBoughtItem > &allitemssold);	//!< Sellig of items. Moving from char and getting paid :D
	
	void telltime();
//@}

//@{
/*!
\name Speech, message & communication stuff
*/
public:
	void talking(cSpeech &speech);				//!< The PC talks, and this finds out who will hear (and send it to them)
	void sysmessage(uint16_t color, std::string txt);	//!< System message (compiled)
	void sysmessage(const char *txt, ...) PRINTF_LIKE(1,2)	//!< System message (In lower left corner)
	void sysmessage(uint16_t color, const char *txt, ...) PRINTF_LIKE(2,3)
								//!< Colored system message (In lower left corner)
	void sysbroadcast(char *txt, ...) PRINTF_LIKE(1,2)	//!< System broadcast in bold text
	void sysbroadcast(std::string txt);			//!< System broadcast (compiled)
//@}

//@{
/*!
\brief Packets
*/
public:
	void pause();
	void resume();

	void showContainer(pItem pCont);
	void light();
	void showBankBox(pChar dest);
	void showSpecialBankBox(pChar dest);
	void statusWindow(pChar target, bool extended = true);
	void updateStatusWindow(pItem item);
	void skillWindow();					//!< send the skill level info to the skill window of client
	void updatePaperdoll();					//!< tells to the client that something has changed in his paperdoll
	void sendMidi(uint16_t id);				//!< plays midi on client (note: if client disabled music it will not play :D)
	void sendItem(pItem pi);				//!< Shows items to client (on the ground or inside containers)
	void senditem_lsd(pItem pi, uint16_t color, sLocation position);
								//!< warps item in world for hallucinatory effect (sets new color and location)
	void cClient::sendchar(pChar pc, bool z = true);	//!< Shows char to client. Checks for gm and invisibility too

	//! audio packets (sound effects & music)
	void playMidi();
	void playSFX(uint16_t sound, bool onlyMe = false, bool repeat = false);
//@}
};

#endif
