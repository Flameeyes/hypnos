/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "logsystem.h"
#include "archs/tinterface.h"
#include "backend/strconstants.h"
#include "objects/citem/cweapon.h"

#include <fstream>
#include <mxml.h>

cWeapon::WeaponMap cWeapon::weaponinfo;

cWeapon::cWeapon() : cItem()
{
}

cWeapon::cWeapon(uint32_t serial) :
	cItem(serial)
{
	combatSkill = skInvalid;
}

/*!
\brief Say if an ID is a weapon of the specified type
\param id id of the weapon
\param type mask of weapon types to tests
*/
bool cWeapon::isWeaponLike( uint16_t id, uint16_t type )
{
	WeaponMap::iterator iter( weaponinfo.find( id ) );
	if( iter==weaponinfo.end() )
		return false;
	else
		return ( iter->second & type );
}

/*!
\brief Tells the combat skill used by the weapon

This function check the combatSkill attribute and, if skInvalid, checks
againts the ID of the weapon
*/
Skill cWeapon::getCombatSkill() const
{
	if ( combatSkill != skInvalid )
		return combatSkill;
	
	if ( isWeaponLike(weaponSword1H|weaponSword2H|weaponAxe1H|weaponAxe2H) )
		return skSwordsmanship;
	
	if ( isWeaponLike(weaponFenc1H|weaponFenc2H) )
		return skFencing;
	
	if ( isWeaponLike(weaponMace1H|weaponMace2H|weaponStaff1H|weaponStaff2H) )
		return skMacefighting;
	
	if ( isWeaponLike(weaponBow|weaponXBow|weaponHXBow) )
		return skArchery;
	
	return skWrestling;
}

/*!
\brief Loads data about weapons from scripts
\todo rewrite it via xml, this will not work!
*/
void cWeapon::loadWeaponsInfo()
{
	outPlain("Loading weapons information...\t\t");
	
	std::ifstream xmlfile("config/weapons.xml");
	if ( ! xmlfile )
	{
		outPlain("[ Failed ]\n");
		LogCritical("Unable to open weapons.xml file.");
		return;
	}
	
	try {
		MXML::Document doc(xmlfile);
		
		MXML::Node *n = doc.main()->child();
		if ( ! n ) return;
		
		do {
			if ( n->name() != "weapon" )
			{
				LogWarning("Unknown node %s in weapons.xml, ignoring", n->name().c_str() );
				continue;
			}

			uint16_t type = nStrConstants::weaponsTypes( n->data() );
			if ( ! type )
			{
				LogWarning("Incomplete node in weapons.xml, ignoring");
				continue;
			}
			
			try {
				MXML::Node *p = n->child();
				
				if ( ! p )
					continue;
				
				do {
					uint16_t id = cVariant(n->data()).toUInt16();
					if ( ! id )
					{
						LogWarning("Incomplete node in weapons.xml, ignoring");
						continue;
					}
				
					weaponinfo[id] = type;
				} while ( (p = p->next()) );
				
			} catch ( MXML::NotFoundError e ) {
				LogWarning("Incomplete node in weapons.xml, ignoring");
				continue;
			}
		} while((n = n->next()));
		
		outPlain("[   OK   ]\n");
	} catch ( MXML::MalformedError e) {
		outPlain("[ Failed ]\n");
		LogCritical("weapons.xml file not well formed.");
	}
}
