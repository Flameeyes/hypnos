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

WEAPONINFOMAP weaponinfo;


void loadweaponsinfo()
{

    cScpIterator* iter = NULL;
    char script1[1024];
    char script2[1024];
	UI16 id=0xFFFF;
	int type=SWORD1H;

	int loopexit=0;
	do
	{
		safedelete(iter);
		iter = Scripts::WeaponInfo->getNewIterator("SECTION WEAPONTYPE %i", type );
		if( iter==NULL ) continue;

		do
		{
			iter->parseLine(script1, script2);
			if ((script1[0]!='}')&&(script1[0]!='{'))
			{
				if (!strcmp("ID", script1)) {
					id = str2num(script2);
					weaponinfo[id]=static_cast<WEAPONTYPE>(type);
				}
			}

		}
        while ( (script1[0]!='}') && (++loopexit < MAXLOOPS) );

		type++;
    }
	while ( (strcmp("EOF", script1)) && (++loopexit < MAXLOOPS) );

    safedelete(iter);

}

bool isWeaponLike( UI16 id, WEAPONTYPE type1, WEAPONTYPE type2, WEAPONTYPE type3 )
{
	WEAPONINFOMAP::iterator iter( weaponinfo.find( id ) );
	if( iter==weaponinfo.end() )
		return false;
	else {
		return ( iter->second == type1 ) || ( iter->second==type2 ) || ( iter->second==type3 );
	}
}

void itemGetPopUpHelp(char *str, P_ITEM pi)
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

void charGetPopUpHelp(char *str, P_CHAR pc)
{
	if (!pc->npc) {
		sprintf(str, TRANSLATE("He/She is a player like you. You've met new people!!"));

		if (pc->IsCounselor()) {
			sprintf(str, TRANSLATE("He/She is a Counselor. You can ask him if you need help on the game"));
		}

		if (pc->IsGM()) {
			sprintf(str, TRANSLATE("He/She is a Game Master. You can ask his help if you're stuck or have bugs or other problems"));
		}
	}
	else {
		if (pc->npcaitype==NPCAI_PLAYERVENDOR)
			sprintf(str, TRANSLATE("He/She is a vendor which sells items owned by a player. Good shopping!"));
		else if (pc->npcaitype==NPCAI_EVIL)
			sprintf(str, TRANSLATE("Run AWAY!!! It will kill you!!"));
	}
}
