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

class cGMPage;
typedef cGMPage *pGMPage;		//!< Pointer to a GM Page
typedef std::list<pGMPage> GMPageList;	//!< List of GM Pages

class cGMPage {
public:
	enum HandledStatus {
		eNotHandled,		//!< The query was never handled
		eHandling,		//!< Query's handling in progress
		eHandled,		//!< Query already handled
		eReQueued		//!< Query was handled, but it was requeued
	};
	cGMPage(pChar pc, std::string &pageReason, bool onlyGM = false);
	
protected:
	std::string reason;		//!< Reason of the page (from the user)

	static GMPageList pages;	//!< List of pages
	static uint32_t nextID;		//!< ID to assign to the next instance
};

void cmdNextCall(pClient cli);
