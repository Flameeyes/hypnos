/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "dirs.h"

#ifdef ENABLE_BINRELOC
#include "libhypnos/prefix.h"
#endif

#ifdef WIN32

#include <wefts_mutex.h>

#ifdef HAVE_WINBASE_H
#include <winbase.h>
#endif

#ifdef HAVE_SHLWAPI_H
#include <shlwapi.h>
#endif

/*!
\brief Gets the directory in which the executable is
*/
static string getExePath()
{
	static Wefts::Mutex m;
	static char *buffer[MAX_PATH];
	
	m.lock();
	
	// _pgmptr contains the full path of the executable file
	// (see GetModuleFileName() function documentation on msdn)
	strncpy(buffer, _pgmptr, MAX_PATH);
	
	PathRemoveFileSpec(buffer);
	string retstr(buffer);
	
	m.unlock();
	return retstr;
}

#endif


string mulsDir;	//!< User directory for MUL files
string logsDir;	//!< User directory for log files

/*!
\brief Gets the path for the file where to store the pid of the program.

\note This function make sense only on Unix, and the returned path is always
	/var/run/hypnos.pid, as FHS states. Handling it here is only to make
	cleaner move it around if needed.
\note You should have /var/run accessible to use that :)
*/
string nDirs::getPidFilePath()
{
	return "/var/run/hypnos.pid";
}

/*!
\brief Gets the default MUL files directory.

This function behave in many different ways:

\li If user defines a new muls' directory, it returns that.
\li Under Linux we use BinReloc to have a relocatable binary, so it returns
	always the right $(PREFIX)/share/games/hypnos/muls directory.
\li Under every other unix-like system, it returns the datadir (expanded from
	the configure script), followed by the above directory structure :)
\li Under Windows, it searches the registry to find out the installation
	directory of Ultima OnLine and, if it can't find it, it returns the
	directory of the executable plus "muls/".

\todo Windows support is still missing...
*/
string nDirs::getMulsDir()
{
	if ( !mulsDir.empty() )
		return *mulsDir;

#ifdef ENABLE_BINRELOC
	return BR_DATADIR("/hypnos/muls/");
#elif defined(__unix__)
	return DATADIR "/games/hypnos/muls";
#else // Win32
	char *s = getHKLMRegistryString( "SOFTWARE\\Origin Worlds Online\\Ultima Online\\1.0", "ExePath" );
	if ( s ) {
		PathRemoveFileSpec(s);
		string strret = s;
		delete[] s;
		return strret;
	} else {
		return getExePath() + "\\muls";
	}
#endif
}

/*!
\brief Sets the MUL files directory

This function is provided to allow the developers to call directly
nDirs::getMulsDir() instead of manage an external source for directories.
*/
void nDirS::setMulsDir(const string &newdir)
{
	mulsDir = newdir;
}

/*!
\brief Gets the default log files directory.

This function behave mainly in two ways:

\li Under an FHS-compliant system, like linux, *BSD, MacOSX and so on, it
	returns /var/log/hypnos/.
\li Under Windows, it returns the directory of the executable plus "logs/"

\note You should have permission to write to /var/log/hypnos for the user
	which is running Hypnos.
*/
string nDirs::getLogsDir()
{
	if ( !logsDir.empty() )
		return *logsDir;

#ifdef __unix__
	return "/var/log/hypnos";
#else
	return getExePath() + "\\logs";
#endif
}

/*!
\brief Sets the log files directory

This function is provided to allow the developers to call directly
nDirs::getLogsDir() instead of manage an external source for directories.
*/
void nDirS::setLogsDir(const string &newdir)
{
	logsDir = newdir;
}

