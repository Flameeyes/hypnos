  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

/*!
\file 
\brief Old Menu related stuff
*/

#ifndef _OLDMENU_H_
#define _OLDMENU_H_


#include "common_libs.h"
#include "packets.h"
#include "menu.h"


/*!
\brief an Old type menu
*/
class cOldMenu : public cBasicMenu
{

	friend class cMakeMenu;
	friend class cPartyMenu;

protected:

	std::map< uint8_t, std::map< uint32_t, std::wstring >  > allPages; //!< all pages

	virtual cServerPacket* build();
	void buildClassic();
	void buildIconList();

public:

	P_MENU type;
	
	wstring title; //!< title
	uint32_t style; //!< style
	uint32_t color; //!< color
	uint32_t width; //!< width
	uint32_t rowForPage; //!< row for page

	cOldMenu();
	~cOldMenu();

	void setParameters( int rowForPage, int pageCount );
	virtual void addMenuItem( int page, int idx, std::wstring desc );

	virtual void handleButton( NXWCLIENT ps, cClientPacket* pkg  );

};

typedef cOldMenu*	P_OLDMENU;



#endif
