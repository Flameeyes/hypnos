/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#ifndef __REGIONS_H
#define __REGIONS_H

static const uint16_t MAX_MAP_CELLS = 0xFFFF;	//!< Maximum count of cells

static const uint8_t REGION_GRIDSIZE = 32;	//!< Horizontal size of the grid
static const uint8_t REGION_COLSIZE = 32;	//!< Vertical size of the grid

static const uint16_t MAP_WIDTH = 6144;		//!< Width of the UO Map
static const uint16_t MAP_HEIGHT = 4096;	//!< Height of the UO Map

static const uint16_t REGION_X_CELLS = MAP_WIDTH/REGION_GRIDSIZE;
						//!< Count of X Cells
static const uint16_t REGION_Y_CELLS = MAP_HEIGHT/REGION_COLSIZE;
						//!< Count of Y Cells

struct region_st
{
	bool inUse;
 	char name[50];
 	int32_t midilist;
 	uint8_t priv;	// 0x01 guarded, 0x02, mark allowed, 0x04, gate allowed, 0x08, recall
			// 0x10 raining, 0x20, snowing, 0x40 magic damage reduced to 0
	char guardowner[50];
	uint8_t snowchance;
	uint8_t rainchance;
	uint8_t drychance;
	uint8_t keepchance;
	uint8_t wtype;
	int32_t guardnum[10];
	int32_t goodsell[256]; // Magius(CHE)
	int32_t goodbuy[256]; // Magius(CHE)
	int32_t goodrnd1[256]; // Magius(CHE) (2)
	int32_t goodrnd2[256]; // Magius(CHE) (2)
	int32_t forcedseason;
	bool ignoreseason;
};

extern region_st region[256];

typedef struct {
	uint32_set charsInRegions;
	uint32_set itemsInRegions;
} region_db_st;

typedef std::map< uint32_t, std::pair<uint16_t, uint16_t> > WHERE_IS_NOW_MAP;

class cRegion
{
private:

	WHERE_IS_NOW_MAP item_where_is_now;
	WHERE_IS_NOW_MAP char_where_is_now;
	void removeNow( pChar pc );
	void removeNow( pItem pi );
	void addNow( pChar pc );
	void addNow( pItem pi );

	region_db_st regions[REGION_X_CELLS][REGION_Y_CELLS];

	bool isValidCoord( uint16_t x, uint16_t y );

public:

	friend class NxwItemWrapper;
	friend class NxwCharWrapper;

	cRegion(); //constructor
	~cRegion(); //destructor
	
	void add( pChar pc );
	void add( pItem pi );
	void remove( pChar pc );
	void remove( pItem pi );


};


#endif

