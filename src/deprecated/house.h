/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief House Related Stuff
*/
#ifndef _HOUSE_H_
#define _HOUSE_H_

void buildhouse( pClient client, pTarget t );

void target_houseOwner( pClient client, pTarget t );
void target_houseEject( pClient client, pTarget t );
void target_houseBan( pClient client, pTarget t );
void target_houseFriend( pClient client, pTarget t );
void target_houseUnlist( pClient client, pTarget t );
void target_houseLockdown( pClient client, pTarget t );
void target_houseRelease( pClient client, pTarget t );
void target_houseSecureDown( pClient client, pTarget t );

#endif
