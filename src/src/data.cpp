/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\author Luxor
\brief Data files handling system
*/

#include "common_libs.h"
#include "data.h"
#include "inlines.h"
#include "nxw_utils.h"

bool statics_cache = false;
bool map_cache = false;

uint16_t map_width = 768;
uint16_t map_height = 512;

namespace data {


std::vector < cMULFile< map_st >* > maps; // Maps (map0.mul map1.mul...)
cMULFile< staticIdx_st >* staticIdx; // staidx0.mul
cMULFile< static_st >* statics; // statics.mul
cMULFile< land_st >* tdLand; // tiledata.mul
cMULFile< tile_st >* tdTile; // tiledata.mul
cMULFile< multiIdx_st >* multiIdx; // multi.idx
cMULFile< multi_st >* multi; // multi.mul

cMULFile< verdata_st >* verIdx; // verdata.mul
cMULFile< land_st >* verLand; // verdata.mul
cMULFile< tile_st >* verTile; // verdata.mul

static int32_t verdataEntries;

static std::string map_path;
static std::string staIdx_path;
static std::string statics_path;
static std::string tiledata_path;
static std::string verdata_path;
static std::string multi_path;
static std::string multiIdx_path;

/*!
\author Luxor
\brief Caches map mul files, trying to maintain a sequential reading to get the best speed.
*/
static void cacheMap()
{
	uint32_t i, pos;

	for ( i = 0; i < maps.size(); i++ ) {
		if ( !maps[i]->isReady() )
			continue;
		map_st m;
		std::map< uint32_t, map_st > *map_cache = new std::map< uint32_t, map_st >;
		ConOut( "\nCaching map %i data ( map0.mul )\t\t", i );
		uint16_t blockX;
		uint8_t xOffset;
		int8_t yOffset;
		int16_t blockY;
		for ( blockX = 0; blockX < map_width; blockX++ ) {
			for ( blockY = map_height - 1; blockY >= 0; blockY-- ) {
				for ( yOffset = 7; yOffset >= 0; yOffset-- ) {
					for ( xOffset = 0; xOffset < 8; xOffset++ ) {
						pos =
							// Block position - A block contains 8x8 cells.
							// Blocks are registered in file by top to bottom columns from left to right.
							( blockX * map_height * MAP_BLOCK_SIZE ) + ( blockY * MAP_BLOCK_SIZE ) +
							// Header of the block, it doesn't interest us.
							MAP_HEADER_SIZE +
							// Cell position in block - A cell is a map_st.
							// Cells are registered in blocks by left to right rows from top to bottom.
							( yOffset * 8 * map_st_size ) + ( xOffset * map_st_size );

						if ( maps[i]->getData( pos, m ) )
							map_cache->insert( pair< uint32_t, map_st >( pos, m ) );
					}
				}
			}
		}
		maps[i]->setCache( map_cache );
		ConOut( "[Done]" );
	}
}

/*!
\author Luxor
\brief Caches statics mul files, trying to maintain a sequential reading to get the best speed.
*/
static void cacheStatics()
{
	uint32_t i, pos;

	if ( !statics->isReady() || !staticIdx->isReady() )
		return;

	static_st s;
	std::map< uint32_t, static_st > *statics_cache = new std::map< uint32_t, static_st >;

	staticIdx_st staidx;
	std::map< uint32_t, staticIdx_st > *staidx_cache = new std::map< uint32_t, staticIdx_st >;

	uint16_t blockX;
	int16_t blockY;
	uint32_t num;
	ConOut( "\nCaching statics data ( staidx0.mul, statics0.mul )\t\t" );
	for ( blockX = 0; blockX < map_width; blockX++ ) {
		for ( blockY = map_height -1; blockY >= 0; blockY-- ) {
			pos =
				// Block position - A block contains (staticIdx_st.length / static_st_size ) statics.
				// Blocks are registered in file by top to bottom columns from left to right.
				( blockX * map_height * staticIdx_st_size ) + ( blockY * staticIdx_st_size );

			if ( !staticIdx->getData( pos, staidx ) || staidx.start < 0 || staidx.length <= 0 )
				continue;
			staidx_cache->insert( pair< uint32_t, staticIdx_st >( pos, staidx ) );

			num = staidx.length / static_st_size;

			for ( i = 0; i < num; i++ ) {
				pos = staidx.start + ( i * static_st_size );
				if ( statics->getData( pos, s ) )
					statics_cache->insert( pair< uint32_t, static_st >( pos, s ) );
			}
		}
	}
	statics->setCache( statics_cache );
	staticIdx->setCache( staidx_cache );
	ConOut( "[Done]" );
}

/*!
\author Luxor
\brief Caches tiledata mul files, trying to maintain a sequential reading to get the best speed.
*/
static void cacheTileData()
{
	if ( !tdLand->isReady() || !tdTile->isReady() )
		return;

	uint32_t pos;
	tile_st t;
	land_st l;
	std::map< uint32_t, land_st > *land_cache = new std::map< uint32_t, land_st >;
	std::map< uint32_t, tile_st > *tile_cache = new std::map< uint32_t, tile_st >;

	uint32_t block;
	uint8_t index;
	ConOut( "\nCaching land data ( tiledata.mul )\t\t" );
	for ( block = 0; block < 512; block++ ) {
		for ( index = 0; index < 32; index++ ) {
			pos =
				// Each block contains 32 land_st.
				( (block + 1) * TILE_HEADER_SIZE ) + ( land_st_size * (index + block * 32) );
			if ( verLand->getData( pos, l ) )
				land_cache->insert( pair< uint32_t, land_st >( pos, l ) );
			else if ( tdLand->getData( pos, l ) )
				land_cache->insert( pair< uint32_t, land_st >( pos, l ) );
		}
	}
	ConOut( "[Done]" );
	tdLand->setCache( land_cache );

	ConOut( "\nCaching tiles data ( tiledata.mul ) \t\t" );
	for ( block = 0; !tdTile->eof(); block++ ) {
		for ( index = 0; index < 32; index++ ) {
			pos =
				// Go beyond the land_st dedicated space.
				TILEDATA_LAND_SIZE +
				// Each block contains 32 tile_st.
				( (block + 1) * TILE_HEADER_SIZE ) + ( tile_st_size * (index + block * 32) );
			if ( verTile->getData( pos, t ) )
				tile_cache->insert( pair< uint32_t, tile_st >( pos, t ) );
			else if ( tdTile->getData( pos, t ) )
				tile_cache->insert( pair< uint32_t, tile_st >( pos, t ) );
		}
	}
	ConOut( "[Done]" );
	tdTile->setCache( tile_cache );
}

/*!
\author Luxor
\brief Caches the verdata index, trying to maintain a sequential reading to get the best speed.
*/
static void cacheVerdataIndex()
{
	if ( !verIdx->isReady() )
		return;

	std::map< uint32_t, verdata_st >* verIdx_cache = new std::map< uint32_t, verdata_st >;
	verIdx->getData( 0, (uint8_t*)(&verdataEntries), 4 );

	verdata_st v;
	int32_t i;
	uint32_t pos;

	ConOut( "\nCaching verdata index ( verdata.mul ) \t\t" );
	for ( i = 0; i < verdataEntries; i++ ) {
		pos = VERDATA_HEADER_SIZE + ( i * verdata_st_size );
		if ( verIdx->getData( pos, v ) )
			verIdx_cache->insert( pair< uint32_t, verdata_st >( pos, v ) );
	}
	ConOut( "[Done]" );
	verIdx->setCache( verIdx_cache );
}

/*!
\author Luxor
\brief Caches the verdata info, trying to maintain a sequential reading to get the best speed.
*/
static void cacheVerdata()
{
	if ( !verIdx->isReady() && !verTile->isReady() && !verLand->isReady() )
		return;

	std::map< uint32_t, tile_st >* verTile_cache = new std::map< uint32_t, tile_st >;
	std::map< uint32_t, land_st >* verLand_cache = new std::map< uint32_t, land_st >;
	verdata_st v;
	tile_st t;
	land_st l;
	uint32_t block, pos;
	uint8_t index;
	int32_t i;

	ConOut( "\nCaching verdata tiledata info ( verdata.mul ) \t\t" );
	for ( i = 0; i < verdataEntries; i++ ) {
		pos = VERDATA_HEADER_SIZE + ( i * verdata_st_size );
		if ( !verIdx->getData( pos, v ) )
			continue;
		if ( v.fileid != VerTileData )
			continue;

		if ( v.block >= 512 ) {
			block = v.block - 512;

			for ( index = 0; index < 32; index++ ) {
				pos = TILE_HEADER_SIZE + v.pos + index * tile_st_size;
				if ( verTile->getData( pos, t ) )
					verTile_cache->insert( pair< uint32_t, tile_st >( pos, t ) );
			}
		} else {
			for ( index = 0; index < 32; index++ ) {
				pos = TILE_HEADER_SIZE + v.pos + index * land_st_size;
				if ( verLand->getData( pos, l ) )
					verLand_cache->insert( pair< uint32_t, land_st >( pos, l ) );
			}
		}
	}
	ConOut( "[Done]" );
	verTile->setCache( verTile_cache );
	verLand->setCache( verLand_cache );
}


#define CHECKMUL( A, B ) if ( !A->isReady() ) { LogError( "ERROR: Mul File %s not found...\n", B ); return; }

/*!
\author Luxor
*/
void init()
{
	//
	// If MULs loading fails, stop the server!
	//
	keeprun = false;

	ConOut("Preparing to open *.mul files...\n(If they don't open, fix your paths in server.cfg)\n");

	maps.push_back( new cMULFile< map_st > ( map_path, "rb" ) );
	CHECKMUL( maps[0], map_path.c_str() );

	staticIdx = new cMULFile< staticIdx_st > ( staIdx_path, "rb" );
	CHECKMUL( staticIdx, staIdx_path.c_str() );

	statics = new cMULFile< static_st > ( statics_path, "rb" );
	CHECKMUL( statics, statics_path.c_str() );

	tdLand = new cMULFile< land_st > ( tiledata_path, "rb" );
	CHECKMUL( tdLand, tiledata_path.c_str() );
	tdTile = new cMULFile< tile_st > ( tiledata_path, "rb" );

	multiIdx = new cMULFile< multiIdx_st > ( multiIdx_path, "rb" );
	CHECKMUL( multiIdx, multiIdx_path.c_str() );

	multi = new cMULFile< multi_st > ( multi_path, "rb" );
	CHECKMUL( multi, multi_path.c_str() );

	verIdx = new cMULFile< verdata_st > ( verdata_path, "rb" );
	CHECKMUL( verIdx, verdata_path.c_str() );
	verLand = new cMULFile< land_st > ( verdata_path, "rb" );
	verTile = new cMULFile< tile_st > ( verdata_path, "rb" );

	//
	// We cache always the tiledata, it's very small and it really improves performances.
	//
	cacheVerdataIndex();
	cacheVerdata();
	cacheTileData();

	//
	// After tiledata caching, verdata's cache becomes useless. Let's free some memory.
	//
	verIdx->setCache( NULL );
	verLand->setCache( NULL );
	verTile->setCache( NULL );

	//
	// Check for statics and map caching
	//
	if ( statics_cache )
		cacheStatics();
	if ( map_cache )
		cacheMap();
	//
	// MULs loaded, keep the server running
	//
	ConOut("MUL files loaded succesfully.\n" );
	keeprun = true;
}

/*!
\author Luxor
*/
void shutdown()
{
	uint32_t i;
	for ( i = 0; i < maps.size(); i++ )
		if ( maps[i] != NULL )
			safedelete( maps[i] );

	if ( staticIdx != NULL )
		safedelete( staticIdx );
	if ( statics != NULL )
		safedelete( statics );
	if ( tdLand != NULL )
		safedelete( tdLand );
	if ( tdTile != NULL )
		safedelete( tdTile );
	if ( multiIdx != NULL )
		safedelete( multiIdx );
	if ( multi != NULL )
		safedelete( multi );
	if ( verIdx != NULL )
		safedelete( verIdx );
	if ( verLand!= NULL )
		safedelete( verLand );
	if ( verTile != NULL )
		safedelete( verTile );
}

/*!
\author Luxor
*/
void setPath( MulFileId id, std::string path )
{
	switch ( id )
	{
		case Map_File:
			map_path = path;
			break;
		case StaIdx_File:
			staIdx_path = path;
			break;
		case Statics_File:
			statics_path = path;
			break;
		case Multi_File:
			multi_path = path;
			break;
		case MultiIdx_File:
			multiIdx_path = path;
			break;
		case TileData_File:
			tiledata_path = path;
			break;
		case VerData_File:
			verdata_path = path;
			break;
		default:
			break;
	}
}

/*!
\author Luxor
*/
std::string getPath( MulFileId id )
{
	switch ( id )
	{
		case Map_File:
			return map_path;
			break;
		case StaIdx_File:
			return staIdx_path;;
			break;
		case Statics_File:
			return statics_path;
			break;
		case Multi_File:
			return multi_path;
			break;
		case MultiIdx_File:
			return multiIdx_path;
			break;
		case TileData_File:
			return tiledata_path;
			break;
		case VerData_File:
			return verdata_path;
			break;
		default:
			break;
	}
	return std::string( " " );
}


/*!
\author Luxor
*/
bool seekMap( uint32_t x, uint32_t y, map_st& m, uint8_t nMap )
{
	if ( nMap >= maps.size() )
		return false;
	if ( !maps[ nMap ]->isReady() )
		return false;

	uint32_t pos;
	uint16_t blockX = x / 8, blockY = y / 8, cellX = x % 8, cellY = y % 8;
	pos =
		// Block position - A block contains 8x8 cells. Blocks are registered in file by top to bottom columns from left to right.
		( blockX * map_height * MAP_BLOCK_SIZE ) + ( blockY * MAP_BLOCK_SIZE ) +
		// Header of the block, it doesn't interest us.
		MAP_HEADER_SIZE +
		// Cell position in block - A cell is a map_st. Cells are registered in blocks by left to right rows from top to bottom.
		( cellY * 8 * map_st_size ) + ( cellX * map_st_size );

	return maps[ nMap ]->getData( pos, m );
}

/*!
\author Luxor
*/
bool collectStatics( uint32_t x, uint32_t y, staticVector& s_vec )
{
	if ( !staticIdx->isReady() || !statics->isReady() )
		return false;


	uint16_t blockX = x / 8, blockY = y / 8;
	uint32_t pos =
		// Block position - A block contains (staticIdx_st.length / static_st_size ) statics.
		// Blocks are registered in file by top to bottom columns from left to right.
		( blockX * map_height * staticIdx_st_size ) + ( blockY * staticIdx_st_size );

	staticIdx_st staidx;
	if ( !staticIdx->getData( pos, staidx ) || staidx.start < 0 || staidx.length <= 0 )
		return false;

	uint32_t num = staidx.length / static_st_size;
	static_st s;
	uint8_t xOffset = x % 8, yOffset = y % 8;
	for ( uint32_t i = 0; i < num; i++ ) {
		pos = staidx.start + ( i * static_st_size );
		if ( !statics->getData( pos, s ) )
			continue;
		if ( s.xoff == xOffset && s.yoff == yOffset )
			s_vec.push_back( s );
	}
	return ( s_vec.size() > 0 );
}

/*!
\author Luxor
*/
bool seekLand( uint16_t id, land_st& land )
{
	if ( !tdLand->isCached() && seekVerLand( id, land ) )
		return true;

	if ( !tdLand->isReady() )
		return false;

	uint16_t block = id / 32;

	uint32_t pos =
		// Each block contains 32 land_st.
		( (block + 1) * TILE_HEADER_SIZE ) + ( land_st_size * id );

	return tdLand->getData( pos, land );
}

/*!
\author Luxor
*/
bool seekTile( uint16_t id, tile_st& tile )
{
	if ( !tdTile->isCached() && seekVerTile( id, tile ) )
		return true;

	if ( !tdTile->isReady() )
		return false;

	uint16_t block = id / 32;

	uint32_t pos =
		// Go beyond the land_st dedicated space.
		TILEDATA_LAND_SIZE +
		// Each block contains 32 tile_st.
		( (block + 1) * TILE_HEADER_SIZE ) + ( tile_st_size * id );

	return tdTile->getData( pos, tile );
}

/*!
\author Luxor
*/
bool seekMulti( uint16_t id, multiVector& m_vec )
{
	if ( !multiIdx->isReady() || !multi->isReady() )
		return false;

	multiIdx_st idx;
	uint32_t pos = id * multiIdx_st_size;
	if ( !multiIdx->getData( pos, idx ) || idx.start < 0 || idx.length <= 0 )
		return false;

	multi_st m;
	uint32_t num = idx.length / multi_st_size;
	for ( uint32_t i = 0; i < num; i++ ) {
		pos = idx.start + ( i * multi_st_size );
		if ( !multi->getData( pos, m ) )
			continue;
		m_vec.push_back( m );
	}
	return ( m_vec.size() > 0 );
}

/*!
\author Luxor
*/
bool seekVerTile( uint16_t id, tile_st& tile )
{
	if ( !verIdx->isReady() || !verTile->isReady() )
		return false;

	int32_t i, block = id / 32 + 512;
	verdata_st v;
	uint32_t pos;

	for ( i = 0; i < verdataEntries; i++ ) {
		pos = VERDATA_HEADER_SIZE + ( i * verdata_st_size );
		if ( !verIdx->getData( pos, v ) )
			continue;
		if ( v.fileid != VerTileData || v.block < 512 )
			continue;

		if ( block != v.block )
			continue;

		pos = TILE_HEADER_SIZE + v.pos + ( id % 32 ) * tile_st_size;
		return verTile->getData( pos, tile );
	}
	return false;
}

/*!
\author Luxor
*/
bool seekVerLand( uint16_t id, land_st& land )
{
	if ( !verIdx->isReady() || !verLand->isReady() )
		return false;

	uint32_t pos;
	verdata_st v;
	int32_t i, block = id / 32;

	for ( i = 0; i < verdataEntries; i++ ) {
		pos = VERDATA_HEADER_SIZE + ( i * verdata_st_size );
		if ( !verIdx->getData( pos, v ) )
			continue;
		if ( v.fileid != VerTileData || v.block >= 512 )
			continue;


		if ( block != v.block )
			continue;

		pos = TILE_HEADER_SIZE + v.pos + ( id % 32 ) * land_st_size;
		return verLand->getData( pos, land );
	}
	return false;
}



/*!
\author Luxor
*/
template <typename T>
cMULFile<T>::cMULFile( std::string path, std::string mode )
{
	m_cache = NULL;
	m_file = fopen( path.c_str(), mode.c_str() );

	// <Xanathar>: MULs path autodetecting
#ifdef WIN32
	if ( !isReady() && ServerScp::g_nAutoDetectMuls ) {
		char *s;
		char fn[800], fn2[800];
		char *f;
		strcpy( fn2, path.c_str() );
		f = splitPath( fn2 );
		s = getHKLMRegistryString( "SOFTWARE\\Origin Worlds Online\\Ultima Online\\1.0", "ExePath" );
		if ( s != NULL ) {
			splitPath( s );
			strcpy( fn, s );
			safedelete( s );
			strcat( fn, "\\" );
			strcat( fn, f );
			ConOut( "*** Can't open %s, trying %s ***\n", path.c_str(), fn );
			m_file = fopen( fn, mode.c_str() );
		}
	}
#endif
	// </Xanathar>
}

/*!
\author Luxor
*/
template <typename T>
bool cMULFile<T>::getData( uint32_t index, T& data )
{
	if ( !isReady() )
		return false;
	if ( !isCached() ) {
		fseek( m_file, index, SEEK_SET );
		return ( fread( &data, sizeof( T ), 1, m_file ) != 0 );
	}

	typename std::map< uint32_t, T >::iterator it = m_cache->find( index );
	if ( it == m_cache->end() )
		return false;

	data = it->second;
	return true;
}

/*!
\author Luxor
*/
template <typename T>
bool cMULFile<T>::getData( uint32_t index, uint8_t* ptr, uint32_t size )
{
	if ( !isReady() )
		return false;
	fseek( m_file, index, SEEK_SET );
	return ( fread( ptr, sizeof( uint8_t ), size, m_file ) != 0 );
}

/*!
\author Luxor
*/
template <typename T>
void cMULFile<T>::setCache( typename std::map< uint32_t, T > *cache )
{
	if ( m_cache != NULL )
		safedelete( m_cache );
	m_cache = cache;
}

/*!
\author Luxor
*/
template <typename T>
bool cMULFile<T>::eof()
{
	if ( m_file == NULL )
		return true;
	return feof( m_file );
}


} // namespace data

