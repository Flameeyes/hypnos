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
\brief Functions that send packages to the Client
\note cut from NoX-Wizard.cpp by Duke, 25.10.00
*/

#if !defined(AFX_SNDPKG_H__D538CC68_E5B0_480A_9752_F00069A33137__INCLUDED_)
#define AFX_SNDPKG_H__D538CC68_E5B0_480A_9752_F00069A33137__INCLUDED_

#include "particles.h"
#include "typedefs.h"


#if _MSC_VER >= 1000
#pragma once
#endif

void SndAttackOK(NXWSOCKET  s, int serial);
void SndDyevat(NXWSOCKET  s, int serial, short id);
void SndUpdscroll(NXWSOCKET  s, short txtlen, const char* txt);

void SndShopgumpopen(NXWSOCKET  s, int serial);

void soundeffect3(P_ITEM pi, uint16_t sound);
void soundeffect4(NXWSOCKET s, P_ITEM pi, uint16_t sound);
void sysbroadcast(char *txt, ...);
	//!< System broadcast in bold text
void sysmessage(NXWSOCKET  s, const char *txt, ...);
	//!< System message (In lower left corner);
void sysmessage(NXWSOCKET  s, short color, const char *txt, ...);
void sysmessageflat(NXWSOCKET  s, short color, const char *txt);
	//!< System message (In lower left corner)
void itemmessage(NXWSOCKET  s, char *txt, int serial, short color=0x0000);
void backpack2(NXWSOCKET s, SERIAL serial);
	//!< Send corpse stuff
void tileeffect(int x, int y, int z, char eff1, char eff2, char speed, char loop);
void senditem(NXWSOCKET  s, P_ITEM pi);
	//!< Send items (on ground);
void senditem_lsd(NXWSOCKET  s, ITEM i,char color1, char color2, int x, int y, signed char z);
void chardel (NXWSOCKET  s);
	//!< Deletion of character
void updatechar(P_CHAR pc);
	//!< If character status has been changed (Polymorph);, resend him
void target(NXWSOCKET  s, int a1, int a2, int a3, int a4, char *txt);
	//!< Send targetting cursor to client
void skillwindow(int s);
	//!< Opens the skills list, updated for client 1.26.2b by LB
void updates(NXWSOCKET  s);
	//!< Update Window
void tips(NXWSOCKET s, uint16_t i, uint8_t flag);
	//!< Tip of the day window
void deny(NXWSOCKET  k,P_CHAR pc, int sequence);
void weblaunch(int s, const char *txt);
	//!< Direct client to a web page
void broadcast(int s);
	//!< GM Broadcast (Done if a GM yells something);
void itemtalk( P_ITEM pi, char *txt);
	//!< Item "speech"


void staticeffect (CHARACTER player, unsigned char eff1, unsigned char eff2, unsigned char speed, unsigned char loop, bool UO3DonlyEffekt=false, ParticleFx *str=NULL, bool skip_old=false );
void movingeffect(CHARACTER source, CHARACTER dest, unsigned char eff1, unsigned char eff2, unsigned char speed, unsigned char loop, unsigned char explode, bool UO3DonlyEffekt=false, ParticleFx *str=NULL, bool skip_old=false);
void staticeffect2(P_ITEM pi, unsigned char eff1, unsigned char eff2, unsigned char speed, unsigned char loop, unsigned char explode, bool UO3DonlyEffekt=false, ParticleFx *str=NULL, bool skip_old=false);

void staticeffect3(uint16_t x, uint16_t y, int8_t z, unsigned char eff1, unsigned char eff2, char speed, char loop, char explode);
void movingeffect3(CHARACTER source, unsigned short x, unsigned short y, signed char z, unsigned char eff1, unsigned char eff2, unsigned char speed, unsigned char loop, unsigned char explode);
void movingeffect3(CHARACTER source, CHARACTER dest, unsigned char eff1, unsigned char eff2, unsigned char speed, unsigned char loop, unsigned char explode,unsigned char unk1,unsigned char unk2,unsigned char ajust,unsigned char type);
void movingeffect2(CHARACTER source, CHARACTER dest, unsigned char eff1, unsigned char eff2, unsigned char speed, unsigned char loop, unsigned char explode);
void bolteffect2(CHARACTER player,char a1,char a2);	// experimenatal, lb

void staticeffectUO3D(CHARACTER player, ParticleFx *sta);
void movingeffectUO3D(CHARACTER source, CHARACTER dest, ParticleFx *sta);
void itemeffectUO3D(P_ITEM pi, ParticleFx *sta);
void MakeGraphicalEffectPkt_(uint8_t pkt[28], uint8_t type, uint32_t src_serial, uint32_t dst_serial, uint16_t model_id, Location src_pos, Location dst_pos, uint8_t speed, uint8_t duration, uint8_t adjust, uint8_t explode );

void SendPauseResumePkt(NXWSOCKET s, uint8_t flag);
void SendDeleteObjectPkt(NXWSOCKET s, SERIAL serial);
void SendDrawObjectPkt(NXWSOCKET s, P_CHAR pc, int z);
void SendSpeechMessagePkt(NXWSOCKET s, uint32_t id, uint16_t model, uint8_t type, uint16_t color, uint16_t fonttype, uint8_t sysname[30], char *text);
void SendUnicodeSpeechMessagePkt(NXWSOCKET s, uint32_t id, uint16_t model, uint8_t type, uint16_t color, uint16_t fonttype, uint32_t lang, uint8_t sysname[30], uint8_t *unicodetext, uint16_t unicodelen);
void SendUpdatePlayerPkt(NXWSOCKET s, uint32_t player_id, uint16_t model, Location pos, uint8_t dir, uint16_t color, uint8_t flag, uint8_t hi_color);
void SendDrawGamePlayerPkt(NXWSOCKET s, uint32_t player_id, uint16_t model, uint8_t unk1, uint16_t color, uint8_t flag, Location pos, uint16_t unk2, uint8_t dir, bool useDispZ = false);
void SendPlaySoundEffectPkt(NXWSOCKET s, uint8_t mode, uint16_t sound_model, uint16_t unkn, Location pos, bool useDispZ = false);
void impowncreate(NXWSOCKET s, P_CHAR pc, int z);
	//!< socket, player to send
void sendshopinfo(int s, int c, P_ITEM pi);
int sellstuff(int s, int i);

void endtrade(SERIAL serial);
void tellmessage(int i, int s, char *txt);
void gmyell(char *txt);

uint16_t goldsfx(int goldtotal);
uint16_t itemsfx(uint16_t item);

void weather(int s, unsigned char bolt);
void dosocketmidi(int s);
void wornitems(NXWSOCKET  s, P_CHAR pc);
void bgsound(int s);
void pweather(NXWSOCKET  s);


#endif
