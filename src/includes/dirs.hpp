/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __DIRS_H__
#define __DIRS_H__

#include "common_libs.hpp"
#include "libhypnos/hypstl/string.hpp"

/*!
\brief Default directories handling

This namespace is used to allow Hypnos to accomply with FHS specifications
under Unix, and still manage the directories under Windows in a Windows-way.
*/
namespace nDirs {
	string getPidFilePath();
	
	string getMulsDir();
	void setMulsDir(string newdir);
	
	string getLogsDir();
	void setLogsDir(string newdir);

}

/*!
\brief Checks if a file exists already
\param filename Relative path of the file to check
\return true if the file exists, else false
*/
bool fileExists(string filename);

/*!
\brief Check if the directory of the given file exists, and if not, create it
\param dirname Relative path of the file to check the directory of
\return false if unable to create the directory, else true
*/
bool ensureDirectory(string filename);

#endif
