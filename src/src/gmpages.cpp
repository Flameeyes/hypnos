/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief GM Pages management implementations
*/

#include "gmpages.h"
#include "objects/cclient.h"

GMPageList cGMPage::pages;
uint32_t cGMPage::nextID = 0;

/*!
\brief Default constructor for cGMPage
\param pc Char who requested the page
\param pageReason reason of the page
\param onlyGM if true only complete GMs will be paged
\note This function already add the page to the list
*/
cGMPage::cGMPage(pChar pc, std::string &pageReason, bool onlyGM)
{
	reason = pageReason;
	
	char *msg;
	asprintf(&msg, "GM Page from %s [%08x]: %s", pc->getCurrentName().c_str(), pc->getSerial(), pageReason.c_str());

	bool notified = false;
	for( cClients::iterator it = cClient::clients.begin(); it != cClient::clients.end(); it++)
	{
		if ( onlyGM ? (*it)->currAccount()->seeGMPages() : (*it)->currAccount()->seeConsPages() )
		{
			notified = true;
			(*it)->sysmessage(msg);
		}
	}
	
	pc->getClient()->sysmessage( notified ?
		"Available Game Masters have been notified of your request." :
		"There was no Game Master available to take your call."
		);
	
	pc->pagegm = false;
	
	free(msg);
	pages.push_back(this);
}

/*!
\brief Destructor for cGMPage
\note This function already removes the page from the list!
*/
cGMPage::~cGMPage()
{
	
}

/*!
\brief Gets the next page
\todo Rewrite it merging counselor and gm calls
*/
void cmdNextCall(pClient cli)
{
	// Type is the same as it is in showgmqueue()
	
	int i;
	int x=0;
	
	P_CHAR pc_currchar = MAKE_CHAR_REF(currchar[s]);
	
	if(pc_currchar->callnum!=0)
	{
		donewithcall(s, type);
	}
	
	if(type==1) //Player is a GM
	{
		for(i=1;i<MAXPAGES;i++)
		{
			if(gmpages[i].handled==0)
			{
				P_CHAR pj=pointers::findCharBySerial( gmpages[i].serial.serial32 );
				if(ISVALIDPC(pj))
				{
					Location dest= pj->getPosition();
					
					pc_currchar->sysmsg("");
					pc_currchar->sysmsg("Transporting to next call: %s", gmpages[i].name);
					pc_currchar->sysmsg("Problem: %s.", gmpages[i].reason);
					pc_currchar->sysmsg("Serial number: %x", gmpages[i].serial.serial32);
					pc_currchar->sysmsg("Paged at %s.", gmpages[i].timeofcall);
					gmpages[i].handled=1;
					pc_currchar->MoveTo( dest );
					pc_currchar->callnum=i;
					pc_currchar->teleport();
					x++;
				}// if
				if(x>0)break;
			}// if
		}// for
		if(x==0) pc_currchar->sysmsg("The GM queue is currently empty");
	} //end first IF
	else //Player is only a counselor
	{
		x=0;
		for(i=1;i<MAXPAGES;i++)
		{
			if(counspages[i].handled==0)
			{
				P_CHAR pj=pointers::findCharBySerial( counspages[i].serial.serial32 );
				if(ISVALIDPC(pj))
				{
					Location dest= pj->getPosition();
					
					pc_currchar->sysmsg("");
					pc_currchar->sysmsg("Transporting to next call: %s", counspages[i].name);
					pc_currchar->sysmsg("Problem: %s.", counspages[i].reason);
					pc_currchar->sysmsg("Serial number: %x", counspages[i].serial.serial32);
					pc_currchar->sysmsg("Paged at %s.", counspages[i].timeofcall);
					counspages[i].handled=1;
					pc_currchar->MoveTo( dest );
					pc_currchar->callnum=i;
					pc_currchar->teleport();
					x++;
					break;
				}// if
			}// else
			if(x>0)break;
		}//for
		if(x==0) pc_currchar->sysmsg("The Counselor queue is currently empty");
	}//if
}
