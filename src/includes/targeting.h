/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Header for targeting handling class
*/

#ifndef _TARGETING_INCLUDED
#define _TARGETING_INCLUDED

#include "common_libs.h"
#include "target.h"

int BuyShop(pClient client, uint32_t c);

void target_envoke( pClient client, pTarget t );
void target_key( pClient client, pTarget t );
void target_axe( pClient client, pTarget t );
void target_sword( pClient client, pTarget t );

void target_expPotion( pClient client, pTarget t );
void target_npcMenu( pClient client, pTarget t );
void target_trigger( pClient client, pTarget t );

void target_follow( pClient client, pTarget t );
void target_attack( pClient client, pTarget t );
void target_playerVendorBuy( pClient client, pTarget t );
void target_allAttack( pClient client, pTarget t );
void target_fetch( pClient client, pTarget t );
void target_guard( pClient client, pTarget t );
void target_transfer( pClient client, pTarget t );

#endif	// _TARGETING_INCLUDED
