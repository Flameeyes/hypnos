/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __VERSION_H__
#define __VERSION_H__

/*!
\brief List of version's developers

Please not remove developers from this string if you aren't a Hypnos Project Manager.
If you added features or fixed bugs in a special version of Hypnos, you can add
yourself, anyway.
*/
static const char *strDevelopers = "Flameeyes, Kheru, Chrono, il_guru, Judas";

//! Hypnos version string
static const char *strVersion = "CVS";

/*!
\brief Emulator's supported client

This is part of the client version control system.
This is NOT necassairily the lastest client.
Don't change it unless you are sure that its packets structure is in synch with network.cpp etc.
*/
static const char *strSupportedClient = "4.0.3";

#endif 
