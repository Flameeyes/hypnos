/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file 
\brief Old Menu related stuff
*/

#ifndef _OLDMENU_H_
#define _OLDMENU_H_

#include "common_libs.hpp"
#include "menu.hpp"

/*!
\brief an Old type menu
*/
class cOldMenu : public cBasicMenu
{

	friend class cMakeMenu;
	friend class cPartyMenu;

protected:
	map< uint8_t, map< uint32_t, unistring >  > allPages; //!< all pages

	virtual void /*cServerPacket*/* build();
	void buildClassic();
	void buildIconList();

public:

	pMenu type;
	
	unistring title; //!< title
	uint32_t style; //!< style
	uint32_t color; //!< color
	uint32_t width; //!< width
	uint32_t rowForPage; //!< row for page

	cOldMenu();
	~cOldMenu();

	void setParameters( int rowForPage, int pageCount );
	virtual void addMenuItem( int page, int idx, unistring desc );

	virtual void handleButton(pClient client, /*cClientPacket*/void* pkg  );

};

typedef cOldMenu*	P_OLDMENU;

#endif
