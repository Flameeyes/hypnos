/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief cWeapon implementation
*/

#include "objects/citem/cweapon.h"

cWeapon::WeaponMap cWeapon::weaponinfo;

cWeapon::cWeapon()
{
	cWeapon(nextSerial());
}

cWeapon::cWeapon(uint32_t serial) :
	cItem(serial)
{
	classType = cItem::itWeapon;
}

/*!
\brief Say if an ID is a weapon of the specified type
\param id id of the weapon
\param type mask of weapon types to tests
*/
const bool cWeapon::isWeaponLike( uint16_t id, uint16_t type )
{
	WeaponMap::iterator iter( weaponinfo.find( id ) );
	if( iter==weaponinfo.end() )
		return false;
	else
		return ( iter->second & type );
}

/*!
\brief Loads data about weapons from scripts
\todo rewrite it via xml, this will not work!
*/
void cWeapon::loadWeaponsInfo()
{
#if 0
	cScpIterator* iter = NULL;
	char script1[1024];
	char script2[1024];
	uint16_t id=0xFFFF;
	uint16_t type=weaponSword1H;

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
					weaponinfo[id]=type;
				}
			}

		}
		while ( (script1[0]!='}') && (++loopexit < MAXLOOPS) );

		type++;
	}while ( (strcmp("EOF", script1)) && (++loopexit < MAXLOOPS) );

	safedelete(iter);
#endif
}
