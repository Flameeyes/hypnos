/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief GM Pages management declarations
*/

#include "common_libs.h"

class cGMPage {
public:
	cGMPage(pPC pc, std::string &pageReason, bool onlyGM = false);
	cGMPage(pGMPage old);
	
	inline pPC getCaller() const
	{ return caller; }
	
	inline pClient getHandler() const
	{ return handler; }
	
	inline bool getGMOnly() const
	{ return gm; }
	
	inline std::string getReason() const
	{ return reason; }
	
	void moveToCaller();
	void requeueGMOnly();
	
	static pGMPage findPage(pClient handler);
	static void showQueue(pClient viewer);
protected:
	std::string reason;		//!< Reason of the page (from the user)
	pPC caller;			//!< Player who requested the page
	pClient handler;		//!< GM who's responding at the page
	bool gm;			//!< If trye is only for GM, else is in main queue

	static GMPageList pages;	//!< List of pages
	static uint32_t nextID;		//!< ID to assign to the next instance
};

void cmdNextCall(pClient cli);
