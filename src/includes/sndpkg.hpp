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

void itemmessage(pClient client, char *txt, pItem pi, short color=0x0000);
void chardel (pClient client);
	//!< Deletion of character
void target(pClient client, int a1, int a2, int a3, int a4, char *txt);
	//!< Send targetting cursor to client

void SendPlaySoundEffectPkt(pClient client, uint8_t mode, uint16_t sound_model, uint16_t unkn, sLocation pos, bool useDispZ = false);

void tellmessage(int i, pClient client, char *txt);
void gmyell(char *txt);

uint16_t goldsfx(int goldtotal);
uint16_t itemsfx(uint16_t item);

void weather(pClient client, unsigned char bolt);
void bgsound(pChar pc);
void pweather(pClient client);

#endif
