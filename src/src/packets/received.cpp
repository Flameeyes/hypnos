/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Implementation of packets classes
*/

#include "settings.h"
#include "inlines.h"
#include "packets/received.h"
#include "packets/send.h"
#include "objects/cchar.h"
#include "objects/cpc.h"
#include "objects/cbody.h"
#include "objects/cclient.h"
#include "objects/citem/cbook.h"
#include "objects/citem/cmap.h"
#include "extras/motd.h"
#include "extras/jails.h"
#include "newbies.h"

pPacketReceive cPacketReceive::fromBuffer(uint8_t *buffer, uint16_t length)
{
	switch(buffer[0])
	{
		case 0x00: return new nPackets::Received::CreateChar(buffer, length);      	// Create Character
		case 0x01: return new nPackets::Received::DisconnectNotify(buffer, length);	// Disconnect Notification
		case 0x02: return new nPackets::Received::MoveRequest(buffer, length);		// Move Request
		case 0x03: return new nPackets::Received::TalkRequest(buffer, length);		// Talk Request
		case 0x04: return NULL;								// God mode toggle
		case 0x05: return new nPackets::Received::AttackRequest(buffer, length);	// Attack Request
		case 0x06: return new nPackets::Received::Doubleclick(buffer, length);		// Double click
		case 0x07: return new nPackets::Received::PickUp(buffer, length);		// Pick Up Item(s)
		case 0x08: return new nPackets::Received::DropItem(buffer, length);		// Drop Item(s)
		case 0x09: return new nPackets::Received::Singleclick(buffer, length);		// Single click
		case 0x12: return new nPackets::Received::ActionRequest(buffer, length);	// Request Skill/Action/Magic Usage
		case 0x13: return new nPackets::Received::WearItem(buffer, length);		// Drop - Wear Item
		case 0x22: return new nPackets::Received::MoveACK_ResyncReq(buffer, length);	// when received, this packet is a Resync Request
		case 0x2c: return new nPackets::Received::RessChoice(buffer, length);		// (Obsolete) Resurrection Menu Choice
		case 0x34: return new nPackets::Received::StatusRequest(buffer, length);	// Get Player Status
		case 0x3a: return new nPackets::Received::SetSkillLock(buffer, length);		// Set Skill Lock (receive version of packet 0x3a)
		case 0x3b: return new nPackets::Received::BuyItems(buffer, length);		// Buy Item(s)
		case 0x56: return new nPackets::Received::MapPlotCourse(buffer, length);	// Map Related
		case 0x5d: return new nPackets::Received::LoginChar(buffer, length);		// Login Character
		case 0x66: return new nPackets::Received::BookPage(buffer, length); 		// Books - Page (receive version)
		case 0x69: return NULL; 							// (Obsolete) Change Text/Emote Color
		case 0x6c: return new nPackets::Received::TargetSelected(buffer, length);	// Targeting Cursor Commands
		case 0x6f: return new nPackets::Received::SecureTrade(buffer,length);		// Secure Trading
		case 0x71: return new nPackets::Received::BBoardMessage(buffer, length);	// Bulletin Board Message
		case 0x72: return new nPackets::Received::WarModeChange(buffer, length);	// Request War Mode Change/Send War Mode status
		case 0x73: return new nPackets::Received::Ping(buffer, length); 		// Ping message
		case 0x75: return new nPackets::Received::RenameCharacter(buffer, length);	// New name for a character
		case 0x7d: return new nPackets::Received::DialogResponse(buffer, length);	// Client Response To Dialog
		case 0x80: return new nPackets::Received::LoginRequest(buffer, length);		// Login Request
		case 0x83: return new nPackets::Received::DeleteCharacter(buffer, length);	// Delete Character
		case 0x91: return new nPackets::Received::GameServerLogin(buffer, length);	// Game Server Login (Server to play selected)
		case 0x93: return new nPackets::Received::BookUpdateTitle(buffer, length);	// Books  Update Title Page (receive version of packet 0x93)
		case 0x95: return new nPackets::Received::DyeItem(buffer, length); 		// Dye item

		//Does this have to be implemented?
		case 0x98: return NULL; 							// All-names 3D (3d clients only packet, receive version 7 bytes long)

		// in old nox is not implemented. Do we need it?
		case 0x9a: return NULL; 							// Console Entry Prompt

		case 0x9b: return new nPackets::Received::RequestHelp(buffer, length);		// Request Help
		case 0x9f: return new nPackets::Received::SellItems(buffer, length); 		// Sell Reply
		case 0xa0: return new nPackets::Received::SelectServer(buffer, length); 	// Select Server
		case 0xa4: return NULL; 							// Client Machine info (It was a sort of lame spyware command .. we have no need for it :D)
		case 0xa7: return new nPackets::Received::TipsRequest(buffer, length);		// Request Tips/Notice
		case 0xac: return new nPackets::Received::GumpTextDialogReply(buffer, length);	// Gump Text Entry Dialog Reply
		case 0xad: return new nPackets::Received::UnicodeSpeechReq(buffer, length);	// Unicode speech request
		case 0xb1: return new nPackets::Received::GumpResponse(buffer, length);		// Gump Menu Selection
		case 0xb2: return new nPackets::Received::ChatMessage(buffer, length);		// Chat Message
		case 0xb5: return new nPackets::Received::ChatWindowOpen(buffer, length);	// Open Chat window
		case 0xb6: return new nPackets::Received::PopupHelpRequest(buffer, length);	// Send Help/Tip Request (popup help)
		case 0xb8: return new nPackets::Received::CharProfileRequest(buffer, length);	// Request Char Profile
		case 0xbb: return NULL; 							// Ultima Messenger (do we need this?)
		case 0xbd: return new nPackets::Received::ClientVersion(buffer, length);	// Client Version Message
		case 0xbe: return new nPackets::Received::AssistVersion(buffer, length);	// Assist Version.... does this packet really exist?
		case 0xbf: return new nPackets::Received::MiscCommand(buffer, length);		// Misc. Commands Packet
		case 0xc2: return new nPackets::Received::TextEntryUnicode(buffer, length);	// Textentry Unicode
		case 0xc8: return new nPackets::Received::ClientViewRange(buffer, length);	// Client view range
		case 0xd1: return new nPackets::Received::LogoutStatus(buffer, length);		// Logout Status
		case 0xd4: return new nPackets::Received::NewBookHeader(buffer, length);	// new Book Header
		case 0xd7: return new nPackets::Received::FightBookSelection(buffer, length);	// Fight Book: move selected
		default: return NULL;	// Discard received packet
	}
}

/*!
\brief Character creation packet
\author Chronodt
\param client client who sent the packet
\note packet 0x00

Mostly taken from old noxwizard.cpp and (vastly :) ) modified to hypnos object system
*/
bool nPackets::Received::CreateChar::execute(pClient client)
{
	// Disconnect-level encryption or transfer error check
	if	((length !=104) ||					// packet length check
		(LongFromCharPtr(buffer+1) != 0xedededed) ||		// pattern checking
		(LongFromCharPtr(buffer+5) != 0xffffffff) ||
		//(buffer[9] != 0x00) ||				// this pattern is different for some clients, so commented out (for now)
		(buffer[10] != 0x00) ||					// at least one "letter" in character name
		buffer[40] != 0x00))					// at least one "letter" in character password
	{
		client->disconnect();
		return false;
	}

	uint8_t sex			= buffer[70];
	uint16_t strength		= buffer[71];
	uint16_t dexterity		= buffer[72];
	uint16_t intelligence		= buffer[73];
	uint16_t skill1			= buffer[74];
	uint16_t skill1value1		= buffer[75];
	uint16_t skill12		= buffer[76];
	uint16_t skill1value2		= buffer[77];
	uint16_t skill3			= buffer[78];
	uint16_t skill1value3		= buffer[79];
	uint16_t SkinColor		= ShortFromCharPtr(buffer + 80) | 0x8000;
	uint16_t HairStyle		= ShortFromCharPtr(buffer + 82);
	uint16_t HairColor		= ShortFromCharPtr(buffer + 84);
	uint16_t FacialHair		= ShortFromCharPtr(buffer + 86);
	uint16_t FacialHairColor	= ShortFromCharPtr(buffer + 88);
	uint16_t StartingLocation	= ShortFromCharPtr(buffer + 90);        // from starting list
	// uint16_t unknown		= ShortFromCharPtr(buffer + 92);
	uint16_t slot			= ShortFromCharPtr(buffer + 94);
	uint32_t clientIP		= LongFromCharPtr (buffer + 96);
	uint16_t shirt_color		= ShortFromCharPtr(buffer + 100);
	uint16_t pants_color		= ShortFromCharPtr(buffer + 102);


	// Disconnect-level protocol error check (possible client hack or too many chars already present in account)
	if (
		!(client->currAccount()->verifyPassword(buffer+40)) ||				//!< Password check
		(client->currAccount()->getCharsNumber()>=nSettings::Server::getMaximumPCs()) ||//!< Max PCs per account check
		((sex !=1) && (sex != 0)) ||							//!< Sex validity check
		(strength + dexterity + intelligence > 80) ||					//!< Stat check: stat sum must be <=80
		(strength < 10)     || (strength > 60)     ||					//!< each stat must be >= 10 and <= 60
		(dexterity < 10)    || (dexterity > 60)    ||
		(intelligence < 10) || (intelligence > 60) ||
		(skillvalue1 + skillvalue2 + skillvalue3 != 100) ||				//!< Skill check : sum of skills selected must be 100
		(skillvalue1 > 50)  || (skillvalue2 > 50)  || (skillvalue3 > 50) ||		//!< each skill must be >= 0 and <= 50. Since the 3 variables are uint8_t, if they are negative they will be seen as a number surely bigger than 127 :)
		(skill1 == skill2)  || (skill2 == skill3)  || (skill3 == skill1)		//!< 3 different skills must be selected
	   )
	{
		client->disconnect();
		return false;
	}


	// Correctable-level protocol error check (mainly out of bounds color or hair/beard style)
	// Color & hairstyle boundary check
	if ((SkinColor <= 0x83EA) || (SkinColor >= 0x8422)) SkinColor = 0x83EA;
	if ((FacialHairColor <= 0x44E) || (FacialHairColor >= 0x4AD)) FacialHairColor = 0x044E;
	if ((HairColor <= 0x44E) || (HairColor >= 0x4AD)) HairColor = 0x044E;

	if (!((HairStyle >= 0x203b && HairStyle <= 0x203d ) || ( HairStyle >= 0x2044 && HairStyle <= 0x204a ))) HairStyle = 0;
	if (!(((FacialHair >= 0x203e && FacialHair <= 0x2041) || ( FacialHair >= 0x204b && FacialHair <= 0x204d )) || sex )) FacialHair = 0; //if female the beard check is always valid :P So FacialHair is put to 0

	// From here building a cBody
	pBody charbody = new cBody();

	if (sex) // 0 = male 1 = female
		charbody->setId(bodyFemale)
	else
		charbody->setId(bodyMale);

	charbody->setStrength(strength);
	charbody->setHitPoints(strength);
	charbody->setMaxHitPoints(strength);

	charbody->setDexterity(dexterity);
	charbody->setStamina(dexterity);
	charbody->setMaxStamina(dexterity);

	charbody->setIntelligence(intelligence);
	charbody->setMana(intelligence);
	charbody->setMaxMana(intelligence);

	// all skills are set to 0 in the cBody constructor, so the only skills needing to be set are the 3 selected
	charbody->SetSkill(skill1, skillvalue1 * 10);
	charbody->SetSkill(skill2, skillvalue2 * 10);
	charbody->SetSkill(skill3, skillvalue3 * 10);

	charbody->setSkinColor(SkinColor);
	charbody->setName((const char*)&buffer[10]);
	charbody->setTitle("");                         //!\todo change this setTitle to something like calculateTitle or DefaultTitle, to have standard title behaviour

	// Items are added directly to the body - Flameeyes
	nNewbies::addHairs(charbody, HairStyle, HairColor, FacialHair, FacialHairColor);
	nNewbies::giveItems(charbody, pants_color, shirt_color);
	
	//Now with the body completed and data verified, building cPC
	pPC pc = new cPC();

	pc->setBody(charbody);
	pc->setTrueBody(charbody);
	pc->npc=false;
	pc->SetPriv(defaultpriv1);
	pc->SetPriv2(defaultpriv2);
	pc->setProfile("");

	pc->MoveTo(nNewbies::startLocations[StartingLocation]->position);

	pc->setDirection(dirNorth);	//should be facing north. not that it matters :D
	pc->namedeedserial=INVALID;
	for (int ii = 0; ii < skTrueSkills; i++) Skills::updateSkillLevel(pc, ii);  //updating skill levels for pc

	// Assignation of new Char to user account
	client->currAccount()->addCharToAccount( pc );

	client->startchar(); //!\todo move startchar from network to cClient
	//clientInfo[s]->ingame=true;
	return true;
}

/*!
\brief Disconnection Request packet
\param client client who sent the packet
\author Chronodt
\note packet 0x01
*/

bool nPackets::Received::DisconnectNotify::execute(pClient client)
{
	if ((length != 5) || (LongFromCharPtr(buffer+1) != 0xffffffff)) return false;
	client->disconnect();
	return true;
}

/*!
\brief Move Request packet: client trying to move PC
\author Chronodt
\param client client who sent the packet
\note packet 0x02
*/

bool nPackets::Received::MoveRequest::execute (pClient client)
{
	if (length != 7) return false;
	pPC pc = client->currChar();
	if(!pc) return false;

	walking(pc, buffer[1], buffer[2]); // buffer[1] = direction, buffer[2] = sequence number
	pc->disturbMed();
	return true;

}

/*!
\brief Talk Request packet (NOT unicode!)
\author Chronodt
\param client client who sent the packet
\note packet 0x03
*/

bool nPackets::Received::TalkRequest::execute (pClient client)
{
	uint16_t size = ShortFromCharPtr(buffer + 1);
	if (length != size) return false;
	pPC pc = client->currChar();
	if(!pc) return false;

	cSpeech speech = cSpeech(std::string(buffer + 8));
	speech.setSpeaker(pc);
	speech.setMode(buffer[3]);
	speech.setColor(ShortFromCharPtr(buffer + 4));
	speech.setFont(ShortFromCharPtr(buffer + 6));
	client->talking(speech);
	return true;
}

/*!
\brief Attack Request Packet
\author Chronodt
\param client client who sent the packet
\note packet 0x05
*/
bool nPackets::Received::AttackRequest::execute (pClient client)
{
	if (length != 5) return false;

	pPC pc = client->currChar();
	if(!pc) return false;

	pChar victim = cSerializable::findCharBySerial(LongFromCharPtr(buffer + 1));  //victim may be an npc too, so it is a pChar
	if(!victim) return false;

	if( pc->isDead() )
		pc->deadAttack(victim);
	else if( nJails::isJailed(pc) )
		client->sysmessage("There is no fighting in the jail cells!");
	else
		pc->attackStuff(victim);
	return true;
}

/*!
\brief Doubleclick Packet
\author Chronodt
\param client client who sent the packet
\note packet 0x06
*/

bool nPackets::Received::Doubleclick::execute(pClient client)
{
	if (length != 5) return false;

	pPC pc = client->currChar();
	if(!pc) return false;

	uint32_t serial = LongFromCharPtr(buffer +1);

	if (cSerializable::isCharSerial(serial))
	{
		pChar pd = cSerializable::findCharBySerial(serial);
		if(!pd) return false;

		pd->doubleClick(client);
		return true;
	}

	pItem pi = cSerializable::findItemBySerial(serial);
	if(!pi) return false; //If it's neither a char nor an item, then it's invalid

	pi->doubleClick(client);
	return true;
}

/*!
\brief Pickup Item Packet
\author Chronodt
\param client client who sent the packet
\note packet 0x07
*/

bool nPackets::Received::PickUp::execute(pClient client)
{
	if (length != 7) return false;
	pItem pi = cSerializable::findItemBySerial(LongFromCharPtr(buffer+1));
	if(!pi) return false;

	uint16_t amount = ShortFromCharPtr(buffer+5);
	client->get_item(pi, amount);  //!< if refused, the get_item automatically bounces the item back
	return true;
}

/*!
\brief Drop Item Packet
\author Chronodt
\param client client who sent the packet
\note packet 0x08
*/

bool nPackets::Received::DropItem::execute(pClient client)
{
	if (length != 14) return false;
	pItem pi = cSerializable::findItemBySerial(LongFromCharPtr(buffer+1));
	if(!pi) return false;

	sLocation drop_at = sLocation(ShortFromCharPtr(buffer+5), ShortFromCharPtr(buffer+7), buffer[9]);
	uint32_t destserial LongFromCharPtr(buffer+10);
	if (destserial == 0xffffffff) client->drop_item(pi, drop_at, NULL); //if dropped in world, there is no item/char with that serial (avoiding a serial search)
	pSerializable destination = cSerializable::findBySerial(destserial);
	client->drop_item(pi, drop_at, destination);
	return true;
}


/*!
\brief Singleclick Packet
\author Chronodt
\param client client who sent the packet
\note packet 0x09
*/
bool nPackets::Received::Singleclick::execute(pClient client)
{
	if (length != 5) return false;
	uint32_t serial = LongFromCharPtr(buffer + 1);
	if ( cSerializable::isCharSerial( serial ) )
	{
		pChar pc = cSerializable::findCharBySerial(serial);
		if(!pc) return false;

		pc->SingleClick( client);
	}
	else
	{
		pItem pi = cSerializable::findItemBySerial(serial);
		if(!pi) return false;

		pi->SingleClick( client );
	}
	return true;
}

/*!
\brief Action Request Packet
\author Chronodt
\param client client who sent the packet
\note packet 0x12

\todo check the last if.... why does it do that check?????? it has no sense...
*/

bool nPackets::Received::ActionRequest::execute(pClient client)
{
	uint16_t size = ShortFromCharPtr(buffer + 1);
	if (length != size) return false;

	uint8_t type = buffer[3];
	pChar pc = client->currChar();
	if(!pc) return false;

	if (type==0xC7) // Action
	{
		if (pc->isMounting()) return true;
		if (!(strcmp(buffer + 4,"bow"))) pc->playAction(0x20);
		if (!(strcmp(buffer + 4,"salute"))) pc->playAction(0x21);
	        return true; // Morrolan
	}
	else if (type) // Skill
	{
		uint16_t i=4;
		while ( (buffer[i]!=' ') && (i < size) ) i++;
		buffer[i]=0;
		Skills::SkillUse(client, str2num(buffer + 4));
		return true;
	}
	else if ((type==0x27)||(type==0x56))  // Spell
	{
		pItem p_j = NULL;
		pItem pack= pc->getBackpack();
		int book = 0;
		if(pack) //lb
		{
			NxwItemWrapper gri;
			gri.fillItemsInContainer( pack, false );
			gri.fillItemWeared( pc_currchar, true, true, false );
			for( gri.rewind(); !gri.isEmpty(); gri++ )
			{
				pItem pj=gri.getItem();
				if (pj)
					if (pj->type==ITYPE_SPELLBOOK)
					{
						p_j=pj;
						break;
					}
			}
		}
		if (p_j!=NULL)
		{
			book=buffer[4]-0x30;
			if (buffer[5]>0x20)
			{
				book=(book*10)+(buffer[5]-0x30);
			}
			if (pc->dead)
			{
				client->sysmessage("Ethereal souls really can't cast spells");
			}
			else
			{
				if (pc->isFrozen())
				{
					if (pc->casting) client->sysmessage("You are already casting a spell.");
					else client->sysmessage("You cannot cast spells while frozen.");
				}
				else
				{
					if (!pc->knowsSpell(static_cast<magic::SpellId>(book-1)))
					{
						client->sysmessage("You don't know that spell yet.");
					}
					else
					{
						magic::beginCasting(static_cast<magic::SpellId>(book-1),client,magic::CASTINGTYPE_SPELL);
					} // if don't knows spell
				} // if frozen
			} // if alive
		} // if has spellbook
	}  // if spell
	else
	{
		if (ShortFromCharPtr(buffer + 2) == 0x0543)  // Open spell book  //Chronodt: ?????? what is this?
		{
			client->sendSpellBook(NULL);
		}
		return true;
	}
	return false; //If flow arrives here, maybe invalid data in packet
}


/*!

\brief Wear Item Packet
\author Chronodt
\param client client who sent the packet
\note packet 0x13
*/


bool nPackets::Received::WearItem::execute(pClient client)
{
	if (length != 10) return false;

	pChar pc = cSerializable::findCharBySerial(LongFromCharPtr(buffer+6));
	if(!pck) return false;

	pItem pi = cSerializable::findItemBySerial(LongFromCharPtr(buffer+1));
	if(!pir) return false;

	client->wear_item(pc, pi);
	return true;
}


/*!
\brief Resync Request Packet
\author Chronodt
\param client client who sent the packet
\note packet 0x22
*/

bool nPackets::Received::MoveACK_ResyncReq::execute(pClient client)
{
	if (length != 3) return false;
	pPC pc = client->currChar();
	if(!pc) return false;

	uint8_t sequence  = buffer[1];
	if( !sequence && !buffer[2] ) client->currChar()->teleport();  //Resync request
	else
	{
	//!\todo verification of sequence for move acknowdledge
	}
	return true;
}

/*!
\brief (probably very much obsolete) Resurrection Choice Menu Packet
\author Chronodt
\param client client who sent the packet
\note packet 0x2c
*/

bool nPackets::Received::RessChoice::execute(pClient client)
{
	if (length != 2) return false;
	if(buffer[1]==0x02)
	{
		pChar murderer=cSerializable::findCharBySerial(client->currChar()->murdererSer);
		if( murderer && SrvParms->bountysactive )
		{
			client->sysmessage("To place a bounty on %s, use the command BOUNTY <Amount>.", murderer->getCurrentName().c_str() );
		}
		client->sysmessage("You are now a ghost.");
	}
	if(buffer[1]==0x01)
	client->sysmessage("The connection between your spirit and the world is too weak.");
	return true;
}

/*!
\brief Receive Status Request Packet: client asks for status or skilllist
\author Chronodt
\param client client who sent the packet
\note packet 0x34
*/

bool nPackets::Received::StatusRequest::execute(pClient client)
{
	if (length != 10) return false;
	if ( client->currChar() != NULL )
	{
		if (buffer[5]==4) client->statusWindow(cSerializable::findCharBySerial(LongFromCharPtr(buffer + 6)), false); //!< NOTE: packet description states sending basic stats, so second argument is false. Correct if necessary
		if (buffer[5]==5) client->skillWindow();
	}
	return true
}

/*!
\brief Set Skill Lock Packet
\author Chronodt
\param client client who sent the packet
\note packet 0x3a
*/

bool nPackets::Received::SetSkillLock::execute(pClient client)
{
	uint16_t size = ShortFromCharPtr(buffer + 1);
	if (length != size) return false;
	// client 1.26.2b+ skill managment packet
	// -> 0,1,2,3 -> ignore them
	// -> 4 = skill number
	// -> 5 = 0 raising (up), 1 falling=candidate for atrophy, 2 = locked
	if ( client->currChar() ) client->currChar()->lockSkill[buffer[4]] = buffer[5]; // save skill managment changes
	return true;
}

/*!
\brief Buy Items Packet
\author Chronodt
\param client client who sent the packet
\note packet 0x3b

\note this packet contains all items purchased by player in buy gump
\note with current buffer implementation in csocket.cpp (1024 bytes) we can handle up to 135 different stacks of purchased items in a single go. Should be enough :D
*/

bool nPackets::Received::BuyItems::execute(pClient client)
{
	uint16_t size = ShortFromCharPtr(buffer + 1);
	if (length != size) return false;

	std::list< sBoughtItem > allitemsbought;

	pNpc npc = dynamic_cast<pNpc>(cSerializable::findCharBySerial(LongFromCharPtr(buffer +3)));  //only npc can sell you items with a menu :D
	if(!npc) return false;

	uint16_t itemtotal=(size - 8)/7;
	if (itemtotal>256) return false;

	for(register int i=0;i<itemtotal;i++)
	{
		uint16_t pos=8+(7*i);

		sBoughtItem b;

		b.layer=buffer[pos];
		uint32_t itemserial = LongFromCharPtr(buffer + pos + 1);
		b.item=cSerializable::findItemBySerial(itemserial);
		if(!b.item)
			continue;
		b.amount=ShortFromCharPtr(buffer + pos + 5);
		allitemsbought.push_back( b );
	}
	client->buyaction(npc, allitemsbought);
	return true;
}

/*!
\brief Receive map pins commands (add, remove, etc.)
\author Chronodt
\param client client who sent the packet
\note packet 0x56

\todo to be completed when treasure maps done
*/

bool nPackets::Received::MapPlotCourse::execute(pClient client)
{
	if (length != 10) return false;

	pMap map = dynamic_cast<pMap>(cSerializable::findItemBySerial(LongFromCharPtr(buffer + 1)));
	if(!map) return false;

	PlotCourseCommands command	= buffer[5];
	int pin				= buffer[6];
	uint16_t x			= ShortFromCharPtr(buffer + 7);
	uint16_t y			= ShortFromCharPtr(buffer + 9);

	switch(command)
	{
		case pccAddPin:
			return map->addPin(x, y);
		case pccInsertPin:
			return map->insertPin(x,y,pin);
		case pccChangePin:
			return map->changePin(x,y,pin);
		case pccRemovePin:
			return map->removePin(pin);
		case pccClearAllPins:
			return map->clearAllPins();
		case pccToggleWritable:
			return map->toggleWritable(this);
	}
	return false;
}

/*!
\brief One packet of the login sequence
\author Chronodt
\param client client who sent the packet
\note packet 0x5d

\todo do the login sequence packets
*/
bool nPackets::Received::LoginChar::execute(pClient client)
{
	if (length != 73) return false;
	if (LongFromCharPtr(buffer+1) != 0xedededed) return false;	//pattern check


	//!\todo revise from here


	loginchars[s] = NULL;

	pChar pc_k=NULL;

	if (acctno[s]>INVALID)
	{
		int j=0;
		Accounts->SetOffline(acctno[s]);
		NxwCharWrapper sc;
		Accounts->GetAllChars( acctno[s], sc );
		for( sc.rewind(); !sc.isEmpty(); sc++ ) {
			pChar pc_i=sc.getChar();
			if(!pc_i)
				continue;
			if (j==buffer[s][0x44]) {
				pc_k=pc_i;
				break;
			}
			j++;
		}

		if (pc_k)
		{
			pc_k->setClient(NULL);
			int32_t nSer = pc_k->getSerial();
			for ( int32_t idx = 0; idx < now; idx++ ) {
				if ( pc_k == loginchars[idx] ) {
					// TODO We need to fix this!!!
					nPackets::Sent::IdleWarning pk(0x5);
					client->sendPacket(&pk);
					Disconnect(s);
					Disconnect(idx);
					return;
				}
			}

			Accounts->SetOnline(acctno[s], pc_k);
			pc_k->logout=INVALID;

			loginchars[s] = pc_k;

			pc_k->setClient(new cNxwClientObj(s));
			startchar(s);
		}
		else
		{
			nPackets::Sent::IdleWarning pk(0x5);
			client->sendPacket(&pk);
			client->disconnect();
		}
	}
}

/*!
\brief Receive changed book page
\author Flameeyes & Chronodt
\param client client who sent the packet
\note packet 0x66
*/

bool nPackets::Received::BookPage::execute(pClient client)
{
	uint16_t size = ShortFromCharPtr(buffer + 1);
	if (length != size) return false;

	pItem book = dynamic_cast<pBook>(cSerializable::findItemBySerial(LongFromCharPtr(buffer + 3)));
	if(!book) return false;

	if ( book->isReadOnly() )
		book->sendPageReadOnly(client, book, ShortFromCharPtr(buffer + 9));
	else
		book->changePages(buffer + 13, ShortFromCharPtr(buffer + 9), ShortFromCharPtr(buffer + 11), size - 13);
	return true;
}

/*!
\brief Receive the target selected by player
\author Chronodt
\param client client who sent the packet
\note packet 0x6c

\todo finish when targets redone
*/


bool nPackets::Received::TargetSelected::execute(pClient client)
{
	if (length != 19) return false;
	pTarget target = client->getTarget();
	if( !target) return false; //maybe it CAN return true if this packet is sent on targeting abortion. verify it! If true, add targeting abortion code instead of returning


	//! \todo finish to update this when targets redone
#if 0
	target->receive( ps );

					if( !target->isValid() )
						target->error( ps );
					else
						target->code_callback( ps, target );
#endif
	return true;
}

/*!
\brief Receive secure trading message
\author Chronodt
\param client client who sent the packet
\note packet 0x6f
*/

bool nPackets::Received::SecureTrade::execute(pClient client)
{
	uint16_t size = ShortFromCharPtr(buffer + 1);
	uint32_t serial = LongFromCharPtr(buffer +4);

	if (length != size) return false;

	pContainer container = dynamic_cast<pContainer>(cSerializable::findItemBySerial(serial));
	if (!container) return false;
	sSecureTradeSession session = client->findTradeSession(container);
	if (!session.tradepartner) return false;

	//container is now this client's secure trading container (a temporary container wich holds the trade items)

	switch(buffer + 3) //Buffer + 3 = Action byte
	{
	case 0://Start trade - Never happens, sent out by the server only.
		return false;
	case 1://Cancel trade. Send each person cancel messages, move items back to owners
		client->endtrade(session);
		break;
	case 2://Change check marks. Possibly conclude trade

		sSecureTradeSession session2 = session.tradepartner->findTradeSession(client);
		if (!session2.tradepartner) return false;

		session->status1  = buffer[11];
		session2->status2 = buffer[11];
		client->sendtradestatus(session);
		if (session->status1 && session->status2) client->dotrade(session);
		break;
	default:
		return false;
	}
	return true;
}


/*!
\brief Receive bullettin board message
\author Chronodt
\param client client who sent the packet
\note packet 0x71

\note Packets flow is:
1) client doubleclicks the board (received 0x06 packets and doubleclick routine works with that)
2) server sends 0x71 command 0 to tell client to open the bulletin board gump and await message data
3) server sends 0x25 (add items to container) to tell to the client the serial numbers of all messages in the board
4) the client replies to 0x25 sending a 0x71 message 4 for each post in the board to get the details of each
   (poster, date and topic)
5) server replies to each 0x71 message 4 sent with a 0x71 message 1 with the required details

if the clients doubleclicks on a message, it sends 0x71 command 3 and the server replies with 0x71 command 2 with
the body of the message
*/


bool nPackets::Received::BBoardMessage::execute(pClient client)
{
	uint16_t size = ShortFromCharPtr(buffer + 1);
	if (length != size) return false;

	// Message \x71 has numerous uses for the Bulletin Board
	// so we need to get the type of message from the client first.

	int msgType = buffer[3];
	pMsgBoard msgboard = dynamic_cast<pMsgBoard>(cSerializable::findItemBySerial(LongFromCharPtr(buffer + 4)));
	pMsgBoardMessage message = dynamic_cast<pMsgBoardMessage>(cSerializable::findItemBySerial(LongFromCharPtr(buffer + 8)));
	//for msgtypes 3, 4, 6 message is the message on which operate, on message 5 it is parent message (message reply)
	if(!msgboard) return false;

	// message is validated inside the switch because subcommand 5 does not need it and may me invalid without consequences
	// in addition an unvalid message means that it has just been deleted by someone else

	switch (msgType)
	{
		case 3:  // Client->Server: Client has dbl-clicked on subject, requesting body of message
		{
			if (message == NULL)
			{
				// Possible situation: two users are browsing a msg board with the same message
				// and the first user deletes that message. The second still sees it in the summary
				// and can stil TRY to open it. We must tell the second player that the message is
				// no longer available and remove it from his/her list.
				nPackets::Sent::DeleteObj pk(LongFromCharPtr(buffer + 8));
				client->sendPacket(&pk);
				client->sysmessage("This message has just been deleted by someone else");
			}
			else
			{
				nPackets::Sent::BBoardCommand pk(msgboard, SendMessageBody, message);
				client->sendPacket(&pk);
			}
			break;
		}

		case 4:  // Client->Server: Client has ACK'ed servers download of posting serial numbers
			 // and requires the summary of (buffer + 8) message (topic, poster id and date)
		{
			if(!message) return false;

			// Server needs to handle ACK from client that contains the posting serial numbers
			msgboard->sendMessageSummary( client, message );
			break;
		}

		case 5:  // Client->Server: Client clicked on Post button (either from the main board or after pressing the Reply)
		{        //                 Reply just switches to the Post item.

			pChar pc = client->currChar();
			if(!pc) return false;

			// Check privledge level against server.cfg msgpostaccess
			if ( !(pc->isGM()) && !(SrvParms->msgpostaccess) )
			{
				client->sysmessage("Thou art not allowed to post messages.");
				return false;
			}

			uint32_t msgSN = LongFromCharPtr(buffer + 8);

			// If this is a reply to anything other than a LOCAL post, abort
			if ( msgSN>0)
			{
				if(!message) return false;
				if ( (message->availability != LOCALPOST) && !global::canReplytoGlobalMsgBoardPosts() )
				{
					#ifdef DEBUG
					ErrOut("MsgBoard: Attempted reply to a global or regional post\n");
					#endif
					client->sysmessage( "You can not reply to global or regional posts");
					return false;
				}
			}


			int subjectlen = buffer[9];
			pMsgBoardMessage newmessage = new cMsgBoardMessage(); 	//creating new message

			//filling new message with data

			newmessage->poster = pc->getSerial();
			newmessage->availability = pc->postType;
			if (pc->postType == REGIONALPOST) newmessage->region = msgboard->getRegion();
			newmessage->subject = std::string(buffer + 10);
			newmessage->body = std::string(buffer + 10 + subjectlen, length - 11 - subjectlen);
			// We need to use this string constructor because there may be many null-terminated strings, and since no
			// process is needed by the server, it is much easier to copy the bulk of the body in a string and just
			// give it back to a client that asks for it :D
			newmessage->setContainer(msgboard);
			newmessage->replyof = msgSN;

			if (!msgboard->addMessage( newmessage ))
			{
				if (pc->postType == LOCALPOST) client->sysmessage("This Message Board has too many messages!" );
				newmessage->Delete(); //if could not link, message should be deleted
			}
			else
			{
				// Send "Add Item to Container" message to client
				nPackets::Sent::ShowItemInContainer pk(newmessage);
				client->sendPacket(&pk);
			}

			break;
		}

	case 6:  // Remove post from Bulletin board
		{
			if (message == NULL)
			{
				// Possible situation: two users are browsing a msg board with the same message
				// and the first user deletes that message. The second still sees it in the summary.
				// Since both are trying to delete it, the result is the same, only we do not have
				// to delete the same item again :D
				nPackets::Sent::DeleteObj pk(LongFromCharPtr(buffer + 8));
				client->sendPacket(&pk);
			}
			else
			{
				if(!message) return false;

				pChar pc= client->currChar();
				if(!pc) return false;

				if ( (pc->isGM()) || (SrvParms->msgpostremove) )
				{
					if ( global::onlyPosterCanDeleteMsgBoardMessage() && (pc->getSerial() != message->poster && !pc->isGM() && (pc->getSerial() != msgboard->getOwner() || message->availability != LOCALPOST )))
						client->sysmessage("You are not allowed to delete this message");
					else
					{
						// if onlyPosterCanDeleteMsgBoardMessage() is true, the only ones
						// who can delete a post are: a gm, the poster and the owner of the
						// msgboard, but the latter only if the message is local
						nPackets::Sent::DeleteObj pk(LongFromCharPtr(buffer + 8));
						client->sendPacket(&pk);
                                        	message->Delete();
					}
				}

			}
			break;
		}


	default:
		return false;
	}
	return true;
}

/*!
\brief Receive warmode change request
\author Chronodt
\param client client who sent the packet
\note packet 0x72
*/

bool nPackets::Received::WarModeChange::execute(pClient client)
{
	if (length != 5) return false;
	pPC pc = client->currChar();
	if( pc )
	{
		if( (pc->inWarMode() && !buffer[1]) || (!pc->inWarMode() && buffer[1]) )  //Translation: if warmode has to change
		{
			pc->toggleWarMode();
			pc->targserial=INVALID;
		}
		// Now we send the packet back :D
		nPackets::Sent::WarModeStatus pk(buffer);
		client->sendPacket(&pk);

		if (pc->dead && pc->war) // Invisible ghost, resend.
			pc->teleport( teleNone );
		client->playMidi();
		pc->disturbMed();
		return true;
	}
	return false;
}

/*!
\brief client Ping
\author Chronodt
\param client client who sent the packet
\note packet 0x73
*/

bool nPackets::Received::Ping::execute(pClient client)
{
	if (length != 2) return false;
	//All this function has to do is send back the packet to the client :D (well if it isn't a ping! :D)
	nPackets::Sent::PingReply pk(buffer);
	client->sendPacket(&pk);
	return true;
}

/*!
\brief Character rename
\author Chronodt
\param client client who sent the packet
\note packet 0x75
*/

bool nPackets::Received::RenameCharacter::execute(pClient client)
{
	if (length != 35) return false;
	pChar pc = cSerializable::findCharBySerial(LongFromCharPtr(buffer + 1));
	if(pc && ( client->currChar()->isGMorCounselor() || client->currChar()->isOwnerOf( pc ) ) )
	{
		pc->setCurrentName( buffer + 5 );
		return true;
	}
	return false;
}

/*!
\brief dialog callback
\author Chronodt
\param client client who sent the packet
\note packet 0x7d
*/


bool nPackets::Received::DialogResponse::execute(pClient client)
{
	if (length != 13) return false;

	//!\todo check menus

	Menus.handleMenu( client );

	return true;
}

/*!
\brief Login request
\author Chronodt
\param client client who sent the packet
\note packet 0x80

\todo another login sequence packet, still to do
*/


bool nPackets::Received::LoginRequest::execute(pClient client)
{
	//!This is the first packet a client sends to the server in the login sequence
	if (length != 62) return false;

	//! \todo crypting code handling here
/*

	if ( clientCrypter[s] != NULL )
	clientCrypter[s]->setCryptMode(CRYPT_LOGIN) ;
	clientInfo[s]->firstpacket=false;
	LoginMain(s);
*/
	//!\todo Add code from loginmain in network.cpp and update it
	return true;
}

/*!
\brief Player tries to delete his char (in the login menu)
\author Chronodt
\param client client who sent the packet
\note packet 0x83
*/

bool nPackets::Received::DeleteCharacter::execute(pClient client)
{
	if (length != 39) return false;
	uint32_t index    = LongFromCharPtr (buffer + 31); // index of character to delete
	uint32_t clientip = LongFromCharPtr (buffer + 35);

/* PkG 0x85,
 *      0x00 => That character password is invalid.
 *      0x01 => That character doesn't exist.
 *      0x02 => That character is being played right now.
 *      0x03 => That charater is not old enough to delete.
		The character must be 7days old before it can be deleted.
 *      0x04 => That character is currently queued for backup and cannot be
 *              deleted.
 *      0x05 => Couldn't carry out your request.
 */

	pAccount account = client->currAccount();
	if(!account->verifyPassword(buffer+1))
	{
		// Password invalid
		nPackets::Sent::CharDeleteError pk(0x0);
		client->sendPacket(&pk);
		return true;
	}

	pPC TrashMeUp = account->getChar(index);	//PC to delete. if index is too large or invalid, it returns NULL

	if ( nSettings::Server::canPlayersDeletePCs() )
	{
		if(!TrashMeUp)
		{
			// Character does not exist
			nPackets::Sent::CharDeleteError pk(0x1);
			client->sendPacket(&pk);
			return true;
		}

		if (TrashMeUp)
		{
			if( SrvParms->checkcharage && (getclockday() < TrashMeUp->getCreationDay() + 7) )
			{
				// Character is too young to die :D
				nPackets::Sent::CharDeleteError pk(0x3);
				client->sendPacket(&pk);
				return true;
			}

			if(TrashMeUp->isOnline())
			{
				// Character is being played right now!
				nPackets::Sent::CharDeleteError pk(0x2);
				client->sendPacket(&pk);
				return true;
			}
			// Character Deletion
			TrashMeUp->Delete();

			nPackets::Sent::CharAfterDelete pk(account);
			client->sendPacket(&pk);

			return true; // All done ;]
		}
	}
	// sending message "0x05 => Couldn't carry out your request"
	nPackets::Sent::CharDeleteError pk(0x5);
	client->sendPacket(&pk);
	return true;
}

/*!
\brief game server login (?)
\author Chronodt
\param client client who sent the packet
\note packet 0x91

\todo another login sequence packet. Still to do
*/

bool nPackets::Received::GameServerLogin::execute(pClient client)
{
	if (length != 65) return false;

	//! \todo another login packet. Revise when encryption done
	/*
	clientInfo[s]->firstpacket=false;
	clientInfo[s]->compressOut=true;
	if ( clientCrypter[s] != NULL )	clientCrypter[s]->setEntering(false);
	CharList(s);  */
	return true;
}

/*!
\brief (old packet) Update title and author of book
\author Chronodt
\param client client who sent the packet
\note packet 0x93

\note still sent by old clients, but usually it sends packet 0xd4 instead
*/

bool nPackets::Received::BookUpdateTitle::execute(pClient client)
{
	if (length != 99) return false;
	char author[30], title[60];

	pBook book = dynamic_cast<pBook>(cSerializable::findItemBySerial(serial));
	if ( !book ) return false;

	//so if clients somehow does not send a null terminated string, we zero the last character to be sure
	strncpy(title, buffer + 9, 59); title[59] = 0;
	strncpy(author, buffer + 69, 29); author[29] = 0;

	book->changeAuthor(author);
	book->changeTitle(title);

	return true;
}

/*!
\brief color selected from dye menu
\author Chronodt
\param client client who sent the packet
\note packet 0x95
*/


bool nPackets::Received::DyeItem::execute(pClient client)
{
	if (length != 9) return false;

	uint32_t serial = LongCharFromPtr(buffer + 1);
	pItem pi = cSerializable::findItemBySerial(serial);
	pChar pc = cSerializable::findCharBySerial(serial);
	color = ShortFromCharPtr(buffer + 7);
	if (pi) pi->dyeItem(client, color);
	else if (pc) pc->dyeChar(client, color);
	else return false;
	return true;
}

/*!
\brief GM page request
\author Chronodt
\param client client who sent the packet
\note packet 0x9b
*/


bool nPackets::Received::RequestHelp::execute(pClient client)
{
	if (length != 258) return false;
	//bytes from buffer + 1 to buffer + 257 i believe are the page text... i hope ...
	//but 257 bytes is a STRANGE number. Check if there isn't a 2byte length or something in it
	gmmenu(s, 1);
	return true;
}

/*!
\brief Receive list of to-be-sold items
\author Chronodt
\param client client who sent the packet
\note packet 0x9f
*/

bool nPackets::Received::SellItems::execute(pClient client)
{
	uint16_t size = ShortFromCharPtr(buffer + 1);
	if (length != size) return false;

	std::list< sBoughtItem > allitemssold;

	pNpc npc = (pNpc)cSerializable::findCharBySerial(LongFromCharPtr(buffer + 3));
	if(!npc) return false;

	int itemtotal=ShortFromCharPtr(buffer + 7);
	if ((itemtotal>256) || (itemtotal == 0)) return false;

	for(i=0;i<itemtotal;i++)
	{
		int pos=9+(6*i);

		sBoughtItem b;

		b.item=cSerializable::findItemBySerial(LongFromCharPtr(buffer + pos));
		if(!b.item)
			continue;
		b.amount=ShortFromCharPtr(buffer + pos + 4);
		allitemssold.push_back( b );
	}
	client->sellaction(npc, allitemssold);
        return true;
}

/*!
\brief Server selected
\author Chronodt
\param client client who sent the packet
\note packet 0xa0

\todo still another login packet. They begin to tire me.... :D
*/

bool nPackets::Received::SelectServer::execute(pClient client)
{
	if (length != 3) return false;
	//!\todo another login packet, and another crypting function to update
	Relay(client);
	return true;
}

/*!
\brief Tips request
\author Chronodt
\param client client who sent the packet
\note packet 0xa7
*/

bool nPackets::Received::TipsRequest::execute(pClient client)
{
	if (length != 4) return false;
	uint16_t i = ShortFromCharPtr(buffer + 1);	// tip index
	uint8_t want_next = buffer[3];			// tip next/prev flag
	std::string msg;				// message to send

	if(want_next) i = i+1;
	else i = i-1;

	if (i==0) i=1;

	msg = nMOTD::getTip(i);

	nPackets::Sent::TipsWindow pkTips(msg, 0x00, i);
	client->sendPacket(&pkTips);
	return true;
}

/*!
\brief Text edit change in gump callback
\author Chronodt
\param client client who sent the packet
\note packet 0xac
*/

bool nPackets::Received::GumpTextDialogReply::execute(pClient client)
{
	uint16_t size = ShortFromCharPtr(buffer + 1);
	if (length != size) return false;
	//!\todo gump remake -_-
	return true;
}

/*!
\brief Unicode speech from player
\author Chronodt
\param client client who sent the packet
\note packet 0xad
*/

bool nPackets::Received::UnicodeSpeechReq::execute(pClient client)
{
	uint16_t size = ShortFromCharPtr(buffer + 1);
	if (length != size) return false;

	pPC pc = client->currChar();
	if(!pc) return false;
	uint8_t mode   = buffer[3];	//0=say,2=emote,8=whisper,9=yell
	uint16_t color = ShortFromCharPtr(buffer + 4);
	uint16_t font  = ShortFromCharPtr(buffer + 6);
	uint32_t language = LongFromCharPtr(buffer + 8);
	int offset = 12;
	pc->unicode=true;
	// Check for command word versions of this packet
	if ( mode >=0xc0 )
	{
		mode &= 0x0F;	// set to normal (cutting off the speech.mul indicator)
		int num_words = ((uint16_t)buffer[12] << 4) & ((uint16_t)buffer[13] >> 4);
		int speech_mul_index = (((uint16_t) buffer[13] & 0xf) << 8) & (uint16_t)buffer[14];
		//I suspect these [num_unknown] bytes here are also speech.mul keyword indexes (12 bits each index)
		int num_unknown = ( num_words / 2 ) * 3 + (num_words & 1) - 2;
		offset += 3 + num_unknown; //in the remainder of code we can ignore these bytes
	}
	cSpeech text = cSpeech(buffer + offset);
	text.setMode(mode);
	text.setColor(color);
	text.setFont(font);
	text.setLanguage(language);
	text.setSpeaker(pc);
	text.setMode(mode);
	client->talking(text);
	return true;
}

/*!
\brief Generic/custom gump callback
\author Chronodt
\param client client who sent the packet
\note packet 0xb1
*/

bool nPackets::Received::GumpResponse::execute(pClient client)
{
	uint16_t size = ShortFromCharPtr(buffer + 1);
	if (length != size) return false;
	//!\todo gump remake
	Menus.handleMenu( ps );
	return true;
}

/*!
\brief text to send in chatroom
\author Chronodt
\param client client who sent the packet
\note packet 0xb2
*/

bool nPackets::Received::ChatMessage::execute(pClient client)
{
	uint16_t size = ShortFromCharPtr(buffer + 1);
	if (length != size) return false;
	//!\todo chat implementation
	//NOTE: old nox had absolutely NO support for ingame chat. We have to write it from scrap
	return true;
}

/*!
\brief Opens chat window
\author Chronodt
\param client client who sent the packet
\note packet 0xb5
*/

bool nPackets::Received::ChatWindowOpen::execute(pClient client)
{
	if (length != 64) return false;
	//!\todo chat implementation
	//NOTE: old nox had absolutely NO support for ingame chat. We have to write it from scrap
	return true;
}

/*!
\brief Popup Help Request
\author Chronodt
\param client client who sent the packet
\note packet 0xb6
*/

bool nPackets::Received::PopupHelpRequest::execute(pClient client)
{
	if (length != 9) return false;

	// T2A Popuphelp request
	if (!nSettings::Server::isEnabledPopupHelp()) return false;

	uint32_t serial = LongCharFromPtr(buffer +1);
	pSerializable p = cSerializable::findBySerial(serial);
	if ( ! p ) return false;

	std::string descr;
	if ( client->currChar() && client->currChar()->canSeeSerials() )
	{
		char *temp;
		asprintf(&temp, "serial : %x", p->getSerial());
		descr = std::string(temp);
		free(temp);
	} else {
		descr = p->getPopupHelp();

	if ( !descr.size()) return true;		//We have parsed the packet, but nothing had to return...
	nPackets::Sent::PopupHelp pk(descr, p);
	client->sendPacket(&pk);
	return true;
}

/*!
\brief Character profile request
\author Chronodt
\param client client who sent the packet
\note packet 0xb8
*/

bool nPackets::Received::CharProfileRequest::execute(pClient client)
{
	//NOTE: this packet is poorly documented -_-

	uint16_t size = ShortFromCharPtr(buffer + 1);
	if (length != size) return false;
	uint32_t serial = LongCharFromPtr(buffer + 4);

	pPC pc = client->currChar();
	pPC who= dynamic_cast<pPC>(cSerializable::findCharBySerial(serial));

	if( !pc || !who ) return false;

	if( buffer[3])
	{ //update profile
		if( ( serial!=pc->getSerial() ) && !pc->isGMorCounselor() )
			return true; //lamer fix, but packet still processed
		int profilesize = ShortFromCharPtr(buffer + 10);
		cSpeech profile(buffer + 12, profilesize);
		profile.clearPackeByteOrder();
		who->setProfile(profile);
	}
	else
	{ //only send
		nPackets::Sent::CharProfile pk(serial, who);
		client->sendPacket(&pk);
	}
	return true;
}

/*!
\brief receive client version
\author Chronodt
\param client client who sent the packet
\note packet 0xbd
*/

bool nPackets::Received::ClientVersion::execute(pClient client)
{
	uint16_t size = ShortFromCharPtr(buffer + 1);
	if (length != size) return false;

	std::string clientNumber((char*)(buffer + 3)); //char* constructor of std::string, takes the null-terminated string
	if ( clientNumber.size() > 10) client->clientDimension = 3;
				  else client->clientDimension = 2;
	client->sysmessage("You are using a %iD client, version %s", client->clientDimension, clientNumber.c_str());

	stringVector::const_iterator viter = std::find(clientsAllowed.begin(), clientsAllowed.end(), "ALL");
	if ( viter != clientsAllowed.end() ) return true; // ALL mode found/activated -> quit

	viter = find(clientsAllowed.begin(), clientsAllowed.end(), "SERVER_DEFAULT");
	if ( viter != clientsAllowed.end() )  // server_default mode ?
	{
		if ( strcmp( clientNumber.c_str(), strSupportedClient) ) // check if client version matches
		{
			client->disconnect();
			return true;
		}
		return true;
	}
	else
	{
		viter = find(clientsAllowed.begin(), clientsAllowed.end(), clientNumber);
		if (viter == clientsAllowed.end() )
		{
			//!\todo find a better InfoOut than socket number :D
			//InfoOut("client %i disconnected by Client Version Control System\n", s);
			client->disconnect();
		}
	}
	return true;
}

/*!
\brief Assist version check
\author Chronodt
\param client client who sent the packet
\note packet 0xbe
*/

bool nPackets::Received::AssistVersion::execute(pClient client)
{
	uint16_t size = ShortFromCharPtr(buffer + 1);
	if (length != size) return false;
	uint32_t version = LongFromCharPtr(buffer + 3);
	std::string stringversion = std::string(buffer + 7);
	if (!nSettings::Server::isEnabledUOAssist() && !version)
	{
		//! \todo verify if client is able to read message before being disconnected (or while the popup window about disconnection is onscreen)
		client->sysmessage("UO Assist is not allowed here!";
		client->disconnect();
		return true;
	}
	else if (nSettings::Server::isEnabledUOAssist() && !version && version != nSettings::Server::getAllowedAssistVersion())
	{
		//! \todo verify if client is able to read message before being disconnected (or while the popup window about disconnection is onscreen)
		client->sysmessage("Wrong version of UO Assist in use. You used version %s", stringversion.c_str());
		client->disconnect();
		return true;
	}
	return true;
}

/*!
\brief Miscellaneous packet
\author Chronodt
\param client client who sent the packet
\note packet 0xbf
*/

bool nPackets::Received::MiscCommand::execute(pClient client)
{
	uint16_t size = ShortFromCharPtr(buffer + 1);
	if (length != size) return false;
	uint16_t subcommand = ShortFromCharPtr(buffer + 3);
	pPC pc = client->currChar();
	// please don't remove the // unknowns ... want to have them as dokumentation
	switch (subcommand)
	{
		case 0x04: // documentation tells me this is "Close generic gump". unverified (Chronodt 5/8/04)
			break;
		case 0x05: // Screen size
		{
			uint16_t x = ShortFromCharPtr(buffer + 7);
			uint16_t y = ShortFromCharPtr(buffer + 9);
			//! \todo should we use this somehow?? Maybe in addition of viewrange?
			break;
		}
		case 0x06: //party subcommand
			//!\todo verify party
			Partys.receive( client );
			break;

		case 0x09:	//Luxor: Wrestling Disarm Macro support
			if ( pc ) pc->setWresMove(WRESDISARM);
			break;
		case 0x0a: //Luxor: Wrestling Stun punch Macro support
			if ( pc ) pc->setWresMove(WRESSTUNPUNCH);
			break;

		case 0x0b: // client language, might be used for server localization

			// please no strcpy or memcpy optimization here, because the input ain't 0-termianted and memcpy might be overkill
			client_lang[0]=buffer[5];
			client_lang[1]=buffer[6];
			client_lang[2]=buffer[7];
			client_lang[3]=0;
			// do dometihng with language information from client
			// ...
			break;

		case 0x0e: // UO:3D menus
			if ( pc ) pc->playAction(buffer[8]);
			break;
		case 0x0f: // unknown, sent once on login
			break;
		case 0x13: // documentation tells me this is a "Request popup menu". unverified (Chronodt 5/8/04)
		{
			uint32_t character_id = LongFromCharPtr(buffer + 5);
			//!\todo this subcommand..... but what does it do?? :D
			break;
		}
		case 0x1a: // Extended stats (statlocks)
		{
			uint8_t  stat = buffer[5];
			uint8_t  status = buffer[6]; // 0: up, 1:down, 2: locked
			//!\todo this is the new client's statlock similar to skill locks, so link this in the statlimit
			break;
		}
		case 0x1c: //Spell selected, client side
		{
			uint32_t serial = LongFromCharPtr(buffer + 5); // player
			uint16_t selected_spell = ShortFromCharPtr(buffer + 9); // 4=heal, 5=magic arrow etc
			//!\todo call magic stuff here. Obiously this packets sends necro & paladin stuff too (clients 4 and above)
			break;
		}
	}
	return true;
}

/*!
\brief  receive text entry unicode
\author Chronodt
\param client client who sent the packet
\note packet 0xc2

\note I totally have NO IDEA on what this packet does or when it is sent
*/
bool nPackets::Received::TextEntryUnicode::execute(pClient client)
{
	uint16_t size = ShortFromCharPtr(buffer + 1);
	if (length != size) return false;
	// I totally have NO IDEA on what this packet does or when it is sent
	// i hope someone does and completes this packet

	uint32_t player_id = LongFromCharPtr(buffer + 3);
	uint32_t message_id = LongFromCharPtr(buffer + 7);
	uint32_t unknown = LongFromCharPtr(buffer + 11);	//always 1
	char language[3];
	memcpy(language, buffer +15, 3);
	cSpeech text = cSpeech(buffer + 18);
	//!\todo finish this packet (and undestand when and why it is sent -_-)
	return true;
}

/*!
\brief client view range
\author Chronodt
\param client client who sent the packet
\note packet 0xc8
*/
bool nPackets::Received::ClientViewRange::execute(pClient client)
{
	if (length != 2) return false;
	client->setVisualRange(buffer[1]); //!<\todo verify if this packet is really sent :)
	nPackets::Sent::ClientViewRange(buffer[1]);
	client->sendPacket(&pk);
	return true;
}

/*!
\brief Logout status
\author Chronodt
\param client client who sent the packet
\note packet 0xd1

\note since this packet can only be used with an almost useless and almost totally undocumented packet..
\note this packet is totally useless...
*/

bool nPackets::Received::LogoutStatus::execute(pClient client)
{
	if (length != 2) return false;
	nPackets::Sent::LogoutStatus pk;
	client->sendPacket(&pk);
	return true;
}

/*!
\brief new book header
\author Chronodt
\param client client who sent the packet
\note packet 0xd4
*/

bool nPackets::Received::NewBookHeader::execute(pClient client)
{
	uint16_t size = ShortFromCharPtr(buffer + 1);
	if (length != size) return false;
	uint32_t serial = LongFromCharPtr(buffer + 3);
	pBook book = dynamic_cast<pBook>(cSerializable::findItemBySerial(serial));
	if ( !book ) return false;
	uint8_t flag1 = buffer[7];
	uint8_t flag2 = buffer[8];
//        uint16_t pagenumber = ShortFromCharPtr(buffer + 9);	//we don't really need this information (for now)

	uint16_t authorsize = ShortFromCharPtr(buffer + 11);
	std::string author  = std::string((char*)(buffer + 13));
	if (author.size() != authorsize) return false;

	uint16_t titlesize  = ShortFromCharPtr(buffer + 13 + authorsize);
	std::string title   = std::string((char*)(buffer + 15 + authorsize));
	if (title.size() != titlesize) return false;

	book->setAuthor(author);
	book->setTitle(title);

	return true;
}

/*!
\brief Fight book icon selected
\author Chronodt
\param client client who sent the packet
\note packet 0xd7
*/
bool nPackets::Received::FightBookSelection::execute(pClient client)
{
	uint16_t size = ShortFromCharPtr(buffer + 1);
	if (length != size) return false;
	uint32_t serial = LongFromCharPtr(buffer + 3);
	//uint16_t unknown1 = ShortFromCharPtr(buffer + 7);
	//uint32_t unknown2 = LongFromCharPtr(buffer + 9);
	uint8_t ability = buffer[13]; //This is the ability selected from fightbook
	//uint8_t unknown3 = buffer[14];
	//!\todo the fightbook system. Remember that if client switches weapon, the fightbook list of abilities changes automatically without server intervention... or server notification, so check weapon first :D
	return true;
}
