/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Unix Architecture specific
*/

#ifndef _PYUO_UNIX_
#define _PYUO_UNIX_

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netdb.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <libgen.h>

#define ioctlsocket ioctl

#include <signal.h>
#include <sys/errno.h>
#include <arpa/inet.h>
#define closesocket(s)	close(s)
#include <sys/utsname.h>

#define SOCKET_ERROR -1

#if defined(__OpenBSD__) || defined(__FreeBSD__)
    #include <pthread.h>
#endif

extern bool pollHUPStatus ();
extern bool pollCloseRequests ();
extern void setup_signals ();
extern void start_signal_thread();

char *strlwr(char *);
char *strupr(char *);

void init_deamon();
void initclock();

void Sleep(unsigned long msec);

#endif