/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Declaration of cAccount class
*/

#ifndef __CACCOUNT_H__
#define __CACCOUNT_H__

class cAccount;
typedef cAccount *pAccount;

#include <zthread/FastMutex.h>
#include <string>
#include <xercesc/parsers/XercesDOMParser.hpp>

class cAccount
{
public:
	static void save(std::string filename);
	static void load(std::string filename);

protected:
	ZThread::FastMutex local_mt;
	static ZThread::FastMutex global_mt;

public:
	cAccount();
	~cAccount();


};

#endif
