/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
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
static const char *strSupportedClient = "3.0.3a";

#define OS    "UNKNOWN OS"
#define OSFIX "[UNKNOWN OS]   "
#define PLATFORMID 0

#ifdef _CONSOLE
#undef OS
#undef OSFIX
#undef PLATFORMID
#define OS    ((ServerScp::g_nDeamonMode==0) ? ("Win32-Console") : ("WinNT-Service"))
#define OSFIX ((ServerScp::g_nDeamonMode==0) ? ("[Win32-Console]") : ("[WinNT-Service]"))
#endif

#ifdef __unix__
#undef OS
#undef OSFIX
#define OS    ((ServerScp::g_nDeamonMode==0) ? ("Linux-Console") : ("Linux-Deamon"))
#define OSFIX ((ServerScp::g_nDeamonMode==0) ? ("[Linux-Console]") : ("[Linux-Daemon ]"))
#endif

#if defined(__OpenBSD__)
#undef OS
#undef OSFIX
#define OS    ((ServerScp::g_nDeamonMode==0) ? ("OpenBSD-Console") : ("OpenBSD-Deamon"))
#define OSFIX ((ServerScp::g_nDeamonMode==0) ? ("[OpenBSD tty]  ") : ("[OpenBSD daemn]"))
#endif

#if defined(__FreeBSD__)
#undef OS
#undef OSFIX
#define OS    ((ServerScp::g_nDeamonMode==0) ? ("FreeBSD-Console") : ("FreeBSD-Deamon"))
#define OSFIX ((ServerScp::g_nDeamonMode==0) ? ("[FreeBSD tty]  ") : ("[FreeBSD daemn]"))
#endif

#endif //__VERSION_H__
