/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "common_libs.h"
#include "inlines.h"

namespace pointers {

	std::map<uint32_t, CharList> pStableMap;
	std::map<uint32_t, pChar> pMounted;

	std::map<uint32_t, CharList> pOwnCharMap;
	std::map<uint32_t, ItemList> pOwnItemMap;

	std::map<uint32_t, ItemList> pContMap;

	std::map<uint32_t, CharList> pMultiCharMap;
	std::map<uint32_t, ItemList> pMultiItemMap;

#ifdef SPAR_LOCATION_MAP
	//
	// Still one flaw to fix.
	// When position of object is changed (e.g. through small) deleting objects will not work because
	// they will not be found in the map
	//
	// Solution:
	//	1.	Add coded xy coordinate to cObject and always use that one
	//	2.	if object not found, walk through the map until it is found then delete it
	//
	typedef std::multimap< uint32_t, pChar >	PCHARLOCATIONMAP;
	typedef PCHARLOCATIONMAP::iterator	PCHARLOCATIONMAPIT;

	struct XY
	{
		uint32_t	x;
		uint32_t	y;
	};

	static XY 		upperLeft	;
	static XY 		lowerRight	;
	PCHARLOCATIONMAP	pCharLocationMap;

	static uint32_t locationTokey( const sLocation& l );
	static uint32_t locationToKey( const uint32_t x, const uint32_t y );
	static void calculateBoundary( const uint32_t x, const uint32_t y, const uint32_t range );

	static uint32_t locationToKey( const sLocation& l )
	{
		return locationToKey( l.x, l.y );
	}

	static uint32_t locationToKey( const uint32_t x, const uint32_t y )
	{
		return (x << 16) + y;
	}

	static void calculateBoundary( const uint32_t x, const uint32_t y, const uint32_t range )
	{
		if( x <= range )
			upperLeft.x = 1;
		else
			upperLeft.x = x - range;

		if( x + range > 6144 )
			lowerRight.x = 6144;
		else
			lowerRight.x = x + range;

		if( y <= range )
			upperLeft.y = 1;
		else
			upperLeft.y = y - range;

		if( y + range > 4096 )
			lowerRight.y = 4096;
		else
			lowerRight.y = y + range;
	}

	void addToLocationMap( const pObject pObject )
	{
		if( pObject != 0 )
		{
			if( cSerializable::isItemSerial( pObject->getSerial() ) )
			{
				if( static_cast<pItem>(pObject)->isInWorld() )
					addItemToLocationMap( static_cast<pItem>(pObject) );
			}
			else
			{
				addCharToLocationMap( static_cast<pChar>(pObject) );
			}
		}
	}

	void updateLocationMap( const pObject pObject )
	{
		if( pObject != 0 )
		{
			if( cSerializable::isItemSerial( pObject->getSerial() ) )
			{
				if( static_cast<pItem>(pObject)->isInWorld() )
				{
					delItemFromLocationMap( static_cast<pItem>(pObject) );
					addItemToLocationMap( static_cast<pItem>(pObject) );
				}
			}
			else
			{
				delCharFromLocationMap( static_cast<pChar>(pObject) );
				addCharToLocationMap( static_cast<pChar>(pObject) );
			}
		}
	}

	void delFromLocationMap( const pObject pObject )
	{
		if( pObject != 0 )
		{
			if( cSerializable::isItemSerial( pObject->getSerial() ) )
			{
				if( static_cast<pItem>(pObject)->isInWorld() )
				{
					delItemFromLocationMap( static_cast<pItem>(pObject) );
				}
			}
			else
			{
				delCharFromLocationMap( static_cast<pChar>(pObject) );
			}
		}
	}

	void addCharToLocationMap( const pChar pWho )
	{
		pWho->setPositionKey();
		pCharLocationMap.insert( pair< uint32_t, pChar >( pWho->getPositionKey(), pWho ) );
	}

	void delCharFromLocationMap( const pChar pWho )
	{
		pair< PCHARLOCATIONMAPIT, PCHARLOCATIONMAPIT > it = pCharLocationMap.equal_range( pWho->getPositionKey() );
		uint32_t pWhoSerial = pWo->getSerial();

		for( ; it.first != it.second; ++it.first )
			if( it.first->second->getSerial() == pWhoSerial32 )
			{
				pCharLocationMap.erase( it.first );
				break;
			}
	}

	void showCharLocationMap()
	{
		PCHARLOCATIONMAPIT it( pCharLocationMap.begin() ), end( pCharLocationMap.end() );

		ConOut( "--------------------------------\n" );
		ConOut( "|      CHAR LOCATION MAP       |\n" );
		ConOut( "--------------------------------\n" );
		ConOut( "|   Key   | X  | Y  |  uint32_t  |\n" );
		ConOut( "--------------------------------\n" );

		uint32_t 	invalidCount	=  0;
		int32_t 	x	  	=  0;
		int32_t 	y		=  0;
		uint32_t	serial		= INVALID;
		for( ; it != end; ++it )
		{
			x = it->first >> 16;
			y = it->first & 0x0000FFFF;
			if( it->second )
				serial = it->second->getSerial();
			else
			{
				++invalidCount;
				serial = INVALID;
			}
			ConOut( "|%10i|%4i|%4i|%10i|\n", it->first, x, y, serial );
		}
		ConOut( "--------------------------------\n" );
		ConOut( "| entries in map : %10i  |\n", pCharLocationMap.size());
		ConOut( "| invalid entries: %10i  |\n", invalidCount );
		ConOut( "--------------------------------\n" );
	}

	CharList* getNearbyChars( pObject pObject, int32_t range, uint32_t flags )
	{
		CharList* 	pvCharsInRange	= 0;
		bool		validCall	= false;
		pChar		pSelf		= 0;

		if( pObject != 0 )
		{
			if( cSerializable::isItemSerial( pObject->getSerial() ) )
			{
				if( static_cast<pItem>(pObject)->isInWorld() )
					validCall = true;
			}
			else
			{
				pSelf = static_cast<pChar>(pObject);
				validCall = true;
			}
			if( validCall )
				pvCharsInRange = getNearbyChars( pObject->getPosition().x, pObject->getPosition().y, range, flags, pSelf );
		}
		return pvCharsInRange;
	}

	CharList* getNearbyChars( uint32_t x, uint32_t y, uint32_t range, uint32_t flags, pChar pSelf )
	{
		CharList* pvCharsInRange = 0;

		if( x > 0 && x < 6145 && y > 0 && y < 4097 )
		{
			pvCharsInRange = new CharList();

			calculateBoundary( x, y, range );

			PCHARLOCATIONMAPIT it(  pCharLocationMap.lower_bound( locationToKey( upperLeft.x,  upperLeft.y ) ) ),
					   end( pCharLocationMap.upper_bound( locationToKey( lowerRight.x, lowerRight.y) ) );
			pChar pc = 0;

			for( ; it != end; ++it )
			{
				pc = it->second;
				if( flags )
				{
					if( pSelf )
					{
						if( (flags & EXCLUDESELF) && pSelf->getSerial() == pc->getSerial() )
						{
							continue;
						}

						if ( (flags & COMBATTARGET) && pSelf->getSerial() == pc->targserial )
						{
							pvCharsInRange->push_back( pc );
							continue;
						}
					}

					if ( pc->npc )
					{
						if ( (flags & NPC) )
						{
							pvCharsInRange->push_back( pc );
							continue;
						}
						continue;
					}

					if ( (flags & ONLINE) && pc->IsOnline() )
					{
						pvCharsInRange->push_back( pc );
						continue;
					}

					if ( (flags & OFFLINE) && !pc->IsOnline() )
					{
						pvCharsInRange->push_back( pc );
						continue;
					}

					if ( (flags & DEAD) && pc->dead )
					{
						pvCharsInRange->push_back( pc );
						continue;
					}
				}
				else
				{
					pvCharsInRange->push_back( pc );
				}
			}
		}
		return pvCharsInRange;
	}

	typedef std::multimap< uint32_t, pItem >	PITEMLOCATIONMAP;
	typedef PITEMLOCATIONMAP::iterator	PITEMLOCATIONMAPIT;

	PITEMLOCATIONMAP	pItemLocationMap;


	void addItemToLocationMap( const pItem pWhat )
	{
		pWhat->setPositionKey();
		pItemLocationMap.insert( pair< uint32_t, pItem >( pWhat->getPositionKey(), pWhat ) );
	}

	void delItemFromLocationMap( const pItem pWhat )
	{
		pair< PITEMLOCATIONMAPIT, PITEMLOCATIONMAPIT > it = pItemLocationMap.equal_range( pWhat->getPositionKey() );
		uint32_t	pWhatSerial = pWhat->getSerial();

		for( ; it.first != it.second; ++it.first )
			if( it.first->second->getSerial() == pWhatSerial32 )
			{
				pItemLocationMap.erase( it.first );
				break;
			}
	}


	ItemList* getNearbyItems( cObject* pObject, uint32_t range, uint32_t flags )
	{
		ItemList* 	pvItemsInRange	= 0;
		bool		validCall	= false;
		pItem		pSelf		= 0;

		if( pObject != 0 )
		{
			if( cSerializable::isItemSerial( pObject->getSerial() ) )
			{
				pSelf = static_cast<pItem>(pObject);
				if( pSelf->isInWorld() )
				{
					validCall = true;
				}
			}
			else
			{
				validCall = true;
			}
			if( validCall )
				pvItemsInRange = getNearbyItems( pObject->getPosition().x, pObject->getPosition().y, range, flags, pSelf );
		}
		return pvItemsInRange;
	}

	ItemList* getNearbyItems( uint32_t x, uint32_t y, uint32_t range, uint32_t flags, pItem pSelf )
	{
		ItemList* pvItemsInRange = 0;

		if( x > 0 && x < 6145 && y > 0 && y < 4097 )
		{
			pvItemsInRange = new ItemList();

			calculateBoundary( x, y, range );

			PITEMLOCATIONMAPIT it(  pItemLocationMap.lower_bound( locationToKey( upperLeft.x,  upperLeft.y ) ) ),
					   end( pItemLocationMap.upper_bound( locationToKey( lowerRight.x, lowerRight.y) ));

			pItem pi = 0;

			for( ; it != end; ++it )
			{
				pi = it->second;

				if( flags )
				{
					if( pSelf )
					{
						if( (flags & EXCLUDESELF) && pSelf->getSerial() == pi->getSerial() )
						{
							continue;
						}
					}
					pvItemsInRange->push_back( pi );
				}
				else
				{
					pvItemsInRange->push_back( pi );
				}
			}
		}
		return pvItemsInRange;
	}

	void showItemLocationMap()
	{
		PITEMLOCATIONMAPIT it( pItemLocationMap.begin() ), end( pItemLocationMap.end() );

		ConOut( "--------------------------------\n" );
		ConOut( "|      ITEM LOCATION MAP       |\n" );
		ConOut( "--------------------------------\n" );
		ConOut( "|   Key   | X  | Y  |  uint32_t  |\n" );
		ConOut( "--------------------------------\n" );

		uint32_t 	invalidCount	=  0;
		int32_t 	x	  	=  0;
		int32_t 	y		=  0;
		uint32_t	serial		= INVALID;
		for( ; it != end; ++it )
		{
			x = it->first >> 16;
			y = it->first & 0x0000FFFF;
			if( it->second )
				serial = it->second->getSerial();
			else
			{
				++invalidCount;
				serial = INVALID;
			}
			ConOut( "|%10i|%4i|%4i|%10i|\n", it->first, x, y, serial );
		}
		ConOut( "--------------------------------\n" );
		ConOut( "| entries in map : %10i  |\n", pItemLocationMap.size());
		ConOut( "| invalid entries: %10i  |\n", invalidCount );
		ConOut( "--------------------------------\n" );
	}
#endif
	/*!
	\brief initializes pointer maps
	\author Luxor
	*/
	void init()
	{


		pStableMap.clear();
		pContMap.clear();
		pMounted.clear();
		pOwnCharMap.clear();
		pOwnItemMap.clear();
		//Chars and Stablers
		pChar pc = 0;

		cAllObjectsIter objs;

		for( objs.rewind(); !objs.IsEmpty(); objs++ )
		{
			if( cSerializable::isCharSerial( objs.getSerial() ) ) {
				pc=(pChar)objs.getObject();
				{
					if( pc->isStabled() ) {
						pStableMap[pc->getStablemaster()].push_back(pc);
					}
					if( pc->mounted )
						pMounted[pc->getOwnerSerial32()]=pc;

					pChar own=cSerializable::findCharBySerial(pc->getOwnerSerial32());
					pc->setOwnerSerial32( own->getSerial(), true );
				}
			}
			else {

				pItem pi = (pItem)objs.getObject();

				updContMap(pi);

				pChar own=cSerializable::findCharBySerial(pi->getOwnerSerial32());
				pi->setOwnerSerial32( own->getSerial(), true );
			}
		}

		std::map< uint32_t, pChar >::iterator iter( pMounted.begin() ), end( pMounted.end() );
		for( ; iter!=end; iter++)
		{
			pc = cSerializable::findCharBySerial(iter->first);
			if(pc)
				pc->setOnHorse();
		}
	}

	/*!
	\brief updates containers map
	\author Luxor
	\param pi the item which the function will update in the containers map
	*/
	void updContMap(pItem pi)
	{
		if ( ! pi ) return;
		vector<pItem>::iterator contIter;
		int32_t ser;

		ser= pi->getContSerial(true);
		if( ser > INVALID ) 
		{
			contIter = find(pContMap[ser].begin(), pContMap[ser].end(), pi);

			if ( !pContMap[ser].empty() && (contIter!=pContMap[ser].end()) )
				pContMap[ser].erase(contIter);
		}

		ser= pi->getContSerial();
		if( ser > INVALID) 
		{
			contIter = find(pContMap[ser].begin(), pContMap[ser].end(), pi);

			if (!pContMap[ser].empty() && contIter != pContMap[ser].end())
				pContMap[ser].erase(contIter);

			pContMap[ser].push_back(pi);
		}
	}

	/*!
	\author Luxor
	*/
	void addToStableMap(pChar pet)
	{
		if ( ! pet ) return;
		
		uint32_t stablemaster=pet->getStablemaster();
		if( stablemaster !=INVALID ) {

			delFromStableMap( pet );

			pStableMap[stablemaster].push_back(pet);
		}
	}

	/*!
	\author Luxor
	*/
	void delFromStableMap(pChar pet)
	{
		if ( ! pet ) return;
		uint32_t stablemaster=pet->getStablemaster();
		if( stablemaster != INVALID ) {

			std::map<uint32_t, vector <pChar> >::iterator vect( pStableMap.find( stablemaster ) );
			if( ( vect!=pStableMap.end() ) && !vect->second.empty() ) {

				vector<pChar>::iterator stableIter;
				stableIter = find( vect->second.begin(), vect->second.end(), pet);
				if( stableIter != vect->second.end())
					vect->second.erase(stableIter);
			}
		}
	}

	/*!
	\author Endymion
	*/
	void addToOwnerMap(pChar pet)
	{
		if ( ! pet ) return;
		
		uint32_t own=pet->getOwnerSerial32();
		if ( own!=INVALID ) {

			delFromOwnerMap( pet );

			pOwnCharMap[own].push_back(pet);
		}
	}

	/*!
	\author Endymion
	*/
	void delFromOwnerMap(pChar pet)
	{
		if ( ! pet ) return;
		
		uint32_t own=pet->getOwnerSerial32();
		if ( own!=INVALID ) {

			std::map<uint32_t, vector <pChar> >::iterator vect( pOwnCharMap.find( own ) );
			if( ( vect!=pOwnCharMap.end() ) && !vect->second.empty() )
			{

				vector<pChar>::iterator iter;
				iter = find(vect->second.begin(), vect->second.end(), pet);
				if( iter != vect->second.end() )
					vect->second.erase(iter);
			}

		}
	}


	/*!
	\author Endymion
	*/
	void addToOwnerMap(pItem pi)
	{
		if ( ! pi ) return;
		uint32_t own=pi->getOwnerSerial32();
		if ( own!=INVALID ) {

			delFromOwnerMap( pi );

			pOwnItemMap[own].push_back(pi);
		}
	}

	/*!
	\author Endymion
	*/
	void delFromOwnerMap(pItem pi)
	{
		if ( ! pi ) return;
		uint32_t own=pi->getOwnerSerial32();
		if ( own!=INVALID ) {

			std::map<uint32_t, vector <pItem> >::iterator vect( pOwnItemMap.find( own ) );
			if( ( vect!=pOwnItemMap.end() ) && !vect->second.empty() )
			{

				vector<pItem>::iterator iter;
				iter = find(vect->second.begin(), vect->second.end(), pi);
				if( iter != vect->second.end() )
					vect->second.erase(iter);
			}

		}
	}

	/*!
	\author Endymion
	*/
	void addToMultiMap( pChar pc )
	{
		if ( ! pc ) return;
		uint32_t multi=pc->getMultiSerial32();
		if ( multi!=INVALID ) {

			delFromMultiMap( pc );

			pMultiCharMap[multi].push_back(pc);
		}

	}

	/*!
	\author Endymion
	*/
	void delFromMultiMap( pChar pc )
	{
		if ( ! pc ) return;
		uint32_t multi=pc->getMultiSerial32();
		if ( multi!=INVALID ) {

			std::map<uint32_t, vector <pChar> >::iterator vect( pMultiCharMap.find( multi ) );
			if( ( vect!=pMultiCharMap.end() ) && !vect->second.empty() )
			{

				vector<pChar>::iterator iter;
				iter = find(vect->second.begin(), vect->second.end(), pc);
				if( iter != vect->second.end() )
					vect->second.erase(iter);
			}

		}

	}

	/*!
	\author Endymion
	*/
	void addToMultiMap( pItem pi )
	{
		if ( ! pi ) return;
		uint32_t multi=pi->getMultiSerial32();
		if ( multi!=INVALID ) {

			delFromMultiMap( pi );

			pMultiItemMap[multi].push_back(pi);
		}

	}

	/*!
	\author Endymion
	*/
	void delFromMultiMap( pItem pi )
	{
		if ( ! pi ) return;
		uint32_t multi=pi->getMultiSerial32();
		if ( multi!=INVALID ) {

			std::map<uint32_t, vector <pItem> >::iterator vect( pMultiItemMap.find( multi ) );
			if( ( vect!=pMultiItemMap.end() ) && !vect->second.empty() )
			{

				vector<pItem>::iterator iter;
				iter = find(vect->second.begin(), vect->second.end(), pi);
				if( iter != vect->second.end() )
					vect->second.erase(iter);
			}

		}

	}

	void eraseContainerInfo( uint32_t ser )
	{

		std::map<uint32_t, vector <pItem> >::iterator vect( pContMap.find( ser ) );
		if( vect!=pContMap.end() ) {

			if( !vect->second.empty() ) {

				vector<pItem>::iterator iter( vect->second.begin() );
				for( ; iter!=vect->second.end(); iter++ ) {
					(*iter)->setContainer(0);
					(*iter)->setOldContainer(0);
				}
			}

			pContMap.erase( vect );
		}
	}


	/*!
	\brief removes a char from chars pointer map and cleares its vector in pContMap
	\param pc the character
	\author Luxor
	*/
	void delChar(pChar pc)
	{
		if ( ! pc ) return;

		delFromStableMap(pc);
		delFromOwnerMap(pc);
#ifdef SPAR_C_LOCATION_MAP
		delFromLocationMap(pc);
#else
		mapRegions->remove(pc);
#endif
		objects.eraseObject( pc );

		eraseContainerInfo( pc->getSerial() );


	}

	/*!
	\brief removes an item from items pointer and from the vector of his container
	\author Luxor
	\param pi the item
	*/
	void delItem(pItem pi)
	{
		if ( ! pi ) return;

		if (pi->isInWorld())
		{
#ifdef SPAR_I_LOCATION_MAP
			pointers::delFromLocationMap(pi);
#else
			mapRegions->remove(pi);
#endif
		}

		objects.eraseObject(pi);

		eraseContainerInfo( pi->getSerial() );

		uint32_t cont=pi->getContSerial();
		if ( cont > INVALID ) {

			std::map<uint32_t, vector <pItem> >::iterator vect( pContMap.find( cont ) );
			if( ( vect!=pContMap.end() ) && !vect->second.empty() ) {

				vector<pItem>::iterator contIter;
				contIter = find( vect->second.begin(), vect->second.end(), pi);
				if( contIter != vect->second.end() )
					vect->second.erase(contIter);
			}

		}
	}

	pChar stableSearch(int serial, int *index)
	{
		if (serial < 0 || (*index) < 0)
			return 0;
		if (pStableMap[serial].empty()) return 0;
		if ((uint32_t)*index >= pStableMap[serial].size()) return 0;
		pChar pet = 0;
		pet = pStableMap[serial][*index];
		(*index)++;
		return pet;
	}

} //namespace
