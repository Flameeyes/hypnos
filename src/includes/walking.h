/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
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
inline const UI08 getLeftDir(UI08 dir)
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
inline const UI08 getRightDir(UI08 dir)
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
inline void getXYfromDir(UI08 dir, UI16 &x, UI16 &y)
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

void handleCharsAtNewPos( P_CHAR pc );
bool handleItemsAtNewPos(P_CHAR pc, UI16 oldx, UI16 oldy, UI16 newx, UI16 newy);
void sendToPlayers( P_CHAR pc, SI08 dir );

void walking(P_CHAR pc, UI08 dir, int sequence);
void npcwalk( P_CHAR pc_i, UI08 newDirection, int type);


#endif

