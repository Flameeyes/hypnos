  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include <stdlib.h>

#include "common_libs.h"
#include "network.h"
#include "sndpkg.h"
#include "race.h"
#include "racescriptentry.h"
#include "raceskill.h"
#include "racetokenizer.h"
#include "inlines.h"
#include "basics.h"

//
//	STATIC DATA
//

bool	Race::activeRaceSystem	= false;
std::string	Race::globalWebRoot;
std::string  Race::globalWebLink;
short	Race::startLocation[3]	= { 0, 0, 0 };
bool	Race::teleportOnEnlist	= false;
bool	Race::withWebInterface	= false;
std::map<uint32_t, class Race*> Race::raceMap;

//
//	STATIC FUNCTIONS
//

Race* Race::getRace( uint32_t raceId )
{
	std::map<uint32_t, class Race*>::iterator raceIter = raceMap.find( raceId );
	if ( raceIter != raceMap.end() )
		return raceIter->second;
	else
		return NULL;
}


RequiredType Race::getBeardPerm( uint32_t raceId )
{
	return raceMap[raceId]->beardPerm;
}

RequiredType Race::getHairPerm( uint32_t raceId )
{
	return raceMap[raceId]->hairPerm;
}

bool Race::isRaceSystemActive( void )
{
	return activeRaceSystem;
}

bool Race::isPlayerRace( const uint32_t raceId )
{
	return raceMap[raceId]->isPlayerRace();
}

void Race::load( const char* scriptFileName )
{
	script = new cScpScript( const_cast<char *>(scriptFileName) );
}

void Race::parse( void )
{
	if ( !script )
	{
		// oops...we need to switch the race system off and create default race ourselves
		activeRaceSystem = false;
		return;
	}

	parseGlobalSection();
	parseRaceSection();

//DEBUG
//	map<uint32_t, class Race*>::iterator it(raceMap.begin()), end(raceMap.end());
//	for(;it!=end;++it)
//		it->second->show();
//	SDbgOut("Finished race parsing\n");
	//scriptConOut();
	safedelete(script);
}

void Race::parseGlobalSection( void )
{
	std::string	sectionName = "SECTION GLOBAL";
	uint32_t globalSectionCount = script->countSectionInStr( sectionName );

	if ( globalSectionCount != 1 )
	{
		if ( globalSectionCount == 0 )
			ErrOut("Race::parseSectionGlobal() no global section found!\n");
		else
			ErrOut("Race::parseSectionGlobal() multiple global sections found!\n");
		activeRaceSystem = false;
		return;
	}

	uint32_t		/*i,*/
			loopexit;
	cScpIterator*	iter = NULL;
	string		lha, rha;

	iter = script->getNewIterator( sectionName );
	if ( iter == NULL )
	{
		// OOPS we've got a major problem here! ABORT ??? -> TODO
		ErrOut("Race::parseSectionGlobal() no global section found!\n");
		return ;
	}
	loopexit=0;
	do
	{
		iter->parseLine( lha, rha );
		switch( lha[0] )
		{
			case '{':
			case '}':
				break;
			case '@':
				break;
			case 'M':
				if ( lha == "MODE" )
				{
					if ( rha == "ON" )
						activeRaceSystem = true;
					else if ( rha == "OFF" )
						activeRaceSystem = false;
					else
					{
						WarnOut("%s invalid %s %s\n", sectionName.c_str(), lha.c_str(), rha.c_str() );
						activeRaceSystem = false;
					}
				}
				break;
			case 'S':
				if ( lha == "STARTLOCATION" )
				{
					// quick and dirty for now
					// this will be fixed using a special class location
					uint32_t x, y, z;
					sscanf( rha.c_str(), "%u %u %u", &x, &y, &z);
					startLocation[0] = x;
					startLocation[1] = y;
					startLocation[2] = z;
				}
				break;
			case 'T':
				if ( lha == "TELEPORTONENLIST"  )
				{
					if ( rha == "ON" )
						teleportOnEnlist = true;
					else if ( rha == "OFF" )
						teleportOnEnlist = false;
					else
					{
						WarnOut("%s invalid %s %s\n", sectionName.c_str(), lha.c_str(), rha.c_str() );
						teleportOnEnlist = false;
					}
				}
				break;
			case 'W':
				if ( lha == "WEBINTERFACE" )
				{
					if ( rha == "ON" )
						withWebInterface = true;
					else if ( rha == "OFF" )
						withWebInterface = false;
					else
					{
						WarnOut("%s invalid %s %s\n", sectionName.c_str(), lha.c_str(), rha.c_str() );
						withWebInterface = false;
					}
				}
				else if ( rha == "WEBLINK" )
				{
					globalWebLink = rha;
				}
				else if ( lha == "WEBROOT" )
				{
					globalWebRoot = rha;
				}
				break;
			default:
				WarnOut("%s invalid %s %s\n", sectionName.c_str(), lha.c_str(), rha.c_str() );
				break;
		}
	} while ( ( lha[0] != '}' ) && ( ++loopexit < MAXLOOPS ) );
}

void Race::parseRaceSection( void )
{
	std::string	sectionName = "SECTION RACE ";
	uint32_t raceCount = script->countSectionInStr( sectionName );

	if ( raceCount <= 0 )
	{
		// Problem. Race.xss containes no RACE sections
		// We need to handle this. Let's create a dummy race 0 and route all
		// requests to it or just switch the race system off. TODO
		ErrOut("Race::loadRaces() no races\n");
		activeRaceSystem = false;
		return;
	}

	uint32_t		/*i,*/
//			raceId,
			loopexit,
			descriptionIndex;
	cScpIterator*	iter = NULL;
	std::string*	sectionParms;
	std::string	section, lha, rha;

	sectionParms = script->getFirstSectionParmsInStr( sectionName );

	do
	{
		section = sectionName + *sectionParms;
    iter = script->getNewIterator( section );
    if ( iter == NULL )
		{
			ErrOut( "Race::loadRaces() no %s\n", section.c_str() );
			return ;
		}
		Race* race = new Race();
		race->id = str2num( *sectionParms );
		loopexit=0;
		descriptionIndex = 0;
		do
		{
			iter->parseLine( lha, rha );
			switch( lha[0] )
			{
				case '{':
				case '}':
					break;
				case '@':
					break;
				case 'A':
					if 			( lha == "ALCHEMY" ||
					     			lha == "skAnatomy" ||
					     			lha == "skAnimalLore" ||
					     			lha == "skArmsLore" ||
					     			lha == "ARCHERY")						race->parseSkill( rha );
					break;
				case 'B':
					if 			( lha == "BEARD" )						race->beardPerm = rha;
					else if ( lha == "BEARDCOLOR" )				race->parseBeardColor( rha );
					else if ( lha == "skBegging" ||
						  			lha == "skBlacksmithing" ||
						  			lha == "skBowcraftING")				race->parseSkill( rha );
					break;
				case 'C':
					if 			( lha == "skCamping" ||
					     			lha == "" ||
					     			lha == "skCartography" ||
					     			lha == "skCooking")						race->parseSkill( rha );
				case 'D':
					if 			( lha == "DESCRIPTION" )			race->parseRaceDescription( rha );
					else if ( lha == "skDetectingHidden" )			race->parseSkill( rha );
					else if ( lha == "DEXCAP" ) 					race->dexCap = str2num( rha );
					else if	( lha == "DEXMOD" )						race->parseAbilityModifiers( DEXTERITY, rha );
					else if	( lha == "DEXSTART" )					race->dexStart = str2num( rha );
					break;
				case 'E':
					if 			( lha == "ENEMY" )						race->m_mapRacialRelations[ (uint32_t) str2num( rha ) ] = ENEMY;
					else if ( lha == "skEnticement" ||
						  			lha == "EVALUATEINTELLECT")	race->parseSkill( rha );
					break;
				case 'F':
					if 			( lha == "FENCING" ||
					     			lha == "FISHING" ||
					     			lha == "skForensics")					race->parseSkill( rha );
					else if ( lha == "FRIEND" ) 					race->m_mapRacialRelations[ (uint32_t) str2num( rha ) ] = FRIEND;
					break;
				case 'G':
					if 			( lha == "GENDER" ) 					race->gender = rha;
					break;
				case 'H':
					if 			( lha == "HAIR" )							race->hairPerm = rha;
					else if ( lha == "HAIRCOLOR" )				race->parseHairColor( rha );
					else if ( lha == "HEALING" ||
						  			lha == "skHerding" ||
						  			lha == "HIDING")						race->parseSkill( rha );
					break;
				case 'I':
					if 			( lha == "skInscription" )			race->parseSkill( rha );
					else if ( lha == "INTCAP" ) 					race->intCap = str2num( rha );
					else if	( lha == "INTMOD" )						race->parseAbilityModifiers( INTELLIGENCE, rha );
					else if	( lha == "INTSTART" )					race->intStart = str2num( rha );
					else if ( lha == "ITEMID" )						race->parseSkill( rha );
					break;
				case 'L':
					if 			( lha == "skLockPicking" ||
					     			lha == "LUMBERJACKING")			race->parseSkill( rha );
				case 'M':
					if 			( lha == "MACEFIGHTING" ||
					     			lha == "skMagery" ||
					     			lha == "skMagicResistance")		race->parseSkill( rha );
					else if ( lha == "MAGICRESISTANT" )		race->magicResistant = true; // may need to change this to %
					else if ( lha == "MEDITATION" ||
						  			lha == "MINING")						race->parseSkill( rha );
					else if ( lha == "MODE" )
						if ( rha == "ON" )									race->activeRace = true;
						else if( rha == "OFF" )							race->activeRace = false;
						else
						{
							WarnOut("%s invalid %s %s\n", sectionName.c_str(), lha.c_str(), rha.c_str() );
							race->activeRace = false;
						}
					else if ( lha == "skMusicianship" )			race->parseSkill( rha );
					break;
				case 'N':
					if 			( lha == "NAME" ) 						race->name = rha;
					else if ( lha == "NEUTRAL" )					race->m_mapRacialRelations[ (uint32_t) str2num( rha ) ] = NEUTRAL;
					break;
				case 'P':
					if 			( lha == "PLURALNAME" ) 			race->pluralName = rha;
					else if ( lha == "POISONING" )				race->parseSkill( rha );
					else if ( lha == "POISONRESISTANCE" ) race->parsePoisonResistance( rha );
					else if ( lha == "skProvocation" )			race->parseSkill( rha );
					break;
				case 'R':
					if 			( lha == "RACETYPE" ) 				race->raceType = rha;
					else if ( lha == "REMOVETRAP" )				race->parseSkill( rha );
					break;
				case 'S':
					if 			( lha == "SKILLCAP" )					race->skillCap = str2num( rha );
					else if ( lha == "SKINCOLOR" ) 				race->parseSkinColor( rha );
					else if ( lha == "skSnooping" ||
						  			lha == "SPIRITSPEAK")				race->parseSkill( rha );
					else if ( lha == "STARTITEM" ) 				race->parseStartItem( rha );
					else if ( lha == "STATCAP" ) 					race->statCap = str2num( rha );
					else if ( lha == "STEALING" ||
						  			lha == "STEALTH")						race->parseSkill( rha );
					else if ( lha == "STRCAP" ) 					race->strCap = str2num( rha );
					else if	( lha == "STRMOD" )						race->parseAbilityModifiers( STRENGTH, rha );
					else if	( lha == "STRSTART" )					race->strStart = str2num( rha );
					else if ( lha == "SWORDMANSHIP" )			race->parseSkill( rha );
					break;
				case 'T':
					if 			( lha == "TAILORING" ||
					     			lha == "TAMING" ||
					     			lha == "TASTEID" ||
					     			lha == "skTactics")						race->parseSkill( rha );
					else if ( lha == "TILEID" ) 					race->tileId = str2num( rha );
					else if ( lha == "TINKERING" ||
						  			lha == "TRACKING")					race->parseSkill( rha );
					break;
				case 'V':
					if 			( lha == "VETERINARY" )				race->parseSkill( rha );
					break;
				case 'W':
					if 			( lha == "WEBLINK" ) 					race->webLink = rha;
					else if ( lha == "WRESTLING" )				race->parseSkill( rha );
					break;
				default	:
					WarnOut("Race::parseRaceSection unknown tag %s\n", lha.c_str());
					break;
			}
		} while ( lha[0] != '}' && ++loopexit < MAXLOOPS );
		raceMap[race->id] = race;
		iter = NULL;
		sectionParms = script->getNextSectionParmsInStr( sectionName );
	} while ( --raceCount );
}

void Race::parseRaceDescription( const string& sectionName )
{
	uint32_t		loopexit,
			descriptionIndex;
	cScpIterator*	iter = NULL;
	string		section("SECTION DESCRIPTION ");
	section += sectionName;

	iter = script->getNewIterator( section );
    	if ( iter == NULL )
	{
		ErrOut( "Race::parseRaceDescription() no %s\n", section.c_str() );
	}
	else
	{
		loopexit=0;
		descriptionIndex = 0;
		string description;
		do
		{
			description = iter->getEntry()->getFullLine();
			if( description[0] != '{' && description[0] != '}' )
				this->description.push_back( description );
		} while ( ( description[0] != '}' ) && ( ++loopexit < MAXLOOPS ) );
	}
}

void Race::parsePoisonResistance( const string& sectionName )
{
	uint32_t		loopexit;
	cScpIterator*	iter = NULL;
	string		section("SECTION POISONRESISTANCE ");
	section += sectionName;

	iter = script->getNewIterator( section );
    	if ( iter == NULL )
	{
		ErrOut( "Race::parsePoisonResistance() no %s\n", section.c_str() );
	}
	else
	{
//		float 	pr;
		string	lha, rha;

		loopexit=0;
		do
		{
			iter->parseLine( lha, rha );
			if ( lha[0]!='}' && lha[0]!='{' )
			{
				switch ( lha[0] )
				{
					case 'D':
						if ( lha == "DEADLY" )
							sscanf( rha.c_str(), "%f", &this->poisonResistance[DEADLYPOISON] );
						break;
					case 'G':
						if ( lha == "GREATER" )
							sscanf( rha.c_str(), "%f", &this->poisonResistance[GREATERPOISON] );
						break;
					case 'L':
						if ( lha == "LETHAL" )
							sscanf( rha.c_str(), "%f", &this->poisonResistance[LETHALPOISON] );
						break;
					case 'N':
						if ( lha == "NORMAL" )
							sscanf( rha.c_str(), "%f", &this->poisonResistance[REGULARPOISON] );
						break;
					case 'W':
						if ( lha == "WEAK" )
							sscanf( rha.c_str(), "%f", &this->poisonResistance[LESSERPOISON]  );
						break;
					default:
						WarnOut("Race::parseRaceSection unknown tag %s\n", lha.c_str());
						break;
				}
			}
		} while ( ( lha[0] != '}' ) && ( ++loopexit < MAXLOOPS ) );
	}
}

void Race::parseSkill( const string& sectionName )
{
	string section("SECTION RACESKILL ");
	section += sectionName;
	RaceScriptEntry	rse( script, section );
	this->skills += rse;
}

void Race::parseAbilityModifiers( const RACIALABILITY ability, const string& sectionName )
{
	string section("SECTION RACEABILITYMODIFIER ");
	section += sectionName;
	RaceScriptEntry	rse( script, section );
	switch( ability )
	{
		case STRENGTH 		:	this->strModifiers = rse; break;
		case DEXTERITY		: this->dexModifiers = rse; break;
		case INTELLIGENCE : this->intModifiers = rse; break;
	}
}

void Race::parseBeardColor( const string& sectionName )
{
	cScpIterator*	iter = NULL;
	string		section("SECTION BEARDCOLOR ");
  section += sectionName;

	iter = script->getNewIterator( section );
    	if ( iter == NULL )
	{
		ErrOut( "Race::parseBeardColor() no %s\n", section.c_str() );
	}
	else
	{
		uint32_t loopexit = 0;
		string color;
		this->beardColor.clear();
		do
		{
			color = iter->getEntry()->getFullLine();
			if( color[0] != '{' && color[0] != '}' )
				this->beardColor.push_back( hex2num(color) );
		} while ( ( color[0] != '}' ) && ( ++loopexit < MAXLOOPS ) );
	}
}

void Race::parseHairColor( const string& sectionName )
{
	cScpIterator*	iter = NULL;
	string		section("SECTION HAIRCOLOR ");
	section += sectionName;

	iter = script->getNewIterator( section );
    	if ( iter == NULL )
	{
		ErrOut( "Race::parseHairColor() no %s\n", section.c_str() );
	}
	else
	{
		uint32_t loopexit = 0;
		string color;
		this->hairColor.clear();
		do
		{
			color = iter->getEntry()->getFullLine();
			switch( color[0] )
			{
				case '{' :
				case '}' :
					break;
				default:
					this->hairColor.push_back( hex2num(color) );
			}
		} while ( ( color[0] != '}' ) && ( ++loopexit < MAXLOOPS ) );
	}
}

void Race::parseSkinColor( const string& sectionName )
{
	cScpIterator*	iter = NULL;
	string		section("SECTION SKINCOLOR ");
	section += sectionName;

	iter = script->getNewIterator( section );
    	if ( iter == NULL )
	{
		ErrOut( "Race::parseSkinColor() no %s\n", section.c_str() );
	}
	else
	{
		uint32_t loopexit = 0;
		string color;
		this->skinColor.clear();
		do
		{
			color = iter->getEntry()->getFullLine();
			if( color[0] != '{' && color[0] != '}' )
				this->skinColor.push_back( hex2num(color) );
		} while ( ( color[0] != '}' ) && ( ++loopexit < MAXLOOPS ) );
	}
}

void Race::parseStartItem( const string& itemReference )
{
	//
	// Clear startItems vector as multiple startitem tags may be available (e.g. by using #copy)
	//
	this->startItems.clear();

	string 		section("SECTION STARTITEM ");
	section += itemReference;
	cScpIterator* 	iter    = NULL;
	iter = script->getNewIterator( section );
	if ( iter )
	{
		int 	loopexit = 0;
		string	itemId;
		do
		{
			itemId = iter->getEntry()->getFullLine();
			if( itemId[0] != '{' && itemId[0] != '}' )
				this->parseStartItemDetails( itemId );
		}
		while (( itemId[0] != '}') && (++loopexit < MAXLOOPS) );
	}
	else
		WarnOut( "Race::parseStartItem() unknown %s\n", section.c_str() );
}

void Race::parseStartItemDetails( const string& itemReference )
{
	string section("SECTION ITEM ");
	section += itemReference;
	cScpIterator* iter = NULL;
	iter = Scripts::Items->getNewIterator(section);
	if ( iter )
	{
		int 		loopexit = 0;
		string		lha, rha;
		RaceStartItem	raceItem;

		raceItem.itemReference = str2num( const_cast<char*>(itemReference.c_str()) );
		do
		{
			iter->parseLine( lha, rha );
			if ( lha[0]!='}' && lha[0]!='{' )
			{
				switch ( lha[0] )
				{
					case 'G':
						if ( lha == "GENDER" )
							raceItem.gender = rha.c_str() ;
						break;
					case 'L':
						if ( lha == "LAYER"  )
							raceItem.layer = str2num( rha );
						break;
					case 'M':
						if ( lha == "MOVABLE" && str2num( rha ) == 2 )
							raceItem.protectedItem = true;
					case 'S':
						if ( lha == "SKINCOLOR" )
							raceItem.skinColor = true;
						break;
				}
			}
		}
		while ( lha[0] != '}' && ++loopexit < MAXLOOPS );

		if( !raceItem.layer && raceItem.protectedItem )
			raceItem.protectedItem = false; // no protection on layer 0

		//this->startItems.push_back( raceItem );
	}
	else
		ErrOut( "Race::parseStartItemDetails() unknown %s\n", itemReference.c_str() );
	safedelete( iter );
}

void Race::scriptConOut( void )
{
	uint32_t raceCount = script->countSectionInStr("SECTION");

	if ( raceCount <= 0 )
	{
		// Problem. Race.xss containes no RACE sections
		// We need to handle this. Let's create a dummy race 0 and route all
		// requests to it or just switch the race system off. TODO
		ErrOut("Race::scriptConOut() no sections\n");
		return;
	}

	uint32_t		/*i,*/
			loopexit;
	cScpIterator*	iter = NULL;
	char 		sect[512], script1[512], script2[512];
	std::string*	sectionParms;
	std::string	sectionName("SECTION");

	sectionParms = script->getFirstSectionParmsInStr(sectionName);

	do
	{
		sprintf( sect, "%s%s", sectionName.c_str(), const_cast<char *>(sectionParms->c_str()) );
    iter = script->getNewIterator( sect );
    if ( iter == NULL )
		{
			// OOPS we've got a major problem here! ABORT ??? -> TODO
			ErrOut( "Race::scriptConOut() no %s %s\n", sectionName.c_str(), sectionParms->c_str());
			return ;
		}
		loopexit=0;
		do
		{
			iter->parseLine(script1, script2);
		} while ( ( script1[0] != '}' ) && ( ++loopexit < MAXLOOPS ) );
		safedelete(iter);
		sectionParms = script->getNextSectionParmsInStr(sectionName);
	} while ( --raceCount );
}

void Race::reload( const char* fileName )
{
	// delete all RaceInstances
	// as there are no pointer references in cChar but only an numerical index no update of pc's or npc's is necessary
	//
	map<uint32_t, class Race*>::iterator raceIter( raceMap.begin() ), raceEnd( raceMap.end() );

	for(; raceIter != raceEnd; ++raceIter )	safedelete(raceIter->second);
	load( fileName );
}



const std::string* Race::getName( const uint32_t raceId )
{
	return &raceMap[raceId]->name;
}

const std::string* Race::getPluralName( const uint32_t raceId )
{
	return raceMap[raceId]->getPluralName();
}

const std::string* Race::getPluralName( void )
{
	return &pluralName;
}

void Race::setName( const uint32_t raceId, const char *newName )
{
//		raceMap[raceId]->Name = newName;
}

float Race::getPoisonResistance( const uint32_t raceId, PoisonType poisonStrength )
{
	float pr = 0.0;
	if ( isRaceSystemActive() )
	{
		std::map<uint32_t, class Race*>::iterator raceIter = raceMap.find( raceId );
		if ( raceIter != raceMap.end() )
		{
			if( raceIter->second->isRaceActive() )
				pr = raceMap[raceId]->poisonResistance[poisonStrength];
		}
		else
			WarnOut("Race.cpp getPoisonResistance() called with unknown race %d\n", raceId);
	}
	return pr;
}

Race::Race( void )
{
	activeRace = false;
	skinColor.clear();
	poisonResistance[1] = 0.0;
	poisonResistance[2] = 0.0;
	poisonResistance[3] = 0.0;
	poisonResistance[4] = 0.0;
	poisonResistance[5] = 0.0;
	tileId = 0;
	webLink = "";
	startItems.clear();
	skillCap = SrvParms->skillcap;
}

Race::~Race( void )
{
}

bool Race::isRaceActive( void )
{
	return activeRace;
}

bool Race::isPlayerRace( void )
{
	return ( raceType == PCRACE || raceType == PCNPCRACE );
}

void Race::setRacialRelation( uint32_t raceId, RACIALRELATION relation )
{
	m_mapRacialRelations[ raceId ] = relation;
}

RACIALRELATION Race::getRacialRelation( uint32_t raceId1, uint32_t raceId2 )
{
	std::map<uint32_t, class Race*>::iterator raceIter = raceMap.find( raceId1 );
	if ( raceIter != raceMap.end() )
		return raceIter->second->getRacialRelation( raceId2 );
	else
		return NEUTRAL;
}

RACIALRELATION Race::getRacialRelation( uint32_t raceId )
{
	std::map<uint32_t, RACIALRELATION>::iterator relationIter = m_mapRacialRelations.find( raceId );

	if ( relationIter != m_mapRacialRelations.end() )
		return m_mapRacialRelations[ raceId ];
	else
		return NEUTRAL;
}

bool Race::isProtectedLayer( uint32_t raceId, unsigned char layer )
{
	if( activeRaceSystem && layer )
	{
		Race* race = raceMap.find( raceId )->second;
		if ( race )
			return race->isProtectedLayer( layer );
	}
	return false;
}

bool Race::isProtectedLayer( unsigned char layer )
{
	vector< RaceStartItem >::iterator iter( startItems.begin() ), end( startItems.end() );

	for( ; iter != end; ++iter )
		if( iter->layer == layer && iter->protectedItem )
			return true;

	return false;
}

uint32_t Race::getSkillCap( void )
{
	return this->skillCap;
}

void	Race::setSkillCap( uint32_t newSkillCap )
{
	this->skillCap = newSkillCap;
}

uint32_t Race::getSkillAdvanceSuccess( uint32_t skillId, uint32_t baseSkill )
{
	return this->skills.getSkill( skillId ).getAdvance( baseSkill ).getSuccess();
}

uint32_t Race::getSkillAdvanceFailure( uint32_t skillId, uint32_t baseSkill )
{
	return this->skills.getSkill( skillId ).getAdvance( baseSkill ).getFailure();
}

uint32_t Race::getSkillAdvanceStrength( uint32_t skillId )
{
	return this->skills.getSkill( skillId).getAdvanceStrength();
}

uint32_t Race::getSkillAdvanceDexterity( uint32_t skillId )
{
	return this->skills.getSkill( skillId).getAdvanceDexterity();
}

uint32_t Race::getSkillAdvanceIntelligence( uint32_t skillId )
{
	return this->skills.getSkill( skillId).getAdvanceIntelligence();
}

uint32_t Race::getStatCap( void )
{
	return this->statCap;
}

uint32_t Race::getStrCap( void )
{
	return this->strCap;
}

int Race::getStrModifier( uint32_t baseStr )
{
	return this->strModifiers.getAbilityModifier( baseStr ).getModifier();
}

uint32_t Race::getStrStart( void )
{
	return this->strStart;
}

uint32_t Race::getDexCap( void )
{
	return this->dexCap;
}

int Race::getDexModifier( uint32_t baseDex )
{
	return this->dexModifiers.getAbilityModifier( baseDex ).getModifier();
}

uint32_t Race::getDexStart( void )
{
	return this->dexStart;
}

uint32_t Race::getIntCap( void )
{
	return this->strCap;
}

int Race::getIntModifier( uint32_t baseInt )
{
	return this->intModifiers.getAbilityModifier( baseInt ).getModifier();
}

uint32_t Race::getIntStart( void )
{
	return this->strCap;
}

bool Race::getCanUseSkill( uint32_t skillId )
{
	return this->skills.getSkill( skillId ).getCanUseSkill();
}

void Race::show( void )
{
	SDbgOut("Race %d %s\n", this->id, this->name.c_str() );
	skills.show();
}
