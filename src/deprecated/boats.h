/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#ifndef __BOATS_H__
#define __BOATS_H__

#include "common_libs.h"
#include "structs.h"
#include "data.h"

/*!
\file
\author Elcabesa
\brief Boat System Stuff
\note over an existing old boat system
*/

//For iSizeShipOffsets->
#define PORT_PLANK 0
#define STARB_PLANK 1
#define HOLD 2
#define TILLER 3

extern signed short int iSmallShipOffsets[4][4][2];
extern signed short int iMediumShipOffsets[4][4][2];
extern signed short int iLargeShipOffsets[4][4][2];

//NEW BOAT SYSTEM

//bool check_boat_position(pClient client,pItem pBoat);

bool boat_collision(pBoat pBoat1, pBoat pBoat2, sPoint w, uint8_t dir,);
bool collision(pBoat pi, sPoint where, uint8_t dir);
bool good_position(pItem pBoat, sLocation where, uint8_t dir);
bool tile_check(multi_st multi, pItem pBoat, map_st map, sPoint w,uint8_t dir);
void LeaveBoat(pChar pc, pItem pi);
void TurnStuff_i(pItem, pItem, int, int);

void TurnStuff_c(pItem, pChar, int, int);

void TurnShip( uint8_t size, uint8_t dir, pItem pPort, pItem pStarboard, pItem pTiller, pItem pHold );

bool Speech(pChar pc, pClient clientocket, std::string &talk );
void OpenPlank(pItem pi);
void PlankStuff(pChar pc, pItem pi);
bool Build(pClient client, pItem pBoat, char);

#endif
