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

class cClient;
typedef cClient *pClient;

typedef std::list<pClient> cClients;

#include "cchar.h"

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
protected:
      	static cClients clients;        	                //!< All the clients

public:
	static const UI32 clientHasCrypto	= 0x00000001;
	static const UI32 clientIsT2A		= 0x00000002;
	static const UI32 clientIsUO3D		= 0x00000004;
	static const UI32 clientIsAoS		= 0x00000008;

protected:
	pPC pc;	        //!< Current char used by the client
	pAccount acc;	//!< Current account logged in by the client
	pSocket sock;	//!< Current socket used by the client

	UI32 flags;	//!< Flags of capabilities of the client
        short int clientDimension;      //!< 2d or 3d client? (must contain 2 or 3) 
public:
	cClient(SI32 sd, struct sockaddr_in* addr);
	~cClient();

	void sendPacket(pPacketSend ps);

	//! Gets the current char owned by the client
	inline pChar currChar() const
	{ return pc; }

	inline pAccount currAccount() const
	{ return acc; }

	void encode(char &*);
	void compress(char &*);
	void encrypt(char &*);

        //! drag & drop methods
protected:
       	bool dragging; //!< true if is dragging
	bool evilDrag; //!< evil dragging, we need this for UO3D clients to save dragging history

        void pack_item(pItem pi, Location &loc, pItem cont);    //!< put dropped item into a container 

public:
        inline bool isDragging() const
        { return dragging; }

        inline void setDragging()
        { dragging=true; }
        
        inline void resetDragging()
        { dragging=false; }

        void get_item(pItem pi, UI16 amount);                   //!< Client grabs an item
        void drop_item(pItem pi, Location &loc, pItem cont);    //!< Item is dropped on ground, char or item
void wear_item(NXWCLIENT ps);	//!< Item is dropped on paperdoll

        //! packet methods
public:
	void showContainer(pItem pCont);
	void playMidi();
	void playSFX(UI16 sound, bool onlyMe = false);
	void light();
	void showBankBox(pChar dest);
	void showSpecialBankBox(pChar dest);
	void statusWindow(pChar sorg, bool extended, bool canrename);
};

#endif
