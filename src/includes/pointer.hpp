/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Header that defines pointers namespace
*/

#ifndef __POINTERS_H__
#define __POINTERS_H__

#include "common_libs.hpp"

/*!
\brief Pointer related stuff
*/
namespace pointers {

	//@{
	/*!
	\name Maps declarations
	\author Luxor
	*/
	extern map<uint32_t, CharList> pStableMap;
	extern map<uint32_t, pChar> pMounted;

	extern map<uint32_t, CharList> pOwnCharMap;
	extern map<uint32_t, ItemList> pOwnItemMap;

	extern map<uint32_t, ItemList> pContMap;

	extern map<uint32_t, CharList> pMultiCharMap;
	extern map<uint32_t, ItemList> pMultiItemMap;
	//@}

	void init();
	void delChar(pChar pc);
	void delItem(pItem pi);
	
	void updContMap(pItem pi);
	
	void addToStableMap(pChar pet);
	void delFromStableMap(pChar pet);

	void addToOwnerMap( pChar pet );
	void delFromOwnerMap( pChar pet );
	void addToOwnerMap( pItem pi );
	void delFromOwnerMap( pItem pi );

	void addToMultiMap( pChar pc );
	void delFromMultiMap( pChar pc );
	void addToMultiMap( pItem pi );
	void delFromMultiMap( pItem pi );
	
	pChar stableSearch(int serial, int *index);
	//
	// Sparhawk:	mapRegion replacement (work in progress)
	//
	enum
	{
		NONE		=   0,
		ONLINE		=   1,
		OFFLINE 	=   2,
		NPC		=   4,
		EXCLUDESELF	=   8,
		DEAD		=  16,
		PARTYMEMBER	=  32,
		COMBATTARGET	= 128
	};

	void 		addToLocationMap( const pObject pObject );
	void 		updateLocationMap( const pObject pObject );
	void 		delFromLocationMap( const pObject pObject );

	void		showCharLocationMap();
	void		addCharToLocationMap( const pChar who );
	void 		delCharFromLocationMap( const pChar who );
	CharList*	getNearbyChars( pObject pObject, uint32_t range, uint32_t flags = 0 );
	CharList*	getNearbyChars( uint32_t x, uint32_t y, uint32_t range, uint32_t flags = 0, pChar pSelf = 0 );

	void		showItemLocationMap();
	void		addItemToLocationMap( const pItem what );
	void		delItemFromLocationMap( const pItem what );
	ItemList*	getNearbyItems( cObject* pObject, uint32_t range, uint32_t flags = 0 );
	ItemList*	getNearbyItems( uint32_t x, uint32_t y, uint32_t range, uint32_t flags = 0, pItem pSelf = 0 );
}

#endif
