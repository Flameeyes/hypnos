/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifdef _WIN32

#include "archs/hypwin32.h"
#include <process.h>

#include <wefts_mutex.h>

namespace arch {
	WSADATA wsaData;
	WORD wVersionRequested;
	long int oldtime, newtime;
	
/*!
\brief Gets a string value from the KHEY_LOCAL_MACHINE registry
\author Xanathar [NoX]
\param key the key name
\param subjey subkey name
*/
char* getHKLMRegistryString(char *key, char *subkey)
{
	// Get values from registry, use REGEDIT to see how data is stored
	HKEY hTestKey;
	DWORD dwRegType, dwBuffSize;
	unsigned char val[5000];

	if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, key, 0, KEY_READ,  &hTestKey) == ERROR_SUCCESS)
	{
		dwBuffSize = 4096;

		int ret = RegQueryValueEx (hTestKey, subkey, NULL,  &dwRegType,  val,  &dwBuffSize);
		if (ret!=ERROR_SUCCESS) return NULL;
		int n = strlen((const char *)val);
		char *p = new char[n+4];
		strcpy(p,(const char *)val);
		RegCloseKey (hTestKey);
		return p;
	}

	return NULL;

}

/*!
\brief Sets a key in the HKEY_LOCAL_MACHINE registry
\author Xanathar [NoX]
\param key the key name
\param subkey subkey name
\param value value to set the key to
*/
void setHKLMRegistryString(char *key, char *subkey, char *value)
{
	// Demonstrate registry open and create functions
	HKEY hTestKey;
	DWORD dwCreateResult;

	// Access using preferred 'Ex' functions
	if (RegCreateKeyEx (HKEY_LOCAL_MACHINE, key, 0, subkey, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,  &hTestKey,  &dwCreateResult) == ERROR_SUCCESS)
	{
		RegSetValueEx(hTestKey, subkey, 0, REG_SZ, (CONST uint8_t *)value, strlen(value));
		RegCloseKey (hTestKey);
	}
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

void initclock()
{
	timeb t ;
	::ftime(&t) ;
	initialserversec = t.time ;
	initialservermill = t.millitm ;
}

} // namespace arch

static char g_szOSVerBuffer[1024];
char* getOSVersionString()
{
	g_szOSVerBuffer[0] = '\0';
	bool l_bWindowsNT = false;

	OSVERSIONINFO vi = { sizeof(vi) };
	char s[80];
	GetVersionEx(&vi);
	if ((vi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS))
	{
		strcpy (s, "Windows 95");
		if (vi.dwMinorVersion>=10)
		{
			strcpy (s, "Windows 98");
			if (strstr(vi.szCSDVersion, "A")!=NULL) strcpy (s, "Windows 98 2nd Edition");
			if ((vi.dwMinorVersion>11)||(vi.dwMajorVersion>=5)) strcpy (s, "Windows ME");
		}
		l_bWindowsNT = false;
	} else {
		strcpy (s, "Windows NT");
		if (vi.dwMajorVersion>=5)
		{
			strcpy (s, "Windows 2000");
			if ((vi.dwMinorVersion>0)||(vi.dwMajorVersion>5)) strcpy (s, "Windows XP");
		}
		l_bWindowsNT = true;
	}

	if (l_bWindowsNT) {
		sprintf(g_szOSVerBuffer, "%s v%d.%d [Build %d] %s",s,vi.dwMajorVersion , vi.dwMinorVersion ,
			vi.dwBuildNumber , vi.szCSDVersion );
	} else {
		sprintf(g_szOSVerBuffer, "%s v%d.%d %s [Build %d]",s,vi.dwMajorVersion , vi.dwMinorVersion ,
		vi.szCSDVersion, vi.dwBuildNumber & 0xFFFF );
	}
	g_OSVer = (l_bWindowsNT) ? OSVER_WINNT : OSVER_WIN9X;

	return g_szOSVerBuffer;
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

char *basename(char *path)
{
	char *ret= path+strlen(path);				// ret= end of string path

	while( (*ret!='\\') && (*ret!='/') ) ret--;	// stop on the first '/' or '\' encountered
	return ++ret;
}

static const char as_buffer[4096];
static Wefts::Mutex as_mutex;

int asprintf(char **strp, const char *fmt, ...)
{
	as_mutex.lock();
	va_list argptr;
	va_start( argptr, fmt );
        int retval = vsnprintf( as_buffer, 4096, fmt, argptr );
	va_end( argptr );
	
	if ( retval == -1 )
	{
		as_mutex.unlock();
		return -1;
	}
	if ( retval >= 4096 )
	{
		*strp = (char*)malloc(retval+1);
		va_start( argptr, fmt );
		int retval2 = vsnprintf( tempbuff, retval+1, fmt, argptr );
		va_end( argptr );
		as_mutex.unlock();
		return retval2;
	}
	
	*strp = (char*)malloc(retval+1);
	strncpy( *strp, as_buffer, retval+1 );
	as_mutex.unlock();
	return retval;
}

int vasprintf(char **strp, const char *fmt, va_list ap)
{
	
	mutex.lock();
        int retval = vsnprintf( as_buffer, 4096, fmt, ap );
	
	if ( retval == -1 )
	{
		as_mutex.unlock();
		return -1;
	}
	if ( retval >= 4096 )
	{
		va_end( ap );
		*strp = (char*)malloc(retval+1);
		int retval2 = vsnprintf( tempbuff, retval+1, fmt, ap );
		as_mutex.unlock();
		return retval2;
	}
	
	*strp = (char*)malloc(retval+1);
	strncpy( *strp, as_buffer, retval+1 );
	as_mutex.unlock();
	return retval;
}

#endif
