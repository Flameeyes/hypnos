  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#ifndef __DATA_H__
#define __DATA_H__

/*!
\file
\author Luxor
\brief Header of data files handling system
*/

extern uint16_t map_width;
extern uint16_t map_height;
extern LOGICAL statics_cache;
extern LOGICAL map_cache;

/*!
\author Luxor
*/
enum {
	MAP_HEADER_SIZE = 4, MAP_BLOCK_SIZE = 196,
	TILE_HEADER_SIZE = 4, TILEDATA_LAND_SIZE = 0x68800,
	VERDATA_HEADER_SIZE = 4
};

/*!
\author Luxor
*/
enum VerFile {
	VerMap = 0x00,
	VerStaIdx,
	VerStatics,
	VerArtIdx,
	VerArt,
	VerAnimIdx,
	VerAnim,
	VerSoundIdx,
	VerSound,
	VerTexIdx,
	VerTexMaps,
	VerGumpIdx,
	VerGumpArt,
	VerMultiIdx,
	VerMulti,
	VerSkillsIdx,
	VerSkills,
	VerTileData = 0x1E,
	VerAnimData
};

/*!
\author Luxor
*/
enum MulFileId {
	Map_File = 0, StaIdx_File, Statics_File, Multi_File, MultiIdx_File, TileData_File, VerData_File
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

typedef std::vector< static_st > staticVector;

struct land_st
{
	uint32_t flags;
	uint16_t id;
	TEXT name[20];
} PACK_NEEDED;

struct tile_st
{
	uint32_t flags;
	uint8_t weight;
	uint8_t quality; // if wearable, it's the layer. if it's a light source, it's the light id.
	uint16_t unknown;
	uint8_t unknown1;
	uint8_t quantity;
	uint16_t animid;
	uint8_t unknown2;
	uint8_t hue;
	uint16_t unknown3;
	int8_t height;
	TEXT name[20];
} PACK_NEEDED;

struct multiIdx_st {
	int32_t start;
	int32_t length;
	int32_t unknown;
} PACK_NEEDED;

struct multi_st {
	int16_t block;
	uint16_t x;
	uint16_t y;
	int16_t height;
	uint32_t flags;
} PACK_NEEDED;

typedef std::vector< multi_st > multiVector;

/*!
\author Luxor
*/
struct verdata_st
{
	int32_t fileid;
	int32_t block;
	int32_t pos;
	int32_t size;
	int32_t various;
} PACK_NEEDED;

/*!
\author Luxor
*/
enum {
	TILEFLAG_BACKGROUND	=	0x00000001,
	TILEFLAG_WEAPON		=	0x00000002,
	TILEFLAG_TRANSPARENT	=	0x00000004,
	TILEFLAG_TRANSLUCENT	=	0x00000008,
	TILEFLAG_WALL		=	0x00000010,
	TILEFLAG_DAMAGING	=	0x00000020,
	TILEFLAG_IMPASSABLE	=	0x00000040,
	TILEFLAG_WET		=	0x00000080,
	TILEFLAG_SURFACE	=	0x00000200,
	TILEFLAG_BRIDGE		=	0x00000400,
	TILEFLAG_STACKABLE	=	0x00000800,
	TILEFLAG_WINDOW		=	0x00001000,
	TILEFLAG_NOSHOOT	=	0x00002000,
	TILEFLAG_PREFIX_A	=	0x00004000,
	TILEFLAG_PREFIX_AN	=	0x00008000,
	TILEFLAG_INTERNAL	=	0x00010000,
	TILEFLAG_FOLIAGE	=	0x00020000,
	TILEFLAG_PARTIAL_HUE	=	0x00040000,
	TILEFLAG_MAP		=	0x00100000,
	TILEFLAG_CONTAINER	=	0x00200000,
	TILEFLAG_WEARABLE	=	0x00400000,
	TILEFLAG_LIGHTSOURCE	=	0x00800000,
	TILEFLAG_ANIMATED	=	0x01000000,
	TILEFLAG_NODIAGONAL	=	0x02000000,
	TILEFLAG_ARMOR		=	0x08000000,
	TILEFLAG_ROOF		=	0x10000000,
	TILEFLAG_DOOR		=	0x20000000,
	TILEFLAG_STAIR_BACK	=	0x40000000,
	TILEFLAG_STAIR_RIGHT	=	0x80000000
};


const uint8_t verdata_st_size = sizeof( verdata_st );
const uint8_t multi_st_size = sizeof( multi_st );
const uint8_t multiIdx_st_size = sizeof( multiIdx_st );
const uint8_t tile_st_size = sizeof( tile_st );
const uint8_t land_st_size = sizeof( land_st );
const uint8_t static_st_size = sizeof( static_st );
const uint8_t staticIdx_st_size = sizeof( staticIdx_st );
const uint8_t map_st_size = sizeof( map_st );


/*!
\author Luxor
*/
namespace data {

void init();
void shutdown();
void setPath( MulFileId id, std::string path );
std::string getPath( MulFileId id );

LOGICAL seekMap( uint32_t x, uint32_t y, map_st& m, uint8_t nMap = 0 ); //<! Luxor: nMap will be used for future multiple maps support.
LOGICAL collectStatics( uint32_t x, uint32_t y, staticVector& s_vec );
LOGICAL seekLand( uint16_t id, land_st& land );
LOGICAL seekTile( uint16_t id, tile_st& tile );
LOGICAL seekMulti( uint16_t id, multiVector& m_vec );
LOGICAL seekVerTile( uint16_t id, tile_st& tile );
LOGICAL seekVerLand( uint16_t id, land_st& land );

/*!
\author Luxor
*/
template <typename T> class cMULFile {
public:
	cMULFile( std::string path, std::string mode );
	~cMULFile() {
		if ( m_file != NULL )
			fclose( m_file );
		if ( m_cache != NULL )
			safedelete( m_cache );
	}
	LOGICAL getData( uint32_t index, T& data );
	LOGICAL getData( uint32_t index, BYTE* ptr, uint32_t size );
	void setCache( std::map< uint32_t, T > *cache );
	LOGICAL eof();
	inline LOGICAL isReady() { return ( m_file != NULL ); }
	inline LOGICAL isCached() { return ( m_cache != NULL ); }
private:
	FILE	*m_file;
	std::map< uint32_t, T > *m_cache;
};

} // namespace data

#endif // __DATA_H__

