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
\brief Addmenu from MakeItem stuff
*/


#ifndef __ADDMENU_H
#define __ADDMENU_H


#include "menu.h"
#include "oldmenu.h"
#include "utils.h"
#include "chars.h"
#include "client.h"


class cRawItem {

public:

    int32_t id;
    COLOR color;
    uint32_t number;

	cRawItem( std::string& s );
	cRawItem( int32_t id=0, COLOR color=0, uint32_t number=0 );
	~cRawItem();

};


/*!
\brief Class MakeItem
*/
class cMakeItem {

public:

    cScriptCommand* command;
    int32_t skillToCheck;
    int32_t minskill;
    int32_t maxskill;
	cRawItem reqitems[2];

    
	cMakeItem();
	~cMakeItem();
    bool checkReq( pChar pc, bool inMenu = false, cRawItem* def = NULL );
};

class cMakeMenu : public cBasicMenu 
{

	private:
		std::vector<cMakeItem*>	makeItems;

	protected:

		uint32_t section;
		P_OLDMENU oldmenu;

		virtual void loadFromScript( pChar pc );
		std::string cleanString( std::string s );
		void execMake( NXWCLIENT ps, uint32_t item );

	protected:
		virtual cServerPacket* build();

	public:

		int skill;
		cRawItem mat[2];
		
		cMakeMenu( uint32_t section );
		cMakeMenu( uint32_t section, pChar pc, int skill, uint16_t firstId=0, COLOR firstColor=0, uint16_t secondId=0, COLOR secondColor=0 );

		~cMakeMenu();

		virtual void handleButton( NXWCLIENT ps, cClientPacket* pkg  );

};

class cAddMenu : public cMakeMenu 
{

	private:
		std::vector< cScriptCommand > commands;
		virtual void loadFromScript( pChar pc );

	public:
		cAddMenu( uint32_t section, pChar pc );
		~cAddMenu();

		virtual void handleButton( NXWCLIENT ps, cClientPacket* pkg  );

};

void showAddMenu( pChar pc, int menu );

/*!
\brief Open a scripted menu
\author Endymion
*/
inline void itemmenu( NXWSOCKET s, int32_t m )
{
	pChar pc = MAKE_CHAR_REF( currchar[s] );
	if( ISVALIDPC( pc ) )
	{
		showAddMenu( pc, m ); 
	}
}

/*!
\brief Open one of the gray GM Call menus
\author Endymion
*/
inline void gmmenu( NXWSOCKET s, int32_t m )
{
	itemmenu( s, 7009+m );
}



#endif
