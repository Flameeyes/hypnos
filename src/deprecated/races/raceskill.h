  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#ifndef __raceskill__
#define __raceskill__

#include "nxwcommn.h"
#include "scp_parser.h"

#include "racescriptentry.h"

class RaceSkillAdvancePoint
{
	private:
		uint32_t			base;
		uint32_t			success;
		uint32_t			failure;
	public:
		RaceSkillAdvancePoint( void );
		uint32_t			getBase( void );
		uint32_t			getSuccess( void );
		uint32_t			getFailure( void );
		RaceSkillAdvancePoint& 	operator=( const string& );
};

class RaceSkillAdvancePoints
{
	private:
		uint32_t	strength,
			dexterity,
			intelligence;
		bool	unhideOnUse;	// also in RaceSkill, but here so scripts/skill.xss can be used
		map< uint32_t, RaceSkillAdvancePoint > skillAdvancePoints; // Hmmm this could just be a vector
		RaceSkillAdvancePoint dummy;
	public:
		RaceSkillAdvancePoints( void );
		uint32_t	getStrength( void );
		uint32_t	getDexterity( void );
		uint32_t	getIntelligence( void );
		bool	getUnhideOnUse( void );
		void	show( void );
		RaceSkillAdvancePoint& getSkillAdvancePoint( uint32_t baseSkill );
		RaceSkillAdvancePoints& operator+=( RaceSkillAdvancePoint& rsap );
		RaceSkillAdvancePoints&	operator=( RaceScriptEntry& rse );
};


class RaceSkillModifier
{
	private:
		uint32_t			base;
		R32				modifier;
	public:
		RaceSkillModifier( void );
		uint32_t			getBase( void );
		R32				getModifier( void );
		RaceSkillModifier&	operator=( const string& );
};

class RaceSkillModifiers
{
	private:
		map< uint32_t, RaceSkillModifier > skillModifiers;
	public:
		RaceSkillModifiers( void );
		RaceSkillModifiers&	operator+=( RaceSkillModifier& rsm );
		RaceSkillModifiers&	operator=( RaceScriptEntry& rse );
};


class RaceSkill
{
	private:
		static string		names[];
	private:
		uint32_t			id;
		bool			canUseSkill;
		bool			canUseModifier;
		bool			canUseAdvance;
		bool			unhideOnUse;
		uint32_t			startValue;
		RaceSkillModifiers	modifier;
		RaceSkillAdvancePoints	advance;
	public:
		RaceSkill( void );
		uint32_t			getId( void );
		const string&		getName( void );
		bool			getCanUseSkill( void );
		bool			getCanUseModifier( void );
		bool			getCanUseAdvance( void );
		bool			getUnhideOnUse( void );
		uint32_t			getStartValue( void );
		void			show( void );
		RaceSkillAdvancePoint&	getAdvance( uint32_t baseSkill );
		uint32_t			getAdvanceStrength( void );
		uint32_t			getAdvanceDexterity( void );
		uint32_t			getAdvanceIntelligence( void );
		RaceSkill&		operator=( RaceScriptEntry& rse );
};

class RaceSkills
{
	private:
		map< uint32_t, RaceSkill > 	skills;
		RaceSkill								dummy;
	public:
									RaceSkills( void );
		void					show( void );
		RaceSkill&		getSkill( uint32_t skillId );
		RaceSkills&		operator+=( RaceSkill& );
		RaceSkills&		operator+=( RaceScriptEntry& );
};


#endif
