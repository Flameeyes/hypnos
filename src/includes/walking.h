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

/*!
\brief Calculates the adjacent direction (counterclockwise)
\param dir initial direction
\return The adjacent direction
*/
inline const uint8_t getLeftDir(uint8_t dir)
{
	dir &= 7;	// make sure it's valid
	return dir==0 ? 7 : dir-1;
}

/*!
\brief Calculates the adjacent direction (clockwise)
\param dir initial direction
\return The adjacent direction
*/
inline const uint8_t getRightDir(uint8_t dir)
{
	dir &= 7;
	return dir==7 ? 0 : dir+1;
}

void handleCharsAtNewPos( pChar pc );
bool handleItemsAtNewPos(pChar pc, uint16_t oldx, uint16_t oldy, uint16_t newx, uint16_t newy);
void sendToPlayers( pChar pc, int8_t dir );

void walking(pChar pc, uint8_t dir, int sequence);
void npcwalk( pChar pc_i, uint8_t newDirection, int type);


#endif

