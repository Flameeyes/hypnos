/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifdef _WIN32

#include "common_libs.h"
#include <process.h>

#include <wefts_mutex.h>

WSADATA wsaData;
WORD wVersionRequested;
long int oldtime, newtime;

bool winSockInit()
{
	wVersionRequested=0x0002;
	int err = WSAStartup(wVersionRequested, &wsaData);
	if (err!=0)
	{
		if (ServerScp::g_nDeamonMode==0)
			MessageBox(NULL,
				"Winsock 2.0 not found. This program requires Winsock 2.0 or later.",
				"Hypnos Network initialization",
				MB_ICONSTOP
				);
		
		ErrOut("ERROR: Winsock 2.0 not found...\n");
		return false;
	}
	return true;
}

void sockManageError(int na)
{
	int bcode = WSAGetLastError ();
	
	const char char *strError = NULL;
	switch(bcode) {
		case WSANOTINITIALISED :
			strError = "Winsock2 initialization problems";
			break;
		case WSAENETDOWN:
			strError = "Network subsystem failure";
			break;
		case WSAEADDRINUSE:
			strError = "Address+port already in use";
			break;
		case WSAEADDRNOTAVAIL:
			strError = "Address use not valid for this machine";
			break;
		case WSAEFAULT:
			strError = "Access violation during binding";
			break;
		case WSAEINPROGRESS:
			strError = "Service provider busy";
			break;
		case WSAEINVAL:
			strError = "Socket already bound";
			break;
		case WSAENOBUFS:
			strError = "Not enough buffers available";
			break;
		case WSAENOTSOCK:
			strError = "Invalid socket";
			break;
		default:
			strError = "Unknown error";
			break;
	}
	
	ErrOut("ERROR: Unable to bind socket (code %d)\n"
		"\tError string: %s\n", bcode, strError);

	if (ServerScp::g_nDeamonMode==0)
		MessageBox(NULL, strError, "Hypnos network error [bind]", MB_ICONSTOP);
}

/*!
\brief Splits a path between path and filename
\author Xanathar [NoX]
\param p the complete path which gets truncated
\return ptr to the filename portion
*/
char *splitPath (char *p)
{
	int i, ln = strlen(p);

	for (i=ln-1; i>=0; i--) {
		if ((p[i]=='\\')) { //||(p[i]=='/')) {
			p[i] = '\0';
			return p+i+1;
		}
	}
	return p; //no backslash found
}

void init_deamon()
{
	WarnOut("Windows needs code to be run in daemon mode...");
}

OSVersion OSVer = OSVER_UNKNOWN;

std::string getOSVersionString()
{
	bool l_bWindowsNT = false;

	OSVERSIONINFO vi = { sizeof(vi) };
	char *s;
	GetVersionEx(&vi);
	if ((vi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS))
	{
		s = "Windows 95";
		if (vi.dwMinorVersion>=10)
		{
			s = "Windows 98";
			if (strstr(vi.szCSDVersion, "A")!=NULL) s = "Windows 98 2nd Edition";
			if ((vi.dwMinorVersion>11)||(vi.dwMajorVersion>=5)) s = "Windows ME";
		}
		l_bWindowsNT = false;
	} else {
		s = "Windows NT";
		if (vi.dwMajorVersion>=5)
		{
			s = "Windows 2000";
			if ((vi.dwMinorVersion>0)||(vi.dwMajorVersion>5)) s = "Windows XP";
		}
		l_bWindowsNT = true;
	}

	char *temp;
	if (l_bWindowsNT) {
		asprintf(&temp, "%s v%d.%d [Build %d] %s",s,vi.dwMajorVersion , vi.dwMinorVersion ,
			vi.dwBuildNumber , vi.szCSDVersion );
	} else {
		asprintf(&temp, "%s v%d.%d %s [Build %d]",s,vi.dwMajorVersion , vi.dwMinorVersion ,
		vi.szCSDVersion, vi.dwBuildNumber & 0xFFFF );
	}
	OSVer = (l_bWindowsNT) ? OSVER_WINNT : OSVER_WIN9X;

	std::string ret(temp);
	free(temp);
	
	return ret;
}

OSVersion getOSVersion()
{
    if (OSVer==OSVER_UNKNOWN) {
        getOSVersionString();
    }
    return OSVer;
}

//@{
/*!
\name Clock functions
*/

unsigned long initialserversec = 0;
unsigned long initialservermill = 0;

void initclock()
{
	timeb t ;
	::ftime(&t) ;
	initialserversec = t.time ;
	initialservermill = t.millitm ;
}

uint32_t getclockday()
{
	uint32_t seconds;
	uint32_t days ;
	timeb buffer ;
	::ftime(&buffer) ;
	seconds = buffer.time ;
	days = seconds/86400 ;  // (60secs/minute * 60 minute/hour * 24 hour/day)
	return days ;
}

uint32_t getclock()
{
	uint32_t milliseconds;
	uint32_t seconds ;
	timeb buffer ;
	::ftime(&buffer) ;
	seconds = buffer.time ;
	milliseconds = buffer.millitm ;
	if (milliseconds < initialservermill)
	{
		milliseconds = milliseconds + 1000 ;
		seconds  = seconds - 1 ;
	}
	milliseconds = ((seconds - initialserversec) * 1000) + (milliseconds -initialservermill ) ;
	return milliseconds ;
}

/*!
\author Keldan
\since 0.82r3
\brief get current system clock time

used by getSystemTime amx function
*/
uint32_t getsysclock()
{
   uint32_t seconds ;
   timeb buffer ;
   ::ftime(&buffer) ;
   seconds = buffer.time ;
   return seconds ;
}

//@}

char *basename(char *path)
{
	// ret= end of string path
	char *ret= path+strlen(path);

	// stop on the first '/' or '\' encountered
	while( (*ret!='\\') && (*ret!='/') ) ret--;
	return ++ret;
}

#endif
