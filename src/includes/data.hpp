/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#ifndef __DATA_H__
#define __DATA_H__

#include "common_libs.hpp"
#include "libhypnos/hypstl/vector.hpp"
#include "libhypnos/hypstl/map.hpp"

/*!
\file
\author Luxor
\brief Header of data files handling system
*/

extern uint16_t map_width;
extern uint16_t map_height;
extern bool statics_cache;
extern bool map_cache;

/*!
\author Luxor
*/
enum {
	MAP_HEADER_SIZE = 4, MAP_BLOCK_SIZE = 196,
	TILE_HEADER_SIZE = 4
};

/*!
\author Luxor
*/
enum MulFileId {
	Map_File = 0, StaIdx_File, Statics_File
};


struct map_st {
	uint16_t id;
	int8_t z;
} PACK_NEEDED;

struct staticIdx_st {
	int32_t start;
	int32_t length;
	int32_t unknown;
} PACK_NEEDED;

struct static_st {
	uint16_t id;
	uint8_t xoff;
	uint8_t yoff;
	int8_t z;
	uint16_t unknown;
} PACK_NEEDED;

typedef vector< static_st > staticVector;

const uint8_t static_st_size = sizeof( static_st );
const uint8_t staticIdx_st_size = sizeof( staticIdx_st );
const uint8_t map_st_size = sizeof( map_st );

/*!
\author Luxor
*/
namespace data {

void init();
void shutdown();
void setPath( MulFileId id, string path );
string getPath( MulFileId id );

bool seekMap( uint32_t x, uint32_t y, map_st& m, uint8_t nMap = 0 ); //<! Luxor: nMap will be used for future multiple maps support.
bool collectStatics( uint32_t x, uint32_t y, staticVector& s_vec );

/*!
\author Luxor
*/
template <typename T> class cMULFile {
public:
	cMULFile( string path, string mode );
	~cMULFile() {
		if ( m_file != NULL )
			fclose( m_file );
		if ( m_cache != NULL )
			safedelete( m_cache );
	}
	bool getData( uint32_t index, T& data );
	bool getData( uint32_t index, uint8_t* ptr, uint32_t size );
	void setCache( map< uint32_t, T > *cache );
	bool eof();
	inline bool isReady() { return ( m_file != NULL ); }
	inline bool isCached() { return ( m_cache != NULL ); }
private:
	FILE	*m_file;
	map< uint32_t, T > *m_cache;
};

} // namespace data

#endif // __DATA_H__
