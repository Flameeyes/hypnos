/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#ifndef __NTSERVICE_H__
#define __NTSERVICE_H__
#ifdef WIN32

// Xanathar.. we prepare this stuff for NT-Service Executables
// not supported yet though :(

	

void __cdecl svcprintf(char *txt, ...) PRINTF_LIKE(1,2);

#ifdef __cplusplus
extern "C" {
#endif

void __cdecl svcprintf_C(char *txt, ...) PRINTF_LIKE(1,2);
void initService ( void );


#ifdef __cplusplus
}
#endif


#endif //WIN32
#endif //__NTSERVICE_H__
