/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#ifndef _SREGIONS_H
#define _SREGIONS_H

#include "common_libs.hpp"
#include "structs.hpp"

void loadregions();
void checkregion(pChar pc);
uint16_t calcRegionFromXY(sLocation pos);	// added by Anthalir
uint16_t calcRegionFromXY(uint16_t x, uint16_t y);
void check_region_weatherchange ();
int16_t getIntensityModifier(uint16_t region1, uint16_t region2);

#endif
