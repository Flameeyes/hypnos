  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

/*!
\file
\brief cNxwClientObj class
\author Luxor & Xanathar
*/
#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "cmdtable.h"
#include "targeting.h"
#include "target.h"

typedef std::vector< std::string > td_cmdparams;

class cNxwClientObj {
private:
	NXWSOCKET m_sck;
public:
	cNxwClientObj( NXWSOCKET s );
	void setLegacySocket(NXWSOCKET s);

	inline const NXWsocket toInt() const
	{ return m_sck; }

	//@{
	/*!
	\name Client Status
	*/
	bool isDragging();
	void setDragging();
	void resetDragging();
	bool inGame();
	P_CHAR currChar();
	int currCharIdx();
	int getRealSocket();
	BYTE *getRcvBuffer();
	//@}

	//@{
	/*!
	\name Packets
	*/
	void send(const void *point, int length);
	void sendSpellBook(P_ITEM pi);
	void sendSFX(unsigned char a, unsigned char b, bool bIncludeNearby = true);
	void sendRemoveObject(P_OBJECT po);
	void sysmsg(char* szFormat, ...);
	void sysmsg(short color, char* szFormat, ...);
	//@}

};

extern P_CHAR loginchars[MAXCLIENT];

class cClient {

private:
	P_TARGET target;
public:

	void resetTarget();
	P_TARGET newTarget( P_TARGET newTarget );
	P_TARGET getTarget();

	SERIAL spyTo; //!< used for spy command
	bool compressOut; //!< compress output with huffman coding
	bool noweather; //!< not weather
	bool lsd; //!< lsd stuff
	bool dragging; //!< true if is dragging
	bool evilDrag; //!< evil dragging, we need this for UO3D clients to save dragging history
	bool newclient; //!< is a new client
	bool firstpacket; //!< fist packet sended
	bool ingame; //!< is ingame

	cClient();
	~cClient();

};

#endif
