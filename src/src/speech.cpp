/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "common_libs.h"
#include "sndpkg.h"
#include "debug.h"
#include "calendar.h"
#include "set.h"
#include "house.h"
#include "npcai.h"
#include "network.h"
#include "commands.h"
#include "boats.h"
#include "inlines.h"
#include "classes.h"
#include "utils.h"

#define MAXBUFFER_REAL MAXBUFFER

static NxwCharWrapper*	nearbyNpcs = NULL;
//static pCharVector	*nearbyPcs  = 0;

extern void checkAmxSpeech(int s, char *speech);

void wchar2char (const char* str);
void char2wchar (const char* str);

static int32_t findKeyword( const std::string &str, const std::string &keyword );
static std::string trimString( const std::string &str );

int response(NXWSOCKET  s)
{
	pChar pc= cSerializable::findCharBySerial( currchar[s] );
	if ( ! pc || ! pc->IsOnline() ) return 0;

	//Araknesh I morti non vengono cagati :)
	//!\todo SPARHAWK --- Hmmm but what about ghost speak????? MUST CHECK THIS OUT
	if ( pc->dead ) return 0;

	Location charpos= pc->getPosition();

	int32_t i, j, skill=-1;

	char search1[50];


	int x=-1;
	int y=0;

	char *comm;
	static uint8_t nonuni[MAXBUFFER_REAL];
	static char buffer1[MAXBUFFER];
	if(pc->unicode)
	{
		j = (buffer[s][1]<<8)+buffer[s][2];
		for (i=13; i<j; i+=2 )
			nonuni[(i-13)/2]=buffer[s][i];
		for (i=0; i < MAXBUFFER_REAL-8; ++i)
			buffer1[i+8]=toupper(nonuni[i]); 	 // Sparhawk 	Let's forget about toupper and use strcasecmp for case insensitive comparisons
								 //		Needed for none english characters? Probably yes
	}
        else
        {
		ConOut("Non-Unicode speech\n");
		for (i=7; i < MAXBUFFER_REAL; i++)
			buffer1[i]=toupper(buffer[s][i]);	// Sparhawk 	Let's forget about toupper and use strcasecmp for case insensitive comparisons
								//		Needed for none english characters? Probably yes
	}
	comm=&buffer1[8];


	x= charpos.x;
	y= charpos.y;

	//
	// name
	//
	bool requestName = ( strstr( comm, "NAME") != NULL ) || ( strstr( comm, "WHO ARE YOU") != NULL );
	//
	// packup
	//	this should become <playervendorname> packup
	//	possibly add all packup
	//
	bool requestPackup = (strstr( comm, "PACKUP") != NULL);
	//
	// time
	//
	bool requestTime = (strstr( comm, "TIME") != NULL);
	//
	// location
	//
	bool requestLocation = (strstr( comm, "LOCATION") != NULL) || (strstr( comm, "WHERE AM I") != NULL);
	//
	// i will take thee
	//
	strcpy(search1,"I WILL TAKE THEE");
	bool requestAcceptEscort = ( strstr( comm, search1) != NULL );
	//
	// destination
	//
	strcpy(search1,"DESTINATION");
	bool requestDestination = ( strstr( comm, search1) != NULL );
	//
	// bank
	//
	bool requestBank = ( strstr( comm, "BANK") != NULL );
	//
	// balance
	//
    	bool requestBalance = ( strstr( comm, "BALANCE") != NULL );
    	//
    	// special bank
    	//
    	bool requestSpecialBank = false;
	if(SrvParms->usespecialbank)
		requestSpecialBank = (strstr( comm, SrvParms->specialbanktrigger) != NULL );

	//
	// train
	// train <skillname>
	//
	bool requestTrain = ( strstr( comm, "TRAIN") != NULL ) || ( strstr( comm, "TEACH") != NULL ) || ( strstr( comm, "LEARN") != NULL );
	//
	// <petname> follow
	//
	bool requestFollow = (strstr( comm, " FOLLOW") != NULL );
	//
	// <petname> follow me
	//
	bool requestFollowMe = (strstr( comm, " FOLLOW ME") != NULL );
	//
	// all attack
	//
	bool requestAllAttack = ( strstr( comm, "ALL ATTACK") != NULL ) || ( strstr( comm, "ALL KILL" ) != NULL );
	//
	// <petname> kill
	//
	bool requestAttack = ( strstr( comm, " KILL") != NULL ) || ( strstr( comm, " ATTACK") != NULL) ;
	//
	// <petname> fetch
	//
	bool requestFetch = (strstr( comm, " FETCH") !=NULL) || (strstr( comm, " GET") !=NULL);
	//
	// <petname> come
	//
	bool requestCome = ( strstr( comm, " COME") != NULL );
	//
	// <petname> guard me
	//
	bool requestGuardMe = ( strstr( comm, " GUARD ME" ) != NULL);
	//
	// <petname> guard
	//
	bool requestGuard = false;
	if (!requestGuardMe)
		requestGuard = ( strstr(comm, " GUARD") != NULL);
	//
	// <petname> stay
	//
	bool requestStay = ( strstr( comm, " STOP") != NULL ) || ( strstr( comm, " STAY") != NULL );
	//
	// <petname> transfer
	//
	bool requestTransfer = ( strstr( comm, " TRANSFER" ) != NULL );
	//
	// <petname> release
	//
	bool requestRelease = ( strstr( comm, " RELEASE" ) != NULL );
	//
	// release all
	//
	bool requestReleaseAll = ( strstr( comm, "RELEASE ALL" ) != NULL );


	bool handledRequest = false;

	NxwCharWrapper sc;
	sc.fillCharsNearXYZ( x, y, VISRANGE, true, false );

	for( sc.rewind(); !sc.isEmpty(); sc++ )
	{
		pChar pc_map = sc.getChar();
		if( pc_map ) {
		
			//
			// Sparhawk	char_dist() should be a configurable audiorange
			//		default configuration in server.cfg
			//		could be overriden through race definition
			//		could be overriden by npc definition
			//		also range should differ for whispering talking and shouting
			//
			if (pc_map->npc && ( pc->distFrom(pc_map) <= 4 ) )
			{
				//k=mapchar;
				//
				//	Disallow responses from dead or disabled npcs
				//
				if ( !TIMEOUT( pc_map->disabled ) )
				{
					switch ( pc_map->getId() )
					{
						case BODY_MALE		:
						case BODY_FEMALE	:
							pc_map->talkAll("Leave me alone!" , false);
							break;
						default		:
							if ( chance( 40 ) )
								pc_map->playMonsterSound(SND_IDLE);
							break;
					}
					handledRequest = true;
					continue;
				}

				//
				// 	Handle name request
				//		Sparhawk	certain classes like druids should be able to get a name from a non humanoid npc
				//				also for certain skill level in animal lore????
				//
				if ( requestName )
				{
					if( pc_map->npcaitype != NPCAI_EVIL )
					{
						switch ( pc_map->getId() )
						{
							case BODY_MALE:
							case BODY_FEMALE:
								pc_map->talkAll( "I am %s.", 0, pc_map->getCurrentName().c_str());
								break;
							default		:
								if ( chance( 40 ) )
									pc_map->playMonsterSound(SND_IDLE);
								break;
						}
					}
					else
					{
						switch ( pc_map->getId() )
						{
							case BODY_MALE		:
							case BODY_FEMALE	:
								pc_map->talkAll( "I %s will kill you.", false, pc_map->getCurrentName().c_str());
								break;
							default		:
								if ( chance( 40 ) )
									pc_map->playMonsterSound(SND_STARTATTACK);
								break;
						}
					}
					handledRequest = true;
					continue;
				}
				//
				// 	Handle packup player vendor request
				//		Sparhawk	packup should preferably handle items still held for sale by the vendor
				//				also all remaining accounting should be done
				//				<playervendorname> packup should be allowed
				//				packup should packup all player vendors within range
				//
				if ( requestPackup && pc_map->npcaitype == NPCAI_PLAYERVENDOR && (pc->isOwnerOf( pc_map ) || pc->IsGM()))
				{
					int pvDeed;
					// lets make the deed and place in your pack and delete vendor.
					strcpy( temp, "Employment deed" );
					pItem pDeed = item::CreateFromScript( "$item_employment_deed", pc->getBackpack() );
					if ( ! pDeed ) return true;
					pvDeed= DEREF_pItem(pDeed);

					pDeed->Refresh();
					pc_map->playMonsterSound(SND_DIE);
					pc_map->Delete();
					sysmessage(s, "Packed up vendor %s.", pc_map->getCurrentName().c_str());
					// return 1;
					handledRequest = true;
				}
				//
				// 	Handle time request
				//
				if ( requestTime )
				{
					if(pc_map->npcaitype!= NPCAI_EVIL && pc_map->npcaitype!= NPCAI_PLAYERVENDOR && (pc_map->getId()==0x0190 || pc_map->getId()==0x0191))
					{
						int hour = Calendar::g_nHour % 12;
						if (hour==0) hour = 12;
						int ampm = (Calendar::g_nHour>=12) ? 1 : 0;
						int minute = Calendar::g_nMinute;
						if (ampm || (!ampm && hour==12))
							pc_ma->talkAll("%s %2.2d %s %2.2d %s", false, "it is now", hour, ":", minute, "in the evening.");
						else
							pc_ma->talkAll("%s %2.2d %s %2.2d %s", false, "it is now", hour, ":",minute, "in the morning.");
						return 1;
					}
					handledRequest = true;
					continue;
				}
				//
				// 	Handle location request
				//
				if ( requestLocation )
				{
					if( pc_map->npcaitype!= NPCAI_EVIL && pc_map->npcaitype!= NPCAI_PLAYERVENDOR  && (pc_map->getId()==0x0190 || pc_map->getId()==0x0191))
					{
						if (strlen(region[pc->region].name)>0)
							pc_map->takAll("You are in %s", false, region[pc->region].name);
						else
							pc_map->takAll("You are in the wilderness", false);
						pc_map->talkAll( "%i %i (%i)", false, charpos.x, charpos.y, charpos.z );
						return 1;	// Sparhawk	No use having multiple npcs return same info
					}
					handledRequest = true;
					continue;
				}
				//
				// 	Handle Accept Escort
				//		Sparhawk	Need to take into account whether pc is allready engaged in escort quest
				//
				if ( requestAcceptEscort && pc_map->questType == cMsgBoard::ESCORTQUEST )
				{
					if ( pc_map->ftargserial == INVALID )
					{
						// Set the NPC to follow the PC
						pc_map->ftargserial = pc->getSerial();

						// Set the NPC to wander freely
						pc_map->npcWander = WANDER_FOLLOW;

						// Set the expire time if nobody excepts the quest
						pc_map->summontimer = ( getClock() + ( MY_CLOCKS_PER_SEC * SrvParms->escortactiveexpire ) );

						// Send out the rant about accepting the escort
						pc_map->talkAll("Lead on! Payment shall be made when we arrive at %s.", false, region[pc_map->questDestRegion].name);

						// Remove post from message board
						((pNPC)pc_map)->removepostEscortQuest();

						// Return 1 so that we indicate that we handled the message
						return 1;
					}
					else
						requestDestination = true;
				}
				//
				// 	Handle find out where escort wants to go
				//
				if ( requestDestination )
				{
					if ( pc_map->questType==cMsgBoard::ESCORTQUEST )
					{
						if ( pc_map->ftargserial == pc->getSerial() )
						{
							// Send out the rant about accepting the escort
							pc_map->talkAll("Lead on to %s. I shall pay thee when we arrive.", 0, region[pc_map->questDestRegion].name);
						}
						else if ( pc_map->ftargserial == INVALID )	// If nobody has been accepted for the quest yet
						{
							// Send out the rant about accepting the escort
							pc_map->talkAll("I am seeking an escort to %s. Wilt thou take me there?", false, region[pc_map->questDestRegion].name);
						}
						else // They must be enroute
						{
							// Send out a message saying we are already being escorted
							pChar pc_ftarg=cSerializable::findCharBySerial(pc_map->ftargserial);
							if( pc_ftarg ) {
								pc_map->talkAll("I am already being escorted to %s by %s.", false, region[pc_map->questDestRegion].name, pc_ftarg->getCurrentName().c_str() );
							}
						}
						// Return success ( we handled the message )
						return 1;
					}
					else
					{
						continue;
					}
				}
				//
				// 	Handle bank
				//		Sparhawk	Soundeffect needed let's try to create one depending on the value of banked stuff
				//
				if ( requestBank )
				{
					if ( pc_map->npcaitype == NPCAI_BANKER )
					{
						pc->openBankBox(pc);
						return 1;
					}
					else
					{
						continue;
					}
				}
				//
				// 	Handle balance
				//
				if ( requestBalance )
				{
					if ( pc_map->npcaitype == NPCAI_BANKER )
					{
						pc_map->talkAll("%s's balance as of now is %i.", true, pc->getCurrentName().c_str(), pc->countBankGold());
						return 1;
					}
					else
					{
						continue;
					}
				}
				//
				// 	Handle special bank
				//		Sparhawk	Soundeffect needed let's try to create one depending on the value of banked stuff
				//
				if( requestSpecialBank )
				{
					if ( pc_map->npcaitype == NPCAI_BANKER )
					{
						pc->openSpecialBank(pc);
						return 1;
					}
					else
						continue;
				}
				//
				// 	Handle train
				//
				//		Sparhawk	All npcs not only humanoids can teach skills
				//				<trainername> train should be allowed
				//
				if ( requestTrain && pc_map->cantrain && pc_map->HasHumanBody() )
				{
					pc->trainer = INVALID; //this is to prevent errors when a player says "train <skill>" then doesn't pay the npc
					for(i=0;i<ALLSKILLS;i++)
					{
						if( strstr( comm, skillname[i]) )
						{
							skill=i;  //Leviathan fix
							break;
						}
					}

					if(skill == INVALID) // Didn't ask to be trained in a specific skill - Leviathan fix
					{
						if( pc->trainer == INVALID) //not being trained, asking what skills they can train in
						{
							pc_map->trainingplayerin='\xFF'; // Like above, this is to prevent  errors when a player says "train <skill>" then doesn't pay the npc
							strcpy(temp,"I can teach thee the following skills: ");
							for(j=0;j<ALLSKILLS;j++)
							{
								if(pc_map->baseskill[j]>10)
								{
									sprintf(temp2,"%s, ", strlwr(skillname[j]));
									strupr(skillname[j]); // I found out strlwr changes the actual  string permanently, so this undoes that
									if(!y)
										temp2[0]=toupper(temp2[0]); // If it's the first skill,  capitalize it.
									strcat(temp,temp2);
									y++;
								}
							}
							if( y ) // skills and a trainer ?
							{
								temp[strlen(temp)-2]='.'; // Make last character a . not a ,	just to look nicer
								pc_map->talk(s, temp,0);
							}
							else
							{
							pc_map->talk(s, "I am sorry, but I have nothing to teach thee",0);
							}
							return 1;
						}
						handledRequest = true;
					}
					else // They do want to learn a specific skill
					{
						if( pc_map->baseskill[skill] > 10 )
						{
							x=skill;
							sprintf(temp,"Thou wishest to learn of %s?",strlwr(skillname[x]));
							strupr(skillname[x]); // I found out strlwr changes the actual string permanently, so this undoes that
							if(pc->baseskill[x] >= 250 )
							{
								strcat(temp, " I can teach thee no more than thou already knowest!");
							}
							else
							{
								uint32_t sum = pc->getSkillSum();
								if (sum >= SrvParms->skillcap * 10)
									strcat(temp, "I can teach thee no more. Thou already knowest too much!");
								else
								{
									int delta = pc_map->getTeachingDelta(pc, skill, sum);
									int perc = (pc->baseskill[x] + delta)/10;

									sprintf(temp2, "Very well I, can train thee up to the level of %i percent for %i gold. Pay for less and I shall teach thee less.",perc,delta);
									strcat(temp, temp2);
									pc->trainer=pc_map->getSerial();
									pc_map->trainingplayerin=x;
								}
								pc_map->talk(s, temp,0);
								return 1;
							}
						}
						pc_map->talk(s, "I am sorry but I cannot train thee in that skill.",0);
						// return 1;
						handledRequest = true;
					}
				}
				//
				// Handle follow
				//
				if ( requestFollow )
				{
					if (pc->isOwnerOf(pc_map) || (pc->IsGM())) //owner of the char || a GM
					{
						strcpy( search1, pc_map->getCurrentName().c_str() );
						strupr( search1 );
						bool requestPetname = ( strstr( comm, search1) != NULL );
						if ( requestPetname )
						{
							pc->guarded = false; // Hmmm still not very nice needs to be changed <sparhawk>
							if ( pc_map->npcaitype == NPCAI_PLAYERVENDOR )
							{
								pc_map->talk(s, "I am sorry but I have to mind the shop.",0);
								return 0;
							}
							if ( requestFollowMe )
							{
								pc_map->ftargserial = pc->getSerial();
								pc_map->npcWander = WANDER_FOLLOW;
								pc_map->playMonsterSound(SND_STARTATTACK);
								return 1;
							}
							//
							// Select target to follow
							//

							pTarget targ = clientInfo[s]->newTarget( new cCharTarget() );
							targ->buffer[0]=pc_map->getSerial();
							targ->code_callback=target_follow;
							targ->send( getClientFromSocket(s) );
							sysmessage( s, "Click on the target to follow." );
							return 1;
						}
					}
					else
						continue;
				}
				//
				// 	Handle Luxor's All Attack
				//
				if ( requestAllAttack )
				{
					NxwCharWrapper sc;
					sc.fillOwnedNpcs( pc, false, true );
					//I don't understand why i must check for animals stabled too... probably a bug
					if ( sc.size()==0 )
					{
						sysmessage(s,"You dont have pets following you");
						return 1;
					}
					if ( region[pc->region].priv&0x01 ) // Ripper..No pet attacking in town.
					{
						sysmessage(s,"You cant have pets attack in town!");
						return 1;
					}
					pc->guarded = false;
					pTarget targ=clientInfo[s]->newTarget( new cCharTarget() );
					targ->code_callback=target_allAttack;
					targ->send( getClientFromSocket( s ) );
					sysmessage( s, "Select the target to attack.");
					return 1;
				}
				//
				// 	Handle attack
				//
				if ( requestAttack )
				{
					if ( pc->isOwnerOf( pc_map ) || ( pc->IsGM() ) )
					{
						pc->guarded = false;
						strcpy(search1, pc_map->getCurrentName().c_str() );
						strupr( search1 );
						bool requestPetname = ( strstr( comm, search1) != NULL );
						if ( requestPetname )
						{
							if (region[pc->region].priv&0x01) // Ripper..No pet attacking in town.
							{
								sysmessage( s, "You can't have pets attack in town!");
								return 0;
							}
							if (pc_map->npcaitype== NPCAI_PLAYERVENDOR )
								return 0; //ripper

							pTarget targ = clientInfo[s]->newTarget( new cCharTarget() );
							targ->code_callback=target_playerVendorBuy;
							targ->buffer[0] = pc_map->getSerial();
							targ->send( getClientFromSocket(s) );
							//pet kill code here
							sysmessage( s, "Select the target to attack.");
							return 1;
						}
					}
					continue;
				}
				//
				// Handle fetch
				//
				if ( requestFetch &&  pc_map->npcaitype != NPCAI_PLAYERVENDOR )
				{
					if (pc->isOwnerOf(pc_map) || (pc->IsGM()))
					{
						strcpy(search1, pc_map->getCurrentName().c_str());
						strupr(search1);
						bool requestPetname = ( strstr( comm, search1) != NULL );
						if ( requestPetname )
						{
							pc->guarded = false;
							pTarget targ = clientInfo[s]->newTarget( new cObjectTarget() );
							targ->code_callback=target_fetch;
							targ->buffer[0]=pc_map->getSerial();
							targ->send( getClientFromSocket(s) );							
							sysmessage( s, "Click on the object to fetch.");
							return 1;
						}
					}
					continue;
				}
				//
				// Handle come
				//
				if ( requestCome )
				{
					if ( pc_map->npcaitype != NPCAI_PLAYERVENDOR && (pc->isOwnerOf(pc_map) || pc->IsGM()))
					{
						strcpy(search1, pc_map->getCurrentName().c_str());
						strupr(search1);
						bool requestPetname = (strstr( comm, search1) != NULL);
						if ( requestPetname )
						{
							pc->guarded = false;
							pc_map->ftargserial=pc->getSerial();
							pc_map->npcWander=WANDER_FOLLOW;
							sysmessage(s, "Your pet begins following you.");
							return 1;
						}
					}
					continue;
				}
				//
				// Handle guard
				//
				if ( (requestGuardMe || requestGuard ) )
				{
					if ( pc_map->npcaitype != NPCAI_PLAYERVENDOR && (pc->isOwnerOf(pc_map) || pc->IsGM()))
					{
						strcpy(search1, pc_map->getCurrentName().c_str());
						strupr(search1);
						bool requestPetname = ( strstr( comm, search1) != NULL);
						if (requestPetname)
						{
							pTarget targ=clientInfo[s]->newTarget( new cCharTarget() );
							targ->code_callback=target_guard;
							targ->buffer[0] = pc_map->getSerial();	// the pet's serial
							targ->buffer[1] = 0;
							if ( requestGuardMe )
								targ->buffer[1]=1;	// indicates we already know whom to guard (for future use)
										// for now they still must click on themselves (Duke)
							targ->send( getClientFromSocket( s ) );
							sysmessage( s, "Click on the char to guard.") ;

							return 1;
						}
					}
					continue;
				}
				//
				// Handle stay
				//	Sparhawk	allow for stay/stop without petname selection is done through target cursor
				//
				if ( requestStay )
				{
					if (pc_map->npcaitype !=NPCAI_PLAYERVENDOR && (pc->isOwnerOf(pc_map) || pc->IsGM()))
					{
						strcpy( search1, pc_map->getCurrentName().c_str());
						strupr( search1 );
						if ( strstr( comm, search1) != NULL ) //if petname is in
						{
							pc->guarded = false; // Sparhawk	How about when more than 1 pets is guarding me??
							//pet stop code here
							pc_map->ftargserial= INVALID;
							pc_map->targserial= INVALID;
							if ( pc_map->war )
								pc_map->toggleCombat();	// Sparhawk Allmost all pet commands need to do this and non pet commands need
												// to check for war status
							pc_map->npcWander=WANDER_NOMOVE;
							return 1;
						}
					}
					continue;
				}
				//
				// Handle transfer ownership of pet
				//
				if ( requestTransfer )
				{
					if ( pc_map->npcaitype != NPCAI_PLAYERVENDOR && (pc->isOwnerOf(pc_map) || pc->IsGM()) )
					{
						strcpy( search1, pc_map->getCurrentName().c_str() );
						strupr( search1 );
						if ( strstr( comm, search1 ) != NULL )
						{
							pc->guarded = false; // Sparhawk	How about when more than 1 pets is guarding me??
							
							pFunctionHandle evt = pc_map->getEvent(cNPC::evtNpcOnTransfer);
							if ( evt )
							{
								tVariantVector params = tVariantVector(2);
								params[0] = pc_map->getSerial(); params[1] = pc->getSerial();
								evt->setParams(params);
								evt->execute();
								if ( evt->bypassed() )
									return 0;
							}
	
							//pet transfer code here
							pTarget targ = clientInfo[s]->newTarget( new cCharTarget() );
							targ->code_callback=target_transfer;
							targ->buffer[0]=pc_map->getSerial();
							targ->send( getClientFromSocket(s) );
							sysmessage( s, "Select character to transfer your pet to.");
							return 1;
						}
					}
					continue;
				}
				//
				// Handle release pet
				//
				if ( requestRelease )
				{
					if ( pc_map->npcaitype != NPCAI_PLAYERVENDOR && (pc->isOwnerOf(pc_map) || pc->IsGM() ))
					{
						strcpy(search1, pc_map->getCurrentName().c_str());
						strupr(search1);
						if ( strstr( comm, search1) != NULL )
						{
							pc->guarded = false; // Sparhawk	How about when more than 1 pets is guarding me??
							if (pc_map->summontimer)
							{
								pc_map->summontimer=getClock();
							}
							//pet release code here
							pc_map->ftargserial=INVALID;
							pc_map->npcWander=WANDER_FREELY_CIRCLE;
							pc_map->setOwnerSerial32(INVALID);
							pc_map->taming=2000;//he cannot be retamed	Sparhawk	This is bullshit!!!
							//taken from 6904t2(5/10/99) - AntiChrist
							pc_map->tamed = false;
							pc_map->talkAll("*%s appears to have decided that it is better off without a master *", false, pc_map->getCurrentName().c_str());
							pc_map->playSFX( 0x01FE );
							if(SrvParms->tamed_disappear==1)
								pc_map->Delete();
							return 1;
						}
					}
					continue;
				}
				//
				// Handle release all pets
				//
				if ( requestReleaseAll )
				{
					if( pc_map->npcaitype != NPCAI_PLAYERVENDOR && (pc->isOwnerOf(pc_map) || pc->IsGM() ) )
					{
						if (pc_map->summontimer)
						{
							pc_map->summontimer=getClock();
						}
						//pet release code here
						pc_map->ftargserial=INVALID;
						pc_map->npcWander=WANDER_FREELY_CIRCLE;
						pc_map->setOwnerSerial32(INVALID);
						pc_map->taming=2000;//he cannot be retamed
						pc_map->tamed = false;
						pc_map->talkAll("*%s appears to have decided that it is better off without a master *", false, pc_map->getCurrentName().c_str());
						pc_map->playSFX( 0x01FE);
						if(SrvParms->tamed_disappear==1)
							pc_map->Delete();
						handledRequest = true;	// Sparhawk	all pets in vicinity should be released
					}
					else
						continue;
				}
				//
				// 	Handle speech trigger
				//
				if ( pc_map->trigger && ( pc_map->trigword.length()>0 ) )
				{
					if (pc_map->trigword.length()>0)
					{
						strcpy(search1, pc_map->trigword.c_str());
						strupr(search1);
						bool requestTrigger = ( strstr( comm, search1) != NULL );
						if ( requestTrigger )
						{
							if ( !TIMEOUT( pc_map->disabled ) )//AntiChrist
								pc_map->talkAll("I'm a little busy now! Leave me be!",0);
							else
							{
								triggerNpc(s,pc_map,TRIGTYPE_NPCWORD);
							}
							//return 1;	// Sparhawk multiple npcs should be triggered if having the same trigword
							handledRequest = true;
						}
					}
					continue;
				}
			}
		}

	}

	if ( handledRequest )
		return 1;
	else
		return 0;


}

void PlVGetgold(NXWSOCKET s, CHARACTER v)//PlayerVendors
{
	if ( s < 0 || s >= now ) //Luxor
		return;
	pChar pc_currchar = cSerializable::findCharBySerial( currchar[s] );
	pChar pc_vendor = cSerializable::findCharBySerial(v);
	
	if ( ! pc_currchar || ! pc_vendor ) return;

	unsigned int pay=0, give=pc_vendor->holdg, t=0;
 	char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var

	if (pc_currchar->isOwnerOf( pc_vendor ))
	{
		if ( pc_vendor->holdg<1)
		{
			pc_vendor->talk(s,"I have no gold waiting for you.",0);
			pc_vendor->holdg=0;
			return;
		}

		if( pc_vendor->holdg <= 65535 )
		{
			if ( pc_vendor->holdg>9)
			{
				pay=(int)(pc_vendor->holdg*.1);
				give-=pay;
			}
			else
			{
				pay = pc_vendor->holdg;
				give=0;
			}
			pc_vendor->holdg=0;
		}
		else
		{
			t = pc_vendor->holdg-65535;
			pc_vendor->holdg=65535;
			pay=6554;
			give=58981;
		}
		if (give) //Luxor
			item::CreateFromScript( "$item_gold_coin", pc_currchar->getBackpack(), give );

		sprintf(temp, "Today's purchases total %i gold. I am keeping %i gold for my self. Here is the remaining %i gold. Have a nice day.",pc_vendor->holdg,pay,give);
		pc_vendor->talk(s,temp,0);
		pc_vendor->holdg=t;
	}
	else
		pc_vendor->talk(s,"I don't work for you!",0);
}

void responsevendor(NXWSOCKET  s, CHARACTER vendor)
{

//	CHARACTER cc=currchar[s];

	pChar pc_currchar = cSerializable::findCharBySerial(currchar[s]);
	pChar pc_vendor = cSerializable::findCharBySerial(vendor);
	if ( ! pc_currChar || ! pc_vendor ) return;

	static char buffer1[MAXBUFFER_REAL]; // static becasue maxbuffer_ral close to stack limit of win-machines
	int i;
	char *comm;

	char *response1;
	char *response2;
	char *response3;
	char *response4;

	char search1[50];
	char search2[50];
	char search3[50];
	char search4[50];

	static char nonuni[MAXBUFFER_REAL];

	if(pc_currchar->unicode)
	{
		wchar2char( (char*) &buffer[s][13]);
		strcpy(nonuni, Unicode::temp);
	}

	int MB = MAXBUFFER_REAL;
	//if (server_data.packetsendstyle==PSS_UOX3) MB = MAXBUFFER_ASYNCH;
	// xan -> not enabled yet :|
	////////// ouch, this hurts !!!

	if(!(pc_currchar->unicode))
	{
	for (i=7; i < MB; i++)
		{
			tbuffer[i]=buffer1[i];
			buffer1[i]=toupper(buffer[s][i]);
		}
	}
	else
	{
	for (i=0; i < MB-8; i++)
		{
			tbuffer[i+8]=buffer1[i+8];
			buffer1[i+8]=toupper(nonuni[i]);
		}
	}
	/// end of that hurts

		comm=&buffer1[8];

		// Player said vendor BUY or vendor sell
		// In that case we alerady have the vendor-number from hte calling function
		// and dont need to search again
		strcpy(search1, "VENDOR");
		strcpy(search2, "SHOPKEEPER");
		strcpy(search4, " BUY");
		response1=(strstr(comm, search1));
		response2=(strstr(comm, search2));
		response4=(strstr(comm, search4));

		if (response4)//AntiChrist
		{
			if (response2 || response1)
			{
				if(pc_vendor->npcaitype==NPCAI_PLAYERVENDOR)
				{
					pc_vendor->talk(s,"What would you like to buy?",0);
					pTarget targ = clientInfo[s]->newTarget( new cItemTarget() );
					targ->code_callback=target_playerVendorBuy;
					targ->buffer[0]=pc_vendor->getSerial();
					targ->send( getClientFromSocket(s) );
					return; // lb bugfix
				}
				else if(BuyShop(s, DEREF_pChar(pc_vendor)))
					return; // lb bugfix
			}
		}

		//PlayerVendors
		response4=0;
		strcpy(search4, " GOLD");
		response4=(strstr( comm, search4));
		if (response4)//AntiChrist
		{
			if (response2 || response1)
			{
				if (pc_vendor->npcaitype==NPCAI_PLAYERVENDOR)
				{
					PlVGetgold(s, DEREF_pChar(pc_vendor));
					return;
				}
			}
		}

		//end PlayerVendors --^

		response4=0;
		strcpy(search4, " SELL");
		response4=(strstr( comm, search4));
		if (response4)//AntiChrist
		{
			if (response2 || response1)
			{
				sellstuff(s, DEREF_pChar(pc_vendor));
				return;
			}
		}


		// IF vendors are referred by NAME the vendor number passed by
		// doesnt have to be correct, thus we have to search again.
		// LB

		response4=strstr(comm," BUY");

		if (response4)//AntiChrist
		{
			
			NxwCharWrapper sc;
			sc.fillCharsNearXYZ( pc_currchar->getPosition(), 2, true, false );
			for( sc.rewind(); !sc.isEmpty(); sc++ ) {
				pChar pc = sc.getChar();
				if(!pc)
					continue;
				strcpy(search3, pc->getCurrentName().c_str());
				strupr(search3);
				response3=(strstr( comm, search3));
				if (response3)
				{
					if(pc->npcaitype==NPCAI_PLAYERVENDOR)
					{
						pc->talk(s,"What would you like to buy?",0);
						pTarget targ= clientInfo[s]->newTarget( new cItemTarget() );
						targ->code_callback = target_playerVendorBuy;
						targ->buffer[0]=pc->getSerial();
						targ->send( getClientFromSocket(s) );
						return;
					}
					else
						if(BuyShop(s, DEREF_pChar(pc)))
							return;
				}
			}
		} // if response4


		//PlayerVendors
		response4=0;
		strcpy(search4, " GOLD");
		response4=(strstr( comm, search4));

		if (response4)
		{
			NxwCharWrapper sc;
			sc.fillCharsNearXYZ( pc_currchar->getPosition(), VISRANGE, true, false );

			for( sc.rewind(); !sc.isEmpty(); sc++ )
			{
				pChar pc = sc.getChar();
				if(!pc)
					continue;

				strcpy(search3, pc->getCurrentName().c_str());
				strupr(search3);
				response3=(strstr( comm, search3));
				if (response3)
				{
					if (pc->npcaitype==NPCAI_PLAYERVENDOR)
					{
						PlVGetgold(s, DEREF_pChar(pc));
						return;
					}
				}
			}
		}


		response4=0;
		strcpy(search4, " SELL");
		response4=(strstr( comm, search4));

		if (response4)
		{
			NxwCharWrapper sc;
			sc.fillCharsNearXYZ( pc_currchar->getPosition(), VISRANGE, true, false );

			for( sc.rewind(); !sc.isEmpty(); sc++ )
			{
				pChar pc = sc.getChar();
				if(!pc)
					continue;
				strcpy(search3, pc->getCurrentName().c_str());
				strupr(search3);
				response3=(strstr( comm, search3));
				if (response3)
				{
					sellstuff(s, DEREF_pChar(pc));
					return;
				}
			}
		} // if response4
	return;
}

/*
static bool respond( pChar pc, NXWSOCKET socket, string &speech )
{
	bool success = false;
	pCharVector	nearbyPlayerVendors;
	uint32_t		i = 0;
	uint32_t		j = nearbyNpcs->size();
	while( i < j )
	{
		pc_a_npc = (*nearbyNpcs)[i];
		if( pc_a_npc->npc_ai == NPCAI_PLAYERVENDOR )
			nearbyPlayerVendors.push_back(pc_a_npc);
		++i;
	}
	if( !nearbyPlayerVendors.empty() )
	{
	}
	return success;
}
*/

static bool pricePlayerVendorItem( pChar pc, NXWSOCKET socket, string &price )
{
	bool success = false;
	if ( pc->fx2 == 17 )
	{
		int i = str2num( const_cast<char*>(price.c_str()) );
		pItem pi = MAKE_ITEM_REF( pc->fx1 );
		if( pi )
		{
			if (i>0)
			{
				pi->value = i;
				pc->fx2 = 18;
				sysmessage( socket, "The price of item %s has been set to %i.", pi->getCurrentName().c_str(), i);
				sysmessage( socket, "Enter a description for this item.");
			}
			else
			{
				pc->fx2 = 18;
				sysmessage( socket, "No price entered, this item's price has been set to %i.", pi->value);
				sysmessage( socket, "Enter a description for this item.");
			}
		}
		else
		{
			sysmessage( socket, "This item is invalid and cannot be priced" );
			pc->fx1 = INVALID;
			pc->fx2 = INVALID;
		}
		success = true;
	}
	return success;
}

static bool describePlayerVendorItem( pChar pc, NXWSOCKET socket, string &description )
{
	bool success = false;
	if( pc->fx2 == 18 )
	{
		pItem pi = MAKE_ITEM_REF( pc->fx1 );
		if( pi )
		{
			//strcpy( pi->desc, description.c_str() );
			pi->vendorDescription = description;
			sysmessage( socket, "This item is now described as %s, ", description.c_str() );
		}
		else
			sysmessage( socket, "This item is invalid and cannot be described");
		pc->fx1 = INVALID;
		pc->fx2 = INVALID;
		success = true;
	}
	return success;
}

static bool renameRune( pChar pc, NXWSOCKET socket, string &name )
{
	bool success = false;
	pItem pi = cSerializable::findItemBySerial( pc->runeserial );
	if( pi )
	{
		pi->setCurrentName("Rune to %s", name.c_str() );
		sysmessage( socket, "Rune renamed to: Rune to %s", name.c_str() );
		pc->runeserial = INVALID;
		success = true;
	}
	return success;
}

static bool renameSelf( pChar pc, NXWSOCKET socket, string &name )
{
	bool success = false;
	if( pc->namedeedserial != INVALID )
	{
		pItem pi = cSerializable::findItemBySerial( pc->namedeedserial );
		if( pi )
		{
			pi->setCurrentName( name.c_str());
			pc->setCurrentName( name.c_str());
			sysmessage(socket, "Your new name is: %s", name.c_str());
		}
		else
			sysmessage(socket, "Invalid namedeed");
		pc->namedeedserial = INVALID;
		success = true;
	}
	return success;
}

static bool renameKey( pChar pc, NXWSOCKET socket, string &name )
{
	bool success = false;
	pItem pi = cSerializable::findItemBySerial( pc->keyserial );
	if( pi )
	{
		pi->setCurrentName( name.c_str() );
		sysmessage( socket, "Key renamed to: %s", name.c_str() );
		success = true;
	}
	pc->keyserial = INVALID;
	return success;
}

static bool pageCouncillor( pChar pc, NXWSOCKET socket, string &reason )
{
	bool success = false;
	if (pc->pagegm == 2) // Counselor page
	{
		char temp[TEMP_STR_SIZE];
		strcpy( counspages[pc->playercallnum].reason, reason.c_str() );
		sprintf(temp, "Counselor Page from %s [%08x]: %s",pc->getCurrentName().c_str(), pc->getSerial(), counspages[pc->playercallnum].reason);
		bool foundCons = false;
		pChar councillor;
		
		NxwSocketWrapper sw;
		sw.fillOnline( );
		
		for( sw.rewind(); !sw.isEmpty(); sw++ )
		{
			NXWCLIENT ps=sw.getClient();
			if( ps==NULL )
				continue;
			
			councillor = ps->currChar();
			if( councillor && councillor->IsCounselor() )
			{
				foundCons = true;
				sysmessage(ps->toInt(), temp);
			}
		}
		if (foundCons)
			sysmessage(socket, "Available Counselors have been notified of your request.");
		else
			sysmessage(socket, "There was no Counselor available to take your call.");
		pc->pagegm = 0;
		success = true;
	}
	return success;
}

static bool resignFromGuild( pChar pc, NXWSOCKET socket, string &resign )
{
	bool success = false;
	if (!resign.compare("I RESIGN FROM MY GUILD"))
	{
		Guilds->Resign( pc, socket );
		success = true; // just to make sure this speech is not processed further
	}
	return success;
}

static bool callGuards( pChar pc, NXWSOCKET socket, string &helpcall )
{
	bool success = false;
	if( helpcall.find("GUARDS") != std::string::npos )
	{
		//
		// TODO Check if not part of another word or part of not help call related speech
		//
		callguards(cSerializable::findCharBySerial(currchar[socket]));
		success = true;
	}
	return success;
}

namespace Speech
{
namespace Stablemaster
{
static bool respond( pChar pc, NXWSOCKET socket, std::string &speech );
static bool stablePet( pChar pc, NXWSOCKET socket, std::string &speech, NxwCharWrapper &nearbyStablemasters );
static bool claimPet( pChar pc, NXWSOCKET socket, std::string &speech, NxwCharWrapper &nearbyStablemasters );

static bool respond( pChar pc, NXWSOCKET socket, std::string &speech )
{
	bool 	success = false;
	NxwCharWrapper	nearbyStablemasters;
	pChar		pc_a_npc;

	for( nearbyNpcs->rewind(); !nearbyNpcs->isEmpty(); (*nearbyNpcs)++ )
	{
		pc_a_npc = nearbyNpcs->getChar();
		if( (pc_a_npc->npc_type == 1) && ( pc_a_npc->distFrom( pc ) <= 5 ) )
			nearbyStablemasters.insertChar( pc_a_npc );
	}

	nearbyStablemasters.rewind();

	if( !nearbyStablemasters.isEmpty() )
	{
		if( !success )	success = stablePet( pc, socket, speech, nearbyStablemasters );
		if( !success )	success = claimPet( pc, socket, speech, nearbyStablemasters );
	}
	return success;
}

static bool stablePet( pChar pc, NXWSOCKET socket, std::string &speech, NxwCharWrapper &nearbyStablemasters )
{
/*
	command					action
	--------------------------------	--------------
	stable					stablemaster nearest to pc stables pet nearest to stablemaster

	stable <pet>				stablemaster nearest to pc stables pet with name <pet>
						if multiple pets with same name exist the one nearest to the stablemaster will be selected

	stable all				stablemaster nearest to pc stables all pets

	<stable master> stable			stablemaster with name <stable master> stables pet nearest to him/her

	<stable master> stable <pet>		stablemaster with name <stable master> stables pet with <name>
						if multiple pets with same name exist the one nearest to the stablemaster

	<stable master> stable all		stablemaster with name <stable master> stables all pets

	<stable master> stable all <pet>	stablemaster with name <stable master> stables all pets with name <pet>
*/
	bool success = false;
	int32_t tokenPosition = findKeyword( speech, "STABLE");
	if( tokenPosition != INVALID )
	{
		std::string 	stablemasterName( trimString( speech.substr( 0, tokenPosition ) ) );
		bool 	findStablemasterByName = (stablemasterName.length() != 0);
		std::string 	petName( trimString( speech.substr( tokenPosition + 6) ) );
		tokenPosition = findKeyword( petName, "ALL");
		bool stableAllPets = (tokenPosition == 0 );
		if( stableAllPets )
			petName = trimString( petName.substr( 3 ) );
		bool 	findPetByName = (petName.length() != 0);
		//
		// Find stable master, in case of multiple stable masters the one nearest will be selected
		//
		pChar		pc_stablemaster = 0;
		pChar		pc_pet = 0;
		pChar		pc_a_npc;
		NxwCharWrapper	petsToStable;
		bool		petFound = false;

		for( nearbyStablemasters.rewind(); !nearbyStablemasters.isEmpty(); nearbyStablemasters++  )
		{
			pc_a_npc = nearbyStablemasters.getChar();
			if( (!findStablemasterByName ||
			    (findStablemasterByName && !strcasecmp(stablemasterName.c_str(), pc_a_npc->getCurrentName().c_str()))))
			{
				if( !pc_stablemaster )
					pc_stablemaster = pc_a_npc;
				else
					if( (!pc_a_npc->war) && ( pc_stablemaster->distFrom( pc ) >  pc_a_npc->distFrom( pc ) ) )
						pc_stablemaster = pc_a_npc;
			}
		}
		if( !pc_stablemaster )
			success = true;
		else
		{
			for( nearbyNpcs->rewind(); !nearbyNpcs->isEmpty(); (*nearbyNpcs)++ )
			{
				pc_a_npc = nearbyNpcs->getChar();
				if( pc->isOwnerOf( pc_a_npc ) && !pc_a_npc->isStabled() && !pc_a_npc->mounted )
				{
					if( (!findPetByName) || (findPetByName && !strcasecmp( petName.c_str(), pc_a_npc->getCurrentName().c_str())))
						if( pc->distFrom( pc_stablemaster ) <= 8 )
							if( stableAllPets )
							{
								petFound = true;
								petsToStable.insertChar( pc_a_npc );
							}
							else
								if( !petFound )
								{
									petFound = true;
									petsToStable.insertChar( pc_a_npc );
								}
								//else
									//
									// Multiple pets select nearest ( NOT IMPLEMENTED YET )
									//
									//if( char_dist( pc_stablemaster, pc_a_npc ) < char_dist( pc_stablemaster, petsToStable[0] ) )
									//	petsToStable[0] = pc_a_npc;
				}
			}
			if( !petFound )
			{
				pc_stablemaster->talk( socket, "Which pet?", 0);
				return true;
			}
			else
			{
				for( petsToStable.rewind(); !petsToStable.isEmpty(); petsToStable++ )
				{
					pc_pet = petsToStable.getChar();
					
					uint32_t pc_pet_serial = pc_pet->getSerial();
		 
					NxwSocketWrapper sw;
					sw.fillOnline( pc_pet, false );
					for ( sw.rewind(); !sw.isEmpty(); sw++ ) {
						NXWCLIENT ps=sw.getClient();
						if( ps==NULL )
							continue;
						
						cPacketSendDeleteObj pk(pc_pet);
						client->sendPacket(&pk);

					}

					pc_pet->war=0;
					//p_pet->ftarg=-1;
					//p_pet->targ=-1;
					pc_pet->attackerserial= INVALID;
					pc->war  	= 0;
					pc->targserial	= INVALID;
#ifdef SPAR_C_LOCATION_MAP
					pointers::delFromLocationMap( pc_pet );
#else
					mapRegions->remove( pc_pet );
#endif
					pc_pet->stable( pc_stablemaster );
					// set timer for fee calculation
					pc_pet->time_unused=0;
					pc_pet->timeused_last = getclock();
				}
				char temp[TEMP_STR_SIZE];
				if( petsToStable.size() == 1 )
					sprintf(temp,"I have stabled %s", pc_pet->getCurrentName().c_str());
				else
					sprintf(temp,"I have stabled %d pets", petsToStable.size() );
				pc_stablemaster->talk(socket,temp,0);
			}
			success = true;
		}
	}
	return success;
}

static bool claimPet( pChar pc, NXWSOCKET socket, std::string &speech, NxwCharWrapper &nearbyStablemasters )
{
	/*
		command					Action
		---------------------------------	-------------
		claim					return a pet
		claim <pet>				return a pet named <pet>
		claim all				return all pets
		claim all <pet>				return all pets named <pet>
		<stablemaster> claim
		<stablemaster> claim <pet>
		<stablemaster> claim all <pet>
	*/
	bool success = false;
	int32_t tokenPosition = findKeyword( speech, "CLAIM");
	if( tokenPosition != INVALID )
	{
		std::string 	stablemasterName( trimString( speech.substr( 0, tokenPosition ) ) );
		bool 	findStablemasterByName = (stablemasterName.length() != 0);
		std::string 	petName( trimString( speech.substr( tokenPosition + 5) ) );
		tokenPosition = findKeyword( petName, "ALL");
		bool 	claimAllPets = (tokenPosition == 0 );
		if( claimAllPets )
			petName = trimString( petName.substr( 3 ) );
		bool 	findPetByName = (petName.length() != 0);
		pChar		pc_a_npc = 0;
		pChar		pc_stablemaster = 0;
		pChar		pc_pet = 0;
		bool 	found = false;
		NxwCharWrapper	stabledPets;
		for( nearbyStablemasters.rewind(); !nearbyStablemasters.isEmpty() && !found; nearbyStablemasters++ )
		{
			pc_a_npc = nearbyStablemasters.getChar();
			if( (!findStablemasterByName ||
			    (findStablemasterByName && !strcasecmp( stablemasterName.c_str(), pc_a_npc->getCurrentName().c_str()))))
			{
				if( !pc_stablemaster )
					pc_stablemaster = pc_a_npc;
				else
					if( pc_stablemaster->distFrom( pc ) > pc_a_npc->distFrom( pc ) )
						pc_stablemaster = pc_a_npc;
				//
				// Stable master found, check stabled animals
				//
				if( !pc_stablemaster->war )
				{
					int32_t ii = 0;
					while ( ( pc_a_npc = pointers::stableSearch( pc_stablemaster->getSerial(), &ii ) ) != 0 )
					{
						if( pc->isOwnerOf( pc_a_npc ) )
							if(findPetByName)
							{
								if(!strcasecmp(petName.c_str(),pc_a_npc->getCurrentName().c_str()))
								{
									found = true;
									stabledPets.insertChar( pc_a_npc );
									if( !claimAllPets )
										break;
								}
							}
							else
							{
								stabledPets.insertChar( pc_a_npc );
								if( !claimAllPets )
								{
									found = true;
									break;
								}
							}
					}
				}
			}
		}
		
stabledPets.rewind();	// GH!

		if( stabledPets.isEmpty() )
		{
				for( nearbyStablemasters.rewind(); !nearbyStablemasters.isEmpty(); nearbyStablemasters++ )
				{
					pc_stablemaster = nearbyStablemasters.getChar();
					if( pc_stablemaster->war )
						pc_stablemaster->talk( socket, "I'm busy!", 0 );
					else
						pc_stablemaster->talk( socket, "I don't have it", 0 );
				}
			success = true;
		}
		else
		{
			//
			// Calculate and display stabling fee = (fee per 10 minutes) * number of 10 minute blocks + 5 basefee
			//
			float 	f_fee	= 0.0f;
			uint32_t	fee	= 0;
			//
			// Calculate fee. Game master stable for free, everyone else has to pay
			//
			if( !pc->IsGM() )
			{
				for( stabledPets.rewind(); !stabledPets.isEmpty(); stabledPets++  )
				{
					pc_pet	 = stabledPets.getChar();
					f_fee	+= ( pc_pet->time_unused / 600.0f) * SrvParms->stablingfee;
					fee	+= ( (int) f_fee) + 5;
				}
			}
			pc_stablemaster->talk( socket, "That's %i gold pieces", false, fee);
			//
			// Check pet owner financials
			//
			if( pc->CountGold() >= fee )
			{
				pc->getBackpack()->DeleteAmount( fee, 0x0EED, INVALID );
				//
				// Unstable pet
				//
				for( stabledPets.rewind(); !stabledPets.isEmpty(); stabledPets++  )
				{
					pc_pet = stabledPets.getChar();
					
					pc_pet->unStable();
					
					pc_pet->timeused_last = getclock();
					pc_pet->time_unused=0;
#ifdef SPAR_C_LOCATION_MAP
					pointers::addToLocationMap( pc_pet );
#else
					mapRegions->add( pc_pet );
#endif
					pc_pet->teleport();
				}
				if( stabledPets.size() == 1 )
					pc_stablemaster->talk( socket, "Thx! Here's your pet", false);
				else
					pc_stablemaster->talk( socket, "Thx! Here're your %d pets", false, stabledPets.size());
			}
			else
				pc_stablemaster->talk( socket, "You don't have %i gold pieces on you!", false, fee );
			success = true;
		}
	}
	return success;
}

} //namespace Stablemaster

namespace Guard
{
static bool respond( pChar pc, NXWSOCKET socket, std::string &speech );
static bool requestChaosShield( pChar pc, NXWSOCKET socket, std::string &speech, NxwCharWrapper &nearbyGuards );
static bool requestOrderShield( pChar pc, NXWSOCKET socket, std::string &speech, NxwCharWrapper &nearbyGuards );
static bool requestHelp( pChar pc, NXWSOCKET socket, std::string &speech, NxwCharWrapper &nearbyGuards );

static bool respond( pChar pc, NXWSOCKET socket, std::string &speech )
{
	bool success = false;
	NxwCharWrapper	nearbyGuards;
	NxwCharWrapper	nearbyOrderGuards;
	NxwCharWrapper	nearbyChaosGuards;
	pChar		pc_a_npc;

	for( nearbyNpcs->rewind(); !nearbyNpcs->isEmpty(); (*nearbyNpcs)++ )
	{
		pc_a_npc = nearbyNpcs->getChar();
		switch( pc_a_npc->npcaitype )
		{
			case NPCAI_GUARD	:
			case NPCAI_TELEPORTGUARD:
				nearbyGuards.insertChar( pc_a_npc );
				break;
			case NPCAI_ORDERGUARD	:
				if( pc->distFrom( pc_a_npc ) < 5 )
					nearbyOrderGuards.insertChar( pc_a_npc );
				break;
			case NPCAI_CHAOSGUARD	:
				if( pc->distFrom( pc_a_npc ) < 5 )
					nearbyChaosGuards.insertChar( pc_a_npc );
				break;
		}
	}
	
nearbyGuards.rewind();		//GH!
nearbyOrderGuards.rewind();
nearbyChaosGuards.rewind();

	if( !nearbyOrderGuards.isEmpty() )
	{
		if( !success )	success = requestOrderShield( pc, socket, speech, nearbyOrderGuards );
	}
	if( !nearbyChaosGuards.isEmpty() )
	{
		if( !success )	success = requestChaosShield( pc, socket, speech, nearbyChaosGuards );
	}
	if( !success )	success = requestHelp( pc, socket, speech, nearbyGuards );
	return success;
}

static bool requestChaosShield( pChar pc, NXWSOCKET socket, std::string &speech, NxwCharWrapper &nearbyChaosGuards )
{
	bool success = false;
	int32_t tokenPosition = findKeyword( speech, "SHIELD");
	if( tokenPosition != INVALID )
	{
		std::string 	objectName( trimString( speech.substr( tokenPosition + 6) ) );
		if( objectName.empty() )
		{
			std::string 	guardName( trimString( speech.substr( 0, tokenPosition ) ) );
			bool 	findGuardByName = (guardName.length() != 0);
			pChar		pc_a_npc;
			pChar		chaosGuard = 0;
			for( nearbyChaosGuards.rewind(); !nearbyChaosGuards.isEmpty(); nearbyChaosGuards++  )
			{
				pc_a_npc = nearbyChaosGuards.getChar();
				if( (!findGuardByName) || findGuardByName && !strcasecmp( guardName.c_str(), pc_a_npc->getCurrentName().c_str() ) )
					if( !chaosGuard )
						chaosGuard = pc_a_npc;
					else
						if( pc->distFrom( chaosGuard ) > pc->distFrom( pc_a_npc ) )
							chaosGuard = pc_a_npc;
			}
			if( chaosGuard )
			{
				if( Guilds->GetType( pc->GetGuildNumber() ) == 2 || pc->IsGM() )
				{
					if( !pc->getAmount(0x1BC3) )
					{
						pItem pi =  pc->GetItemOnLayer( 2 );
						if( pi )
						{
							if( pi->getId() != 0x1BC3 )
								success = true;
						}
						else
							success = true;
					}
					if( success )
					{
						item::CreateFromScript( "$item_chaos_shield", pc->getBackpack() );
						chaosGuard->talk( socket, "Here's is your new shield.", 0);
					}
					else
						chaosGuard->talk( socket, "You already have a Chaos shield!", 0);
				}
				else
					chaosGuard->talk( socket,"You're not a Chaos guild member!", 0);
				success = true;
			}
		}
	}
	return success;
}

static bool requestOrderShield( pChar pc, NXWSOCKET socket, std::string &speech, NxwCharWrapper &nearbyOrderGuards )
{
	bool success = false;
	int32_t tokenPosition = findKeyword( speech, "SHIELD");
	if( tokenPosition != INVALID )
	{
		std::string 	objectName( trimString( speech.substr( tokenPosition + 6) ) );
		if( objectName.empty() )
		{
			std::string 	guardName( trimString( speech.substr( 0, tokenPosition ) ) );
			bool 	findGuardByName = (guardName.length() != 0);
			pChar		pc_a_npc;
			pChar		orderGuard = 0;
			for( nearbyOrderGuards.rewind(); !nearbyOrderGuards.isEmpty(); nearbyOrderGuards++  )
			{
				pc_a_npc = nearbyOrderGuards.getChar();
				if( (!findGuardByName) || findGuardByName && !strcasecmp( guardName.c_str(), pc_a_npc->getCurrentName().c_str() ) )
					if( !orderGuard )
						orderGuard = pc_a_npc;
					else
						if( pc->distFrom( orderGuard ) > pc->distFrom( pc_a_npc ) )
							orderGuard = pc_a_npc;
			}
			if( orderGuard )
			{
				if( Guilds->GetType( pc->GetGuildNumber() ) == 2 || pc->IsGM() )
				{
					if( !pc->getAmount(0x1BC4) )
					{
						pItem pi =  pc->GetItemOnLayer( 2 );
						if( pi )
						{
							if( pi->getId() != 0x1BC4 )
								success = true;
						}
						else
							success = true;
					}
					if( success )
					{
						item::CreateFromScript( "$item_order_shield", pc->getBackpack() );
						orderGuard->talk( socket, "Here's is your new shield.", 0);
					}
					else
						orderGuard->talk( socket, "You already have an Order shield!", 0);
				}
				else
					orderGuard->talk( socket,"You're not an Order guild member!", 0);
				success = true;
			}
		}
	}
	return success;
}

static bool requestHelp( pChar pc, NXWSOCKET socket, std::string &speech, NxwCharWrapper &nearbyGuards )
{
	bool success = false;
	//if( region[pc->region].priv&0x01 && SrvParms->guardsactive || !TIMEOUT( pc->antiguardstimer ) )
	return success;
}

} // namespace Guards

static bool buyFromVendor( pChar pc, NXWSOCKET socket, string &speech, NxwCharWrapper &nearbyVendors )
{
	bool success = false;
//	int32_t tokenPosition = INVALID;
	/*
	if(!speech.compare(0,10,"VENDOR BUY") || !speech.compare(0,14,"SHOPKEEPER BUY") )
	{
		success = true;
	}
	else
		if( tokenPosition = speech.find("BUY") != INVALID )
		{
		;
		}
	*/
	//
	// select nearest vendor
	//

	nearbyVendors.rewind();
	pChar	pc_vendor = nearbyVendors.getChar(); //first
	pChar	pc_a_npc;
	for( nearbyVendors.rewind(); !nearbyVendors.isEmpty(); nearbyVendors++ )
	{
		pc_a_npc = nearbyVendors.getChar();
		if( pc->distFrom( pc_vendor ) >  pc->distFrom( pc_a_npc ) )
			pc_vendor = pc_a_npc;
	}
	if( pc_vendor->npcaitype == NPCAI_PLAYERVENDOR )
	{
		pc_vendor->talk( socket, "What would you like to buy?", 0 );
		pTarget targ = clientInfo[socket]->newTarget( new cItemTarget() );
		targ->buffer[0]= pc_vendor->getSerial();
		targ->send( getClientFromSocket( socket ) );
		success = true;
	}
	else
		if( BuyShop( socket, DEREF_pChar( pc_vendor ) ) )
			success = true;
	return success;

}

}// namespace Speech


void makeGhost( wstring* from, wstring* to )
{
	to->erase();

	if ( from == NULL || to == NULL )
		return;

	wstring::iterator iter( from->begin() ), end( from->end() );
	for( ; iter != end; iter++ ) {
		/*if( (*iter)!=32 )
			(*to)+= ((*iter) %2)? L'O' : L'o';*/

		(*to) += ( (RandomNum(1,2) == 2) ? L'O' : L'o' );
	}

}




static int32_t findKeyword( const std::string &str, const std::string &keyword )
{
	int32_t tokenPosition = str.find( keyword );
	if( tokenPosition != INVALID )
		if( (tokenPosition != 0) && str[tokenPosition-1] != ' ' )
			tokenPosition = INVALID;
		else
			if( (tokenPosition + keyword.length() < str.length() -1) && str[tokenPosition+keyword.length()] != ' ')
				tokenPosition = INVALID;
	return tokenPosition;
}

static std::string trimString( const std::string &str )
{
	uint32_t i = 0;
	uint32_t j = str.length();

	while( (i < j) && str[i] == ' ' )
		++i;
	--j;
	while( str[j] == ' ' )
		--j;
	return str.substr( i, (j-i)+1 );
}


void char2wchar (const char* str)
{
	memset(&Unicode::temp[0], 0, 1024);
	uint32_t size = strlen(str);
	// client wants to have a 0 as very fist byte.
	// after that 0 the unicode text
	// after it two(!) 0's as termintor
	uint32_t j=1;
	for (uint32_t i = 0; i < size; i++)
	{
		Unicode::temp[j] = str[i];
		j+=2;
	}
}
