  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/


/***********************************************************************************
  Xanathar : New server.cfg interface

  Works like the classic server.cfg except that :

		o It contains even the options once contained in noxwizard.ini & noxwizard.cfg
		o Setup happens in two phases, since .ini data should be read after Map initialization
		o It supports dynamic configuration through CFG <section>.<option>=value

  for example n remote administration or as a gm command one can type


	  CFG REPSYS.MAXKILLS=6  [in remote admin]

	  'CFG REPSYS.MAXKILLS=6  [in gm commands]

	or something equivalent to
	  execCfgCommand("REPSYS.MAXKILLS=%d", 6); [in amx scripts]

    A stock function will be contained in amx scripts :

		setCfgOption("REPSYS", "MAXKILLS", 6);

	Similar to

    stock setCfgOption (const section[], const property[], const value)
	{
		execCfgCommand("%s.%s=%d", section, property, value);
	}


***********************************************************************************/

#include "common_libs.h"
#include "party.h"
#include "debug.h"
#include "version.h"
#include "calendar.h"
#include "amx/amxscript.h"
#include "data.h"
#include "extractionskills.h"
#include "inlines.h"
#include "scripts.h"

static char temp_map[120];
static char temp_statics[120];
static char temp_staidx[120];
static char temp_verdata[120];
static char temp_tiledata[120];
static char temp_multimul[120];
static char temp_multiidx[120];

//NoX-Wizard defaults, ripped from NXWCFG
//[Reputation]
namespace ServerScp {

int g_nAllowMagicInTown=0;
int g_nSnoopWillCriminal=1;
int g_nStealWillCriminal=2;
int g_nChopWillCriminal=2;
int g_nPolymorphWillCriminal=1;
int g_nLootingWillCriminal=1;
int g_nLowKarmaWillCriminal=1;
int g_nHelpingGreyWillCriminal=1;
int g_nHelpingCriminalWillCriminal=2;
int g_nGreyCanBeLooted=1;
int g_nEnableKarmaLock=1;
int g_nVerboseCrontab = 0;
int g_nPopUpHelp = 1;
int g_nStatDailyLimit = 999;

int g_nWalkIgnoreWetBit = 0;

char* g_strKarmaUnlockPrayer="I PRAY MY GODS TO FORGIVE MY ACTIONS";
int g_nEnableBooks = 0;
int g_nUnequipOnReequip = 0;
int g_nEquipOnDclick = 0;

//[Special]
int g_nShowPCNames=1;
int g_nShowFireResistanceInTitle=1;
int g_nShowPoisonResistanceInTitle=1;
int g_nShowParalisysResistanceInTitle=1;
int g_nDisableFieldResistanceRaise=1;
int g_nAdventureMode=0;

//[Remote_Administration]
int g_nRacTCPPort=2594;
int g_nUseCharByCharMode=1;

//[AMX Scripting]
int g_nDeamonMode = 0;

//[Windows]
int g_nAutoDetectMuls=1;
int g_nLineBuffer=50;

int g_nAutoDetectIP=1;
int g_nBehindNat=0;
int g_nStealthArLimit = 1;
int g_nLoadDebugger = 0;
int g_nCheckBySmall = 0;
int g_nStopOnAnyCall = 0; //<-- XAN : UNUSED! :D

int g_nRedirectOutput = 0;
char g_szOutput[2048];

int g_nShoveStmDamage = 2;
float g_fStaminaUsage[6]={0.10f,0.25f,0.50f,0.75f,1.0f,10.0f};

int g_nUseNewNpcMagic = 1;

int g_nPlayersCanDeleteRoles = 1;
unsigned int g_nLimitRoleNumbers = 5;
int g_nLimitPlayerSparring = 0;
int g_nStatsAdvanceSystem = 0;
int g_nBankLimit = 0;

// important: loaddefault has to be called before.
// currently it is. makes no sense to change that too.
TIMERVAL g_nRestockTimeRate = 15;

}

extern bool g_bNTService;


void loadclientsallowed()
{
   int num_c=0, s=0, loopexit=0;
   std::vector<std::string> cls;

   do
   {
		readFullLine();

		if(!(strcmp(script1,"SERVER_DEFAULT"))) { s=1; break; }
		else if(!(strcmp(script1,"ALL"))) { s=2; break; }
		else if (strlen(script1)>3)
		{
			num_c++;
			cls.push_back(script1);
		}
   }
   while ( (strcmp(script1, "}")) && (++loopexit < MAXLOOPS) );

   if (num_c==0 && s==0) s=1; // no entry == pick server_default

   switch(s)
   {
	   case 0:
               clientsAllowed.pop_back();  // delete the entry from loaddefaults
			   copy(cls.begin(), cls.end(), back_inserter(clientsAllowed) ); // copy temp vector to perm vector
		       break;

	   case 1:
		       break; // already in vector from loaddefaults

	   case 2:
		       std::string sd("ALL");
		       clientsAllowed.pop_back();    // delete server_defualt entry from loaddefaults
			   clientsAllowed.push_back(sd); // now add the correct one
		       break;
   }


}

void loadserverdefaults()
{
	std::string sd("ALL");

	strcpy(ServerScp::g_szOutput, "nxwout");

	clientsAllowed.push_back(sd);
	strcpy(serv[0][0], "NoX-Wizard Shard");
	strcpy(serv[0][1], "127.0.0.1");
	strcpy(serv[0][2], "2593");

	strcpy(temp_map, "./map0.mul");
	strcpy(temp_statics, "./statics0.mul");
	strcpy(temp_staidx, "./staidx0.mul");
	strcpy(temp_verdata, "./verdata.mul");
	strcpy(temp_tiledata, "./tiledata.mul");
	strcpy(temp_multimul, "./multi.mul");
	strcpy(temp_multiidx, "./multi.idx");

	strcpy(server_data.Unicodelanguage,"ENU"); //UNICODE font name for multiple language support - N6
	strcpy(server_data.specialbanktrigger,"WARE"); //AntiChrist - Special Bank word trigger
	server_data.usespecialbank=0;	//AntiChrist - 1= Special Bank enabled
	server_data.goldweight=0.005;	//what a goldpiece weighs this is in hundreths of a stone! AntiChrist
	server_data.poisontimer=180; // lb
	server_data.decaytimer=DECAYTIMER;
	// Should we check character age for delete?
	server_data.checkcharage = 0 ;

	server_data.skillcap=700;	// default=no cap
	server_data.statcap=200;		// default=no cap

	//taken from 6904t2(5/10/99) - AntiChrist
	server_data.playercorpsedecaymultiplier=3;	// Player's corpse decays 3x the normal decay rate
	server_data.lootdecayswithcorpse=1;			// JM - 0 Loot disappears with corpse, 1 loot gets left when corpse decays

	server_data.auto_a_create = 1;                     // for auto accounts

	server_data.invisibiliytimer=INVISTIMER;
	server_data.bandageincombat=1;				// 0=no, 1=yes; can use bandages while healer and/or patient are in combat ?
	server_data.inactivitytimeout=300;			// seconds of inactivity until player will be disconnected
	server_data.hitpointrate=REGENRATE1;
	server_data.staminarate=REGENRATE2;
	server_data.manarate=REGENRATE3;
	server_data.skilladvancemodifier=1000;		// Default Modulo val for skill advance
	server_data.statsadvancemodifier=500;		// Default Modulo val for stats advance
	server_data.gatetimer=GATETIMER;
	server_data.minecheck=1;
	server_data.showdeathanim=1;				// 1 = yes/true
	server_data.combathitmessage=1;
	server_data.attackstamina=0;				// AntiChrist - attacker looses stamina when hits
	server_data.monsters_vs_animals=0;			// By default monsters won't attack animals;
	server_data.animals_attack_chance=15;		// a 15% chance
	server_data.animals_guarded=0;				// By default players can attack animals without summoning guards
	server_data.npc_base_fleeat=NPC_BASE_FLEEAT;
	server_data.npc_base_reattackat=NPC_BASE_REATTACKAT;
	server_data.maxabsorbtion=20;				// 20 Arm (single armour piece) -- Magius(CHE)
	server_data.maxnohabsorbtion=100;			// 100 Arm (total armour) -- Magius(CHE)
	server_data.sellbyname=0;		// Values= 0(Disabled) or 1(Enabled) - The NPC Vendors buy from you if your item has the same name of his item! --- Magius(CHE)
	server_data.skilllevel=5;		// Range from 1 to 10 - This value if the difficulty to create an item using a make command: 1 is easy, 10 is difficult! - Magius(CHE)
	server_data.sellmaxitem=50;		// Set maximum amount of items that one player can sell at one time (5 is standard OSI) --- Magius(CHE)
	server_data.npcdamage=2;
	server_data.rank_system=1;		// Rank system to make various type of a single intem based on the creator's skill! - Magius(CHE)
	server_data.guardsactive=1;
	server_data.errors_to_console=1;
	server_data.bg_sounds=2;
	server_data.announceworldsaves=1;
	server_data.joinmsg=1;
	server_data.partmsg=1;
	server_data.stablingfee=0.25;
	server_data.savespawns=1;

	server_data.gm_log=0;
	server_data.pvp_log=0;
	server_data.speech_log=0;
	server_data.server_log=0;

	server_data.quittime=300;//Instalog

	strcpy(server_data.msgboardpath, "save/msgboard/");	   // Dupois - Added Dec 20, 1999 for message boards (current dir)
	server_data.backup_save_ratio=1; //LB

	server_data.msgpostaccess=0;           // Dupois - Added Dec 20, 1999 for message boards (GM only)
	server_data.msgpostremove=0;           // Dupois - Added Dec 20, 1999 for message boards (GM only)
	server_data.msgretention=30;           // Dupois - Added Dec 20, 1999 for message boards (30 Days)
	server_data.escortactive=1;            // Dupois - Added Dec 20, 1999 for message boards (Active)
	server_data.escortinitexpire=86400;    // Dupois - Added Dec 20, 1999 for message boards (24 hours)
	server_data.escortactiveexpire=1800;   // Dupois - Added Dec 20, 1999 for message boards (1/2 Hour)
	server_data.escortdoneexpire=1800;     // Dupois - Added Dec 20, 1999 for message boards (1/2 Hour)

	server_data.bountysactive=1;           // Dupois - Added July 18, 2000 for bountys (1-Active)
	server_data.bountysexpire=0;           // Dupois - Added July 18, 2000 for bountys (0-Never - in days)


	server_data.runningstaminasteps=15;// AntiChrist - how often ( in steps ) stamina decreases while running

	server_data.boatspeed=0.75;//Boats

	server_data.cutscrollreq=1;//AntiChrist

	server_data.persecute=1;//AntiChrist

	server_data.feature=0; //Transparent mode: don't send packet 0xB9 

	server_data.tamed_disappear=1; // Ripper
	server_data.houseintown=0; // Ripper
	server_data.shoprestock=1; //Ripper
	server_data.commandPrefix = '\''; //Ripper

	server_data.CheckBank = 2000; // Ripper..2000 default
	server_data.defaultSelledItem = 10;

	server_data.housedecay_secs=604800*2; // LB, one week //xan, 2 weeks default

	server_data.eclipsemode=0; // Light to dark..

	//cwmWorldState->SetLoopSaveAmt(-1);

	speed.srtime=30;
	speed.itemtime=(float)1.5;
	speed.npctime=(float)1.0; // lb, why -1 zippy ??????
	speed.npcaitime=(float)1.2;
	speed.tamednpctime=(float)0.9;//AntiChrist
	speed.npcfollowtime=(float)0.5; //Ripper

	resource.logs=3;
	resource.logtime=600;
	resource.logarea=10;
	resource.lograte=3;
	resource.logtrigger=0;
	resource.logstamina=-5;
	//REPSYS
	repsys.crimtime=120;
	repsys.maxkills=4;
	repsys.murderdecay=420;
	//RepSys ---^

	Begging::initialize();
	
	Fishing::basetime=FISHINGTIMEBASE;
	Fishing::randomtime=FISHINGTIMER;
	spiritspeak_data.spiritspeaktimer=SPIRITSPEAKTIMER;

	server_data.blockaccbadpass=0;		//elcabesa tempblock
	server_data.n_badpass=0;			//elcabesa tempblock
	server_data.time_badpass=0;			//elcabesa tempblock
	server_data.always_add_hex=0;	//endymion add command stuff
	server_data.staminaonhorse=0.50; 
	server_data.disable_z_checking=0;
 	server_data.archivePath="save/";
 	server_data.savePath="backup/";
}

extern bool g_bInMainCycle;

void commitserverscript() // second phase setup
{
 	char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var

	if (startcount<9)
	{
		if (startcount==0)
		{   //default starting locz
			ConOut("\n  Warning no starting locations... defaulting to classic ones!\n");
			strcpy(start[startcount][0], "Yew");
			strcpy(start[startcount][1], "The Sturdy Bow");
			strcpy(start[startcount][2], "567");
			strcpy(start[startcount][3], "978");
			strcpy(start[startcount][4], "0");
			startcount++;
			strcpy(start[startcount][0], "Minoc");
			strcpy(start[startcount][1], "The Barnacle Tavern");
			strcpy(start[startcount][2], "2477");
			strcpy(start[startcount][3], "407");
			strcpy(start[startcount][4], "15");
			startcount++;
			strcpy(start[startcount][0], "Britain");
			strcpy(start[startcount][1], "Sweet Dreams Inn");
			strcpy(start[startcount][2], "1496");
			strcpy(start[startcount][3], "1629");
			strcpy(start[startcount][4], "10");
			startcount++;
			strcpy(start[startcount][0], "Moonglow");
			strcpy(start[startcount][1], "The Scholars Inn");
			strcpy(start[startcount][2], "4404");
			strcpy(start[startcount][3], "1169");
			strcpy(start[startcount][4], "0");
			startcount++;
			strcpy(start[startcount][0], "Trinsic");
			strcpy(start[startcount][1], "The Traveller's Inn");
			strcpy(start[startcount][2], "1844");
			strcpy(start[startcount][3], "2745");
			strcpy(start[startcount][4], "0");
			startcount++;
			strcpy(start[startcount][0], "Magincia");
			strcpy(start[startcount][1], "The Great Horns Tavern");
			strcpy(start[startcount][2], "3738");
			strcpy(start[startcount][3], "2223");
			strcpy(start[startcount][4], "20");
			startcount++;
			strcpy(start[startcount][0], "Jhelom");
			strcpy(start[startcount][1], "The Morning Star Inn");
			strcpy(start[startcount][2], "1378");
			strcpy(start[startcount][3], "3817");
			strcpy(start[startcount][4], "0");
			startcount++;
			strcpy(start[startcount][0], "Skara Brae");
			strcpy(start[startcount][1], "The Falconers Inn");
			strcpy(start[startcount][2], "594");
			strcpy(start[startcount][3], "2227");
			strcpy(start[startcount][4], "0");
			startcount++;
			strcpy(start[startcount][0], "Vesper");
			strcpy(start[startcount][1], "The Ironwood Inn");
			strcpy(start[startcount][2], "2771");
			strcpy(start[startcount][3], "977");
			strcpy(start[startcount][4], "0");
			startcount++;
		}
		ConOut("\n  Warning, insufficient starting locations... padding...\n");
		for (; startcount<9; startcount++)
		{
			strcpy(start[startcount][0], start[0][0]);
			strcpy(start[startcount][1], start[0][1]);
			strcpy(start[startcount][2], start[0][2]);
			strcpy(start[startcount][3], start[0][3]);
			strcpy(start[startcount][4], start[0][4]);
		}
	}

	data::setPath( Map_File, std::string( temp_map ) );
	data::setPath( StaIdx_File, std::string( temp_staidx ) );
	data::setPath( Statics_File, std::string( temp_statics ) );
	data::setPath( VerData_File, std::string( temp_verdata ) );
	data::setPath( TileData_File, std::string( temp_tiledata ) );
	data::setPath( Multi_File, std::string( temp_multimul ) );
	data::setPath( MultiIdx_File, std::string( temp_multiidx ) );

		// name resovling of server-address, LB 7-JULY 2000

	unsigned int i;
	unsigned long ip;
	sockaddr_in m_sin;
	hostent *hpe;
	char *name;

    m_sin.sin_family = AF_INET;

	for (i = 0; i < servcount; i++)
	{
		ip = inet_addr(serv[i][1]);

		if (ip == INADDR_NONE) // adresse-name instead of ip adress given ! trnslate to ip string
		{
			name = serv[i][1];
			ConOut("host: %s\n", name);
			hpe = gethostbyname(name);

			if (hpe == NULL)
			{
#if	defined(__unix__)
				// We should be able to use the xti error functions, cant find them so...
				// sprintf(temp,"warning: %d resolving name: %s\n", t_srerror(t_errno),name) ;
#ifndef __BEOS__
				sprintf(temp, "warning: Error desolving name: %s : %s\n", name, hstrerror(h_errno));
#endif
#else
				sprintf(temp, "warning: %d resolving name: %s\n", WSAGetLastError(), name);
#endif
				LogWarning(temp);
				LogWarning("switching to localhost\n");

				strcpy(serv[i][1], "127.0.0.1");
			}
			else
			{
				memcpy(&(m_sin.sin_addr), hpe->h_addr, hpe->h_length);

				strncpy(serv[i][1], inet_ntoa(m_sin.sin_addr), 28);
				serv[i][1][29] = 0;
			} // end else resolvable
		}
	} // end server loop

}

void saveserverscript()
{
	FILE *file;
	file=fopen("config/server.cfg", "w");
	if(!file) return; //only write can be..
	const char * t;
	std::vector<std::string>::const_iterator vis( clientsAllowed.begin() ), vis_end( clientsAllowed.end() );

	fprintf(file, "SECTION SHARD\n");
	fprintf(file, "{\n");
	fprintf(file, "// The name of your shard \n");
	fprintf(file, "NAME %s\n", serv[0][0]); // lb
	fprintf(file, "// The IP address of your shard. Ignored if AUTODETECTIP is on. \n");
	fprintf(file, "IP %s\n", saveip); // lb
	fprintf(file, "// The TCP port the server will listen to. \n");
	fprintf(file, "PORT %s\n", serv[0][2]); // lb
	fprintf(file, "// Set this to enable plug&play configuration of the network address \n");
	fprintf(file, "// Do not enable this if you want to create a login server or if you have \n");
	fprintf(file, "// particular security reasons about not enabling it! \n");
	fprintf(file, "AUTODETECTIP %i\n", ServerScp::g_nAutoDetectIP); // lb
	fprintf(file, "BEHINDNAT %i\n", ServerScp::g_nBehindNat); // lb
	fprintf(file, "}\n\n");

	fprintf(file, "SECTION MULFILES\n");
	fprintf(file, "{\n");
	fprintf(file, "// In this section you have to define paths to mul files.\n");
	fprintf(file, "// You may skip this section if you use a Windows computer on which UO T2A or\n");
	fprintf(file, "// Renaissance is installed and AUTODETECTMULS is defined under the Windows section (this is the default)\n");
	fprintf(file, "// \n");
	fprintf(file, "// The file containing the map, usually map0.mul for T2A client and Britannia on UO3D,\n");
	fprintf(file, "// map2.mul for Ilshenar map on UO Third Dawn.\n");
	fprintf(file, "MAP %s\n", data::getPath( Map_File ).c_str());
	fprintf(file, "//  Setting to 1 loads the map's info into RAM. Warning: requires A LOT of memory!\n");
	fprintf(file, "MAP_CACHE %i\n", map_cache );
	fprintf(file, "// The files containing the statics, usually statics0.mul and staidx0.mul for T2A client\n");
	fprintf(file, "// and Britannia on UO3D, statics2.mul and staidx2.mul for Ilshenar map on UO Third Dawn.\n");
	fprintf(file, "STATICS %s\n", data::getPath( Statics_File ).c_str());
	fprintf(file, "STAIDX %s\n", data::getPath( StaIdx_File ).c_str());
	fprintf(file, "//  Setting to 1 loads the statics's info into RAM, significant speed boost\n");
	fprintf(file, "STATICS_CACHE %i\n", statics_cache );
	fprintf(file, "// The version data you'll use (verdata.mul, usually)\n");
	fprintf(file, "VERDATA %s\n", data::getPath( VerData_File ).c_str());
	fprintf(file, "// The tile data you'll use (tiledata.mul, usually)\n");
	fprintf(file, "TILEDATA %s\n", data::getPath( TileData_File ).c_str());
	fprintf(file, "// The files containing multi data (multi.mul and multi.idx usually)\n");
	fprintf(file, "MULTIMUL %s\n", data::getPath( Multi_File ).c_str());
	fprintf(file, "MULTIIDX %s\n", data::getPath( MultiIdx_File ).c_str());
	fprintf(file, "}\n\n");

	fprintf(file, "SECTION SKILLS\n");
	fprintf(file, "{\n");
	fprintf(file, "// sets the skillcap for players \n");
	fprintf(file, "SKILLCAP %i\n",server_data.skillcap);
	fprintf(file, "// sets the statcap for players \n");
	fprintf(file, "STATCAP %i\n",server_data.statcap);
	fprintf(file, "// sets the maximum stat gain in a single day (or until a reset in crontab.cfg) \n");
	fprintf(file, "DAILYSTATCAP %i\n",ServerScp::g_nStatDailyLimit);
	fprintf(file, "// enable stat advance sistem 1 or not 0\n");
	fprintf(file, "STATSADVANCESYSTEM %i\n",ServerScp::g_nStatsAdvanceSystem);
	fprintf(file, "// the higher the value, the slower players will gain skill \n");
	fprintf(file, "SKILLADVANCEMODIFIER %i\n",server_data.skilladvancemodifier);
	fprintf(file, "// the higher the value, the slower players will gain stats (values between 1000\n");
	fprintf(file, "// -2000 resemble OSI standards) \n");
	fprintf(file, "STATSADVANCEMODIFIER %i\n",server_data.statsadvancemodifier);
	fprintf(file, "// This value multiplied by an item's minskill value indicates the maximum skill diff to create \n");
	fprintf(file, "SKILLLEVEL %i\n",server_data.skilllevel); // By Magius(CHE)
	fprintf(file, "// How close you have to be to beg \n");
	fprintf(file, "BEGGING_RANGE %i\n",Begging::range);
	fprintf(file, "BEGGING_TIME %i\n",Begging::timer);
	fprintf(file, "// Text players will say when using the begging skill\n");
	fprintf(file, "BEGGING_char0 %s\n",begging_data.text[0]);
	fprintf(file, "BEGGING_char1 %s\n",begging_data.text[1]);
	fprintf(file, "BEGGING_char2 %s\n",begging_data.text[2]);
	fprintf(file, "// Minimum amount of time needed for fishing \n");
	fprintf(file, "BASE_FISHING_TIME %i\n",fishing_data.basetime);
	fprintf(file, "// Random amount of time on top of the base time \n");
	fprintf(file, "RANDOM_FISHING_TIME %i\n",fishing_data.randomtime);
	fprintf(file, "// How long Spirit Speak lasts for (in seconds) \n");
	fprintf(file, "SPIRITSPEAKTIMER %i\n",spiritspeak_data.spiritspeaktimer);
	fprintf(file, "}\n\n");

	fprintf(file, "SECTION BANK\n");
	fprintf(file, "{\n");
	fprintf(file, "// Special bank will give each character a second bank box, intention is to use\n");
	fprintf(file, "// the first one for gold, and the special one for items \n");
	fprintf(file, "USESPECIALBANK %i\n",server_data.usespecialbank);	//AntiChrist - specialbank
	fprintf(file, "// The Trigger can be any word. When a character types it in the game, the\n");
	fprintf(file, "// special bank box will be opened \n");
	fprintf(file, "SPECIALBANKTRIGGER %s\n",server_data.specialbanktrigger);
	fprintf(file, "// If not 0, limit the number of items in a bank box\n");
	fprintf(file, "BANKITEMLIMIT %d\n",ServerScp::g_nBankLimit);
	fprintf(file, "}\n\n");

	fprintf(file, "SECTION STAMINA\n");
	fprintf(file, "{\n");
	fprintf(file, "// Define how often stamina will be reduced by 1 when running \n");
	fprintf(file, "RUNNINGSTAMINASTEPS %i\n",server_data.runningstaminasteps);//Instalog
	fprintf(file, "// Stamina to loose when players shove something out of the way \n");
    fprintf(file, "SHOVESTAMINALOSS %i\n",ServerScp::g_nShoveStmDamage);
	fprintf(file, "// Defines how much stamina is used each step\n");
	fprintf(file, "// at <0-24> <25-49> <50-74> <75-99> <100> <over> per cent of maximum weight\n");
	fprintf(file, "STAMINAUSAGE %f %f %f %f %f %f\n",ServerScp::g_fStaminaUsage[0],ServerScp::g_fStaminaUsage[1],ServerScp::g_fStaminaUsage[2],ServerScp::g_fStaminaUsage[3],ServerScp::g_fStaminaUsage[4],ServerScp::g_fStaminaUsage[5]);
	fprintf(file, "// %% of stamina loss when riding ( es 0.5 is 50%% ) \n");
	fprintf(file, "STAMINALOSSONHORSE %f\n",server_data.staminaonhorse);//AntiChrist - gold weight
	fprintf(file, "// Weight of gold coins \n");
	fprintf(file, "GOLDWEIGHT %f\n",server_data.goldweight);//AntiChrist - gold weight
	fprintf(file, "}\n\n");

	fprintf(file, "SECTION SKILLS\n");
	fprintf(file, "{\n");
	fprintf(file, "// Limit stealthwith AR \n");
	fprintf(file, "STEALTHARLIMIT %i\n",ServerScp::g_nStealthArLimit);//Instalog
	fprintf(file, "// 1= casting from scrolls requires less skill than casting from spellbook, 0 = no difference \n");
	fprintf(file, "CUT_SCROLL_REQUIREMENTS %i\n",server_data.cutscrollreq);//AntiChrist
	fprintf(file, "// 1= Allow using bandages while fighting, 0=Forbid usage of bandages while fighting \n");
	fprintf(file, "BANDAGEINCOMBAT %i\n",server_data.bandageincombat);
	fprintf(file, "// 0=Let skill raise freely with sparring otherwise defending player must have N skill points\n");
	fprintf(file, "// more than the attacker for the attacker to gain (in PvP only).\n");
	fprintf(file, "SPARRINGLIMIT %i\n",ServerScp::g_nLimitPlayerSparring);
	fprintf(file, "}\n\n");

	fprintf(file, "SECTION LOG\n");
	fprintf(file, "{\n");
	fprintf(file, "// 1= Activate server logging, 0=deactivate logging  \n");
	fprintf(file, "SERVER_LOG %i\n",server_data.server_log); //Lb, splitt log to those 4
	fprintf(file, "// 1= Activate speech logging, 0=deactivate logging \n");
	fprintf(file, "SPEECH_LOG %i\n",server_data.speech_log);
	fprintf(file, "// 1= Activate player combat logging, 0=deactivate logging  \n");
	fprintf(file, "PVP_LOG %i\n",server_data.pvp_log);
	fprintf(file, "// 1= Activate gm action logging, 0=deactivate logging  \n");
	fprintf(file, "GM_LOG %i\n",server_data.gm_log);
	fprintf(file, "// 1=Show errors in server console, 0=don't show them \n");
	fprintf(file, "ERRORS_TO_CONSOLE %i\n",server_data.errors_to_console);
	fprintf(file, "// path where the logs will be written, separator is '/', path must exists\n");
	fprintf(file, "// and subdirectories \"speech\" and \"GM\" must also exist\n");
	fprintf(file, "LOG_PATH logs/\n");
	fprintf(file, "}\n\n");

	fprintf(file, "SECTION TIMERS\n");
	fprintf(file, "{\n");
	fprintf(file, "// Sets the decaying time (seconds) for items \n");
	fprintf(file, "DECAYTIMER %i\n",server_data.decaytimer);
	fprintf(file, "// Sets the duration (seconds) of poison \n");
	fprintf(file, "POISONTIMER %i\n",server_data.poisontimer); // lb
	fprintf(file, "// Number of seconds that a gate remains open  \n");
	fprintf(file, "GATETIMER %i\n",server_data.gatetimer);
	fprintf(file, "// Characters will be logged out when inactive for the specified time (seconds?) \n");
	fprintf(file, "INACTIVITYTIMEOUT %i\n",server_data.inactivitytimeout);
	fprintf(file, "// Defines how long (seconds) a character stays in the game when he logs out \n");
	fprintf(file, "CHAR_TIME_OUT %i\n",server_data.quittime);//Instalog
	fprintf(file, "// Define how often a boat moves \n");
	fprintf(file, "BOAT_SPEED %f\n",server_data.boatspeed);//Boats
	fprintf(file, "// Fee payed for stabled animals \n");
	fprintf(file, "STABLING_FEE %f\n",server_data.stablingfee);//Boats
	fprintf(file, "// Length of time (INGAME seconds) until a player house will decay - counter is cpu intensitive\n");
	fprintf(file, "// so it only gets updated every 11 minutes - opening a house door will reset counter and refresh the house\n");
	fprintf(file, "HOUSEDECAY_SECS %ld\n",server_data.housedecay_secs);
	fprintf(file, "// Sets the decaying time for character corpses. \n");
	fprintf(file, "// Formula: decaytimer * multiplier = decaying time for corpses \n");
	fprintf(file, "PLAYERCORPSEDECAYMULTIPLIER %i\n", server_data.playercorpsedecaymultiplier);
	fprintf(file, "// 1= loot decays with corpse, 0 = loot gets put on the ground when corpse decays \n");
	fprintf(file, "LOOTDECAYSWITHCORPSE %i\n", server_data.lootdecayswithcorpse);
	fprintf(file, "// Sets the duration (seconds) of the invisibility spell \n");
	fprintf(file, "INVISTIMER %i\n",server_data.invisibiliytimer);
	fprintf(file, "}\n\n");

	fprintf(file, "SECTION SERVER\n");
	fprintf(file, "{\n");
	
	fprintf(file, "SAVEPATH %s\n", server_data.savePath.c_str() );
	fprintf(file, "// Set directory where worldfile backups will be stored\n");
	fprintf(file, "ARCHIVEPATH %s\n", server_data.archivePath.c_str() );
	fprintf(file, "// Enables / Disable features for UO:T2A/UO:R/UO:3D Clients (VERY! experimental)\n");
	fprintf(file, "// 0: Dont send packet, (default)\n");
	fprintf(file, "// 1: T2A (chat button, popup help, ...) 2: LBR (plus T2A) animations \n");
 	fprintf(file, "ENABLEFEATURE %d\n", server_data.feature);
	fprintf(file, "// Enables / Disable PopUp Help for UO:T2A/UO:R/UO:3D Clients \n");
 	fprintf(file, "POPUPHELP %i\n",ServerScp::g_nPopUpHelp);
	fprintf(file, "// 1= ghosts can attack living players and drain mana,\n");
	fprintf(file, "// 0 = ghosts cannot attack living characters  \n");
	fprintf(file, "PERSECUTION %i\n",server_data.persecute);//AntiChrist
	fprintf(file, "// 1= Allow auto creation of accounts, 0=disable auto account creation \n");
	fprintf(file, "// If you disable it, you can easily create accounts using the ADDACCT\n");
	fprintf(file, "// command in the Remote Administration console\n");
	fprintf(file, "AUTO_CREATE_ACCTS %i\n", server_data. create);
	fprintf(file, "// if 0 it won't do char age checking. Else it's the number of days before a\n");
	fprintf(file, "// user is able to delete a character \n");
	fprintf(file, "CHECKCHARAGE %i\n", server_data.checkcharage) ;
	fprintf(file, "// 1=restock is being done automatically, 0=restock needs to be done manually by staff \n");
	fprintf(file, "SHOPRESTOCK %i\n",server_data.shoprestock); // Ripper
	fprintf(file, "// 1= show animation (character falls backwards on the ground), 2= don't show animation  \n");
	fprintf(file, "SHOWDEATHANIM %i\n",server_data.showdeathanim);
	fprintf(file, "// Check this to 1 to display chars name whenever they are single clicked \n");
	fprintf(file, "SHOWPCNAMES %i\n", ServerScp::g_nShowPCNames); // Luxor
	fprintf(file, "// 1= guards are active as specified in regions.scp, 0= turns guards off globally,\n");
	fprintf(file, "// overriding region settings \n");
	fprintf(file, "GUARDSACTIVE %i\n",server_data.guardsactive);
	fprintf(file, "// Must be 1 - 10, Higher the number, less chance of background sounds  \n");
	fprintf(file, "BG_SOUNDS %i\n",server_data.bg_sounds);
	fprintf(file, "// 1=spawned npcs will be saved to the worldfile, 0=spawned npcs will not be saved \n");
 	fprintf(file, "SAVESPAWNREGIONS %i\n",server_data.savespawns);
	fprintf(file, "// 1=Broadcasts a message to all players that a worldsave is being done,\n");
	fprintf(file, "// 0=don't broadcast worldsaves \n");
	fprintf(file, "ANNOUNCE_WORLDSAVES %i\n",server_data.announceworldsaves);
	fprintf(file, "// 1= Broadcasts a notice to all players when someone logs in, 0= don't broadcast anything \n");
	fprintf(file, "JOINMSG %i\n",server_data.joinmsg);
	fprintf(file, "// 1= Broadcasts a notice to all players when someone logs out, 0= don't broadcast anything \n");
	fprintf(file, "PARTMSG %i\n",server_data.partmsg);
	fprintf(file, "// 0=Animal wanders off when released, 1=Animal disappears when released \n");
	fprintf(file, "TAMED_DISAPPEAR %i\n",server_data.tamed_disappear); // Ripper
	fprintf(file, "// 1 = Allow placement of houses in towns, 0= forbid placement of houses in towns  \n");
	fprintf(file, "HOUSEINTOWN %i\n",server_data.houseintown); // Ripper
	fprintf(file, "// Sets the command prefix for in-game commands \n");
	fprintf(file, "COMMANDPREFIX %c\n", server_data.commandPrefix ); // Ripper
	fprintf(file, "// Max number of charachters per account  \n");
	fprintf(file, "LIMITEROLENUMBERS %d\n",ServerScp::g_nLimitRoleNumbers); // Ripper
	fprintf(file, "// Set if players can delete their charachters \n");
	fprintf(file, "PLAYERCANDELETEROLES %d\n", ServerScp::g_nPlayersCanDeleteRoles ); // Ripper
	fprintf(file, "// Set to enable books\n");
	fprintf(file, "UNEQUIPONREEQUIP %d\n", ServerScp::g_nUnequipOnReequip ); // juliunus
	fprintf(file, "// Set to equip on double-clicking\n");
	fprintf(file, "EQUIPONDCLICK %d\n", ServerScp::g_nEquipOnDclick ); // Anthalir
	fprintf(file, "// Language of the server. For UNICODE fonts \n");	//N6
	fprintf(file, "LANGUAGE %s\n",server_data.Unicodelanguage);			//End-N6
	fprintf(file, "// 1 = Add command params are in hex ( no need 0x ), 0 = normal \n");	//Endymion
	fprintf(file, "ADDCOMMANDHEX %d\n",server_data.always_add_hex);		//Endymion
	fprintf(file, "// 1 = Speech override are case sensitive, 0 = case insensitive \n");	//Endymion
	fprintf(file, "CSSOVERRIDES %d\n",ServerScp::g_css_override_case_sensitive);		//Endymion
	fprintf(file, "// 1 = Disable Z checking ( cool for worldbuilder but not for gamplay ) \n");	//Endymion
	fprintf(file, "DISABLEZCHECKING %d\n",server_data.disable_z_checking);		//Endymion
	fprintf(file, "}\n\n");

	fprintf(file, "SECTION BILLBOARDS\n");
	fprintf(file, "{\n");
	fprintf(file, "// Path to message board files, default is the same directory where NoXWizard is \n");
	fprintf(file, "MSGBOARDPATH %s\n",server_data.msgboardpath);              // Dupois - Added Dec 20, 1999 for msgboard.cpp
	fprintf(file, "// 0 = only GMs can post messages, 1= anyone can post \n");
	fprintf(file, "MSGPOSTACCESS %i\n",server_data.msgpostaccess);            // Dupois - Added Dec 20, 1999 for msgboard.cpp
	fprintf(file, "// 0 = only GMs can remove messages, 2= anyone can remove \n");
	fprintf(file, "MSGPOSTREMOVE %i\n",server_data.msgpostremove);            // Dupois - Added Dec 20, 1999 for msgboard.cpp
	fprintf(file, "// Length of time to retain messages (0 indefinite), not sure on measurements \n");
	fprintf(file, "MSGRETENTION %i\n",server_data.msgretention);              // Dupois - Added Dec 20, 1999 for msgboard.cpp
	fprintf(file, "// 1=enable escort quests (msgboard), 0=disable quests \n");
	fprintf(file, "ESCORTACTIVE %i\n",server_data.escortactive);              // Dupois - Added Dec 20, 1999 for escorts
	fprintf(file, "// Length of time that the escort is summoned for (seconds?)  \n");
	fprintf(file, "ESCORTINITEXPIRE %i\n",server_data.escortinitexpire);      // Dupois - Added Dec 20, 1999 for escorts
	fprintf(file, "// Length of time in seconds before the escort quest expires on board \n");
	fprintf(file, "ESCORTACTIVEEXPIRE %i\n",server_data.escortactiveexpire);  // Dupois - Added Dec 20, 1999 for escorts
	fprintf(file, "// Length of time in seconds for which escort is alive once taken  \n");
	fprintf(file, "ESCORTDONEEXPIRE %i\n",server_data.escortdoneexpire);      // Dupois - Added Dec 20, 1999 for escorts
	fprintf(file, "// 1= Enable bounties, 0 = Disable bounties  \n");
	fprintf(file, "BOUNTYSACTIVE %i\n",server_data.bountysactive);            // Dupois - Added July 18, 2000 for bountys
	fprintf(file, "// Length of time (seconds?) until bounty expires  \n");
	fprintf(file, "BOUNTYSEXPIRE %i\n",server_data.bountysexpire);            // Dupois - Added July 18, 2000 for bountys
	fprintf(file, "}\n\n");

	fprintf(file, "SECTION SPEED\n"); //Lag Fix -- Zippy
	fprintf(file, "{\n");
	fprintf(file, "// How often items are checked for action (in seconds) \n");
	fprintf(file, "CHECK_ITEMS %f\n",speed.itemtime);
	fprintf(file, "// How often NPCs are checked for action (in seconds)  \n");
	fprintf(file, "CHECK_NPCS %f\n",speed.npctime);
	fprintf(file, "// How often tamed NPCs are checked for action (in seconds)  \n");
	fprintf(file, "CHECK_TAMEDNPCS %f\n",speed.tamednpctime);//AntiChrist
	fprintf(file, "// This is for the speed of npcs that are following you, the higher the x the slower they are\n");
	fprintf(file, "CHECK_NPCFOLLOW %f\n",speed.npcfollowtime);//Ripper
	fprintf(file, "// How often the NPCs AI is checked (must be no more than CHECK_NPCS)  \n");
	fprintf(file, "CHECK_NPCAI %f\n",speed.npcaitime);
	fprintf(file, "// Number of seconds between spawn regions being checked for more spawns  \n");
	fprintf(file, "CHECK_SPAWNREGIONS %i\n",speed.srtime);
	fprintf(file, "}\n\n");

	fprintf(file, "SECTION COMBAT\n");
	fprintf(file, "{\n");
	fprintf(file, "// 1=Display hit messages (\"player hits your left leg\" etc), 0=no messages  \n");
	fprintf(file, "COMBAT_HIT_MESSAGE %i\n",server_data.combathitmessage);
	fprintf(file, "// maximum damage that can be absorbed by a character via armor  \n");
	fprintf(file, "MAX_ABSORBTION %i\n",server_data.maxabsorbtion);
	fprintf(file, "// maximum damage that can be absorbed by an npc via armor \n");
	fprintf(file, "MAX_NON_HUMAN_ABSORBTION %i\n",server_data.maxnohabsorbtion);
	fprintf(file, "// If 1, animals and monsters would attack each other  \n");
	fprintf(file, "MONSTERS_VS_ANIMALS %i\n",server_data.monsters_vs_animals);
	fprintf(file, "// chance (percent) that a monster will attack an animal  \n");
	fprintf(file, "ANIMALS_ATTACK_CHANCE %i\n",server_data.animals_attack_chance);
	fprintf(file, "// If 1, then animals are guarded in guard regions  \n");
	fprintf(file, "ANIMALS_GUARDED %i\n",server_data.animals_guarded);
	fprintf(file, "// Damage/Rate is the amount of damage that an NPC will sustain  \n");
	fprintf(file, "NPC_DAMAGE_RATE %i\n",server_data.npcdamage);
	fprintf(file, "// base hp which NPCs flee at  \n");
	fprintf(file, "NPC_BASE_FLEEAT %i\n",server_data.npc_base_fleeat);
	fprintf(file, "// base hp which NPCs will attack at  \n");
	fprintf(file, "NPC_BASE_REATTACKAT %i\n",server_data.npc_base_reattackat);
	fprintf(file, "// Amount of stamina gained (or lost, if negative) on attacking. \n");
	fprintf(file, "// Must have equal to at least the absolute value of this to attack as well  \n");
	fprintf(file, "ATTACKSTAMINA %i\n",server_data.attackstamina);	// antichrist (6)
	fprintf(file, "}\n\n");

	fprintf(file, "SECTION VENDOR\n");
	fprintf(file, "{\n");
	fprintf(file, "// if 1, then item is sold by name rather than ID (doesn't work correctly)\n");
	fprintf(file, "SELLBYNAME %i\n",server_data.sellbyname);	// Magius(CHE)
	fprintf(file, "// maximum number of items a character can sell during one transaction \n");
	fprintf(file, "SELLMAXITEM %i\n",server_data.sellmaxitem);	// Magius(CHE)
	fprintf(file, "// advanced trade system, if 1, then goods prices do fluctuate (based on GOOD settings)\n");
	fprintf(file, "TRADESYSTEM %i\n",server_data.trade_system);	// Magius(CHE)
	fprintf(file, "// Restock every n seconds \n");
	fprintf(file, "RESTOCKRATE %i\n", ServerScp::g_nRestockTimeRate );
	fprintf(file, "// if 1, then difficulty is graded and varying quality pieces are made  \n");
	fprintf(file, "RANKSYSTEM %i\n",server_data.rank_system);// Moved by Magius(CHE)
	fprintf(file, "// x=minimum item price required to let the player pay for\n");
	fprintf(file, "// an item with gold from his bankbox \n");
	fprintf(file, "CHECKBANK %i\n",server_data.CheckBank); // Ripper
	fprintf(file, "// Default number of item selled by vendor for every item \n");
	fprintf(file, "SELLFORITEM %i\n",server_data.defaultSelledItem);
	fprintf(file, "}\n\n");

	fprintf(file, "SECTION PARTYSYSTEM\n");
	fprintf(file, "{\n");
	fprintf(file, "// Set to 1 to enable party members share karma gains and losses  \n");
	fprintf(file, "PARTYSHAREKARMA %i\n", Partys.shareKarma );
	fprintf(file, "// Set to 1 to enable party members share fame gains and losses  \n");
	fprintf(file, "PARTYSHAREFAME %i\n", Partys.shareFame );
	fprintf(file, "// Set to 1 to enable private party messages \n");
	fprintf(file, "ENABLEPRIVATEMSG %i\n", Partys.canPMsg );
	fprintf(file, "// Set to 1 to enable party members to send a message to all his friends.\n");
	fprintf(file, "ENABLEPUBLICMSG %i\n", Partys.canBroadcast );
	fprintf(file, "}\n\n");

	fprintf(file, "SECTION REGENERATE\n");
	fprintf(file, "{\n");
	fprintf(file, "// Define how often (seconds) will hitpoints regenerate by 1  \n");
	fprintf(file, "HITPOINTS_REGENRATE %i\n",server_data.hitpointrate);
	fprintf(file, "// Define how often (seconds) stamina will regenerate by 1  \n");
	fprintf(file, "STAMINA_REGENRATE %i\n",server_data.staminarate);
	fprintf(file, "// Define how often (seconds) mana will regenerate by 1  \n");
	fprintf(file, "MANA_REGENRATE %i\n",server_data.manarate);
	fprintf(file, "}\n\n");

	fprintf(file, "SECTION RESOURCE\n");
	fprintf(file, "{\n");
	fprintf(file, "// Dictates areas that can be mined. 0 is anywhere, 1 is mountains\n");
	fprintf(file, "// and cave floors, 2 is those areas that are flagged as mining regions\n");
	fprintf(file, "MINECHECK %i\n",server_data.minecheck);
	fprintf(file, "// Amount of ore in an area  \n");
	fprintf(file, "ORE_PER_AREA %i\n", ores.n);
	fprintf(file, "// Ore area width  \n");
	fprintf(file, "ORE_AREA_WIDTH %i\n", ores.area_width);
	fprintf(file, "// Ore area height  \n");
	fprintf(file, "ORE_AREA_HEIGHT %i\n", ores.area_height);
	fprintf(file, "//  How long it takes to respawn ore, in seconds \n");
	fprintf(file, "ORE_RESPAWN_TIME %i\n",ores.time);
	fprintf(file, "ORE_RESPAWN_RATE %i\n",ores.rate);
	fprintf(file, "// Stamina spent when mining \n");
	fprintf(file, "ORE_STAMINA %i\n", ores.stamina);
	fprintf(file, "// How many logs are in the area \n");
	fprintf(file, "LOGS_PER_AREA %i\n",resource.logs);
	fprintf(file, "// How long it takes for the logs to respawn  \n");
	fprintf(file, "LOG_RESPAWN_TIME %i\n",resource.logtime);
	fprintf(file, "LOG_RESPAWN_RATE %i\n",resource.lograte);
	fprintf(file, "// Number of log areas there are in the world \n");
	fprintf(file, "LOG_RESPAWN_AREA %i\n",resource.logarea);
	fprintf(file, "// Trigger fire for custom lumberjacking \n");
	fprintf(file, "LOG_TRIGGER %i\n",resource.logtrigger);
	fprintf(file, "// Stamina spent when lumberjacking \n");
	fprintf(file, "LOG_STAMINA %i\n",resource.logstamina);
	fprintf(file, "}\n\n");

	fprintf(file, "SECTION REPSYS\n");
	fprintf(file, "{\n");
	fprintf(file, "// Length of time (seconds) until the murder counts decrease by 1 \n");
	fprintf(file, "MURDER_DECAY %i\n", repsys.murderdecay);
	fprintf(file, "// Maximum amount of kills a character is allowed to have before he turns into a murderer\n");
	fprintf(file, "MAXKILLS %i\n", repsys.maxkills);
	fprintf(file, "// Length of time (seconds) before the criminal flag timer wears off\n");
	fprintf(file, "CRIMINAL_TIME %i\n", repsys.crimtime);
	fprintf(file, "// Check this to 1 to allow some spells to be cast in town\n");
	fprintf(file, "ALLOWMAGICINTOWN %d\n", ServerScp::g_nAllowMagicInTown);
	fprintf(file, "// Sets how the player become after snooping : 0:remains blue, 1:goes grey, 2:goes criminal\n");
	fprintf(file, "SNOOPWILLCRIMINAL %d\n", ServerScp::g_nSnoopWillCriminal);
	fprintf(file, "// Sets how the player become after stealing : 0:remains blue, 1:goes grey, 2:goes criminal\n");
	fprintf(file, "STEALWILLCRIMINAL %d\n", ServerScp::g_nStealWillCriminal);
	fprintf(file, "// Sets how the player become after chopping : 0:remains blue, 1:goes grey, 2:goes criminal\n");
	fprintf(file, "CHOPWILLCRIMINAL %d\n", ServerScp::g_nChopWillCriminal);
	fprintf(file, "// Sets how the player become after polymorphing : 0:remains blue, 1:goes grey, 2:goes criminal\n");
	fprintf(file, "POLYMORPHWILLCRIMINAL %d\n", ServerScp::g_nPolymorphWillCriminal);
	fprintf(file, "// Sets how the player become after looting a blue : 0:remains blue, 1:goes grey, 2:goes criminal\n");
	fprintf(file, "LOOTINGWILLCRIMINAL %d\n", ServerScp::g_nLootingWillCriminal);
	fprintf(file, "// Sets how the player become helping a grey : 0:remains blue, 1:goes grey, 2:goes criminal\n");
	fprintf(file, "HELPINGGREYWILLCRIMINAL %d\n", ServerScp::g_nHelpingGreyWillCriminal);
	fprintf(file, "// Sets how the player become helping a criminal : 0:remains blue, 1:goes grey, 2:goes criminal\n");
	fprintf(file, "HELPINGCRIMINALWILLCRIMINAL %d\n", ServerScp::g_nHelpingCriminalWillCriminal);
	fprintf(file, "// 1 if greys can be looted freely\n");
	fprintf(file, "GREYCANBELOOTED %d\n", ServerScp::g_nGreyCanBeLooted);
	fprintf(file, "// Set to 1 to enable Karma Lock when karma level drops down\n");
	fprintf(file, "ENABLEKARMALOCK %d\n", ServerScp::g_nEnableKarmaLock);
	fprintf(file, "// Karma unlock prayer\n");
	fprintf(file, "KARMAUNLOCKPRAYER %s\n", ServerScp::g_strKarmaUnlockPrayer);
	fprintf(file, "}\n\n");



	fprintf(file, "SECTION TIME_LIGHT\n");
	fprintf(file, "{\n");
	fprintf(file, "// Here follows current time parameters\n");
	fprintf(file, "ABSDAY %i\n", day);
	fprintf(file, "DAY %i\n", Calendar::g_nDay);
	fprintf(file, "HOUR %i\n", Calendar::g_nHour);
	fprintf(file, "MINUTE %i\n", Calendar::g_nMinute);
	fprintf(file, "MONTH %i\n", Calendar::g_nMonth);
	fprintf(file, "WEEKDAY %i\n", Calendar::g_nWeekday);
	fprintf(file, "YEAR %i\n", Calendar::g_nYear);
	fprintf(file, "// Here follows moon(s) status\n");
	fprintf(file, "MOON1UPDATE %i\n", moon1update);
	fprintf(file, "MOON2UPDATE %i\n", moon2update);
	fprintf(file, "MOON1 %i\n", moon1);
	fprintf(file, "MOON2 %i\n", moon2);
	fprintf(file, "SEASON %i\n", season); // lb
	fprintf(file, "// The light in dungeons\n");
	fprintf(file, "DUNGEONLIGHTLEVEL %i\n", dungeonlightlevel);
	fprintf(file, "// A fixed light level for the world\n");
	fprintf(file, "WORLDFIXEDLEVEL %i\n", worldfixedlevel);
	fprintf(file, "// Current light level\n");
	fprintf(file, "WORLDCURLEVEL %i\n", worldcurlevel);
	fprintf(file, "// World brightest light level\n");
	fprintf(file, "WORLDBRIGHTLEVEL %i\n", worldbrightlevel);
	fprintf(file, "// World darkest light level\n");
	fprintf(file, "WORLDDARKLEVEL %i\n", worlddarklevel);
	fprintf(file, "// Seconds to make a ingame time minute\n");
	fprintf(file, "SECONDSPERUOMINUTE %i\n", secondsperuominute); // lb
	fprintf(file, "// Season to display instead of standard winter one (mainly because of graphics leaks\n");
	fprintf(file, "// in the standard winter season ). Default is display spring (0). Winter is 3.\n");
	fprintf(file, "OVERRIDEWINTERSEASON %i\n", Calendar::g_nWinterOverride); // lb
	fprintf(file, "}\n\n");

	fprintf(file, "SECTION MAGIC_RESISTANCE\n");
	fprintf(file, "{\n");
	fprintf(file, "// Check this to 1 to display whenever a charachter is fire-resistant \n");
	fprintf(file, "SHOWFIRERESISTANCEINTITLE %i\n", ServerScp::g_nShowFireResistanceInTitle); // lb
	fprintf(file, "// Check this to 1 to display whenever a charachter is poison-resistant \n");
	fprintf(file, "SHOWPOISONRESISTANCEINTITLE %i\n", ServerScp::g_nShowPoisonResistanceInTitle); // lb
	fprintf(file, "// Check this to 1 to display whenever a charachter is freeze-resistant \n");
	fprintf(file, "SHOWPARALISYSRESISTANCEINTITLE %i\n", ServerScp::g_nShowParalisysResistanceInTitle); // lb
	fprintf(file, "// Check this to 1 to disable magic resistance raise when walk on fields \n");
	fprintf(file, "DISABLEFIELDRESISTANCERAISE %i\n", ServerScp::g_nDisableFieldResistanceRaise); // lb
 	fprintf(file, "}\n\n");

	fprintf(file, "SECTION REMOTE_ADMIN\n");
	fprintf(file, "{\n");
	fprintf(file, "// This is the TCP port where the telnet client connects to. \n");
	fprintf(file, "// Using 0 will disable it. \n");
	fprintf(file, "PORT %i\n", ServerScp::g_nRacTCPPort); // lb
	fprintf(file, "// Set to 0 to use line by line mode, to 1 to char by char mode \n");
	fprintf(file, "CHARMODE %i\n", ServerScp::g_nUseCharByCharMode); // lb
	fprintf(file, "}\n\n");

	fprintf(file, "SECTION NOXWIZARD_SYSTEM\n");
	fprintf(file, "{\n");
	fprintf(file, "// Set to 1 enables enhanced npc magic \n");
	fprintf(file, "NEWNPCMAGIC %i\n", ServerScp::g_nUseNewNpcMagic);
	fprintf(file, "}\n\n");

	fprintf(file, "SECTION LINUX\n");
	fprintf(file, "{\n");
	fprintf(file, "// Runs as a deamon under Linux.\n");
	fprintf(file, "// Output goes to the file specified in PRELOAD, STDOUTFILE \n");
	fprintf(file, "DEAMONMODE %i\n", ServerScp::g_nDeamonMode); // lb
	fprintf(file, "}\n\n");

	fprintf(file, "SECTION WINDOWS\n");
	fprintf(file, "{\n");
	fprintf(file, "// Set this to change the number of rows in the console window [console mode only] \n");
	fprintf(file, "LINEBUFFER %i\n", ServerScp::g_nLineBuffer); // lb
	fprintf(file, "// Set this to enable plug&play configuration of mul paths \n");
	fprintf(file, "// (Requires you have correctly installed a T2A/Renaissance client) \n");
	fprintf(file, "AUTODETECTMULS %i\n", ServerScp::g_nAutoDetectMuls); // lb
	fprintf(file, "}\n\n");


	fprintf(file, "SECTION MAP\n");
	fprintf(file, "{\n");
	fprintf(file, "// These are width and height of the map. Set to 768x512 for Britannian Maps, or \n");
	fprintf(file, "// to 288x200 for Ilshenar maps.\n");
	fprintf(file, "MAPWIDTH %i\n", map_width); // lb
	fprintf(file, "MAPHEIGHT %i\n", map_height); // lb
	fprintf(file, "}\n\n");

	fprintf(file, "SECTION WALKING\n");
	fprintf(file, "{\n");
	fprintf(file, "// Enable this if you want clients to be able to walk over water-on-earth tiles.  \n");
	fprintf(file, "// This may have bad side effects, do not enable if you're not sure on what you're doing!  \n");
	fprintf(file, "// It's primarly designed for custom MAP0.MULs.  \n");
	fprintf(file, "IGNOREWETBIT %i\n",ServerScp::g_nWalkIgnoreWetBit);
	fprintf(file, "}\n\n");

	fprintf(file, "SECTION NEWPLAYERS\n");
	fprintf(file, "{\n");
	fprintf(file, "// Initial gold for players  \n");
	fprintf(file, "INITIALGOLD %i\n",goldamount);
	fprintf(file, "// Initial priv1 value (don't touch if you don't know exactly what you're doing!)  \n");
	fprintf(file, "INITIALPRIV1 %i\n",defaultpriv1);
	fprintf(file, "// Initial priv2 value (don't touch if you don't know exactly what you're doing!)  \n");
	fprintf(file, "INITIALPRIV2 %i\n",defaultpriv2);
	fprintf(file, "}\n\n");

	fprintf(file, "SECTION START_LOCATIONS\n");
	fprintf(file, "{\n");
	fprintf(file, "// Raw list of starting locations  \n");
	fprintf(file, "// Note : you must always have exactly nine starting locations.  \n");
	fprintf(file, "// If you insert less starting locations, the first one is dupped for padding.  \n");
	fprintf(file, "// You can't use CFG commands on this section. \n");
	fprintf(file, "// \n");
	for (unsigned int i=0; i<startcount; i++)
	{
		fprintf(file, "CITY %s\n",start[i][0]);
		fprintf(file, "PLACE %s\n",start[i][1]);
		fprintf(file, "POSITION %s,%s,%s\n",start[i][2],start[i][3],start[i][4]);
	}
	fprintf(file, "}\n\n");


	fprintf(file, "//Use this section for Client Version Control. You can't use CFG commands on this section\n");
	fprintf(file, "SECTION CLIENTS_ALLOWED\n");
	fprintf(file, "{\n");

  for (; vis != vis_end;  ++vis)
  {
    t = (*vis).c_str();  // a bit pervert to store c++ strings and operate with c strings, admitably
	  strcpy(temp,t);
	  fprintf(file, "%s\n",temp);
  }

	fprintf(file, "}\n\n\n\n");


	fprintf(file, "SECTION BLOCK_ACC_PSS\n");		//elcabesa tempblock
	fprintf(file, "{\n");							//elcabesa tempblock
	fprintf(file, "// set to 1 if you want an account blocked due to many bad password\n");	//elcabesa tempblock
	fprintf(file, "BLOCKACCBADPASS %i\n",server_data.blockaccbadpass);						//elcabesa tempblock
	fprintf(file, "// number of times you can mistake password before account will be blocked.\n");	//elcabesa tempblock
	fprintf(file, "N_BADPASS %i\n",server_data.n_badpass);								//elcabesa tempblock
	fprintf(file, "// time in minutes the account stay blocked\n");							//elcabesa tempblock
	fprintf(file, "TIME_BLOCKED %i\n",server_data.time_badpass);							//elcabesa tempblock
	fprintf(file, "}\n\n");
}

int cfg_command (char *commandstr)
{
	char b[80];
	int i;

	//copy, uppercase and truncate
	for (i=0; i<79; i++)
	{
		if ((commandstr[i]>='a')&&(commandstr[i]<='z'))
			b[i] = commandstr[i]+ 'A'-'a';
		else b[i] = commandstr[i];
		if (b[i]=='\0') break;
	}
	b[79] = '\0'; //xan -> truncate to avoid overflowz

	char *section = b;
	char *property = NULL;
	char *value = NULL;
	int ln = strlen(b);

	for (i=0; i<ln; i++)
	{
		if (b[i]=='.')
		{
			b[i]='\0';
			property = b+i+1;
			break;
		}
	}

	if (property==NULL) return -3;

	ln = strlen(property);

	for (i=0; i<ln; i++)
	{
		if ((property[i]==' ')||(property[i]=='=')||(property[i]==':')||(property[i]==','))
		{
			property[i]='\0';
			value = property+i+1;
			break;
		}
	}
	if (value==NULL) return -4;

	gprop = property;
	gval = value;

	//now we have section, property and value.. parse them all :)
	return chooseSection(section, parseCfgLine);
}
