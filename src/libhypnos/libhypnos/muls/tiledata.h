/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __LIBHYPMUL_TILEDATA_H__
#define __LIBHYPMUL_TILEDATA_H__

#include "libhypmul/mulfiles.h"
#include "libhypmul/mmappedfile.h"

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
class nMULFiles::cTiledataLand
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
		
		uint32_t getTextureID() const
		{ return mtohs(flags); }
		
		std::string getName();
	} PACK_NEEDED;

	uint32_t unknown;
	cEntry entries[32];
} PACK_NEEDED;

/*!
\class fTiledataLand tiledata.h "libhypmul/tiledata.h"
\brief Access class to tiledata.mul land blocks

This class handles the access to the tiledata.mul file, restricted to the 512 
land blocks.

*/
class nMULFiles::fTiledataLand : public tMMappedFile<cTiledataLand>
{
protected:
	cTiledataLand &tile(uint16_t id) const
	{ return array[id/32].entries[id%32]; }
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
	
	std::string getString(uint16_t id) const
	{
		return tile(id).getString();
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
class nMULFiles::cTiledataStatic
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
	cEntry entries[32];
} PACK_NEEDED;

/*!
\class fTiledataStatic tiledata.h "libhypmul/tiledata.h"
\brief Access class to tiledata.mul static blocks

This class handles the access to the tiledata.mul file, restricted to the 
variables static blocks.

*/
class nMULFiles::fTiledataStatic : public tMMappedFile<cTiledataStatic>
{
protected:
	cTiledataStatic &tile(uint16_t id) const
	{ return array[id/32].entries[id%32]; }
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


#endif
