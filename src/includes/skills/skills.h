/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Skill related functions
*/

#ifndef __SKILLS_H__
#define __SKILLS_H__

#include "target.h"

/*!
\brief Skill related stuff
*/
namespace nSkills {
	sSkillInfo infos[skTrueSkills];

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

	//@{
	/*!
	\name Musicianship stuff
	*/
	void PeaceMaking(pClient client);
	void PlayInstrumentWell(pClient client, pItem pi);
	void PlayInstrumentPoor(pClient client, pItem pi);
	pItem GetInstrument(pClient client);
	void target_provocation1( pClient client, pTarget t );
	void target_enticement1( pClient client, pTarget t );
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
	\name Alchemy stuff
	*/
	void CreatePotion(pChar pc, uint8_t type, uint8_t sub, pItem mortar);
	void DoPotion(pClient client, uint8_t type, uint8_t sub, pItem mortar);
	void target_alchemy( pClient client, pTarget t );
	void target_bottle( pClient client, pTarget t );
	void PotionToBottle(pChar pc, pItem mortar);
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
	void MakeMenu( pChar pc, int m, int skill, uint16_t firstId=0, COLOR firstColor=0, uint16_t secondId=0, COLOR secondColor=0 );

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
	*/
	void Cartography(pClient client); //!< By Polygon - opens the cartography skillmenu
	bool HasEmptyMap(pChar pc); //!< By Polygon - checks if player has an empty map
	bool DelEmptyMap(pChar pc); //!< By Polygon - deletes an empty map from the player's pack
	void Decipher(pItem tmap, pClient client); //!< By Polygon - attempt to decipher a tattered treasure map
	//@}

	int GetAntiMagicalArmorDefence(pChar pc); //!< blackwind meditation armor stuff
};
void snooping( pChar snooper, pItem cont );


void SkillVars();
void loadskills();

#endif
