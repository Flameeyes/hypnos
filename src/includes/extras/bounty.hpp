/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Bounty System Stuff
\author Dupois
\since July 17, 2000
*/

#ifndef __BOUNTY_H__
#define __BOUNTY_H__

#include "objects/citem/cmsgboard.hpp"

// Ask victim if they want to post a bounty on the
// murderer, and if so, then return the amount of the
// reward they want to place on the murderers head.
void BountyAskVictim  ( uint32_t nVictimSerial,
                        uint32_t nMurdererSerial );

// Create the bounty on the murderer
bool BountyCreate     ( pChar pc,
                        int nRewardAmount );

// Remove the bounty from the murderer
bool BountyDelete     ( pChar pc );

// Check and then withdraw the bounty amount
bool BountyWithdrawGold( pChar pVictim, int nAmount );

#endif
