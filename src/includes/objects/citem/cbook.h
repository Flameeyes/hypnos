/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Declaration of cBook Class
*/

#ifndef __CBOOKS_H__
#define __CBOOKS_H__

#include "common_libs.h"
#include "objects/citem.h"

/*!
\brief Item class for books
\author Flameeyes

Ultima OnLine provides some different books with different gumps to write the
PCs memories.
We can have mainly two types of books: writable books and read only books.
Read only books are often used in quests or to provide useful informations to
players.

Read write books are like the one the player is getting at the default character
creation, where there's space for the title and the author, and the pages are
blank to be wrote on.

This class provides storage space and functions to handle the books, which are
called by the correspondent UOP packet to get or set the data of the book.

\todo Missing a way to define the different gump of the book (if possible)
\todo Missing a GM-only editable book support
\todo Missing a datafile-loaded readonly book support
*/
class cBook : public cItem
{
public:
//@{
/*!
\name Static members and Typedefs
*/
	//! List of pages (vector of vectors of strings)
	typedef std::vector< stringVector > tpages;
//@}
	

//@{
/*!
\name Constructors and operators
*/
public:
	cBook();
	cBook(uint32_t serial);
	cBook &operator =(const cBook &oldbook);
//@}

//@{
/*!
\name Flags
*/
public:
	inline bool isReadOnly() const
	{ return flags & flagIsReadOnly; }

	inline void setReadOnly(bool set = true)
	{ setFlag(flags, flagIsReadOnly, set); }
protected:
	//! The book is read only (can't be modified neither by GMs)
	static const uint64_t flagIsReadOnly	= 0x0000000000010000ull;
//@}

public:
	void doubleClicked(pClient client);
	void openBookReadOnly(pClient client);
	void openBookReadWrite(pClient client);
	void sendPageReadOnly(pClient client, uint16_t p);
	void changePages(char *packet, uint16_t p, uint16_t l, uint16_t s);

	//! Returns the author of the book
	inline std::string getAuthor() const
	{ return author; }
	
	/*!
	\brief Change the book author
	\param auth new author of the book
	*/
	inline void setAuthor(std::string auth)
	{ author = auth; }
	
	//! Returns the title of the book
	inline std::string getTitle() const
	{ return title; }
	
	/*!
	\brief Change the book title
	\param titl new title of the book
	*/
	void cBook::setTitle(std::string titl)
	{ title = titl; }

	//! Passes the pages of the book
	inline void getPages(tpages &pags) const
	{ pags = pages; }
	 
	//! Gets a given page
	inline stringVector getPage(uint16_t n) const
	{
		if ( n < pages.size() )
			return pages[n];
		else
			return stringVector();
	}
	
	//! return the number of pages of the book
	inline uint16_t getNumPages() const
	{ return pages.size(); }

protected:
	std::string author;		//!< author of the book
	std::string title;		//!< title of the book
	tpages pages;			//!< pages of the book
};

#endif
