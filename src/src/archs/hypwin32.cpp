/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifdef WIN32

#include "common_libs.h"
#include "backend/notify.h"

#include <process.h>
#include <wefts_mutex.h>

WSADATA wsaData;
WORD wVersionRequested;
long int oldtime, newtime;

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
	outWarning("Windows needs code to be run in daemon mode...");
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

char *basename(char *path)
{
	// ret= end of string path
	char *ret= path+strlen(path);

	// stop on the first '/' or '\' encountered
	while( (*ret!='\\') && (*ret!='/') ) ret--;
	return ++ret;
}

#endif
