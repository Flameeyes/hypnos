/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Win32 Registry access

This file contains functions to access the Windows Registry. These functions
are used to find out the Ultima OnLine's installation path and to set up the
service entry.
*/

#ifdef WIN32

#include "common_libs.h"
#include "archs/win32registry.h"

#ifdef HAVE_WINNT_H
#include <winnt.h>
#endif

#ifdef HAVE_WINREG_H
#include <winreg.h>
#endif

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

#endif
