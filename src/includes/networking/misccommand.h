/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

/*!
\file
\brief Misc Command's sent packets
\author Flameeyes

This file contains the data for the subcommands of packet 0xBF. This packet uses
quite a lot of subcommands (and then sub-subcommands for parties) so I preferred
splitting it out of the main packets file to maintain them smaller.

\note This file \b must not be included directly, but is included by the sent.h
	file.
*/

#ifndef __PACKETS_SENT_H__
#error Never include this file directly
#endif

namespace nPackets { namespace Sent {
	/*!
	\brief Sends to a party the notify of an accepted add member invitation
		(Subcommand 0x06-0x01)
	\author Flameeyes
	
	This class is a subcommand of 0xBF-0x06 packet.
	
	This class simply sends all the party's members to all the party's
	members.
	
	\see cParty class
	*/
	class PartyAddMember : public cPacketSend
	{
	protected:
		pParty party;	//!< Party to send to the members
	public:
		PartyAddMember(pParty p)
			: party(p)
		{ }
	};
	
	/*!
	\brief Sends a party the notify of a member which left the party
		(Subcommand 0x06-0x02)
	\author Flameeyes
	
	This class is a subcommand of 0xBF-0x06 packet
	
	This class sends the serial of the player who left the party and the
	list of the members' serial remaining in the party.
	
	\see cParty class
	*/
	class PartyRemoveMember : public cPacketSend
	{
	protected:
		pPC removed;	//!< Member removed from the party
		pParty party;	//!< Party to send to the members
	public:
		PartyRemoveMember(pPC r, pParty p)
			: removed(r), party(p)
		{ }
	};
	
	/*!
	\brief Sends a party message to a specific member (Subcommand 0x06-0x03)
	\author Flameeyes
	
	This class is a subcommand of 0xBF-0x06 packet
	
	\see cParty class
	*/     
	class PartyPrivateMessage : public cPacketSend
	{
	protected:
		pPC target;		//!< Target to send the message to
		cSpeech message;	//!< Message to send
	public:
		PartyPrivateMessage(pPC t, cSpeech msg)
			: target(t), message(msg)
		{ }
		
		void prepare();
	};
	
	/*!
	\brief Sends a party message to all the party's members
		(Subcommand 0x06-0x04)
	
	This class is a subcommand of 0xBF-0x06 packet.
	
	\see cParty class
	*/
	class PartyBroadcast : public cPacketSend
	{
	protected:
		pPC sender;		//!< Member who sent the message
		cSpeech message;	//!< Message to send
	public:
		PartyBroadcast(pPC s, cSpeech msg)
			: sender(s), message(msg)
		{ }
		
		void prepare();
	};
} }
