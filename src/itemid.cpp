/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief functions that wrap the checking for a list of item IDs
\todo cleanup and include into cItem
*/
#include "nxwcommn.h"
#include "itemid.h"
#include "debug.h"
#include "npcai.h"
#include "scp_parser.h"
#include "items.h"
#include "chars.h"
#include "inlines.h"
#include "scripts.h"



void itemGetPopUpHelp(char *str)
{
//	int id = (pi->id1<<8)+pi->id2;
	int type = pi->type;
	if (pi->IsInstrument())
		sprintf(str, TRANSLATE("This item is a musical instrument. You can use it for bardic skills like enticement or provocation"));
	else if (type == ITYPE_DOOR)
		sprintf(str, TRANSLATE("This a door. To open or close it, double click on it."));
	else if (type == ITYPE_LOCKED_DOOR)
		sprintf(str, TRANSLATE("This a locked door. To open or close it, click on the proper key and target it."));
	else if (type == ITYPE_FOOD)
		sprintf(str, TRANSLATE("This food you can eat when you're hungry. To eat, double click the food, but beware poisoned food!"));
	else if (type == ITYPE_RUNE)
		sprintf(str, TRANSLATE("This is a rune for use with recall, mark and gate travel spells"));
	else if (type == ITYPE_RESURRECT)
		sprintf(str, TRANSLATE("If you dye (or are dead) you can double click this item to resurrect!"));
	else if (type == ITYPE_KEY)
		sprintf(str, TRANSLATE("This is a key you can use (double click) to open doors"));
	else if (type == ITYPE_SPELLBOOK)
		sprintf(str, TRANSLATE("This is the spellbook, where you can write down your own spells for later use"));
	else if (type == ITYPE_POTION)
		sprintf(str, TRANSLATE("This is a potion! You can drink that when you need its effects... but beware of poison potions!"));
}

