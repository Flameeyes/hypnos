/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Functions that send packages to the Client
\note cut from NoX-Wizard.cpp by Duke, 25.10.00
*/

#ifndef __SNDPKG_H__
#define __SNDPKG_H__

#include "common_libs.h"
#include "particles.h"

void SndDyevat(pClient client, pClient clienterial, short id);
void SndUpdscroll(pClient client, short txtlen, const char* txt);

void itemmessage(pClient client, char *txt, pClient clienterial, short color=0x0000);
void backpack2(pClient client, uint32_t serial);
	//!< Send corpse stuff
void tileeffect(int x, int y, int z, char eff1, char eff2, char speed, char loop);
void chardel (pClient client);
	//!< Deletion of character
void updatechar(pChar pc);
	//!< If character status has been changed (Polymorph);, resend him
void target(pClient client, int a1, int a2, int a3, int a4, char *txt);
	//!< Send targetting cursor to client
void skillwindow(pClient client);
	//!< Opens the skills list, updated for client 1.26.2b by LB
void updates(pClient client);
	//!< Update Window
void weblaunch(pClient client, const char *txt);
	//!< Direct client to a web page
void broadcast(pClient client);
	//!< GM Broadcast (Done if a GM yells something);
void itemtalk( pItem pi, char *txt);
	//!< Item "speech"


void staticeffect (pChar player, uint16_t effect, unsigned char speed, unsigned char loop, bool UO3DonlyEffekt=false, ParticleFx *str=NULL, bool skip_old=false );
void staticeffect2(pItem pi, unsigned char eff1, unsigned char eff2, unsigned char speed, unsigned char loop, unsigned char explode, bool UO3DonlyEffekt=false, ParticleFx *str=NULL, bool skip_old=false);

void staticeffect3(sLocation pos, uint16_t eff, uint8_t speed, uint8_t loop, uint8_t explode);
void movingeffect3(int/*SERIAL*/ source, unsigned short x, unsigned short y, signed char z, unsigned char eff1, unsigned char eff2, unsigned char speed, unsigned char loop, unsigned char explode);
void movingeffect3(int/*SERIAL*/ source, int/*SERIAL*/ dest, unsigned char eff1, unsigned char eff2, unsigned char speed, unsigned char loop, unsigned char explode,unsigned char unk1,unsigned char unk2,unsigned char ajust,unsigned char type);

void staticeffectUO3D(int/*SERIAL*/ player, ParticleFx *sta);
void movingeffectUO3D(int/*SERIAL*/ source, int/*SERIAL*/ dest, ParticleFx *sta);
void itemeffectUO3D(pItem pi, ParticleFx *sta);
void MakeGraphicalEffectPkt_(uint8_t pkt[28], uint8_t type, uint32_t src_serial, uint32_t dst_serial, uint16_t model_id, sLocation src_pos, sLocation dst_pos, uint8_t speed, uint8_t duration, uint8_t adjust, uint8_t explode );

void SendDrawObjectPkt(pClient client, pChar pc, int z);

void SendUnicodeSpeechMessagePkt(pClient client, uint32_t id, uint16_t model, uint8_t type, uint16_t color, uint16_t fonttype, uint32_t lang, uint8_t sysname[30], uint8_t *unicodetext, uint16_t unicodelen);

void SendPlaySoundEffectPkt(pClient client, uint8_t mode, uint16_t sound_model, uint16_t unkn, sLocation pos, bool useDispZ = false);
void impowncreate(pClient client, pChar pc, int z);
	//!< socket, player to send
void sendshopinfo(pClient client, pChar pc, pItem pi);
pClient clientellstuff(pClient client, int i);


void tellmessage(int i, pClient client, char *txt);
void gmyell(char *txt);

uint16_t goldsfx(int goldtotal);
uint16_t itemsfx(uint16_t item);

void weather(pClient client, unsigned char bolt);
void wornitems(pClient client, pChar pc);
void bgsound(pChar pc);
void pweather(pClient client);

#endif
