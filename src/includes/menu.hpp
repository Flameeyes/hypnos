/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file 
\brief Menu related stuff
\author Endymion
*/

#ifndef _MENU_H_
#define _MENU_H_

#include "common_libs.hpp"

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

typedef void ( *menu_callback ) ( pMenu, pClient, int32_t );
	//!< Menu callback function type

/*!
\brief an Basic Menu
\author Endymion
*/
class cBasicMenu {

	friend class cOldMenu;
	friend class cNewAddMenu;
	friend class cMenus;

	private:

		uint32_set whoSeeThis; //!< who are seeing this menu

	protected:

		virtual void /*cServerPacket*/* createPacket();
		virtual void /*cServerPacket*/* build();

	public:

		MENU_TYPE id; //!< gump id
		menu_callback hard; //!< hard code callback

		cBasicMenu( MENU_TYPE id );
		virtual ~cBasicMenu();

		uint32_t	serial; //!< serial

		void setCallBack( string arg );
		void setCallBack( FUNCIDX fn );

		virtual void handleButton( pClient client, void /*cClientPacket*/* pkg  );
		virtual void show( pChar pc );
};

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
		map< uint32_t, int32_t > rc_button;	//!< return code for for button
		map< uint32_t, int32_t > rc_radio;	//!< return code for for radio
		map< uint32_t, int32_t > rc_checkbox;	//!< return code for for checkbox
		map< uint32_t, int32_t > rc_edit;	//!< return code for for edit

		map< uint32_t, FUNCIDX > buttonCallbacks;	//!< all callback for button
		map< uint32_t, int32_t > editProps;	//!< all edit property

		vector< uint32_t >* switchs; //!< switch ids on after menu selection
		map< uint32_t, unistring >* textResp; //!< edit field response

		uint32_t addString( unistring s );

		void removeCommand( string command );
		void removeCommand( char* s, ... ) PRINTF_LIKE(2,3);

		void setPropertyField( uint32_t type, uint32_t obj, int prop, int subProp, int subProp2, bool data );
		void setPropertyField( uint32_t type, uint32_t obj, int prop, int subProp, int subProp2, unistring data );
		
		bool getPropertyFieldBool( uint32_t type, uint32_t obj, int prop, int subProp, int subProp2 );
		unistring getPropertyField( uint32_t type, uint32_t obj, int prop, int subProp, int subProp2 );

		int32_t getIntFromProps( int prop, int prop2, int prop3 );
		void getPropsFromInt( int32_t returnCode, int& prop, int& prop2, int& prop3 );

	protected:
		vector< string >	commands; //!< all commands
		vector< unistring >	texts; //!< all strings
	
		virtual void /*cServerPacket*/* createPacket();

	public:
				
		uint32_t x; //!< x coord where show
		uint32_t y; //!< y location where show
		uint32_t pageCount;	//!< page count
		uint32_t pageCurrent;	//!< current page
		uint32_t buffer[MENU_BUFF_COUNT];
		string buffer_str[MENU_BUFF_COUNT];

		cMenu( MENU_TYPE id, uint32_t x, uint32_t y, bool canMove, bool canClose, bool canDispose );
		~cMenu();

		void setCloseable( bool canClose );
		bool getCloseable();
		void setDisposeable( bool canDispose );
		bool getDisposeable();
		void setMoveable( bool canMove );
		bool getMoveable();

		virtual void handleButton( pClient client,  void /*cClientPacket*/* pkg  );

		void addCommand( string command );
		void addCommand( char* s, ... ) PRINTF_LIKE(2,3);

		void addBackground( uint32_t gumpId, uint32_t width, uint32_t height );
		void addButton( uint32_t x, uint32_t y, uint32_t up, uint32_t down, int32_t returnCode, bool pressable );
		void addButtonFn( uint32_t x, uint32_t y, uint32_t up, uint32_t down, int32_t returnCode, bool pressable, FUNCIDX fn );
		void addCheckbox( uint32_t x, uint32_t y, uint32_t off, uint32_t on, uint32_t checked, int32_t result );
		void addCheckertrans( uint32_t x, uint32_t y, uint32_t width, uint32_t height );
		void addCroppedText( uint32_t x, uint32_t y, uint32_t width, uint32_t height, unistring text, uint32_t hue );
		void addGump( uint32_t x, uint32_t y, uint32_t gump, uint32_t hue );
		void addHtmlGump( uint32_t x, uint32_t y, uint32_t width, uint32_t height, unistring html, uint32_t hasBack, uint32_t canScroll );
		void addInputField( uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint16_t textId, unistring data, uint32_t hue = 0 );
		void addPropertyField( uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t property, uint32_t subProperty, uint32_t hue = 0, uint32_t subProperty2 = 0 );
		void addRadioButton( uint32_t x, uint32_t y, uint32_t off, uint32_t on, uint32_t checked, int32_t result  );
		void addResizeGump( uint32_t x, uint32_t y, uint32_t gumpId, uint32_t width, uint32_t height );
		void addText( uint32_t x, uint32_t y, unistring data, uint32_t hue = 0 );
		void addTilePic( uint32_t x, uint32_t y, uint32_t tile, uint32_t hue = 0 );
		void addTiledGump( uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t gump, uint32_t hue );
		void addXmfHtmlGump( uint32_t x, uint32_t y, uint32_t width, uint32_t height, unistring clilocid, uint32_t hasBack , uint32_t canScroll );
		//void 	addGroup( uint32_t group = 0 );
		void addPage( uint32_t page );
		void addPageButton( uint32_t x, uint32_t y, uint32_t up, uint32_t down, uint32_t page );

		bool getCheckBox( uint32_t checkbox, bool raw=false );
		bool getRadio( uint32_t radio, bool raw=false );
		unistring* getText( uint32_t text, bool raw=false );
		int32_t getButton( int32_t rawButton );
};


class cIconListMenu : public cBasicMenu
{

	protected:

		virtual void /*cServerPacket*/* createPacket();
//		vector< pkg_icon_list_menu_st > icons;
		map< uint32_t, int32_t > iconData;

	public:

		string question;

		cIconListMenu();
		~cIconListMenu();

		virtual void handleButton( pClient client,  void /*cClientPacket*/* pkg  );
		void addIcon( uint16_t model, uint16_t color, string response );
		void addIcon( uint16_t model, uint16_t color, int32_t data, string response );

};


bool isIconList( pClient client );
bool isIconList( uint8_t cmd );

/*!
\brief all Menus
*/
class cMenus
{
	
	friend class cBasicMenu;

	private:
		uint32_t current_serial; //!< current serial
		MenuMap menuMap; //!< every opened menus

		map< uint32_t, set<uint32_t> > whoSeeWhat; //!< player see menus

		bool removeFromView( pMenu menu, uint32_t chr );

	public:

		cMenus();
		~cMenus();

		pMenu insertMenu( pMenu menu );
		uint32_t removeMenu( uint32_t menu, pChar pc = NULL );
		bool handleMenu( pClient client );
		pMenu getMenu( uint32_t menu );
		bool showMenu( uint32_t menu, pChar pc );

};

extern cMenus Menus;

#endif
