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
\brief Menu related stuff
\author Endymion
*/

#ifndef _MENU_H_
#define _MENU_H_


#include "common_libs.h"
#include "packets.h"


#define MENUSTYLE_LARGE 128

//rename after from M_ to MENUTYPE_
enum MENU_TYPE {
	MENUTYPE_INVALID = INVALID,
	MENUTYPE_STONE = 0,
	MENUTYPE_SCROLL,
	MENUTYPE_PAPER,
	MENUTYPE_BLACKBOARD,
	MENUTYPE_TRASPARENCY,
	MENUTYPE_CUSTOM,
	MENUTYPE_ICONLIST = 16,
	MENUTYPE_ADDMENU
};

#define MENU_CLOSE 0

typedef void ( *menu_callback )	( P_MENU, NXWCLIENT, int32_t );

/*!
\brief an Basic Menu
\author Endymion
*/
class cBasicMenu {

	friend class cOldMenu;
	friend class cNewAddMenu;
	friend class cMenus;

	private:

		std::set<uint32_t> whoSeeThis; //!< who are seeing this menu

	protected:

		AmxFunction* callback; //!< function callback

		virtual cServerPacket* createPacket();
		virtual cServerPacket* build();

	public:

		MENU_TYPE id; //!< gump id
		menu_callback hard; //!< hard code callback

		cBasicMenu( MENU_TYPE id );
		virtual ~cBasicMenu();

		uint32_t	serial; //!< serial

		void setCallBack( std::string arg );
		void setCallBack( FUNCIDX fn );

		virtual void handleButton( NXWCLIENT ps, cClientPacket* pkg  );
		virtual void show( pChar pc );


};


#define ISVALIDPM( M )	( M!=NULL ) 
#define VALIDATEPM( M )	if( !ISVALIDPM(M) )	return;
#define VALIDATEPMR( M, R )	if( !ISVALIDPM(M) )	return R;

#define MENU_BUFF_COUNT 4
#define ISVALIDMENUBUFFER( I ) ( (I>INVALID) && (I<MENU_BUFF_COUNT) )

/*!
\brief an Menu
\author Endymion
*/
class cMenu : public cBasicMenu
{

	private:

		bool closeable;
		bool moveable;
		bool disposeable;
		
		uint32_t rc_serialCurrent;	//!< current code serial current
		std::map< uint32_t, int32_t > rc_button;	//!< return code for for button
		std::map< uint32_t, int32_t > rc_radio;	//!< return code for for radio
		std::map< uint32_t, int32_t > rc_checkbox;	//!< return code for for checkbox
		std::map< uint32_t, int32_t > rc_edit;	//!< return code for for edit

		std::map< uint32_t, FUNCIDX > buttonCallbacks;	//!< all callback for button
		std::map< uint32_t, int32_t > editProps;	//!< all edit property

		std::vector< uint32_t >* switchs; //!< switch ids on after menu selection
		std::map< uint32_t, std::wstring >* textResp; //!< edit field response

		uint32_t addString( wstring s );

		void removeCommand( std::string command );
		void removeCommand( char* s, ... );

		void setPropertyField( uint32_t type, SERIAL obj, int prop, int subProp, int subProp2, bool data );
		void setPropertyField( uint32_t type, SERIAL obj, int prop, int subProp, int subProp2, std::wstring data );
		
		bool getPropertyFieldBool( uint32_t type, SERIAL obj, int prop, int subProp, int subProp2 );
		std::wstring getPropertyField( uint32_t type, SERIAL obj, int prop, int subProp, int subProp2 );

		int32_t getIntFromProps( int prop, int prop2, int prop3 );
		void getPropsFromInt( int32_t returnCode, int& prop, int& prop2, int& prop3 );

	protected:
		std::vector< std::string >	commands; //!< all commands
		std::vector< wstring >	texts; //!< all strings
	
		virtual cServerPacket* createPacket();

	public:
				
		uint32_t x; //!< x coord where show
		uint32_t y; //!< y location where show
		uint32_t pageCount;	//!< page count
		uint32_t pageCurrent;	//!< current page
		uint32_t buffer[MENU_BUFF_COUNT];
		std::string buffer_str[MENU_BUFF_COUNT];

		cMenu( MENU_TYPE id, uint32_t x, uint32_t y, bool canMove, bool canClose, bool canDispose );
		~cMenu();

		void setCloseable( bool canClose );
		bool getCloseable();
		void setDisposeable( bool canDispose );
		bool getDisposeable();
		void setMoveable( bool canMove );
		bool getMoveable();

		virtual void handleButton( NXWCLIENT ps,  cClientPacket* pkg  );

		void addCommand( std::string command );
		void addCommand( char* s, ... );

		void addBackground( uint32_t gumpId, uint32_t width, uint32_t height );
		void addButton( uint32_t x, uint32_t y, uint32_t up, uint32_t down, int32_t returnCode, bool pressable );
		void addButtonFn( uint32_t x, uint32_t y, uint32_t up, uint32_t down, int32_t returnCode, bool pressable, FUNCIDX fn );
		void addCheckbox( uint32_t x, uint32_t y, uint32_t off, uint32_t on, uint32_t checked, int32_t result );
		void addCheckertrans( uint32_t x, uint32_t y, uint32_t width, uint32_t height );
		void addCroppedText( uint32_t x, uint32_t y, uint32_t width, uint32_t height, wstring text, uint32_t hue );
		void addGump( uint32_t x, uint32_t y, uint32_t gump, uint32_t hue );
		void addHtmlGump( uint32_t x, uint32_t y, uint32_t width, uint32_t height, wstring html, uint32_t hasBack, uint32_t canScroll );
		void addInputField( uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint16_t textId, wstring data, uint32_t hue = 0 );
		void addPropertyField( uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t property, uint32_t subProperty, uint32_t hue = 0, uint32_t subProperty2 = 0 );
		void addRadioButton( uint32_t x, uint32_t y, uint32_t off, uint32_t on, uint32_t checked, int32_t result  );
		void addResizeGump( uint32_t x, uint32_t y, uint32_t gumpId, uint32_t width, uint32_t height );
		void addText( uint32_t x, uint32_t y, wstring data, uint32_t hue = 0 );
		void addTilePic( uint32_t x, uint32_t y, uint32_t tile, uint32_t hue = 0 );
		void addTiledGump( uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t gump, uint32_t hue );
		void addXmfHtmlGump( uint32_t x, uint32_t y, uint32_t width, uint32_t height, wstring clilocid, uint32_t hasBack , uint32_t canScroll );
		//void 	addGroup( uint32_t group = 0 );
		void addPage( uint32_t page );
		void addPageButton( uint32_t x, uint32_t y, uint32_t up, uint32_t down, uint32_t page );

		bool getCheckBox( uint32_t checkbox, bool raw=false );
		bool getRadio( uint32_t radio, bool raw=false );
		std::wstring* getText( uint32_t text, bool raw=false );
		int32_t getButton( int32_t rawButton );
};


class cIconListMenu : public cBasicMenu
{

	protected:

		virtual cServerPacket* createPacket();
		std::vector< pkg_icon_list_menu_st > icons;
		std::map< uint32_t, int32_t > iconData;

	public:

		std::string question;

		cIconListMenu();
		~cIconListMenu();

		virtual void handleButton( NXWCLIENT ps,  cClientPacket* pkg  );
		void addIcon( uint16_t model, COLOR color, std::string response );
		void addIcon( uint16_t model, COLOR color, int32_t data, std::string response );

};


bool isIconList( NXWSOCKET s );
bool isIconList( uint8_t cmd );

typedef std::map< uint32_t, P_MENU > MENU_MAP;

/*!
\brief all Menus
*/
class cMenus
{
	
	friend class cBasicMenu;

	private:
		uint32_t current_serial; //!< current serial
		MENU_MAP menuMap; //!< every opened menus

		std::map< uint32_t, std::set<SERIAL> > whoSeeWhat; //!< player see menus

		bool removeFromView( P_MENU menu, uint32_t chr );

	public:

		cMenus();
		~cMenus();

		P_MENU insertMenu( P_MENU menu );
		uint32_t removeMenu( SERIAL menu, pChar pc = NULL );
		bool handleMenu( NXWCLIENT ps );
		P_MENU getMenu( uint32_t menu );
		bool showMenu( uint32_t menu, pChar pc );

};

extern cMenus Menus;

#endif
