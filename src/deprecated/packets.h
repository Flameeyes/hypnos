  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/


/*!
\file packets.h
\brief Declaration of class cPackets and derived, all the Uo Packets know
\note All these information are from Wolfpack ( very thx to )
\note client version 3.0.
\todo Send skill pkg 0x3a not implemented yet
\todo Bulletin board message [0x71]
*/

#ifndef __PACKETS_H
#define __PACKETS_H

#include "constants.h"
#include "typedefs.h"
#include <string>
#include "abstraction/endian.h"

using namespace std;

/*!
\brief Base class of all packets
\author Endymion
\since 0.83a
*/
class cPacket
{

protected:

	int headerSize; //!< header size ( NOT SENDED OR RECEIVED. USE INTERNAL )
	int offset; //!< current baffer offset ( NOT SENDED OR RECEIVED. USE INTERNAL )
	char* getBeginValid();

public:
	uint8_t cmd;		//!< packet id


} PACK_NEEDED;


/*!
\brief Base class of all packets send from client to server
\author Endymion
\since 0.83a
*/
class cClientPacket : public cPacket {
protected:
	char* getBeginValidForReceive();
	void getFromSocket( NXWSOCKET socket, char* b, int size );
	void getStringFromSocket( NXWSOCKET socket, string& s, int lenght );
	void getUnicodeStringFromSocket( NXWSOCKET s, wstring& c, int size=INVALID ); 

public:
	virtual void receive( NXWCLIENT ps );
	virtual ~cClientPacket() { };
} PACK_NEEDED;

/*!
\brief Base class of all packets send to server to client
\author Endymion
\since 0.83a
*/
class cServerPacket : public cPacket {
public:
	virtual void send( NXWCLIENT ps );
	virtual void send( pChar pc );
	virtual ~cServerPacket() { };

} PACK_NEEDED;


typedef cPacket* P_PACKET;	//!< pointer to cPacket
typedef cClientPacket* P_CLIENT_PACKET;	//!< pointer to cClientPacket
typedef cServerPacket* P_SERVER_PACKET;	//!< pointer to cServerPacket




/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
///////////////////////// PACKETS NOT WORK ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////


#define PKG_GODMODE_TOGGLE 0x04;
/*!
\brief Used in God client
\author Endymion
\since 0.83
\note 0x04
*/
class cPacketGodModeToggle : public cClientPacket {
public:

	uint8_t	mode; //!< 0=off, 1=on

	cPacketGodModeToggle();

};


#define PKG_RESS_MENU 0x2C;
/*!
\brief Resurrection Menu Choice
\author Endymion
\since 0.83
\note 0x2C
\note Resurrection menu has been removed from UO, so now is unused
*/
class cPacketRessMenu : public cClientPacket {

public:

	uint8_t	mode;		//!< action ( 2=ghost, 1=resurrect, 0=from server )

	cPacketRessMenu();

} PACK_NEEDED;


#define PKG_ANIM 0x6E;
/*!
\brief Character Animation
\author Endymion
\since 0.83
\note 0x6E
\note movement type
0x00 = walk
0x01 = walk faster
0x02 = run
0x03 = run (faster?)
0x04 = nothing
0x05 = shift shoulders
0x06 = hands on hips
0x07 = attack stance (short)
0x08 = attack stance (longer)
0x09 = swing (attack with knife)
0x0a = stab (underhanded)
0x0b = swing (attack overhand with sword)
0x0c = swing (attack with sword over and side)
0x0d = swing (attack with sword side)
0x0e = stab with point of sword
0x0f = ready stance
0x10 = magic (butter churn!)
0x11 = hands over head (balerina)
0x12 = bow shot
0x13 = crossbow
0x14 = get hit
0x15 = fall down and die (backwards)
0x16 = fall down and die (forwards)
0x17 = ride horse (long)
0x18 = ride horse (medium)
0x19 = ride horse (short)
0x1a = swing sword from horse
0x1b = normal bow shot on horse
0x1c = crossbow shot
0x1d = block #2 on horse with shield
0x1e = block on ground with shield
0x1f = swing, and get hit in middle
0x20 = bow (deep)
0x21 = salute
0x22 = scratch head
0x23 = 1 foot forward for 2 secs
0x24 = same
*/
class cPacketAnim : public cServerPacket {

public:

	Serial	chr;		//!< serial of the character
	uint16_t	mov;		//!< movement model ( see note )
	uint8_t	unk;		//!< unknown ( 0x00 )
	uint8_t	dir;		//!< direction
	uint16_t	repeat;		//!<  ( 0=repeat forever, 1=once, 2=twice, ?n=n times? )
	uint8_t	wards;		//!< forward/backwards ( 0=forward, 1=backwards )
	uint8_t	flag;		//!< repeat flag ( 0=Don't repeat, 1=repeat )
	uint8_t	deelay;		//!< frame delay ( 0x00=fastest, 0xFF=too slow to watch )

	cPacketAnim();	

} PACK_NEEDED;


#define PKG_NEW_SUBSERVER 0x76;
/*!
\brief New subserver
\author Endymion
\since 0.83
\note 0x76
*/
class cPacketNewSubserver : public cServerPacket {

public:

	uint16_t	x;			//!< x location
	uint16_t	y;			//!< y location
	uint16_t	z;			//!< z location
	uint8_t	unk;		//!< unknow ( always 0 )
	uint16_t	sbx;		//!< server boundry x
	uint16_t	sby;		//!< server boundry y
	uint16_t	sbwithd;	//!< server boundry Width
	uint16_t	sbheight;	//!< server boundry Height

	cPacketNewSubserver();

} PACK_NEEDED;


#define PKG_RESPONSE_TO_DIALOG 0x7D;
/*!
\brief Client response to dialog
\author Endymion
\since 0.83
\note 0x7D
*/
class cPacketResponseToDialog : public cClientPacket {

public:

	euint32_t	serial;		//!< the dialog serial ( echoed back from 7C packet )
	euint16_t	id;		//!< the model id ( echoed back from 7C packet )
	euint16_t	index;		//!< index of choice ( 1 based )
	euint16_t	model;		//!< model of choice
	eCOLOR	color;		//!< color

	cPacketResponseToDialog();

} PACK_NEEDED;

#define PKG_LOGIN_DENIED 0x82;
/*!
\brief Login denied
\author Endymion
\since 0.83
\note 0x82
\note why
 0x00 = unknow user
 0x01 = account already in use
 0x02 = account disabled
 0x03 = password bad
 0x04 and higer = communication failed
*/
class cPacketLoginDenied : public cServerPacket {

public:

	uint8_t	why;		//!< why ( see note )

	cPacketLoginDenied();

} PACK_NEEDED;

#define PKG_DELETE_CHARACHTER 0x83;
/*!
\brief Delete Character
\author Endymion
\since 0.83
\note 0x83
\note used on login not in game :)
*/
class cPacketDeleteCharacter : public cClientPacket {

public:

	string	passwd;		//!< the password
	uint32_t	idx;		//!< the char index
	uint32_t	ip;			//!< the client ip

	cPacketDeleteCharacter();
	void receive( NXWCLIENT ps );

} PACK_NEEDED;

/*
typedef struct resaftdel_st {
	string	name;		//!< the name
	string	passwd;		//!< the password
}

*//*!
\brief Resend Character After delete
\author Endymion
\since 0.83
\note 0x86
\note used on login not in game :)
*//*
#define PKG_RESEND_CHARACHTER_AFTER_DELETE 0x86;
class cPacketResendCharacterAfterDelete : public cServerPacket {
private:
	uint16_t	size;		//!< the size of packet
	uint8_t	n;			//!< number of character resend

public:

	vector<resaftdel_st> chr;	//!< the characters to resend

	cPacketResendCharacterAfterDelete();
	void send( NXWCLIENT ps );

};*/





//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/////////////////////ONLY WORKING PACKETS/////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////




#define PKG_UNICODE_SPEECH 0xAE;
/*!
\brief Send speech ( unicode mode )
\author Endymion
\since 0.83
\note 0xAE
*/
class cPacketUnicodeSpeech : public cServerPacket {

private:
	euint16_t	size; 
public:
	euint32_t	obj;		//!< from ( 0xFFFFFFFF=system message)
	euint16_t model;		//!< item hex number | 0xFFFF=system
	euint8_t	type;		//!< text type
	eCOLOR	color;		//!< text color
	euint16_t	font;		//!< text font
	euint32_t	language;   //!< language
	string	name;		//!< name of who speech ( 30 character )
	wstring*	msg;	//!< message

	cPacketUnicodeSpeech();
	void send( NXWCLIENT ps );

} PACK_NEEDED;


//@{
/*!
\name map packets
\brief map relatede packets
*/

#define PKG_MAP 0x56;
/*!
\brief Map related
\author Endymion
\since 0.83
\note 0x56
\note
command:
	1 = add map point, 
	2 = add new pin with pin number. (insertion. other pins after the number are pushed back.) 
	3 = change pin
	4 = remove pin
	5 = remove all pins on the map 
	6 = toggle the 'editable' state of the map. 
	7 = return msg from the server to the request 6 of the client. 
*/
class cPacketMap : public cServerPacket {

public:
	euint32_t	obj;		//!< the map
	euint8_t	command;	//!< command see note
	euint8_t	plotting;	//!< plotting state ( 1=on, 0=off only valid if command 7)
	euint16_t	x;		//!< x location (relative to upper left corner of the map, in pixels)
	euint16_t	y;		//!< y location (relative to upper left corner of the map, in pixels)

	cPacketMap();

} PACK_NEEDED;

#define PKG_MAP_MESSAGE 0x90;
/*!
\brief Map Message
\author Endymion
\since 0.83
\note 0x90
*/
class cPacketMapMessage : public cServerPacket {

public:
	euint32_t	key;	//!< the key used
	euint16_t	id;		//!< gump art id (0x139D)
	euint16_t	xa;		//!< x location (relative to upper left corner of the map, in pixels)
	euint16_t	ya;		//!< y location (relative to upper left corner of the map, in pixels)
	euint16_t	xb;		//!< x location (relative to upper left corner of the map, in pixels)
	euint16_t	yb;		//!< y location (relative to upper left corner of the map, in pixels)
	euint16_t   width;	//!< gump width in pixels
	euint16_t   height;	//!< gump height in pixels

	cPacketMapMessage();

} PACK_NEEDED;

//@}

//@{
/*!
\name walking packets
\brief walking relatede packets
*/

#define PKG_WALK 0x97;
/*!
\brief Moves player played by client one step in requested direction.
\author Endymion
\since 0.83
\note 0x97
\note Doesn't move if there are obstacles
*/
class cPacketWalk : public cServerPacket {

public:
	euint8_t   direction;	//!< direction
	cPacketWalk();

} PACK_NEEDED;


//@}

//@{
/*!
\name char profile packets
\brief char profile related packets
*/

#define PKG_CHAR_PROFILE 0xB8;
/*!
\brief Char Profile Request
\author Endymion
\since 0.83
\note 0xB8
*/
class cPacketCharProfileReqOnly : public cClientPacket {
private:
	euint16_t	size; //!< size
public:
	eBool	update; //!< update
	euint32_t chr; //!< character

	cPacketCharProfileReqOnly();

};

/*!
\brief Char Profile Request ( maybe an update )
\author Endymion
\since 0.83
\note 0xB8
*/
class cPacketCharProfileReq : public cClientPacket {
private:
	euint16_t	size;	//!< size
public:
	eBool	update; //!< update
	euint32_t	chr;	//!< character
    euint16_t	type;	//!< type (0x0001 – Update)
private:
	euint16_t	len;	//!< # of unicode characters
public:
	wstring profile;	//!< new profile, in unicode, not null terminated.

	cPacketCharProfileReq();
	void receive( NXWCLIENT PS );

};

/*!
\brief Char Profile
\author Endymion
\since 0.83
\note 0xB8
*/
class cPacketCharProfile : public cServerPacket {
private:
	euint16_t size;	//!< size
public:
	euint32_t chr;	//!< character

	std::string title;	//!< title

	wstring* staticProfile;	//!< static profile, cant be edited 
	wstring* profile;	//!< profile, can be edited

	cPacketCharProfile();
	void send( NXWCLIENT ps );

};

//@}

//@{
/*!
\name client options packets
\brief client options related packets
*/



#define PKG_FEATURES 0xB9;

/*!
\brief Features
\author Endymion
\since 0.83
\note 0xB9
*/
class cPacketFeatures : public cServerPacket {

public:
	euint16_t feature;

	cPacketFeatures();
};

#define FEATURE_T2A 0x0001 //!< enable chat button
#define FEATURE_LBR 0x0002 //!< plays MP3 instead of midis, 2D LBR client shows new LBR monsters
#define FEATURE_LBR_WITHOUT_CHAT 0x0004 //!< lbr without chat

/*if (MSB not set)
Bit# 1 T2A upgrade, enables chatbutton, 
Bit# 2 enables LBR update.  (of course LBR installation is required)
(plays MP3 instead of midis, 2D LBR client shows new LBR monsters,
)

if (MSB set)
Bit# 3 T2A upgrade, enables chatbutton, 
Bit# 4 enables LBR update.  
Bit#5  enables AOS update (Aos monsters/map (AOS installation required for that) , 
AOS skills/necro/paladin/fight book stuff  – works for ALL clients 4.0 and above)

Note1: this message is send immediately after login.
Note2: on OSI  servers this controls features OSI enables/disables via “upgrade codes.”
Note3: a 3 doesn’t seem to “hurt” older (NON LBR) clients.
*/


//@}

//@{
/*!
\name utilitys packets
\brief utilitys related packets
*/

#define PKG_WEB_BROWSER 0xA5;

/*!
\brief Launch web browser
\author Endymion
\since 0.83
\note 0xA5
*/
class cPacketWebBrowser : public cServerPacket {
private:
	euint16_t size; //<! size
public:

	std::string link;

	cPacketWebBrowser();
	void send( NXWCLIENT ps );
};

//@}

//@{
/*!
\name menu packets
\brief menu related packets
*/


#define PKG_MENU 0xB0;

/*!
\brief Show an menu
\author Endymion
\since 0.83
\note 0xB0
*/
class cPacketMenu : public cServerPacket {
private:
	euint16_t size; //<! size
public:

	euint32_t serial; //!< the serial
	euint32_t id; //!< gump serial
	euint32_t	x; //!< x location
	euint32_t	y; //!< x location

private:
	euint16_t cmd_length; //!< command section length
public:
	std::vector<std::string>* commands; //!< commands ( zero terminated )

private:
	 euint16_t numTextLines; //!<text lines number
public:


//	for everty vector item
	euint16_t len; //!< text lenth for every unicode string	
	std::vector< wstring >* texts; //!< text ( every string is NOT null terminated )
//end

	cPacketMenu();
	void send( NXWCLIENT ps );
};


typedef struct {
	euint16_t	id; //!< textentries id
	euint16_t	textlength; //!< text length
	wstring text; //!< text ( not nullterminated )
} text_entry_st;


#define PKG_MENU_SELECTION 0xB1;

/*!
\brief A menu item is selected
\author Endymion
\since 0.83
\note 0xB1
*/

class cPacketMenuSelection : public cClientPacket {

private:
	euint16_t	size;	//!< size
public:

	euint32_t serial; //!< the serial ( first Id in PKG_MENU )
	euint32_t id; //!< gump serial (second Id in PKG_MENU )
	euint32_t	buttonId; //!< which button pressed or 0 if closed

private:
	euint32_t	switchcount; //!<  response info for radio buttons and checkboxes, any switches listed here are switched on
public:
	std::vector<uint32_t> switchs; //!< switch ids
private:
	euint32_t textcount; //!< response info for textentries
public:
//	std::vector<text_entry_st> text_entries; //!< text entries
	std::map< uint32_t, std::wstring> text_entries; //!< text entries

	cPacketMenuSelection();
	void receive( NXWCLIENT PS );

};


typedef struct  {

	euint16_t	model; //!< model id number of shown icon ( if grey menu then always 0x00 as msb )
	eCOLOR	color; //!< icon color
//	euint8_t resp_length, //needed but managed into send, so not need var
	std::string response;	

} pkg_icon_list_menu_st;

#define PKG_ICON_LIST_MENU 0x7C;

/*!
\brief Show an icon list menu
\author Endymion
\since 0.83
\note 0x7C
*/
class cPacketIconListMenu : public cServerPacket {
private:
	euint16_t size; //<! size
public:

	euint32_t serial; //!< the serial
	euint16_t id; //!< the gump
private:
	euint8_t question_length; //!< question length
public:
	std::string question; //!< question text

private:
	euint8_t icon_count; //!< icon number
public:
	std::vector< pkg_icon_list_menu_st >* icons; //!< icons

	cPacketIconListMenu();
	void send( NXWCLIENT ps );
};

//@}

//@{
/*!
\name target packets
\brief target related packets
*/

#define PKG_QUEST_ARROW 0xBA;

/*!
\brief Show a quest arrow
\author Endymion
\since 0.83
\note 0xBA
*/
class cPacketQuestArrow : public cServerPacket {

public:

	eBool active; //<! active
	euint16_t x; //!< x location
	euint16_t y; //!< y location

	cPacketQuestArrow();

};


#define PKG_TARGETING 0x6C;
/*!
\brief Targeting Cursor Commands
\author Endymion
\since 0.83
\note 0x6C
\remarks the model number shouldn’t be trusted
*/
template< class T >
class cPacketTargetingCursor : public T {

public:

	euint8_t	type;		//!< type ( 0=Select Object, 1=Select X, Y, Z )
	euint32_t	cursor;		//!< cursor id
	euint8_t	cursorType;	//!< cursor type

	//The following are always sent but are only valid if sent by client
	euint32_t	clicked;	//!<  clicked on id
	euint16_t	x ;			//!< click x Location
	euint16_t	y ;			//!< click y Location
	euint8_t	unk;		//!< nown (0x00)
	eint8_t	z ;			//!< click z Location
	euint16_t	model;		//!< model number ( 0=map/landscape tile, else static tile )
	
	cPacketTargetingCursor();

} PACK_NEEDED;

//@}

//@{
/*!
\name party packets
\brief party related packets
*/


#define PKG_GENERAL_INFO 0xBF;

/*!
\brief General info packet
\author Endymion
\since 0.83
\note 0xBF
*/
template< class T >
class cPacketGeneralInfo : public T {

protected:
	euint16_t size;	//<! size
	euint16_t subcmd;	//!< the subcmd
public:
	cPacketGeneralInfo();

};

#define GEN_INFO_SUBCMD_PARTY 6

/*!
\brief Party packets
\author Endymion
\since 0.83
\note 0xBF - 6
*/
template< class T >
class cSubPacketParty : public cPacketGeneralInfo<T> {
public:
	euint8_t subsubcommand;

	cSubPacketParty();
};

#define PARTY_SUBCMD_ADD 1

/*!
\brief Add a new member to party
\author Endymion
\since 0.83
\note 0xBF - 6 - 1
*/
class clPacketAddPartyMember : public cSubPacketParty< cClientPacket > {
public:
	euint32_t member; //!< the member, if 0 the targeting cursor appears

	clPacketAddPartyMember();
	void receive( NXWCLIENT ps );
};

/*!
\brief Send all party members after an add
\author Endymion
\since 0.83
\note 0xBF - 6 - 1
*/
class csPacketAddPartyMembers : public cSubPacketParty< cServerPacket > {
private:
	euint8_t count;	//!< members count
public:
	std::vector<P_PARTY_MEMBER>* members;
	void send( NXWCLIENT ps );

	csPacketAddPartyMembers();

};

#define PARTY_SUBCMD_REMOVE 2

/*!
\brief Remove a member to party
\author Endymion
\since 0.83
\note 0xBF - 6 - 2
*/
class clPacketRemovePartyMember : public cSubPacketParty< cClientPacket > {
public:
	euint32_t member; //!< the member, if 0 the targeting cursor appears

	clPacketRemovePartyMember();
	void receive( NXWCLIENT ps );
};

/*!
\brief Remove a member to party, and resend list
\author Endymion
\since 0.83
\note 0xBF - 6 - 2
*/
class csPacketRemovePartyMembers : public cSubPacketParty< cServerPacket > {
private:
	euint8_t count;	//!< members count
public:
	euint32_t member; //!< the member removed
	std::vector<P_PARTY_MEMBER>* members; //!< all members

	csPacketRemovePartyMembers();
	void send( NXWCLIENT ps );

};

#define PARTY_SUBCMD_MESSAGE 3

/*!
\brief Tell to a party member a message
\author Endymion
\since 0.83
\note 0xBF - 6 - 3
\note message is NULL terminated
*/
class clPacketPartyTellMessage : public cSubPacketParty< cClientPacket > {
public:
	euint32_t member; //!< to member
	std::wstring message; //!< the message

	clPacketPartyTellMessage();
	void receive( NXWCLIENT ps );

};

/*!
\brief Tell to a party member a message
\author Endymion
\since 0.83
\note 0xBF - 6 - 3
\note message is NULL terminated
*/
class csPacketPartyTellMessage : public cSubPacketParty< cServerPacket > {
public:
	euint32_t member; //!< from member
	std::wstring* message; //!< the message

	csPacketPartyTellMessage();
	void send( NXWCLIENT ps );

};

#define PARTY_SUBCMD_BROADCAST 4

/*!
\brief Tell to all party members a message
\author Endymion
\since 0.83
\note 0xBF - 6 - 4
\note message is NULL terminated
*/
class csPacketPartyTellAllMessage : public cSubPacketParty< cServerPacket > {
public:
	euint32_t from; //!< from member
	std::wstring* message; //!< the message

	csPacketPartyTellAllMessage();
	void send( NXWCLIENT ps );

};

/*!
\brief Tell to all party members a message
\author Endymion
\since 0.83
\note 0xBF - 6 - 4
\note message is NULL terminated
*/
class clPacketPartyTellAllMessage : public cSubPacketParty< cClientPacket > {
public:
	std::wstring message; //!< the message

	clPacketPartyTellAllMessage();
	void receive( NXWCLIENT ps );

};

#define PARTY_SUBCMD_CANLOOT 6

/*!
\brief Other party memeber can loot this
\author Endymion
\since 0.83
\note 0xBF - 6 - 6
*/
class clPacketPartyCanLoot : public cSubPacketParty< cClientPacket > {
public:
	eBool canLoot; //!< other member can loot this

	clPacketPartyCanLoot();
	void receive( NXWCLIENT ps );
};

#define PARTY_SUBCMD_INVITE 7

/*!
\brief Accept party invitation
\author Endymion
\since 0.83
\note 0xBF - 6 - 7
*/
class csPacketPartyInvite : public cSubPacketParty< cServerPacket > {
public:
	euint32_t leader; //!< party leader

	csPacketPartyInvite();
	void send( NXWCLIENT ps );
};

#define PARTY_SUBCMD_ACCEPT 8

/*!
\brief Accept party invitation
\author Endymion
\since 0.83
\note 0xBF - 6 - 8
*/
class clPacketPartyAccept : public cSubPacketParty< cClientPacket > {
public:
	euint32_t leader; //!< party leader

	clPacketPartyAccept();
	void receive( NXWCLIENT ps );
};

#define PARTY_SUBCMD_DECLINE 9

/*!
\brief Decline party invitation
\author Endymion
\since 0.83
\note 0xBF - 6 - 9
*/
class clPacketPartyDecline : public cSubPacketParty< cClientPacket > {
public:
	euint32_t leader; //!< party leader

	clPacketPartyDecline();
	void receive( NXWCLIENT ps );
};







//@}


#endif
