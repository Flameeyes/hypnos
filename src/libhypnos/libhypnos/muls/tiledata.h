/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __LIBHYPNOS_MULS_TILEDATA_H__
#define __LIBHYPNOS_MULS_TILEDATA_H__

#include "libhypnos/muls/mulfiles.h"
#include "libhypnos/mmappedfile.h"
#include "libhypnos/exceptions.h"

namespace nLibhypnos {
namespace nMULFiles {

//@{
/*!
\name Tiledata flags
\brief Flags used in tiledata.mul to set attributes for tiledata elements
*/
	//! Is a background
	static const uint32_t flagTileBackground	= 0x00000001;
	//! Is a weapon
	static const uint32_t flagTileWeapon		= 0x00000002;
	//! Is transparent
	static const uint32_t flagTileTransparent	= 0x00000004;
	//! Is translucent
	static const uint32_t flagTileTranslucent	= 0x00000008;
	//! Is a wall
	static const uint32_t flagTileWall		= 0x00000010;
	//! Can damage
	static const uint32_t flagTileDamaging		= 0x00000020;
	//! Can't be passed over
	static const uint32_t flagTileImpassable	= 0x00000040;
	//! Is 'wet' (like water)
	static const uint32_t flagTileWet		= 0x00000080;
	//! Unknown flag
	static const uint32_t flagTileUnknown1		= 0x00000100;
	//! Is a surface
	static const uint32_t flagTileSurface		= 0x00000200;
	//! Is a bridge
	static const uint32_t flagTileBridge		= 0x00000400;
	//! Is stackable (generic?)
	static const uint32_t flagTileStackable		= 0x00000800;
	//! Is a window
	static const uint32_t flagTileWindow		= 0x00001000;
	//! Can't be shooted
	static const uint32_t flagTileNoShoot		= 0x00002000;
	//! Has 'a' as prefix
	static const uint32_t flagTileAPrefix		= 0x00004000;
	//! Has 'an' as prefix
	static const uint32_t flagTileAnPrefix		= 0x00008000;
	//! Is internal (like beard, hairs, and so on)
	static const uint32_t flagTileInternal		= 0x00010000;
	//! Are foliage (leaves)
	static const uint32_t flagTileFoliage		= 0x00020000;
	//! Partially hued
	static const uint32_t flagTilePartialHue	= 0x00040000;
	//! Unknown flag
	static const uint32_t flagTileUnknown2		= 0x00080000;
	//! Is a map
	static const uint32_t flagTileMap		= 0x00100000;
	//! Is a container
	static const uint32_t flagTileContainer		= 0x00200000;
	//! Is wearable
	static const uint32_t flagTileWearable		= 0x00400000;
	//! Is a light source
	static const uint32_t flagTileLightSource	= 0x00800000;
	//! Is animated
	static const uint32_t flagTileAnimated		= 0x01000000;
	//! Hasn't diagonal directions
	static const uint32_t flagTileNoDiagonal	= 0x02000000;
	//! Unknown flag
	static const uint32_t flagTileUnknown		= 0x04000000;
	//! Is armor
	static const uint32_t flagTileArmor		= 0x08000000;
	//! Is a roof
	static const uint32_t flagTileRoof		= 0x10000000;
	//! Is a door
	static const uint32_t flagTileDoor		= 0x20000000;
	//! Is a stair (back)
	static const uint32_t flagTileStairBack		= 0x40000000;
	//! Is a stair (right)
	static const uint32_t flagTileStairRight	= 0x80000000;
//@}

/*!
\file
\brief Tiledata handling classes

The tiledata.mul file is a strange file. It consists of 2 different sections,
one with so called 'Land Blocks' and one with the so called 'Static Blocks'.

For this, we threat is as two different files, with two classes, one which maps
the land tiles and one which maps the static tiles.
*/

/*!
\brief Internal structure for a tiledata's land block

A tiledata land block is composed of:
\li \b dword header (unknown)
\li \b landentry[32]

The landentries are structured like that:
\li \b dword flags (see the tileflags)
\li \b word textureid (if zero, the land hasn't a texture)
\li \b char[20] name of the tile

Information grabbed from Krrios File Formats
*/
class cTiledataLand
{
public:
	class cEntry
	{
	protected:
		uint32_t flags;	//!< Flags for the tile
		uint16_t textId;//!< Texture's id
		char name[20];	//!< Name of the tile
	public:
		uint32_t getFlags() const
		{ return mtohl(flags); }
		
		uint16_t getTextureID() const
		{ return mtohs(textId); }
		
		std::string getName() const;
	} PACK_NEEDED;

	uint32_t unknown;
	cEntry entries[32];
} PACK_NEEDED;

/*!
\class fTiledataLand tiledata.h "libhypnos/muls/tiledata.h"
\brief Access class to tiledata.mul land blocks

This class handles the access to the tiledata.mul file, restricted to the 512 
land blocks.

*/
class fTiledataLand : public tplMMappedFile<cTiledataLand>
{
protected:
	cTiledataLand::cEntry &tile(uint16_t id) const
	{
		if ( id >= 512 ) throw eOutOfBound(511, id);
		return array[id/32].entries[id%32];
	}
public:
	fTiledataLand();
	~fTiledataLand();
	
	uint32_t getFlags(uint16_t id) const
	{
		return tile(id).getFlags();
	}
	
	uint16_t getTextureID(uint16_t id) const
	{
		return tile(id).getTextureID();
	}
	
	std::string getName(uint16_t id) const
	{
		return tile(id).getName();
	}
};

/*!
\brief Internal structure for a tiledata's static block

A tiledata static block has a structure like that:
\li \b uint32_t header (unknown)
\li \b staticentry[32]

The staticentries are structured like that:
\li \b uint32_t flags
\li \b uint8_t weight
\li \b uint8_t quality
\li \b uint16_t unknown
\li \b uint8_t unknown
\li \b uint16_t quantity
\li \b uint16_t animationID
\li \b uint8_t unknown
\li \b uint8_t hue
\li \b uint8_t unknown
\li \b uint8_t unknown
\li \b uint8_t height
\li \b char[20] name
*/
class cTiledataStatic
{
public:
	class cEntry
	{
	protected:
		uint32_t flags;		//!< Flag of the tile
		uint8_t weight;		//!< Weight of the item (if 255 not movable)
		uint8_t quality;	//!< Quality of the item
		uint16_t unk1;		//!< Unknown
		uint8_t unk2;		//!< Unknown
		uint16_t quantity;	//!< Quantity of the item
		uint16_t animID;	//!< Animation ID (when equipped)
		uint8_t unk3;		//!< Unknown
		uint8_t hue;		//!< Hue (colored lights?)
		uint8_t unk4;		//!< Unknown
		uint8_t unk5;		//!< Unknown
		uint8_t height;		//!< Height of the tile
		char name[20];		//!< Name of the tile
	public:
		uint32_t getFlags() const
		{ return mtohl(flags); }
		
		uint8_t getWeight() const
		{ return weight; }
		
		uint16_t getQuantity() const
		{ return mtohs(quantity); }
		
		uint16_t getAnimID() const
		{ return mtohs(animID); }
		
		uint8_t getHue() const
		{ return hue; }
		
		uint8_t getHeight() const
		{ return height; }
		
		std::string getName() const;
	} PACK_NEEDED;
	
	uint32_t unknown;
	cEntry entries[32] PACK_NEEDED;
} PACK_NEEDED;

/*!
\class fTiledataStatic tiledata.h "libhypnos/muls/tiledata.h"
\brief Access class to tiledata.mul static blocks

This class handles the access to the tiledata.mul file, restricted to the 
variables static blocks.

*/
class fTiledataStatic : public tplMMappedFile<cTiledataStatic>
{
protected:
	cTiledataStatic::cEntry &tile(uint16_t id) const
	{
		if ( id >= getCount()*32 )
			throw eOutOfBound(getCount()*32-1, id);
		
		return array[id/32].entries[id%32];
	}
public:
	fTiledataStatic();
	~fTiledataStatic();
	
	uint32_t getFlags(uint16_t id) const
	{ return tile(id).getFlags(); }
	
	uint8_t getWeight(uint16_t id) const
	{ return tile(id).getWeight(); }
	
	uint16_t getQuantity(uint16_t id) const
	{ return tile(id).getQuantity(); }
	
	uint16_t getAnimID(uint16_t id) const
	{ return tile(id).getAnimID(); }
	
	uint8_t getHue(uint16_t id) const
	{ return tile(id).getHue(); }
	
	uint8_t getHeight(uint16_t id) const
	{ return tile(id).getHeight(); }
	
	std::string getName(uint16_t id) const
	{ return tile(id).getName(); }
};

}}

#endif
