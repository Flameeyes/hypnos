/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Console management
*/
#ifndef __ARCHS_CONSOLE_H__
#define __ARCHS_CONSOLE_H__

#include "backend/notify.h"
#include "archs/tinterface.h"

/*!
\brief Console handling thread
*/
class tConsoleInterface : public tInterface
{
private:
public:
	tConsoleInterface();
	
	void *run();
	
	void output(tInterface::Level lev, const std::string &str);
};

#endif
