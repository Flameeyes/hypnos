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
\brief Set Related Stuff
*/

#ifndef __SET_H__
#define __SET_H__

#include "common_libs.h"
#include "guild.h"

class NxwWrapper {

private:
	uint32_vector vect;
	uint32_vector::iterator current;

protected:
	void next();

public:
	NxwWrapper();
	~NxwWrapper();

	bool isEmpty();
	void clear();
	uint32_t size();
	
	void rewind();
	NxwWrapper& operator++(int);


	uint32_t get();
	void insert( uint32_t s );
	
	void copyQ( const NxwWrapper& from );
};

class NxwSerialWrapper : public NxwWrapper {

protected: 
	void fillSerialInContainer( uint32_t serial, bool bIncludeSubContained = true, bool bIncludeOnlyFirstSubcont=false );
	void fillSerialInContainer( cObject* obj, bool bIncludeSubContained = true, bool bIncludeOnlyFirstSubcont=false );

public:
	NxwSerialWrapper();
	~NxwSerialWrapper();

	uint32_t getSerial();
	void insertSerial( uint32_t s );
	void insertSerial( cObject* obj );



};

class NxwCharWrapper : public NxwSerialWrapper {

public:
	NxwCharWrapper();
	NxwCharWrapper( const NxwCharWrapper& that );
	~NxwCharWrapper();

	pChar getChar();
	void insertChar( pChar pc );

	void fillOwnedNpcs( pChar pc, bool bIncludeStabled = true, bool bOnlyFollowing = false );
	void fillCharsAtXY( Location location, bool bExcludeOfflinePlayers = true, bool bOnlyPlayer = false );
	void fillCharsAtXY( uint16_t x, uint16_t y, bool bExcludeOfflinePlayers = true, bool bOnlyPlayer = false );
	void fillCharsNearXYZ ( uint16_t x, uint16_t y, int nDistance = VISRANGE, bool bExcludeOfflinePlayers = true, bool bOnlyPlayer = false );
	void fillCharsNearXYZ ( Location location, int nDistance = VISRANGE, bool bExcludeOfflinePlayers = true, bool bOnlyPlayer = false );
	void fillPartyFriend( pChar pc, uint32_t nDistance = VISRANGE, bool bExcludeThis = false );
	void fillNpcsNearXY( uint16_t x, uint16_t y, int nDistance = VISRANGE );
	void fillNpcsNear( pChar pc, int nDistance = VISRANGE );
	void fillNpcsNear( pItem pi, int nDistance = VISRANGE );
	void fillGuildMembers( uint32_t guild );
	void fillGuildRecruits( uint32_t guild );

};

class NxwItemWrapper : public NxwSerialWrapper {

public:
	NxwItemWrapper();
	NxwItemWrapper( const NxwItemWrapper& that );
	~NxwItemWrapper();

	pItem getItem();
	void insertItem( pItem pi );

	void fillItemsInContainer ( pItem pi, bool bIncludeSubContained = true, bool bIncludeOnlyFirstSubcont=false);
	void fillItemWeared( pChar pc, bool bIncludeLikeHair = false, bool bIncludeProtectedLayer = false, bool bExcludeIllegalLayer=true );
	void fillItemsAtXY( uint16_t x, uint16_t y, int32_t type = INVALID, int32_t id = INVALID );
	void fillItemsAtXY( Location location, int32_t type = INVALID, int32_t id = INVALID );
	void fillItemsNearXYZ ( uint16_t x, uint16_t y, int nDistance = VISRANGE, bool bExcludeNotMovableItems = true);
	void fillItemsNearXYZ ( Location location, int nDistance = VISRANGE, bool bExcludeNotMovableItems = true);
	void fillGuilds( uint32_t guild );

};

class NxwSocketWrapper : public NxwWrapper {

public:
	NxwSocketWrapper();
	~NxwSocketWrapper();

	uint32_t getSocket();
	NXWCLIENT getClient();
	void insertSocket( NXWSOCKET s );
	void insertClient( NXWCLIENT ps );

	void fillOnline( pChar onlyNearThis, bool bExcludeThis = true, uint32_t nDistance = VISRANGE );
	void fillOnline( Location location, int nDistance = VISRANGE );
	void fillOnline( pItem onlyNearThis,int nDistance = VISRANGE );
	void fillOnline( );

};


const int RS_BACKPACK_ITEM = 1;
const int RS_BANK_ITEM = 2;
const int RS_EQUIpItem = 3;


namespace amxSet {

	typedef std::map< uint32_t, NxwWrapper* > AMX_WRAPPER_DB;

	
	uint32_t create(  );
	void deleteSet( uint32_t iSet );
	bool end( uint32_t iSet );
	void rewind( uint32_t iSet );
	void next( uint32_t iSet );
	uint32_t size( uint32_t iSet);

	uint32_t get( SERIAL iSet );
	void add( uint32_t iSet, SERIAL nVal );
	void copy( uint32_t iSet, const NxwWrapper& from );

	void addOwnedNpcs( uint32_t iSet, pChar pc, bool includeStabled, bool onlyFollowing );
 	void addPartyFriend( uint32_t iSet, pChar pc, int distance, bool excludeThis );
 	void addNpcsNearXY( uint32_t iSet, uint16_t x, uint16_t y, int distance );
	void addNpcsNearObj( uint32_t iSet, pChar pc, int nDistance );
	void addNpcsNearObj( uint32_t iSet, pItem pi, int nDistance );
 	void addItemsInContainer( uint32_t iSet, pItem pi, bool includeSubCont, bool includeOnlyFirstSubCont );
 	void addItemWeared( uint32_t iSet, pChar pc, bool includeLikeHair, bool includeProtectedLayer, bool excludeIllegalLayer );
 	void addItemsAtXY( uint32_t iSet, uint16_t x, uint16_t y, uint32_t type );
 	void addItemsNearXY( uint32_t iSet, uint16_t x, uint16_t y, int distance, bool excludeNotMovable );
 	void addAllOnlinePlayers( uint32_t iSet );
	void addOnlinePlayersNearChar( uint32_t iSet, pChar pc, bool excludeThis, int distance );
	void addOnlinePlayersNearItem( uint32_t iSet, pItem pi, int distance );
	void addOnlinePlayersNearXY( uint32_t iSet, uint16_t x, uint16_t y, int distance );

	void addGuildMembers( uint32_t iSet, SERIAL guild );
	void addGuildRecruits( uint32_t iSet, SERIAL guild );
	void addGuilds( uint32_t iSet, SERIAL guild );

}

#endif //__SET_H__
