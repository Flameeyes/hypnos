/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief GM Pages management implementations
*/

#include "gmpages.h"
#include "common_libs.h"
#include "objects/cchar.h"
#include "networking/cclient.h"
#include "objects/cpc.h"
#include "objects/caccount.h"

GMPageList cGMPage::pages;
uint32_t cGMPage::nextID = 0;

/*!
\brief Default constructor for cGMPage
\param pc Char who requested the page
\param pageReason reason of the page
\param onlyGM if true only complete GMs will be paged
\note This function already add the page to the list
*/
cGMPage::cGMPage(pPC pc, std::string &pageReason, bool onlyGM)
{
	reason = pageReason;
	
	char *msg;
	asprintf(&msg, "GM Page from %s: %s", pc->getBody()->getCurrentName().c_str(), pageReason.c_str());

	bool notified = false;
	for( ClientSList::iterator it = cClient::clients.begin(); it != cClient::clients.end(); it++)
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
	
	pc->pagegm = 0;
	
	free(msg);
	pages.push_back(this);
	caller = pc;
	handler = NULL;
	gm = onlyGM;
}

/*!
\brief Constructor from a previous page
\param old Previous page to copy

Used by requeueGMOnly()
*/
cGMPage::cGMPage(pGMPage old)
{
	pages.push_back(this);
	caller = old->getCaller();
	handler = old->getHandler();
	reason = old->getReason();
	gm = true;
}

/*!
\brief Destructor for cGMPage
\note This function already removes the page from the list!
*/
cGMPage::~cGMPage()
{
	pages.remove(this);
}

/*!
\brief Finds the page handled by the requested handler
\param handler pClient of the GM handling the page
\return pointer to the page handled, or NULL if no pages found for it
*/
pGMPage cGMPage::findPage(pClient handler)
{
	for( GMPageList::iterator it = pages.begin(); it != pages.end(); it++ )
		if ( (*it)->getHandler() == handler )
			return (*it);
	
	return NULL;
}

/*!
\brief Shows the page list to the given client
\param viewer Client who's reading the page list
*/
void cGMPage::showQueue(pClient viewer)
{
	//!\todo Need to add configure option for the minimum level
	if ( viewer->currAccount()->getLevel() < 2 )
		return;
	
	//!\todo Need to add configure also for this!
	bool gmpages = viewer->currAccount()->getLevel() >= 4;
	for( GMPageList::iterator it = pages.begin(); it != pages.end(); it++ )
	{
		if ( !(*it)->getHandler() && ( gmpages || ! (*it)->getGMOnly() ) )
		{
			viewer->sysmessage( "Next unhandled page from %s", (*it)->getCaller()->getBody()->getCurrentName().c_str() );
			viewer->sysmessage( "Problem: %s.", (*it)->getReason().c_str() );
			//!\todo Need to add support for time
//			viewer->sysmessage("Paged at %s.", gmpages[i].timeofcall);
		}
	}
	
	viewer->sysmessage( "");
	viewer->sysmessage( "Total pages in queue: %i", pages.size() );
}

//! Moves the handler to the caller
void cGMPage::moveToCaller()
{
	if ( ! handler || ! caller )
		return;

	handler->currChar()->MoveTo( caller->getBody()->getPosition() );
	handler->sysmessage("Transporting to your current call.");
	handler->currChar()->teleport();
}

//! Requeue a page in the GM only queue
void cGMPage::requeueGMOnly()
{
	new cGMPage(this);
	handler->sysmessage("Call successfully transferred to the GM queue.");
	delete this;
}

#if 0
/*!
\brief Gets the next page
\todo Rewrite it merging counselor and gm calls
*/
void cmdNextCall(pClient cli)
{
	// Type is the same as it is in showgmqueue()
	
	int i;
	int x=0;
	
	pChar pc_currchar = cSerializable::findCharBySerial(currchar[s]);
	
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
				pChar pj=cSerializable::findCharBySerial( gmpages[i].serial.serial32 );
				if(pj)
				{
					sLocation dest= pj->getPosition();
					
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
				pChar pj=cSerializable::findCharBySerial( counspages[i].serial.serial32 );
				if(pj)
				{
					sLocation dest= pj->getPosition();
					
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
#endif
