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
\brief Declaration of miscellaneous class
\author Zippy

Declaration of class cCommands, cGuilds, cGump, MapStaticIterator,
cMapStuff, cFishing, cSkills, cNetworkStuff and a lot of structures
 */
#ifndef __Classes_h
#define __Classes_h

#include "targeting.h"

/*!
\brief New structure for basic guild related infos
\author DasRaetsel
*/
struct guild_st
{
	uint8_t	free;				//!< Guild slot used?
	char	name[41];			//!< Name of the guild
	char	abbreviation[4];		//!< Abbreviation of the guild
	int32_t	type;				//!< Type of guild (0=standard/1=chaos/2=order)
	char	charter[51];			//!< Charter of guild
	char	webpage[51];			//!< Web url of guild
	int32_t	stone;				//!< The serial of the guildstone
	int32_t	master;				//!< The serial of the guildmaster
	int32_t	recruits;			//!< Amount of recruits
	int32_t	recruit[MAXGUILDRECRUITS+1];	//!< Serials of candidates
	int32_t	members;			//!< Amount of members
	int32_t	member[MAXGUILDMEMBERS+1];	//!< Serials of all the members
	int32_t	wars;				//!< Amount of wars
	int32_t	war[MAXGUILDWARS+1];		//!< Numbers of Guilds we have declared war to
	int32_t	priv;				//!< Some dummy to remember some values
};

/*!
\brief Old Guild Class
\todo Remove it
*/
class cGuilds
{
private:
	void EraseMember(int c);
	void EraseGuild(int guildnumber);
	void ToggleAbbreviation(int s);
	int SearchSlot(int guildnumber, int type);
	void ChangeName(pClient client, char *text);
	void ChangeAbbreviation(int s, char *text);
	void ChangeTitle(int s, char *text);
	void ChangeCharter(int s, char *text);
	void ChangeWebpage(int s, char *text);
	int CheckValidPlace(int s);
	void Broadcast(int guildnumber, char *text);
	void CalcMaster(int guildnumber);
	void SetType(int guildnumber, int type);
public:
	guild_st guilds[MAXGUILDS]; //lb, moved from common_libs.h cauz global variabels cant be changed in constuctors ...
	cGuilds();
	virtual ~cGuilds();
	int  GetType(int guildnumber);
	void StonePlacement(int s);
	void Menu(int s, int page);
	void Resign(pChar pc, pClient clientocket);
	void Recruit(int s);
	void TargetWar(int s);
	void StoneMove(int s);
	int Compare(pChar player1,pChar player2);
	void GumpInput(int s, int type, int index, char *text);
	void GumpChoice(pClient client, int main, int sub);
	int SearchByStone(int s);
	void Title(int s, int player2);
	void Read(int guildnumber);
	void Write(FILE *wscfile);
	void CheckConsistancy();
};
////////////////////////END REMOVE////////////////////////////


// use this value whereever you need to return an illegal z value
const signed char illegal_z = -128;	// reduced from -1280 to -128, to fit in with a valid signed char


#endif
