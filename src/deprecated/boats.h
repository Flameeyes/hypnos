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

//For iShipitem::
#define PORT_P_C 0//Port Plank Closed
#define PORT_P_O 1//Port Planl Opened
#define STAR_P_C 2//Starboard Plank Closed
#define STAR_P_O 3//Starb Plank Open
#define TILLERID 5//Tiller
#define HOLDID 4//Hold

extern signed short int iSmallShipOffsets[4][4][2];
extern signed short int iMediumShipOffsets[4][4][2];
extern signed short int iLargeShipOffsets[4][4][2];
extern char cShipItems[4][6];

//NEW BOAT SYSTEM

//bool check_boat_position(pClient client,pItem pBoat);

struct boat_db
{
	uint32_t serial;
	uint32_t tiller_serial;
	uint32_t l_plank_serial;
	uint32_t r_plank_serial;
	uint32_t container;
	pItem p_serial;
	pItem p_tiller;
	pItem p_l_plank;
	pItem p_r_plank;
	pItem p_container;

};


pItem findmulti(Location where);
bool inmulti(Location where,pItem pi);
void insert_boat(pItem pi);
boat_db* search_boat(int32_t ser);
pItem search_boat_by_plank(pItem pl);

class cBoat
{
	protected:
		bool boat_collision(pItem pBoat1,int x1, int y1,int dir,pItem pBoat2);
		bool collision(pItem pi, Location where,int dir);
		bool good_position(pItem pBoat, Location where, int dir);
		bool tile_check(multi_st multi,pItem pBoat,map_st map,int x, int y ,int dir);
		void LeaveBoat(pChar pc, pItem pi);
		void TurnStuff_i(pItem, pItem, int, int);

		void TurnStuff_c(pItem, pChar, int, int);
		void iMove(pClient client, int dir, pItem pBoat, bool forced = true);

		void TurnShip( uint8_t size, int32_t dir, pItem pPort, pItem pStarboard, pItem pTiller, pItem pHold );
	public:

		cBoat();
		pItem GetBoat(Location pos);
		virtual ~cBoat();
		bool Speech(pChar pc, pClient clientocket, std::string &talk );
		void OpenPlank(pItem pi);
		void PlankStuff(pChar pc, pItem pi);
		bool Build(pClient client, pItem pBoat, char);
		void Move(pClient client, int dir, pItem pBoat);
		void Turn(pItem, int);
};

typedef std::map<int,boat_db> BOATS;
extern BOATS	s_boat;

extern cBoat* Boats;

#endif
