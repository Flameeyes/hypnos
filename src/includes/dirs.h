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

#endif
