  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/


#include "menu.h"
#include "amx/amxwraps.h"


#include "inlines.h"

cMenus Menus;

/*!
\brief Constructor
\author Endymion
*/
cMenus::cMenus()
{
	current_serial=0;
}


/*!
\brief Destructor
\author Endymion
*/
cMenus::~cMenus()
{
}


/*!
\brief Create a new menu
\author Endymion
\param menu the menu
\return the serial of new menu
*/
P_MENU cMenus::insertMenu( P_MENU menu )
{
	menu->serial=++current_serial;
	menuMap.insert( make_pair( menu->serial, menu ) );
	return menu;
}


P_MENU cMenus::getMenu( uint32_t menu )
{
	MENU_MAP::iterator iter( menuMap.find( menu ) );
	if( iter != menuMap.end() )
		return iter->second;
	return NULL;	
}

bool cMenus::showMenu( uint32_t menu, pChar pc )
{
	MENU_MAP::iterator iter( menuMap.find( menu ) );
	if( iter != menuMap.end() )
	{
		iter->second->show( pc );
		return true;
	}
	return false;	
}

bool isIconList( NXWSOCKET s )
{
	return isIconList( buffer[s][0] );
}

bool isIconList( uint8_t cmd )
{
	return cmd==PKG_RESPONSE_TO_DIALOG;
}

/*!
\brief 
\author Endymion
\return true if menu need delete
*/
bool cMenus::removeFromView( P_MENU menu, uint32_t chr )
{
	
	menu->whoSeeThis.erase( chr );

	std::map<uint32_t, std::set<SERIAL> >::iterator i( whoSeeWhat.find( menu->serial ) );
	if( i!=whoSeeWhat.end() ) {
		i->second.erase( chr );

		if( i->second.empty() )
			whoSeeWhat.erase( i );
	}

	return menu->whoSeeThis.empty();
}

uint32_t cMenus::removeMenu( SERIAL menu, pChar pc )
{

	MENU_MAP::iterator iter( menuMap.find( menu ) );
	if( iter != menuMap.end() )
	{		
		bool needDelete = removeFromView( iter->second, pc->getSerial() );

		if( needDelete ) {
			delete iter->second;
			menuMap.erase( iter );
			return INVALID;
		}
		else
			return menu;
	}
	return INVALID;
}

bool cMenus::handleMenu( NXWCLIENT ps )
{

	if( ps==NULL )
		return false;

	pChar pc=ps->currChar();
	if ( ! pc ) return false;
	
	cClientPacket* p =  NULL;

	uint32_t serial;
	if( isIconList( ps->toInt() ) ) {
		p = new cPacketResponseToDialog();
		p->receive( ps );
		serial = ((cPacketResponseToDialog*)p)->serial.get();
	}
	else {
		p = new cPacketMenuSelection();
		p->receive( ps );
		serial = ((cPacketMenuSelection*)p)->serial.get();
	}


	P_MENU menu = Menus.getMenu( serial );
	if ( ! menu ) return false;

	uint32_t menu_serial = menu->serial;

	menu->handleButton( ps, p );

	removeMenu( menu_serial, pc );		
	
	return true;

}







cBasicMenu::cBasicMenu( MENU_TYPE id )
{
	serial=INVALID;
	this->id=id;
	callback=NULL;
	hard=NULL;
}

cBasicMenu::~cBasicMenu()
{
	if( callback!=NULL )
		delete callback;
}


void cBasicMenu::setCallBack( std::string arg )
{
	if( callback!=NULL )
		safedelete( callback );

	callback = new AmxFunction( const_cast< char* >( arg.c_str() ) );
}

void cBasicMenu::setCallBack( int fn )
{
	if( callback!=NULL )
		safedelete( callback );

	callback = new AmxFunction( fn );
}

void cBasicMenu::handleButton( NXWCLIENT ps, cClientPacket* pkg  )
{
}

void cBasicMenu::show( pChar pc )
{
	if ( ! pc ) return;

	NXWCLIENT ps=pc->getClient();
	if( ps==NULL ) return;

	cServerPacket* packet = build();
	packet->send( ps );
	delete packet;

	Menus.whoSeeWhat[ pc->getSerial() ].insert( serial );
	whoSeeThis.insert( pc->getSerial() );
}

cServerPacket* cBasicMenu::createPacket()
{
	return NULL;
}

cServerPacket* cBasicMenu::build()
{
	return createPacket();
}









cMenu::cMenu( MENU_TYPE id, uint32_t x, uint32_t y, bool canMove, bool canClose, bool canDispose ) : cBasicMenu( id )
{
	this->x = x;
	this->y = y;

	moveable=closeable=disposeable=true;

	setMoveable( canMove );
	setCloseable( canClose );
	setDisposeable( canDispose );

	rc_serialCurrent = 1;
	pageCount=1;
	pageCurrent=0;

	for( int i=0; i<4; ++i ) {
		buffer[i] = INVALID;
	}

}

cMenu::~cMenu()
{
}

void cMenu::addCommand( std::string command )
{
	commands.push_back( command );
}

void cMenu::addCommand( char* formatStr, ... )
{
	char temp[TEMP_STR_SIZE];
	
	va_list vargs;
	va_start(vargs, formatStr );
	vsnprintf( temp, sizeof(temp)-1, formatStr, vargs);
	va_end(vargs);

	addCommand( std::string( temp ) );
}

void cMenu::removeCommand( std::string command )
{
	std::vector< std::string >::iterator iter( commands.begin() ), end( commands.end() );
	for( ; iter!=end; iter++ ) {
		if( (*iter)==command ) {
			commands.erase( iter );
			return;
		}
	}
}


uint32_t cMenu::addString( wstring u )
{
	texts.push_back( u );
	return texts.size()-1;
}

void cMenu::addButton( uint32_t x, uint32_t y, uint32_t up, uint32_t down, int32_t returnCode, bool pressable )
{
	rc_button.insert( make_pair( rc_serialCurrent, returnCode ) );
	addCommand( "{button %d %d %d %d %d %d %d}", x, y, up, down, pressable, pageCurrent, rc_serialCurrent++ );
}

void cMenu::addButtonFn( uint32_t x, uint32_t y, uint32_t up, uint32_t down, int32_t returnCode, bool pressable, FUNCIDX fn )
{
	addButton( x, y, up, down, returnCode, pressable );
	buttonCallbacks.insert( make_pair( rc_serialCurrent-1, fn ) );

}

void cMenu::addGump( uint32_t x, uint32_t y, uint32_t gump, uint32_t hue )
{
	addCommand( "{gumppic %d %d %d hue=%d}", x, y, gump, hue );
}

void cMenu::addTiledGump( uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t gump, uint32_t hue )
{
	addCommand( "{gumppictiled %d %d %d %d %d %d}", x, y, width, height, gump, hue );
}

void cMenu::addHtmlGump( uint32_t x, uint32_t y, uint32_t width, uint32_t height, wstring html, uint32_t hasBack, uint32_t canScroll )
{
	addCommand( "{htmlgump %d %d %d %d %d %d %d}", x, y, width, height, addString(html), hasBack, canScroll );
}

void cMenu::addXmfHtmlGump( uint32_t x, uint32_t y, uint32_t width, uint32_t height, wstring clilocid, uint32_t hasBack , uint32_t canScroll )
{
	addCommand( "{xmfhtmlgump %d %d %d %d %s %d %d}", x, y, width, height, addString(clilocid), hasBack, canScroll );
}

void cMenu::addCheckertrans( uint32_t x, uint32_t y, uint32_t width, uint32_t height )
{
	addCommand( "{checkertrans %d %d %d %d}", x, y, width, height );
}

void cMenu::addCroppedText( uint32_t x, uint32_t y, uint32_t width, uint32_t height, wstring text, uint32_t hue )
{
	addCommand( "{croppedtext %d %d %d %d %d %d}", x, y, width, height, hue, addString(text) );
}

void cMenu::addText( uint32_t x, uint32_t y, wstring data, uint32_t hue )
{
	addCommand( "{text %d %d %d %d}", x, y, hue, addString(data) ); //text <Spaces from Left> <Space from top> <Length, Color?> <# in order>
}

void cMenu::addBackground( uint32_t gump, uint32_t width, uint32_t height )
{
	addResizeGump( 0, 0, gump, width, height );
}

void cMenu::addResizeGump( uint32_t x, uint32_t y, uint32_t gump, uint32_t width, uint32_t height )
{
	addCommand( "{resizepic %d %d %d %d %d}", x, y, gump, width, height );
}

void cMenu::addTilePic( uint32_t x, uint32_t y, uint32_t tile, uint32_t hue )
{
	addCommand( "{tilepic %d %d %d %d}", x, y, tile, hue );
}

void cMenu::addInputField( uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint16_t textId, wstring data, uint32_t hue )
{
	rc_edit.insert( make_pair( textId, rc_serialCurrent ) );
	addCommand( "{textentry %d %d %d %d %d %d %d}", x, y, width, height, hue, rc_serialCurrent++, addString(data) );
}

void cMenu::addPropertyField( uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t property, uint32_t subProperty, uint32_t hue, uint32_t subProperty2 )
{
	
	VAR_TYPE t = getPropertyType( property );
	int32_t props = getIntFromProps( property, subProperty, subProperty2 );

	if( t==T_BOOL ) 
	{
		addCheckbox( x, y, 0x00D2, 0x00D3, getPropertyFieldBool( buffer[0], buffer[1], property, subProperty, subProperty2 ), props );
	}
	else
	{
		addInputField( x, y, width, height, props, getPropertyField( buffer[0], buffer[1], property, subProperty, subProperty2 ), hue );
	}
	editProps.insert( make_pair( props, rc_serialCurrent-1 ) );
	
}

void cMenu::addCheckbox( uint32_t x, uint32_t y, uint32_t off, uint32_t on, uint32_t checked, int32_t result )
{
	rc_checkbox.insert( make_pair( result, rc_serialCurrent ) );
	addCommand( "{checkbox %d %d %d %d %d %d}", x, y, off, on, checked, rc_serialCurrent++ );
}

void cMenu::addRadioButton( uint32_t x, uint32_t y, uint32_t off, uint32_t on, uint32_t checked, int32_t result  )
{
	rc_radio.insert( make_pair( result, rc_serialCurrent ) );
	addCommand( "{radio %d %d %d %d %d %d}", x, y, off, on, checked, rc_serialCurrent++ );
}


void cMenu::addPageButton( uint32_t x, uint32_t y, uint32_t up, uint32_t down, uint32_t page )
{
	addCommand( "{button %d %d %d %d 0 %d 0}", x, y, up, down, page );
}

/*!
\brief Add a new page
\author Endymion
\param page the page num, if INVALID is used automatic page count
*/
void cMenu::addPage( uint32_t page )
{
	pageCurrent=page;
	if( page>=pageCount )
		pageCount=page+1;

	if( page < 256 )
	{
		addCommand( "{page %d}", page );
	}
}

/*
void cMenu::addGroup( uint32_t group )
{
	addCommand( "{group %d}", group );
}
*/

int32_t cMenu::getButton( int32_t rawButton )
{
	return rc_button[ rawButton ];
}



void cMenu::handleButton( NXWCLIENT ps, cClientPacket* pkg  )
{
	
	cPacketMenuSelection* p = (cPacketMenuSelection*)pkg;

	pChar pc = ps->currChar();
	if ( ! pc ) return;

	uint32_t button = p->buttonId.get();

	this->switchs = &p->switchs;
	this->textResp = &p->text_entries;
	
	uint32_t buttonReturnCode;
	if( button!=MENU_CLOSE ) { 

		buttonReturnCode = getButton( button );

		std::map< uint32_t, FUNCIDX >::iterator iter( buttonCallbacks.find( button ) );
		if( iter!=buttonCallbacks.end() ) {

			AmxFunction func( iter->second );
			func.Call( serial, pc->getSerial(), buttonReturnCode );
			return;

		}
	}
	else {
		buttonReturnCode = 0;
	}

	//set property if there are

	if( ( buttonReturnCode!=MENU_CLOSE ) && ( buttonReturnCode==buffer[3] ) ) { 
		std::map< uint32_t, int32_t >::iterator propIter( editProps.begin() ), lastProp( editProps.end() );
		for( ; propIter!=lastProp; ++propIter ) {

			int32_t props = propIter->first;
			int prop, prop2, prop3;
			getPropsFromInt( props, prop, prop2, prop3 );  

			if( getPropertyType( prop )!=T_BOOL ) {
				std::wstring* data = getText( propIter->second, true );
				if( data!=NULL )
					setPropertyField( buffer[0], buffer[1], prop, prop2, prop3, *data );
			}
			else {
				setPropertyField( buffer[0], buffer[1], prop, prop2, prop3, getCheckBox( propIter->second, true ) );
			}
		}
	}
	
	if( callback!=NULL )
		callback->Call( serial, pc->getSerial(), buttonReturnCode );
	else
		hard( this, ps, buttonReturnCode );
}


bool cMenu::getPropertyFieldBool( uint32_t type, SERIAL obj, SERIAL prop, SERIAL subProp, SERIAL subProp2 )
{
	
	switch( type ) {
	
		case PROpCharACTER: {
			pChar pc = pointers::findCharBySerial( obj );
			if ( ! pc ) return false;

			return getCharBoolProperty( pc, prop, subProp );

		}
		case PROpItem : {
			pItem pi = pointers::findItemBySerial( obj );
			if ( ! pi ) return false;

			return getItemBoolProperty( pi, prop, subProp );

		}
		case PROP_CALENDAR:
		case PROP_GUILD:
		default:
			return false;
	}

}

void cMenu::setPropertyField( uint32_t type, SERIAL obj, SERIAL prop, SERIAL subProp, SERIAL subProp2, bool data )
{
	switch( type ) {
	
		case PROpCharACTER: {
			pChar pc = pointers::findCharBySerial( obj );
			if ( ! pc ) return;
			if( data!=getCharBoolProperty( pc, prop, subProp ) )
				setCharBoolProperty( pc, prop, subProp, subProp2, data );
			}
			break;
		case PROpItem : {
			pItem pi = pointers::findItemBySerial( obj );
			if ( ! pi ) return;
			if( data!=getItemBoolProperty( pi, prop, subProp ) )
				setItemBoolProperty( pi, prop, subProp, data );
			}
			break;
		case PROP_CALENDAR:
		case PROP_GUILD:
		default:
			return;
	}
		
}

void cMenu::setPropertyField( uint32_t type, SERIAL obj, SERIAL prop, SERIAL subProp, SERIAL subProp2, std::wstring data )
{
	VAR_TYPE t = getPropertyType( prop );
	switch( type ) {
	
		case PROpCharACTER: {
			pChar pc = pointers::findCharBySerial( obj );
			if ( ! pc ) return;

			switch( t ) {
				case T_CHAR : {
					char value = str2num( data );
					if( value!=getCharCharProperty( pc, prop, subProp ) )
						setCharCharProperty( pc, prop, subProp, subProp2, value );
					}
					break;
				case T_INT: {
					int value = str2num( data );
					if( value!=getCharIntProperty( pc, prop, subProp ) )
						setCharIntProperty( pc, prop, subProp, subProp2, value );
					}
					break;
				case T_SHORT: {
					short value = str2num( data );
					if( value!=getCharShortProperty( pc, prop, subProp ) )
						setCharShortProperty( pc, prop, subProp, subProp2, value );
					}
					break;
				case T_STRING: {
					std::string value;
					wstring2string( data, value );
					if( value!=std::string( getCharStrProperty( pc, prop, subProp ) ) )
						setCharStrProperty( pc, prop, subProp, subProp2, const_cast<char*>(value.c_str()) );
					}
					break;
				case T_UNICODE: {
					if( data!=getCharUniProperty( pc, prop, subProp ) )
						setCharUniProperty( pc, prop, subProp, subProp2, data );
					}
					break;
			}
		}
		break;
		case PROpItem : {
			pItem pi = pointers::findItemBySerial( obj );
			if ( ! pi ) return;

			switch( t ) {
				case T_CHAR : {
					char value = str2num( data );
					if( value!=getItemCharProperty( pi, prop, subProp ) )
						setItemCharProperty( pi, prop, subProp, value );
					}
					break;
				case T_INT: {
					int value = str2num( data );
					if( value!=getItemIntProperty( pi, prop, subProp ) )
						setItemIntProperty( pi, prop, subProp, value );
					}
					break;
				case T_SHORT: {
					short value = str2num( data );
					if( value!=getItemShortProperty( pi, prop, subProp ) )
						setItemShortProperty( pi, prop, subProp, value );
					}
					break;
				case T_STRING: {
					std::string value;
					wstring2string( data, value );
					if( value!=std::string( getItemStrProperty( pi, prop, subProp ) ) )
						setItemStrProperty( pi, prop, subProp, const_cast<char*>(value.c_str()) );
					}
					break;
				case T_UNICODE:
					if( data!=getItemUniProperty( pi, prop, subProp ) )
						setItemUniProperty( pi, prop, subProp, data );
					break;
			}
		}
		break;
		case PROP_CALENDAR:
		case PROP_GUILD:
		default:
			return;
	}


}

template< typename T >
std::wstring toWstr( T num )
{
	wchar_t buffer[TEMP_STR_SIZE];
#ifdef WIN32
	swprintf( buffer, L"%i", num );
#else
	swprintf( buffer, TEMP_STR_SIZE, L"%i", num );
#endif

	return std::wstring( buffer );
}

std::wstring toWstr( const char* s )
{
	std::wstring data;
	string2wstring( std::string(s), data );
	return data;
}


std::wstring cMenu::getPropertyField( uint32_t type, SERIAL obj, SERIAL prop, SERIAL subProp, SERIAL subProp2 )
{
	
	VAR_TYPE t = getPropertyType( prop );
	switch( type ) {
	
		case PROpCharACTER: {
			pChar pc = pointers::findCharBySerial( obj );
			if ( ! pc ) return std::wstring();

			switch( t ) {
				case T_CHAR :
					return toWstr( getCharCharProperty( pc, prop, subProp ) );
				case T_INT:
					return toWstr( getCharIntProperty( pc, prop, subProp ) );
				case T_SHORT:
					return toWstr( getCharShortProperty( pc, prop, subProp ) );
				case T_STRING:
					return toWstr( getCharStrProperty( pc, prop, subProp ) );
				case T_UNICODE: 
					return getCharUniProperty( pc, prop, subProp );
			}
		}
		break;
		case PROpItem : {
			pItem pi = pointers::findItemBySerial( obj );
			if ( ! pi ) return std::wstring();

			switch( t ) {
				case T_CHAR :
					return toWstr( getItemCharProperty( pi, prop, subProp ) );
				case T_INT:
					return toWstr( getItemIntProperty( pi, prop, subProp ) );
				case T_SHORT:
					return toWstr( getItemShortProperty( pi, prop, subProp ) );
				case T_STRING:
					return toWstr( getItemStrProperty( pi, prop, subProp ) );
				case T_UNICODE:
					return getItemUniProperty( pi, prop, subProp );
			}
		}
		break;
		case PROP_CALENDAR:
		case PROP_GUILD:
		default:
			return std::wstring(L"Not implemented yet");
	}


	return std::wstring( L"Error" );
}

void cMenu::setMoveable( bool canMove )
{
	if( !canMove )
		addCommand("{nomove}");
	else 
		if( !moveable )
			removeCommand( std::string("{nomove}") );
	moveable=canMove;
}

bool cMenu::getMoveable()
{
	return moveable;
}

void cMenu::setCloseable( bool canClose )
{
	if( !canClose )
		addCommand("{noclose}");
	else
		if( !closeable )
			removeCommand( std::string("{noclose}") );
	closeable=canClose;
}

bool cMenu::getCloseable()
{
	return closeable;
}

void cMenu::setDisposeable( bool canDispose )
{
	if( !canDispose )
		addCommand("{nodispose}");
	else
		if( !disposeable )
			removeCommand( std::string("{nodispose}") );
	disposeable=canDispose;
}

bool cMenu::getDisposeable()
{
	return disposeable;
}

bool cMenu::getCheckBox( uint32_t checkbox, bool raw )
{
	if( raw )
		return find( switchs->begin(), switchs->end(), checkbox )!=switchs->end();
	else
		return find( switchs->begin(), switchs->end(), rc_checkbox[checkbox] )!=switchs->end();
}

bool cMenu::getRadio( uint32_t radio, bool raw )
{
	if( raw )
		return find( switchs->begin(), switchs->end(), radio )!=switchs->end();
	else
		return find( switchs->begin(), switchs->end(), rc_radio[radio] )!=switchs->end();
}

std::wstring* cMenu::getText( uint32_t text, bool raw )
{
	std::map< uint32_t, std::wstring >::iterator iter;
	if( raw )
		iter= textResp->find( text );
	else {
		iter= textResp->find( rc_edit[text] );
	}
	return ( iter!=textResp->end() )? &iter->second : NULL;
}

int32_t cMenu::getIntFromProps( int prop, int prop2, int prop3 )
{
	return 0xF0000000 + ( (prop&0xFFF)<<16 ) + ( (prop2&0xFF)<<8 ) + ( prop3&0xFF );
}

void cMenu::getPropsFromInt( int32_t returnCode, int& prop, int& prop2, int& prop3 )
{
	prop =  (returnCode>>16)&0xFFF;
	prop2 = (returnCode>>8)&0xFF;
	prop3 = returnCode&0xFF;
}


cServerPacket* cMenu::createPacket()
{
	cPacketMenu* packet = new cPacketMenu; //!< menu packet with new packet system

	packet->serial =serial;
	packet->id=id;
	packet->x = x;
	packet->y = y;

	packet->commands = &commands;
	packet->texts = &texts;

	return packet;
}





cIconListMenu::cIconListMenu() : cBasicMenu( MENUTYPE_ICONLIST )
{
}

cIconListMenu::~cIconListMenu()
{
}

cServerPacket* cIconListMenu::createPacket() 
{
	cPacketIconListMenu* p = new cPacketIconListMenu;
	
	p->serial = serial;
	p->id = id;

	p->question = question;
	p->icons = &icons;

	return p;
}

void cIconListMenu::addIcon( uint16_t model, COLOR color, std::string response )
{
	pkg_icon_list_menu_st icon;
	icon.color=color;
	icon.model=model;
	icon.response=response;

	icons.push_back( icon );
}

void cIconListMenu::addIcon( uint16_t model, COLOR color, int32_t data, std::string response )
{
	iconData.insert( make_pair( icons.size(), data ) );
	addIcon( model, color, response );
}


void cIconListMenu::handleButton( NXWCLIENT ps,  cClientPacket* pkg  )
{

	pChar pc = ps->currChar();
	if ( !pc ) return;

	cPacketResponseToDialog* p = (cPacketResponseToDialog*)pkg;

	uint32_t index = p->index.get();
	
	callback->Call( serial, pc->getSerial(), index, icons[index-1].model.get(), icons[index-1].color.get(), iconData[index-1] );

}

