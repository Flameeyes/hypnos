/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "pywin32.cpp"
#include <process.h>

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
		RegSetValueEx(hTestKey, subkey, 0, REG_SZ, (CONST BYTE *)value, strlen(value));
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

/*!
\brief Thread abstraction namespace
\author Xanathar
*/
namespace tthreads {
/*!
\author Xanathar
\param funk pointer to thread function
\param param pointer to a volatile buffer created with should be eventually
freed by the thread itself
*/
int startTThread( TTHREAD ( *funk )( void * ), void* param )
{
	pthread_t pt;
	return pthread_create( &pt, NULL, funk, param );
}

} //namespaze

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
