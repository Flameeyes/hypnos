/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __SKILLS_H__
#define __SKILLS_H__

#include "common_libs.h"
#include "enums.h"

/*!
\brief Skill related stuff
*/
namespace nSkills {
	//! Advance level of a skills
	struct sSkillAdvance {
		uint16_t base;		//!< Maximum level of the skill
		uint16_t success;	//!< Advance if success
		uint16_t failure;	//!< Advance if failure
	};
	
	//! Informations about the skills
	struct sSkillInfo
	{
		static const uint8_t flagUnhideOnUse	= 0x01; //!< Unhide after a good use of the skill
		static const uint8_t flagUnhideOnFail	= 0x02; //!< Unhide after a failure of the skill
		
		uint16_t str;			//!< Strength component of the skill
		uint16_t dex;			//!< Dexterity component of the skill
		uint16_t in_;			//!< Intelligence component of the skill
		uint8_t flags;			//!< Flags of the skill
		
		vector<sSkillAdvance> advances;
						//!< Advance levels
		
		const char *madeword;		//!< Word for items/char prepared with that skill
		
		sSkillInfo();
	};

	extern sSkillInfo infos[skTrueSkills];

	inline void loadSkillVars();
	void loadskills();
	
	//@{
	/*!
	\name General Skill stuff
	*/
	bool AdvanceSkill(pChar pc, int sk, char skillused);
	void AdvanceStats(pChar pc, int sk);
	void SkillUse(pClient client, int x);
	void updateSkillLevel(pChar pc, int s);
	//@}


	//@{
	/*!
	\name Hiding/Stealth stuff
	*/
	void Hide(pClient client);
	void Stealth(pClient client);
	//@}

	void Meditation(pClient client);

	//@{
	/*!
	\name Blacksmithing stuff
	*/
	int CalcRank(pClient client,int skill); // by Magius(CHE)
	void ApplyRank(pClient client,int c,int rank); // by Magius(CHE)
	int GetSubIngotAmt(int p, char id1, char id2, char color1, char color2);
	int DeleSubIngot(int p, int id1, int id2, int color1, int color2, int amount);
	void target_repair( pClient client, pTarget t );
	void target_smith( pClient client, pTarget t );
	//@}

	//@{
	/*!
	\name ID-Stuff
	*/
	void target_itemId( pClient client, pTarget t );
	//@}

	//@{
	/*!
	\name Tinkering stuff
	*/
	void target_tinkerAxel( pClient client, pTarget t );
	void target_tinkerAwg( pClient client, pTarget t );
	void target_tinkerClock( pClient client, pTarget t );
	//@}

	//@{
	/*!
	\name Cooking stuff
	*/
	void target_cookOnFire( pClient client, pTarget t );
	//@}

	//@{
	/*!
	\name Tailoring stuff
	*/
	void target_tailoring( pClient client, pTarget t );
	void target_wheel( pClient client, pTarget t );
	void target_loom( pClient client, pTarget t );
	//@}

	//@{
	/*!
	\name Bowcrafting stuff
	*/
	void target_fletching( pClient client, pTarget t );
	void target_bowcraft( pClient client, pTarget t );
	//@}

	void target_removeTraps( pClient client, pTarget t );
	
	void target_carpentry( pClient client, pTarget t );

	void MakeMenu( pChar pc, int m, int skill, pItem first, pItem second = NULL );
	void MakeMenu( pChar pc, int m, int skill, uint16_t firstId=0, uint16_t firstColor=0, uint16_t secondId=0, uint16_t secondColor=0 );

	//@{
	/*!
	\name Mining stuff
	*/
	void target_mine( pClient client, pTarget t );
	void GraveDig(pClient client);
	//@}

	void target_smeltOre( pClient client, pTarget t );
	void target_tree( pClient client, pTarget t );

	void target_detectHidden( pClient client, pTarget t );

	//@{
	/*!
	\name Healing stuff
	*/
	void target_healingSkill( pClient client, pTarget t );
	//@}

	void SpiritSpeak(pClient client);
	
	void target_armsLore( pClient client, pTarget t );
	void target_tame( pClient client, pTarget t );

	//@{
	/*!
	\name Thievery stuff
	*/
	void target_randomSteal( pClient client, pTarget t );
	void target_stealing( pClient client, pTarget t );
	void PickPocketTarget(pClient client);
	void target_lockpick( pClient client, pTarget t );
	//@}

	void target_animalLore( pClient client, pTarget t );
	
	void target_forensics( pClient client, pTarget t );
	
	void target_poisoning( pClient client, pTarget t );

	void TDummy(pClient client);
	
	void target_tinkering( pClient client, pTarget t );
	
	void AButte(pClient client1, pItem pButte);

	void Persecute(pClient client); //!< AntiChrist persecute stuff

	//@{
	/*!
	\name Cartography Stuff
	\todo Move all this inside cMap class
	*/
	void Cartography(pClient client);
	pMap getEmptyMap(pChar pc);
	//@}

	int GetAntiMagicalArmorDefence(pChar pc); //!< blackwind meditation armor stuff
};
void snooping( pChar snooper, pItem cont );

#endif
