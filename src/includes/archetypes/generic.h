/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __ARCHETYPES_ITEMS_H__
#define __ARCHETYPES_ITEMS_H__

/*!
\brief Script Archetypes

In this namespace we declare the classes to store and use the items' and
characters' archetypes read from the XML datafiles.
*/
namespace nArchetypes
{
//@{
/*!
\brief Creation functions

These functions are used to create new items or characters based on a given
XML datafile identifier (a string).
*/

	pItem createItem(uint16_t baseid);
	pItem createItem(std::string id);
	
	pPC createPlayer(std::string id);
	pNPC createCharacter(std::string id);

//@}
};

#endif
