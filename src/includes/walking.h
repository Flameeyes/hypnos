/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Walking functions' declaration
*/

#ifndef __WALKING_H__
#define __WALKING_H__

void handleCharsAtNewPos( pChar pc );
bool handleItemsAtNewPos(pChar pc, uint16_t oldx, uint16_t oldy, uint16_t newx, uint16_t newy);
void sendToPlayers( pChar pc, int8_t dir );

void walking(pChar pc, uint8_t dir, int sequence);
void npcwalk( pChar pc_i, uint8_t newDirection, int type);


#endif
