/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Exceptions' handling classes
*/

#ifndef __NETWORKING_EXCEPTIONS_H__
#define __NETWORKING_EXCEPTIONS_H__

#include "common_libs.h"
#include "libhypnos/exceptions.hpp"

/*!
\class eBufferIncomplete exceptions.h "networking/exceptions.h"
\brief Incomplete buffer to receive the packet

This exception is thrown by the constructors of cPacketReceived classes which
tests for the lenght of the packet to be exact. The right way to handle this
is wait for more data and store it \b after the current one.
*/
class eBufferIncomplete : public eException
{
public:
	eBufferIncomplete()
	{ }
};

/*!
\class eErrorSending exceptions.h "networking/exceptions.h"
\brief Error sending data

This exception is thrown by every sending operation which fails after a
Cabal::TCPSocket::sendAll(), resulting in less bytes sent than the one asked.
*/
class eErrorSending : public eException
{
public:
	const uint16_t requested;
	const uint16_t sent;
	eErrorSending(uint16_t aRequested, uint16_t aSent)
		: requested(aRequested), sent(aSent)
	{ }
};

#endif
