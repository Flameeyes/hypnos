  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

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
namespace Skills {
	//@{
	/*!
	\name General Skill stuff
	*/
	char AdvanceSkill(CHARACTER s, int sk, char skillused);
	void AdvanceStats(CHARACTER s, int sk);
	void SkillUse(NXWSOCKET s, int x);
	void updateSkillLevel(pChar pc, int s);
	//@}


	//@{
	/*!
	\name Hiding/Stealth stuff
	*/
	void Hide(NXWSOCKET s);
	void Stealth(NXWSOCKET s);
	//@}

	//@{
	/*!
	\name Musicianship stuff
	*/
	void PeaceMaking(NXWSOCKET s);
	void PlayInstrumentWell(NXWSOCKET s, int i);
	void PlayInstrumentPoor(NXWSOCKET s, int i);
	int GetInstrument(NXWSOCKET s);
	void target_provocation1( NXWCLIENT ps, pTarget t );
	void target_enticement1( NXWCLIENT ps, pTarget t );
	//@}

	void Meditation(NXWSOCKET s);

	//@{
	/*!
	\name Blacksmithing stuff
	*/
	int CalcRank(NXWSOCKET s,int skill); // by Magius(CHE)
	void ApplyRank(NXWSOCKET s,int c,int rank); // by Magius(CHE)
	int GetSubIngotAmt(int p, char id1, char id2, char color1, char color2);
	int DeleSubIngot(int p, int id1, int id2, int color1, int color2, int amount);
	void target_repair( NXWCLIENT ps, pTarget t );
	void target_smith( NXWCLIENT ps, pTarget t );
	//@}

	//@{
	/*!
	\name ID-Stuff
	*/
	void target_itemId( NXWCLIENT ps, pTarget t );
	//@}

	//@{
	/*!
	\name Alchemy stuff
	*/
	void CreatePotion(CHARACTER s, char type, char sub, int mortar);
	void DoPotion(NXWSOCKET s, int32_t type, int32_t sub, pItem mortar);
	void target_alchemy( NXWCLIENT ps, pTarget t );
	void target_bottle( NXWCLIENT ps, pTarget t );
	void PotionToBottle(pChar pc, pItem mortar);
	//@}

	//@{
	/*!
	\name Tinkering stuff
	*/
	void target_tinkerAxel( NXWCLIENT ps, pTarget t );
	void target_tinkerAwg( NXWCLIENT ps, pTarget t );
	void target_tinkerClock( NXWCLIENT ps, pTarget t );
	//@}

	//@{
	/*!
	\name Cooking stuff
	*/
	void target_cookOnFire( NXWCLIENT ps, pTarget t );
	//@}

	//@{
	/*!
	\name Tailoring stuff
	*/
	void target_tailoring( NXWCLIENT ps, pTarget t );
	void target_wheel( NXWCLIENT ps, pTarget t );
	void target_loom( NXWCLIENT ps, pTarget t );
	//@}

	//@{
	/*!
	\name Bowcrafting stuff
	*/
	void target_fletching( NXWCLIENT ps, pTarget t );
	void target_bowcraft( NXWCLIENT ps, pTarget t );
	//@}

	void target_removeTraps( NXWCLIENT ps, pTarget t );
	
	void target_carpentry( NXWCLIENT ps, pTarget t );

	void MakeMenu( pChar pc, int m, int skill, pItem first, pItem second = NULL );
	void MakeMenu( pChar pc, int m, int skill, uint16_t firstId=0, COLOR firstColor=0, uint16_t secondId=0, COLOR secondColor=0 );

	//@{
	/*!
	\name Mining stuff
	*/
	void target_mine( NXWCLIENT ps, pTarget t );
	void GraveDig(NXWSOCKET s);
	//@}

	void target_smeltOre( NXWCLIENT ps, pTarget t );
	void target_tree( NXWCLIENT ps, pTarget t );

	void target_detectHidden( NXWCLIENT ps, pTarget t );

	//@{
	/*!
	\name Healing stuff
	*/
	void target_healingSkill( NXWCLIENT ps, pTarget t );
	//@}

	void SpiritSpeak(NXWSOCKET s);
	
	void target_armsLore( NXWCLIENT ps, pTarget t );
	void target_tame( NXWCLIENT ps, pTarget t );

	//@{
	/*!
	\name Thievery stuff
	*/
	void target_randomSteal( NXWCLIENT ps, pTarget t );
	void target_stealing( NXWCLIENT ps, pTarget t );
	void PickPocketTarget(NXWCLIENT ps);
	void target_lockpick( NXWCLIENT ps, pTarget t );
	//@}

	void target_animalLore( NXWCLIENT ps, pTarget t );
	
	void target_forensics( NXWCLIENT ps, pTarget t );
	
	void target_poisoning( NXWCLIENT ps, pTarget t );

	void TDummy(NXWSOCKET s);
	
	void target_tinkering( NXWCLIENT ps, pTarget t );
	
	void AButte(NXWSOCKET s1, pItem pButte);

	void Persecute(NXWSOCKET s); //!< AntiChrist persecute stuff

	//@{
	/*!
	\name Cartography Stuff
	*/
	void Cartography(NXWSOCKET s); //!< By Polygon - opens the cartography skillmenu
	bool HasEmptyMap(CHARACTER cc); //!< By Polygon - checks if player has an empty map
	bool DelEmptyMap(CHARACTER cc); //!< By Polygon - deletes an empty map from the player's pack
	void Decipher(pItem tmap, NXWSOCKET s); //!< By Polygon - attempt to decipher a tattered treasure map
	//@}

	int GetAntiMagicalArmorDefence(CHARACTER p); //!< blackwind meditation armor stuff
};
void snooping( pChar snooper, pItem cont );


void SkillVars();
void loadskills();

#endif
