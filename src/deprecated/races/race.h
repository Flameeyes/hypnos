  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#ifndef __race__
#define __race__

#include "common_libs.h"
#include "scp_parser.h"

#include "raceability.h"
#include "racegender.h"
#include "racerequiredtype.h"
#include "raceskill.h"
#include "racestartitem.h"
#include "racetype.h"

//#include "cPoison.h"

typedef enum { ENEMY, NEUTRAL, FRIEND } RACIALRELATION;

typedef enum { NOBEARD,	SHORTBEARD, LONGBEARD, GOATEE, MOUSTACHE, SHORTBEARDWITHMOUSTACHE, LONGBEARDWITHMOUSTACHE, VANDYKE } BEARDSTYLE;

typedef enum { NOPOISON, LESSERPOISON, REGULARPOISON, GREATERPOISON, DEADLYPOISON, LETHALPOISON, POISONSTRENGHTS = 6 } POISONSTRENGTH;

class Race
{
	private:
		//
		//	private static data
		//
		static	std::map<uint32_t, class Race*>
					raceMap;
		static	cScpScript* 	script;
		static	bool		activeRaceSystem;

	private:
		//
		//	private static methods
		//
		static	void		enlistDialog1( const NXWSOCKET socket );	// Main enlist menu
		static	void		dialogRaceInfo( const NXWSOCKET socket );		// 	Information about races
		static	void		dialogNotImplemented( const NXWSOCKET socket );	// 		General race information
		static	void		dialogPlayerRaceInfo( const NXWSOCKET socket );	//		Player race list
		static	void		dialogNonPlayerRaceInfo( const NXWSOCKET socket );	//		Non player race list
		static	void		dialogAnyRaceInfo( const NXWSOCKET socket );	//		Any race list
		static	void		dialogChoosePlayerRace( const NXWSOCKET socket );	//	Select race
		static	void		enlistDialog121( const NXWSOCKET socket );	//		Race parameter dialog

		static	void		showRaceDescription( const NXWSOCKET socket, uint32_t raceId );
//		static	void		showRaceDescription( const NXWSOCKET socket, const uint32_t raceId, short int gumpId, bool canClose, bool canMove, bool withOk );
		static	void		showRaceDescription( const NXWSOCKET socket, const uint32_t raceId, BYTE gumpId, bool canClose, bool canMove, bool withOk );
//		static	void		showRaceNameList( const NXWSOCKET socket, const RACETYPE raceType, short int gumpId, bool canClose, bool canMove, bool withOk ); // race list pc | npc | both
		static	void		showRaceNameList( const NXWSOCKET socket, const RACETYPE raceType, BYTE gumpId, bool canClose, bool canMove, bool withOk ); // race list pc | npc | both

		static	void		parseGlobalSection( void );
		static	void		parseRaceSection( void );
		static	void		scriptConOut( void );

	public:

		static	bool		teleportOnEnlist;
		static	bool		withWebInterface;
		static	string		globalWebRoot;
		static	string		globalWebLink;
		static	short		startLocation[3];
		
		//
		//	public static methods
		//
		static	void		load( const char* scriptName );
		static	void		reload( const char* scriptName );
		static	void		parse( void );

		static	void		enlist( const NXWSOCKET socket );
		//static	void		resurrect( const P_CHAR pc );

		static	bool		isRaceSystemActive( void );
		static	const 	string*	getName( const uint32_t raceId );
		static	const 	string*	getPluralName( const uint32_t raceId );
		static	void		setName( const uint32_t raceId, const char *newName )	;

		static	float			getPoisonResistance( const uint32_t raceId, PoisonType poisonStrength )	;
		static	bool		isPlayerRace( const uint32_t raceId );
		static	RACIALRELATION	getRacialRelation( uint32_t raceId1, uint32_t raceId2 );
		static	Race*		getRace( uint32_t raceId );
		static	RequiredType	getBeardPerm( uint32_t raceId );
		static	RequiredType	getHairPerm( uint32_t raceId );

		static	bool		isProtectedLayer( uint32_t raceId, unsigned char layer );

		//static	uint32_t		getSkillCap( uint32_t raceId );
		//static	uint32_t		setSkillCap( uint32_t raceId, newSkillCap );

	private:
			//
			// race id
			//
			uint32_t				id;
			//
			// race name
			//
			string				pluralName;
			//
			// race mode
			//
			bool				activeRace;
			//
			// pc or npc race
			//
			//
			// males or females only or mixed
			//
			Gender				gender;
			//
			// -- race description
			//
			//
			// -- appearance
			//
			//
			// -- abilities
			//
			uint32_t				statCap;

			uint32_t				strStart;
			uint32_t				strCap;
			RaceAbilityModifiers		strModifiers;

			uint32_t				dexStart;
			uint32_t				dexCap;
			RaceAbilityModifiers		dexModifiers;

			uint32_t				intStart;
			uint32_t				intCap;
			RaceAbilityModifiers		intModifiers;
			//
			// -- resistances
			//
			bool				magicResistant;
			float				poisonResistance[6];
			//
			// -- relations
			//
			std::map<uint32_t, RACIALRELATION>	m_mapRacialRelations;
			//
			// -- items gained when pc becomes race member
			//
			vector< class RaceStartItem >   startItems;
			//
			// -- skills
			//
			uint32_t				skillCap;
			RaceSkills			skills;
			//
			// -- miscelaneous
			//
			uint32_t				tileId;
			uint32_t				font;
			uint32_t				say;
			uint32_t				emote;

	private:
			void				parseAbilityModifiers( const RACIALABILITY ability, const string& sectionName );
			void				parseBeardColor( const string& sectionName );
			void				parseHairColor( const string& sectionName );
			void				parsePoisonResistance( const string& sectionName );
			void				parseRaceDescription( const string& sectionName );
			void				parseSkinColor( const string& sectionName );
			void 				parseSkill( const string& sectionName );
			void 				parseStartItem( const string& itemReference );
			void 				parseStartItemDetails( const string& itemReference );

	public:
							Race( void );
			virtual				~Race( void );

			bool				getCanUseSkill( uint32_t skillId );
			uint32_t				getDexCap( void );
			int				getDexModifier( uint32_t baseDex );
			uint32_t				getDexStart( void );
			uint32_t				getIntCap( void );
			int				getIntModifier( uint32_t baseDex );
			uint32_t				getIntStart( void );
			const string*			getPluralName( void );
			RACIALRELATION			getRacialRelation( uint32_t raceId );
			uint32_t				getSkillAdvanceDexterity( uint32_t skillId );
			uint32_t				getSkillAdvanceIntelligence( uint32_t skillId );
			uint32_t				getSkillAdvanceStrength( uint32_t skillId );
			uint32_t				getSkillAdvanceSuccess( uint32_t skillId, uint32_t baseSkill );
			uint32_t				getSkillAdvanceFailure( uint32_t skillId, uint32_t baseSkill );
			uint32_t				getSkillCap( void );
			uint32_t				getStatCap( void );
			uint32_t				getStrCap( void );
			int				getStrModifier( uint32_t baseDex );
			uint32_t				getStrStart( void );

			bool				isPlayerRace( void );
			bool				isProtectedLayer( unsigned char layer );
			bool				isRaceActive( void );

			void				setRacialRelation( uint32_t raceId, RACIALRELATION relation );
			void				setSkillCap( uint32_t newSkillCap );
			void				show( void );

			vector< uint32_t >			skinColor;
			string				webLink;
			std::vector< string >		description;
			RaceType			raceType;
			uint32_t				skin;
			//uint32_t		skinColor;
			RequiredType			hairPerm;
			uint32_t				hairStyle;
			vector< uint32_t>			hairColor;
			RequiredType			beardPerm;
			vector< BEARDSTYLE >		beardStyles;
			vector< uint32_t >			beardColor;
			string				name;
};



#endif
