/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Extraction skills related stuff
\author Endymion
*/

#ifndef __EXTRACTION_SKILLS_H
#define __EXTRACTION_SKILLS_H

#include "sndpkg.h"

class cResource {
public:

	cResource( ) { this->consumed=0; timer=getclock(); }

	uint32_t timer; //!< timer for respawn
	uint32_t consumed; //!< amount of resource consumed

};

typedef cResource* pResource;

typedef std::map< uint64_t, cResource > RESOURCE_MAP;

class cResources {


private:

	RESOURCE_MAP resources; //!< all resources

	uint64_t getBlocks( Location location  );
	pResource createBlock( Location location );
	void deleteBlock( Location Location );
	bool checkRes( pResource res );

public:


	uint32_t		n; //!< number of resource max
	uint32_t	time; //!< 
	uint32_t	rate; //!< respawn rate
	int32_t		stamina; //!< stamina used
	uint32_t		area_width; //!< resource area width
	uint32_t		area_height; //!< resource area height

	cResources( uint32_t areawidth = 10, uint32_t areaheight = 10 );
	~cResources();

	void setDimArea( uint32_t areawidth, uint32_t areaheight );
	
	pResource getResource( Location location );
	void checkResource( Location location, pResource& res );
	void decreaseResource( Location location, pResource res = NULL );
	bool thereAreSomething( pResource res );

	void checkAll();

};


extern cResources ores;


#endif

