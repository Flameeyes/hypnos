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
*/

#ifndef __SNDPKG_H__
#define __SNDPKG_H__

#include "common_libs.h"
#include "particles.h"

void itemmessage(pClient client, char *txt, pClient clienterial, short color=0x0000);
void chardel (pClient client);
	//!< Deletion of character
void updatechar(pChar pc);
	//!< If character status has been changed (Polymorph);, resend him
void target(pClient client, int a1, int a2, int a3, int a4, char *txt);
	//!< Send targetting cursor to client
void weblaunch(pClient client, const char *txt);
	//!< Direct client to a web page
void broadcast(pClient client);
	//!< GM Broadcast (Done if a GM yells something);


void staticeffectUO3D(int/*SERIAL*/ player, ParticleFx *sta);
void movingeffectUO3D(int/*SERIAL*/ source, int/*SERIAL*/ dest, ParticleFx *sta);
void itemeffectUO3D(pItem pi, ParticleFx *sta);

void SendUnicodeSpeechMessagePkt(pClient client, uint32_t id, uint16_t model, uint8_t type, uint16_t color, uint16_t fonttype, uint32_t lang, uint8_t sysname[30], uint8_t *unicodetext, uint16_t unicodelen);

void SendPlaySoundEffectPkt(pClient client, uint8_t mode, uint16_t sound_model, uint16_t unkn, sLocation pos, bool useDispZ = false);


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
