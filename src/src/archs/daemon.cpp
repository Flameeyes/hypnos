/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2003 - NoX-Wizard Project                                  |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "archs/daemon.h"

void init_deamon()
{
	int i ;
	pid_t pid ;

	if ((pid = fork() ) != 0)
		_exit(0) ;
	
	setsid() ;
	if ((pid=fork()) != 0)
	{
		std::fstream fPid("nxwpid", std::ios::out);
		fPid << pid << std::endl;
		fPid.close() ;
		_exit(0);
	}
	// We should close any dangling descriptors we have
	for (i=0 ; i < 64 ; i++)
		close(i) ;
}
