/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Teleport related functions
*/

#ifndef __TELPORT_H__
#define __TELPORT_H__

#include "common_libs.hpp"

void objTeleporters(pChar pc);
int validtelepos(pChar pc);
void advancementobjects(pChar pc, int x, int always);
void teleporters(pChar pc);
void read_in_teleport();

#endif
