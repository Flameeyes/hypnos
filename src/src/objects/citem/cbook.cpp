/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Implementation of cBook Class
*/
#include "objects/citem/cbook.h"
#include "networking/cclient.h"
#include "networking/sent.h"

//! Base constructor
cBook::cBook()
	: cItem()
{
	setReadOnly(false);

	pages = tpages(pages.size(), std::vector<std::string>(8, std::string("")));
}

//! Constructor when serial is known
cBook::cBook(uint32_t serial)
	: cItem(serial)
{
	setReadOnly(false);

	pages = tpages(pages.size(), std::vector<std::string>(8, std::string("")));
}

/*!
\brief Assignment operator
\param oldbook book to assign to this
*/
cBook &cBook::operator =(const cBook &oldbook)
{
	cItem::operator=(dynamic_cast<const cItem&>(oldbook));
	
	setReadOnly(oldbook.isReadOnly());
	author = oldbook.getAuthor();
	title = oldbook.getTitle();
	oldbook.getPages(pages);
	return *this;
}

/*!
\brief parse the 0x66 package replied by client for page changes
\author Flameeyes
\param packet pointer to packet got from socket
\param p index of page to change
\param l lines to process
\param size size of the packet buffer
*/
void cBook::changePages(char *packet, uint16_t p, uint16_t l, uint16_t size)
{
	p--; l--;
	uint16_t bp = 0, lin = 0, lp = 0;
	char ch;
	char s[34];
	if ( p >= pages.size() )
	{
		LogWarning("Invalid page index in packet (index: %u max: %uz)\n", p, pages.size());
		return;
	}
	if ( l >= pages[p].size() )
	{
		LogWarning("Invalid line index in packet (index: %u max: %uz)\n", l, pages[p].size());
		return;
	}

	while( bp < size )
	{
		if ( bp > 512 ) bp = size;
		ch = packet[bp];

		if ( lin < 33 )
			s[lin] = ch;
		else
			s[33] = ch;

		bp++;
		lin++;

		if ( ! ch )
		{
			lp++;
			lin = 0;
			pages[p][lp-1] = s;
		}

	}
}

/*!
\author Flameeyes
\brief Manage the doubleclick of a book-type item
\param client Client which double clicked on the book
*/
void cBook::doubleClicked(pClient client)
{
	if ( ! nSettings::Server::isEnabledBookSystem() )
	{
		client->sysmessage("Books are currently disabled, sorry.");
		return;
	}
	
	if ( isReadOnly() )
		openBookReadOnly(client);
	else
		openBookReadWrite(client);
}

/*!
\author Flameeyes
\brief Sends to the client data for opening book in read/write mode
\param client client to send the book to
\note it sends <b>a lot</b> of data to client....

Here we <b>have to</b> send the \b entire book, because in writable mode the 
client only sends out packets if something gets changed.
*/
void cBook::openBookReadWrite(pClient client)
{
	nPackets::Sent::BookHeader pk1(this, false);
	client->sendPacket(&pk1);
	
	nPackets::Sent::BookPagesReadWrite pk2(this);
	client->sendPacket(&pk2);
}

/*!
\brief Sends the book read only to a client
\param client client to send the book to
*/
void cBook::openBookReadOnly(pClient client)
{
	nPackets::Sent::BookHeader pk(this, false);
	client->sendPacket(&pk);
}

/*!
\brief send to the specified client the page for reading only
\author Flameeyes
\param client client to send the book page
\param p index of page to send
*/
void cBook::sendPageReadOnly(pClient client, uint16_t p)
{
	if ( p >= pages.size() )
		return;
	
	nPackets::Sent::BookPageReadOnly pk(this, p);
	client->sendPacket(&pk);
}
