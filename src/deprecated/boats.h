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

static const uint16_t XBORDER = 200;	//!< Border from x-limit
static const uint16_t YBORDER = 200;	//!< Border from y-limit

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

pItem findmulti(sLocation where);
bool inmulti(sLocation where,pItem pi);

bool boat_collision(pItem pBoat1,int x1, int y1,int dir,pItem pBoat2);
bool collision(pItem pi, sLocation where,int dir);
bool good_position(pItem pBoat, sLocation where, int dir);
bool tile_check(multi_st multi,pItem pBoat,map_st map,int x, int y ,int dir);
void LeaveBoat(pChar pc, pItem pi);
void TurnStuff_i(pItem, pItem, int, int);

void TurnStuff_c(pItem, pChar, int, int);
void iMove(pClient client, int dir, pItem pBoat, bool forced = true);

void TurnShip( uint8_t size, int32_t dir, pItem pPort, pItem pStarboard, pItem pTiller, pItem pHold );

pItem GetBoat(sLocation pos);
bool Speech(pChar pc, pClient clientocket, std::string &talk );
void OpenPlank(pItem pi);
void PlankStuff(pChar pc, pItem pi);
bool Build(pClient client, pItem pBoat, char);
void Move(pClient client, int dir, pItem pBoat);
void Turn(pItem, int);

#endif
