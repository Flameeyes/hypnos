/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
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
\author Akron aka Flameeyes (complete rewrite)
*/
class cBook : public cItem
{
public:
//@{
/*!
\name Static members and Typedefs
*/
	//! List of pages (vector of vectors of strings)
	typedef std::vector< std::vector<std::string> > tpages;
//@}
	

//@{
/*!
\name Constructors and operators
\author Akron
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
	inline const bool isReadOnly() const
	{ return flags & flagIsReadOnly; }

	inline void setReadOnly(bool set = true)
	{ setFlag(flagIsReadOnly, set); }
private:
	static const uint64_t flagIsReadOnly	= 0x0000000000010000ull;	//!< Is the book read only?
//@}

	void doubleClicked(pClient client);
	void openBookReadOnly(pClient client);
	void openBookReadWrite(pClient client);
	void sendPageReadOnly(pClient client, uint16_t p);
	void changePages(char *packet, uint16_t p, uint16_t l, uint16_t s);

	//!< Returns the author of the book
	inline std::string getAuthor() const
	{ return author; }
	
	/*!
	\brief Change the book author
	\param auth new author of the book
	*/
	inline void setAuthor(std::string auth)
	{ author = auth; }
	
	//!< Returns the title of the book
	inline std::string getTitle() const
	{ return title; }
	
	/*!
	\brief Change the book title
	\param titl new title of the book
	*/
	void cBook::setTitle(std::string titl)
	{ title = titl; }

	//!< Passes the pages of the book
	inline void getPages(tpages &pags) const
	{ pags = pages; }
	
	//!< return the number of pages of the book
	inline uint32_t getNumPages() const
	{ return pages.size(); }

protected:
	std::string author;		//!< author of the book
	std::string title;		//!< title of the book
	tpages pages;			//!< pages of the book
};

#endif
