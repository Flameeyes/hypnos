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

#include "cclient.h"

/*!
\author Flameeyes
\brief Packet to be sent
*/
class cPacketSend
{
public:
	virtual ~cPacketSend {}
	//! Prepare the buffer to be sent
	virtual void prepare() = 0;

	//! Prepare the packet if not prepared and return the buffer
	inline const UI08 *getBuffer()
	{
		if ( ! length )
			prepare();

		return buffer;
	}

	//! Prepare the packet if not prepared and return the length of the buffer
	inline const UI16 getLength()
	{
		if ( ! length )
			prepare();

		return length;
	}

	virtual void fixForClient(cClient::ClientType ct)
	{ }

private:
	cPacketSend();
protected:
	UI08 *buffer;	//!< Pointer to the buffer
	UI16 length;	//!< Length of the buffer
};

class cPacketSendAction
{
public:
	/*!
	\brief Create a cPacketSendAction
	\param s serial of the char which do the action
	\param a id of the action to execute
	*/
	inline cPacketSendAction(UI32 s, UI16 a) :
		serial(s), action(a),
		buffer(NULL), length(NULL)
	{ }

	inline ~cPacketSendAction()
	{ if ( buffer ) delete buffer; }

	void prepare();

protected:
	const UI32 serial;	//!< Serial of the char which do the action
	const UI16 action;	//!< id of the action to execute
}

#endif