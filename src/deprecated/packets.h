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

#define PKG_PLAYER_STATUS 0x34;
/*!
\brief Get Player Status
\author Endymion
\since 0.83
\note 0x34
*/
class cPacketPlayerStatus : public cClientPacket {

public:

	uint32_t	pattern;	//!< pattern ( 0xEDEDEDED )
	uint8_t	type;		//!< get type ( 4=Basic Stats (Packet 0x11 Response), 5=Request Skills (Packet 0x3A Response) )
	Serial	player;		//!< serial of player

	cPacketPlayerStatus();	

} PACK_NEEDED;


typedef struct {
	uint8_t	flag;	//!< flag ( 0x1A )
	Serial	item;	//!< serial of item (from 3C packet)
	uint16_t	n;		//!< number of bought item
} buyitem_st;

#define PKG_BUY 0x3B;
/*!
\brief Buy Item(s)
\author Endymion
\since 0.83a
\note 0x3B
*/
class cPacketBuy : public cServerPacket {

private:
	uint16_t	size;		//!< packet size
public:
	Serial	vendor;		//!< serial of vendor
private:
	uint8_t	flag;		//!< flag ( 0=no items following, 2=items following ) 1=onlyoneitem?
public:
	vector<buyitem_st> list;

	void send( NXWCLIENT ps );
	cPacketBuy();

} PACK_NEEDED;

/*!
\brief struct with informations about items in containers
*/
typedef struct {
	Serial	item;		//!< serial of item
	uint32_t	model;		//!< model
	uint8_t	unk;		//!< unknown (0x00)
	uint16_t	n;		//!< number of items in stack
	uint16_t	x;		//!< x Location in container
	uint16_t	y;		//!< y Location in container
	Serial	cont;		//!< serial of container
	eCOLOR	color;		//!< color of item
} itemincont_st;

#define PKG_CONTAINER 0x3C;
/*!
\brief Items in Container
\author Endymion
\since 0.83
\note 0x3C
*/
class cPacketContainer : public cServerPacket {

private: 
	uint16_t	size;		//!< packet size
	uint16_t	n;			//!< number of itemincont_st
public:
	vector<itemincont_st> list;
	
	void send( NXWCLIENT ps );
	cPacketContainer();	

} PACK_NEEDED;

#define PKG_PERSONAL_LIGHT 0x4E;
/*!
\brief Personal Light Level
\author Endymion
\since 0.83
\note 0x4E
*/
class cPacketPersonalLight : public cServerPacket {
public:

	Serial	chr;		//!< serial of character
	uint8_t	level;		//!< light level

	cPacketPersonalLight();	

} PACK_NEEDED;


#define PKG_OVERALL_LIGHT 0x4F;
/*!
\brief Overall Light Level
\author Endymion
\since 0.83
\note 0x4F
*/
class cPacketOverallLight : public cServerPacket {
public:

	uint8_t	level;		//!< light level ( 0x00=day, 0x09=OSI night, 0x1F=Black )

	cPacketOverallLight();

} PACK_NEEDED;

#define PKG_IDLE_WARNING 0x53;
/*!
\brief Displays a pre-defined message in client
\author Endymion
\since 0.83
\note 0x53
\note type are:
 1=no character
 2=char exists
 3=can't connect
 4=can't connect
 5=character already in world
 6=login problem
 7=idle
 8=can't connect
*/
class cPacketIdleWarning : public cServerPacket {

public:

	uint8_t	type;		//!< type ( see note )

	cPacketIdleWarning();	

} PACK_NEEDED;

#define PKG_SOUND 0x54;
/*!
\brief Play Sound Effect
\author Endymion
\since 0.83
\note 0x54
*/
class cPacketSound : public cServerPacket {

public:

	uint8_t	mode;		//!< mode ( 0=quiet repeating, 1=single normally played sound effect )
	uint16_t	model;		//!< sound model
	uint16_t	unk;		//!< unknown ( speed/volume modifier? Line of sight stuff? )
	uint16_t	x;			//!< x Location
	uint16_t	y;			//!< y Location
	uint16_t	z;			//!< z Location

	cPacketSound();

} PACK_NEEDED;

#define PKG_LOGIN_COMPLETE 0x55;
/*!
\brief Login Complete, start game
\author Endymion
\since 0.83
\note 0x55
*/
class cPacketLoginComplete : public cServerPacket {

	cPacketLoginComplete();	

} PACK_NEEDED;

#define PKG_PLOT_COURSE 0x56;
/*!
\brief Plot course for ships
\author Endymion
\since 0.83
\note 0x56
\note type
 1= add map point
 2= add new pin with pin number ( insertion, other pins after the number are pushed back )
 3= change pin
 4= remove pin
 5= remove all pin on the map
 6= toggle the "editable" state of the map
 7= return msg from the server to the request 6 of the client
*/
class cPacketPlotCourse : public cServerPacket {

	Serial	serial;		//!< serial
	uint8_t	type;		//!< type 
	uint8_t	state;		//!< plotting state ( 1=on, 0=off, valid only if type 7 )
	uint16_t	x;			//!< x pin ( relative to upper left corner of the map, inpixel, for points )
	uint16_t	y;			//!< y pin ( relative to upper left corner of the map, inpixel, for points )
	
	cPacketPlotCourse();	

} PACK_NEEDED;

#define PKG_TIME 0x5B;
/*!
\brief Time
\author Endymion
\since 0.83
\note 0x5B
*/
class cPacketTime : public cServerPacket {

	uint8_t	hour;		//!< hour
	uint8_t	minute;		//!< minute
	uint8_t	second;		//!< second

	cPacketTime();	

} PACK_NEEDED;

/*!
\brief Login Character
\author Endymion
\since 0.83
\note 0x5D
\remarks ClientIP should not be trusted.
*/
#define PKG_LOGIN 0x5D;
class cPacketLogin : public cClientPacket {

	uint32_t	pattern;	//!< pattern ( 0xEDEDEDED )
	string	name;		//!< name ( 30 characters )
	string	passwd;		//!< password ( 30 characters )
	uint32_t	slot;		//!< slot choosen ( 0 based )
	uint32_t	ip;			//!< client ip
	
	cPacketLogin();	
	void receive( NXWCLIENT ps );

} PACK_NEEDED;

#define PKG_WEATHER 0x65;
/*!
\brief Set Weather
\author Endymion
\since 0.83
\note 0x65
\note Temperature has no effect at present
\remarks maximum number of weather effects on screen is 70
\remarks If it is raining, you can add snow by setting the num
to the num of rain currently going, plus the number of snow you want
\attention Weather messages are only displayed when weather starts
\note Weather will end automatically after 6 minutes without any weather change packets
\remarks You can totally end weather (to display a new message) by teleporting
\note weather type:
0x00="It starts to rain"
0x01="A fierce storm approaches."
0x02="It begins to snow"
0x03="A storm is brewing.",
0xFF=None (turns off sound effects),
0xFE=(no effect?? Set temperature?)
0x78,0x20,0x4F,0x4E = reset, never tested
*/
class cPacketWeather : public cServerPacket {

	uint8_t	type;		//!< type ( see note )
	uint8_t	n;			//!< number of weather effects on screen
	uint8_t	temp;		//!< temperature
	
	
	cPacketWeather();	

} PACK_NEEDED;

#define PKG_MIDI 0x6D;
/*!
\brief Play Midi Music
\author Endymion
\since 0.83
\note 0x6D
*/
class cPacketMidi : public cServerPacket {

public:

	uint16_t	music;		//!< music id

	cPacketMidi();	

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

/*|
\brief Secure Trading
\author Endymion
\since 0.83
\note 0x6F
*//*not implemented yet
class cPacketSecureTrading : public cServerPacket {

private:

	uint16_t size; //!< packet size

public:

	uint8_t action
	uint8_t[4] id1
	uint8_t[4] id2
	uint8_t[4] id3
	uint8_t nameFollowing (0 or 1)
	If (nameFollowing = 1)
	uint8_t[?] charName

	cPacketSecureTrading();	

} PACK_NEEDED;
*/

#define PKG_EFFECT 0x70;
/*!
\brief Graphical Effect
\author Endymion
\since 0.83
\note 0x70
\note direction type
0=go from source to dest
1=lightning strike at source
2=stay at current x,y,z
3=stay with current source character id
*/
class cPacketEffect : public cServerPacket {

public:

	uint8_t	dir;		//!< direction type ( see note )
	Serial	chr;		//!< serial of source character
	Serial	target;		//!< serial of target character
	uint16_t	model;		//!< model of the first frame of the effect
	uint16_t	sx;			//!< x Location of source
	uint16_t	sy;			//!< y Location of source
	int8_t	sz;			//!< z Location of source
	uint16_t	tx;			//!< x Location of target
	uint16_t	ty;			//!< y Location of target
	int8_t	tz;			//!< z Location of target
	uint8_t	speed;		//!< speed of the animation
	uint8_t	duration;	//!< duration ( 0=really long, 1=shortest )
	uint16_t	unk;		//!< unknown (0 works)
	uint8_t	direct;		//!< no adjust direction during animation ( 0=yes, else no ) ( ?is bool?)
	uint8_t	explode;	//!< explode on impact

	cPacketEffect();	

} PACK_NEEDED;

#define PKG_WAR_MODE 0x72;
/*!
\brief Request War Mode Change/Send War Mode status
\author Endymion
\since 0.83
\note From Server,Client
\note 0x72
\note server replies with 0x77 packet
*/
class cPacketWarMode : public cServerPacket /*CLIENTALSOPD*/ {

public:

	uint8_t	flag;		//!< flag ( 0=Normal, 1=Fighting )
	uint8_t	unk[3];		//!< unknown (always 00 32 00 in testing)

	cPacketWarMode();

} PACK_NEEDED;


/*!
\brief Ping message
\author Endymion
\since 0.83a
\note From ??
\note 0x73
\note server replies with 0x77 packet
*//*
class cPacketPing : public cPackets {

public:

	uint8_t seq; //!<  

	cPacketPing();	

} PACK_NEEDED;*/

/*
typedef struct {
	uint32_t price;
	uint8_t length; of text description
	uint8_t[text length] item description
} openbuyitem_st ;

*//*
\brief Open Buy Window 
\author Endymion
\since 0.83
\note 0x74
\note This packet is always preceded by a describe contents packet (0x3c) with the
container id as the (vendorID | 0x40000000) and then an open container packet
(0x24?) with the vendorID only and a model number of 0x0030 (probably the model
number for the buy screen)
*//*
#define PKG_OPEN_BUY 0x74;
class cPacketOpenBuy : public cServerPacket {

private:
	uint16_t size; //!< packet size
public:
	Serial vendor; //!< serial of verndor | 0x40000000
private:
	uint8_t n; //!< number of items of items
public:
	vector<openbuyitem_st> list;

	cPacketOpenBuy();	
	void send( NXWCLIENT ps );

} PACK_NEEDED;

*/

#define PKG_RENAME 0x75;
/*!
\brief Rename Character
\author Endymion
\since 0.83
\note 0x75
*/
class cPacketRename : public cServerPacket {

public:

	Serial	chr;		//!< serial of chr
	string	newname;	//!< the new name ( 30 characters )

	cPacketRename();
	void send( NXWCLIENT ps );

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

#define PKG_UPDATE_PLAYER 0x77;
/*!
\brief Update player
\author Endymion
\since 0.83
\note 0x77
*/
class cPacketUpdatePlayer : public cServerPacket {

public:

	Serial	player;		//!< the serial of player
	uint16_t	model;		//!< the model id
	uint16_t	x;			//!< x location
	uint16_t	y;			//!< y location
	int8_t	z;			//!< z location
	uint8_t	dir;		//!< direction
	eCOLOR	color;		//!< color
	uint8_t	flag;		//!< flag ( bitset )
	uint8_t	highcolor;	//!< highlight color

	cPacketUpdatePlayer();

} PACK_NEEDED;

//0x78 not yet



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
