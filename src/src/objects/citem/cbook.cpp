/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Implementation of cBook Class
*/
#include "objects/citem/cbook.h"
#include "objects/cclient.h"
#include "basics.h"

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
\author Akron
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
		WarnOut("Invalid page index in packet (index: %d max: %d) [books.cpp]\n", p, pages.size());
		return;
	}
	if ( l >= pages[p].size() )
	{
		WarnOut("Invalid line index in packet (index: %d max: %d) [books.cpp]\n", l, pages[p].size());
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
\author Akron
\brief Manage the doubleclick of a book-type item
\param client Client which double clicked on the book
*/
void cBook::doubleClicked(pClient client)
{
	pPC pc = client->currChar();

	if ( ! nSettings::Server::isEnabledBookSystem() )
	{
		client->sysmessage("Books are currently disabled, sorry.");
		return;
	}
	
	if ( isReadOnly() )
		openBookReadOnly(client);
	else
		openBookReadWrite(client);

	// Should we update this? not sure..
	morey = getNumPages();
}

/*!
\author Akron
\brief Sends to the client data for opening book in read/write mode
\param client client to send the book to
\note it sends <b>a lot</b> of data to client....
*/
void cBook::openBookReadWrite(pClient client)
{
	uint8_t bookopen[9] = { 0x93, 0x40, 0x01, 0x02, 0x03, 0x01, 0x01, 0x00, 0x02 };

	uint16_t bytes;

	char booktitle[61];
	char bookauthor[31];

	strncpy(bookauthor, author.c_str(), 30);
	bookauthor[30] = '\0';
	
	strncpy(booktitle, title.c_str(), 60);
	booktitle[60] = '\0';
	
	LongToCharPtr(getSerial(), bookopen+1);
	ShortToCharPtr(pages.size(), bookopen+7);

	Xsend(s, bookopen, 9);

	Xsend(s, booktitle, 60);
	Xsend(s, bookauthor, 30);

	//////////////////////////////////////////////////////////////
	// Now we HAVE to send the ENTIRE Book                       /
	// Cauz in writeable mode the client only sends out packets  /
	// if something  gets changed                                /
	// this also means -> for each bookopening in writeable mode /
	// lots of data has to be send.                              /
	//////////////////////////////////////////////////////////////

	uint8_t bookpage_pre[9] = { 0x66, 0x01, 0x02, 0x40, 0x01, 0x02, 0x03, 0x00, 0x01 };
	uint8_t bookpage[4] = { 0x00, 0x00, 0x00, 0x08 };

	bytes=9;

	LongToCharPtr(getSerial(), bookpage_pre+3);

	tpages::iterator it ( pages.begin() );
	for ( ; it != pages.end(); it++ )
	{
		bytes += 4; // 4 bytes for each page
		uint16_t j = 0;
		for( std::vector<std::string>::iterator it2 = (*it).begin(); it2 != (*it).end(); it2++, j++ )
			bytes += (*it2).size() + 1;
		while ( j++ < 8 )
			bytes += 2;
	}

	ShortToCharPtr(pages.size(), bookpage_pre+7);
	ShortToCharPtr(bytes, bookpage_pre+1);
	Xsend(s, bookpage_pre, 9);

	i = 1;
	it = pages.begin();
	for ( ; it != pages.end(); it++, i++ )
	{
		ShortToCharPtr(i, bookpage);

		Xsend(s, bookpage, 4);

		uint16_t j = 0;
		for( std::vector<std::string>::iterator it2 = (*it).begin(); it2 != (*it).end(); it2++, j++ )
			Xsend(s, (*it2).c_str(), (*it2).size()+1);
		while ( j++ < 8 )
			Xsend(s, " ", 2);
	}
}

/*!
\brief Sends the book read only to a client
\param client client to send the book to
*/
void cBook::openBookReadOnly(pClient client)
{
	uint8_t bookopen[9] = { 0x93, 0x40, 0x01, 0x02, 0x03, 0x00, 0x01, 0x00, 0x02 };

	char booktitle[61];
	char bookauthor[31];

	strncpy(bookauthor, author.c_str(), 30);
	bookauthor[30] = '\0';
	
	strncpy(booktitle, title.c_str(), 60);
	booktitle[60] = '\0';

	LongToCharPtr(getSerial(), bookopen+1);
	ShortToCharPtr(pages.size(), bookopen+7);

	Xsend(s, bookopen, 9);
	Xsend(s, booktitle, 60);
	Xsend(s, bookauthor, 30);
}

/*!
\brief send to the specified client the page for reading only
\author Akron
\param client client to send the book page
\param p index of page to send
*/
void cBook::sendPageReadOnly(pClient client, uint16_t p)
{
	uint8_t bookpage[13] =
	//	  cmd   -blocksize  --------book id-------  --pages---  --pagenum-  -linenum--
		{ 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08 };
	//	   0     1     2     3     4     5     6     7     8     9     10    11    12
	uint16_t bytes=13;

	if ( p >= pages.size() )
		return;

	std::vector<std::string> selpage = pages[p];

	for(std::vector<std::string>::iterator it = selpage.begin(); it != selpage.end(); it++)
		bytes += (*it).size() + 1;

	ShortToCharPtr(bytes, bookpage+1);
	LongToCharPtr(getSerial(), bookpage+3);
	ShortToCharPtr(p, bookpage+9);
	ShortToCharPtr(selpage.size(), bookpage+11);

	Xsend(s, bookpage, 13);

	uint16_t j = 0;
	for(std::vector<std::string>::iterator its = selpage.begin(); its != selpage.end(); its++, j++)
		Xsend(s, (*its).c_str(), (*its).size()+1);
}
