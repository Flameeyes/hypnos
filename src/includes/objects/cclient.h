/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
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
      	static ClientList clients;	//!< All the clients
	static ClientList cGMs;		//!< GMs' clients \todo Need to be used

public:
	static const uint32_t clientHasCrypto		= 0x00000001;
	static const uint32_t clientIsT2A		= 0x00000002;
	static const uint32_t clientIsUO3D		= 0x00000004;
	static const uint32_t clientIsAoS		= 0x00000008;

protected:
	pPC pc;	        //!< Current char used by the client
	pAccount acc;	//!< Current account logged in by the client
	pSocket sock;	//!< Current socket used by the client

	uint32_t flags;			//!< Flags of capabilities of the client
        short int clientDimension;	//!< 2d or 3d client? (must contain 2 or 3)
public:
	cClient(int32_t sd, struct sockaddr_in* addr);
	~cClient();

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
	static ClientList getOnlineGMs()
	{ return cGMs; }
protected:
       	bool dragging; //!< true if is dragging
	bool evilDrag; //!< evil dragging, we need this for UO3D clients to save dragging history

        // internally used by drop_item:
        void pack_item(pItem pi, Location &loc, pItem cont);            //!< put dropped item into a container
        void dump_item(pItem pi, Location &loc, pItem cont);            //!< Item is dropped on ground or a character
        bool droppedOnChar(pItem pi, Location &loc, pItem cont);        //!< Item is dropped on a character
        bool droppedOnPet(pItem pi, Location &loc, pItem cont);         //!< Item is dropped on a pet
        bool droppedOnGuard(pItem pi, Location &loc, pItem cont);       //!< Item is dropped on a guard
        bool droppedOnBeggar(pItem pi, Location &loc, pItem cont);      //!< Item is dropped on a beggar
        bool droppedOnTrainer(pItem pi, Location &loc, pItem cont);     //!< Item is dropped on a trainer
        bool droppedOnSelf(pItem pi, Location &loc, pItem cont);        //!< Item is dropped on self
        void get_item(pItem pi, uint16_t amount);                   //!< Client grabs an item
        void drop_item(pItem pi, Location &loc, pItem cont);    //!< Item is dropped on ground, char or item
        void wear_item(pChar pck, pItem pi);                            //!< Item is dropped on paperdoll
        void item_bounce3(const pItem pi);                              //!< simple bouncing
        void item_bounce4(const pItem pi);                              //!< bounce & checkid before resending item
        void item_bounce5(const pItem pi);                              //!< bounce & resend item
        void item_bounce6(const pItem pi);                              //!< advanced bouncing

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
public:
        void buyaction(pNPC npc, std::list< sBoughtItem > &allitemsbought);    	//!< Getting purchased item and gold/availability check
        void sellaction(pNPC npc, std::list< sBoughtItem > &allitemssold);	//!< Sellig of items. Moving from char and getting paid :D
        static void sendtradestatus(pContainer cont1, pContainer cont2);  	//!< updates secure trade window
        static void dotrade(pContainer cont1,pContainer cont2);			//!< concludes trade (either swapping items or returning them)
	static void endtrade(uint32_t serial);					//!< closing trade window : called when one client ends the transaction (either accepted or canceled)
//@}

//@{
/*!
\name Speech, message & communication stuff
*/

public:
	void talking(std::string &speech)	//!< The PC talks, and this finds out who will hear (and send it to them)

//@}

//@{
/*!
\brief Packets
*/
public:
	void showContainer(pItem pCont);
	void light();
	void showBankBox(pChar dest);
	void showSpecialBankBox(pChar dest);
	void statusWindow(pChar target, bool extended = true);
	void updateStatusWindow(pItem item);
	void skillWindow();
	void updatePaperdoll();
	void sendMidi(char num1, char num2);

	//! audio packets (sound effects & music)
	void playMidi();
	void playSFX(uint16_t sound, bool onlyMe = false);
//@}
};

#endif
