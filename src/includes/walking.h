/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
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
\author Duke
\param dir initial direction
\return the adjacent direction
*/
inline const uint8_t getLeftDir(uint8_t dir)
{
	dir &= 7;	// make sure it's valid
	return dir==0 ? 7 : dir-1;
}

/*!
\brief Calculates the adjacent direction (clockwise)
\author Duke
\param dir initial direction
\return the adjacent direction
*/
inline const uint8_t getRightDir(uint8_t dir)
{
	dir &= 7;
	return dir==7 ? 0 : dir+1;
}

/*!
\brief Calculats and changes the given coords one step into the given direction
\author Duke
\param dir the direction
\param x reference to the x coord
\param y reference to the y coord
\todo use reference instead of pointer?
*/
inline void getXYfromDir(uint8_t dir, uint16_t &x, uint16_t &y)
{
	switch(dir&0x07)
	{
	case 0: y--;		break;
	case 1: x++; y--;	break;
	case 2: x++;		break;
	case 3: x++; y++;	break;
	case 4: y++;		break;
	case 5: x--; y++;	break;
	case 6: x--;		break;
	case 7: x--; y--;	break;
	}
}

void handleCharsAtNewPos( pChar pc );
bool handleItemsAtNewPos(pChar pc, uint16_t oldx, uint16_t oldy, uint16_t newx, uint16_t newy);
void sendToPlayers( pChar pc, int8_t dir );

void walking(pChar pc, uint8_t dir, int sequence);
void npcwalk( pChar pc_i, uint8_t newDirection, int type);


#endif

