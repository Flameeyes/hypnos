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

#include "common_libs.h"
#include "version.h"

#ifdef HAVE_SYS_UTSNAME_H
#include <sys/utsname.h>
#endif

#ifdef HAVE_WINBASE_H
#include <winbase.h>
#endif

std::string getOSVersionString()
{
#ifdef HAVE_UNAME
	char *temp;
	struct utsname info;
	uname(&info);
	asprintf(&temp, "%s %s on a %s", info.sysname, info.release, info.machine);
	
	std::string s(temp);
	free(temp);
	
	return s;
#elif defined(WIN32)
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
		asprintf(&temp, "%s v%d.%d [Build %d] %s",s, vi.dwMajorVersion, vi.dwMinorVersion,
			vi.dwBuildNumber , vi.szCSDVersion );
	} else {
		asprintf(&temp, "%s v%d.%d %s [Build %d]",s, vi.dwMajorVersion, vi.dwMinorVersion,
			vi.szCSDVersion, vi.dwBuildNumber & 0xFFFF );
	}

	std::string ret(temp);
	free(temp);
	
	return ret;
#elif defined(__unix__)
	return "Unix";
#else
	return "Unknown";
#endif
}
