/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief functions that handle the teleporting
\author Duke
*/
#include "common_libs.h"
#include "inlines.h"
#include "basics.h"

void read_in_teleport()
{
	char text[1024];
	char seps[]	= " ,\t\n";
	char *token;

	cScpIterator* iter = NULL;

	iter = Scripts::Regions->getNewIterator("SECTION TELEPORT");

	if(iter==NULL)
	{
		ErrOut("Teleport Data not found\n");
		error=1;
		keeprun=false;
		return;
	}

	do
	{
		strcpy(text, iter->getEntry()->getFullLine().c_str());

		if ((text[0]!=';')&&(text[0]!='/')&&(text[0]!='}')&&(text[0]!='{'))
		{
			sTeleLocations dummy;

			token = strtok( text, seps );

			dummy.origem.x = atoi(token);
			token = strtok( NULL, seps );
			dummy.origem.y = atoi(token);
			token = strtok( NULL, seps );
			if (token[0] == 'A')
				dummy.origem.z = 127/*999*/;
			else
				dummy.origem.z = atoi(token);

			token = strtok( NULL, seps );
			dummy.destination.x = atoi(token);
			token = strtok(NULL, seps );
			dummy.destination.y = atoi(token);
			token = strtok(NULL, seps);
			dummy.destination.z = atoi(token);

			tele_locations.insert(pair<int, sTeleLocations>(dummy.origem.x, dummy));

		}
	} while ((text[0]!='}'));

	safedelete(iter);

}

int validtelepos(pChar pc)
{
	if ( ! pc ) return INVALID;
	int z=INVALID;

	if ( pc->getPosition().x >= 1397 && pc->getPosition().x <= 1400 &&
		pc->getPosition().y >= 1622 && pc->getPosition().y <= 1630 )
		z=28;
	if ( pc->getPosition().x >= 1510 && pc->getPosition().x <= 1537 &&
		pc->getPosition().y >= 1455 && pc->getPosition().y<=1456 )
		z=15;
	return z;

}

void advancementobjects(pChar pc, int x, int allways)
{
	if ( ! pc ) return;

	pPC tmp;
	pClient client = (tmp = dynamic_cast<pPC>(tmp))? tmp-getClient(): NULL; 

	char sect[512];
	int loopexit=0;
	std::string script1, script2;
	cScpIterator* iter = NULL;
	int i;

	pItem packnum=NULL;
	pItem hairobject=NULL, beardobject=NULL;

	if ((pc->advobj==0)||(allways==1))
	{
		staticFX(pc, 0x373A, 0, 15);
		pc->playSFX(0x01E9);
		pc->advobj=x;
		sprintf(sect, "SECTION ADVANCEMENT %i", x);
		iter = Scripts::Advance->getNewIterator(sect);
		if (iter==NULL)
		{
			ErrOut("advancement object: Script section not found. Aborting.\n");
			pc->advobj=0;
			return;
		}
		else
			do
			{
				iter->parseLine(script1, script2);
				if ((script1[0]!='}')&&(script1[0]!='{'))
				{
					if	(script1[0]=='@') pc->loadEventFromScript( const_cast< char* >(script1.c_str()), const_cast< char* >(script2.c_str()));
					else if ("STR" == script1 || "STRENGTH" == script1 )
					{
						pc->setStrength( getRangedValue(script2) );
						pc->st2 = pc->getStrength();
					}
					else if ("DEX" == script1 ||"DEXTERITY" == script1 )
					{
						pc->dx= getRangedValue(script2);
						pc->dx2 = pc->dx;
					}
					else if ("INT" == script1 ||"INTELLIGENCE" == script1 )
					{
						pc->in= getRangedValue(script2);
						pc->in2 = pc->in;
					}
					else if ("ALCHEMY" == script1 ||"SKILL0" == script1 ) pc->baseskill[ALCHEMY] = getRangedValue(script2);
					else if ("skAnatomy" == script1 ||"SKILL1" == script1 ) pc->baseskill[skAnatomy] = getRangedValue(script2);
					else if ("skAnimalLore" == script1 ||"SKILL2" == script1 ) pc->baseskill[skAnimalLore] = getRangedValue(script2);
					else if ("ITEMID" == script1 ||"SKILL3" == script1 ) pc->baseskill[ITEMID] = getRangedValue(script2);
					else if ("skArmsLore" == script1 ||"SKILL4" == script1 ) pc->baseskill[skArmsLore] = getRangedValue(script2);
					else if ("skParrying" == script1 ||"SKILL5" == script1 ) pc->baseskill[skParrying] = getRangedValue(script2);
					else if ("skBegging" == script1 ||"SKILL6" == script1 ) pc->baseskill[skBegging] = getRangedValue(script2);
					else if ("skBlacksmithing" == script1 ||"SKILL7" == script1 ) pc->baseskill[skBlacksmithing] = getRangedValue(script2);
					else if ("skBowcraft" == script1 ||"SKILL8" == script1 ) pc->baseskill[skBowcraft] = getRangedValue(script2);
					else if ("skPeacemaking" == script1 ||"SKILL9" == script1 ) pc->baseskill[skPeacemaking] = getRangedValue(script2);
					else if ("skCamping" == script1 ||"SKILL10" == script1 ) pc->baseskill[skCamping] = getRangedValue(script2);
					else if ("" == script1 ||"SKILL11" == script1 ) pc->baseskill[] = getRangedValue(script2);
					else if ("skCartography" == script1 ||"SKILL12" == script1 ) pc->baseskill[skCartography] = getRangedValue(script2);
					else if ("skCooking" == script1 ||"SKILL13" == script1 ) pc->baseskill[skCooking] = getRangedValue(script2);
					else if ("skDetectingHidden" == script1 ||"SKILL14" == script1 ) pc->baseskill[skDetectingHidden] = getRangedValue(script2);
					else if ("skEnticement" == script1 ||"SKILL15" == script1 ) pc->baseskill[skEnticement] = getRangedValue(script2);
					else if ("skEvaluatingIntelligence" == script1 ||"SKILL16" == script1 ) pc->baseskill[skEvaluatingIntelligence] = getRangedValue(script2);
					else if ("HEALING" == script1 ||"SKILL17" == script1 ) pc->baseskill[HEALING] = getRangedValue(script2);
					else if ("FISHING" == script1 ||"SKILL18" == script1 ) pc->baseskill[FISHING] = getRangedValue(script2);
					else if ("skForensics" == script1 ||"SKILL19" == script1 ) pc->baseskill[skForensics] = getRangedValue(script2);
					else if ("skHerding" == script1 ||"SKILL20" == script1 ) pc->baseskill[skHerding] = getRangedValue(script2);
					else if ("HIDING" == script1 ||"SKILL21" == script1 ) pc->baseskill[HIDING] = getRangedValue(script2);
					else if ("skProvocation" == script1 ||"SKILL22" == script1 ) pc->baseskill[skProvocation] = getRangedValue(script2);
					else if ("skInscription" == script1 ||"SKILL23" == script1 ) pc->baseskill[skInscription] = getRangedValue(script2);
					else if ("skLockPicking" == script1 ||"SKILL24" == script1 ) pc->baseskill[skLockPicking] = getRangedValue(script2);
					else if ("skMagery" == script1 ||"SKILL25" == script1 ) pc->baseskill[skMagery] = getRangedValue(script2);
					else if ("skMagicResistance" == script1 ||"RESIST" == script1 ||"SKILL26" == script1 ) pc->baseskill[skMagicResistance] = getRangedValue(script2);
					else if ("skTactics" == script1 ||"SKILL27" == script1 ) pc->baseskill[skTactics] = getRangedValue(script2);
					else if ("skSnooping" == script1 ||"SKILL28" == script1 ) pc->baseskill[skSnooping] = getRangedValue(script2);
					else if ("skMusicianship" == script1 ||"SKILL29" == script1 ) pc->baseskill[skMusicianship] = getRangedValue(script2);
					else if ("skPoisoning" == script1 ||"SKILL30" == script1 ) pc->baseskill[skPoisoning] = getRangedValue(script2);
					else if ("skArchery" == script1 ||"SKILL31" == script1 ) pc->baseskill[skArchery] = getRangedValue(script2);
					else if ("SPIRITSPEAK" == script1 ||"SKILL32" == script1 ) pc->baseskill[SPIRITSPEAK] = getRangedValue(script2);
					else if ("skStealing" == script1 ||"SKILL33" == script1 ) pc->baseskill[skStealing] = getRangedValue(script2);
					else if ("skTailoring" == script1 ||"SKILL34" == script1 ) pc->baseskill[skTailoring] = getRangedValue(script2);
					else if ("skTaming" == script1 ||"SKILL35" == script1 ) pc->baseskill[skTaming] = getRangedValue(script2);
					else if ("skTasteID" == script1 ||"SKILL36" == script1 ) pc->baseskill[skTasteID] = getRangedValue(script2);
					else if ("skTinkering" == script1 ||"SKILL37" == script1 ) pc->baseskill[skTinkering] = getRangedValue(script2);
					else if ("skTracking" == script1 ||"SKILL38" == script1 ) pc->baseskill[skTracking] = getRangedValue(script2);
					else if ("skVeterinary" == script1 ||"SKILL39" == script1 ) pc->baseskill[skVeterinary] = getRangedValue(script2);
					else if ("skSwordsmanship" == script1 ||"SKILL40" == script1 ) pc->baseskill[skSwordsmanship] = getRangedValue(script2);
					else if ("skMacefighting" == script1 ||"SKILL41" == script1 )pc->baseskill[skMacefighting] = getRangedValue(script2);
					else if ("FENCING" == script1 ||"SKILL42" == script1 ) pc->baseskill[FENCING] = getRangedValue(script2);
					else if ("skWrestling" == script1 ||"SKILL43" == script1 ) pc->baseskill[skWrestling] = getRangedValue(script2);
					else if ("skLumberjacking" == script1 ||"SKILL44" == script1 ) pc->baseskill[skLumberjacking] = getRangedValue(script2);
					else if ("skMining" == script1 ||"SKILL45" == script1 ) pc->baseskill[skMining] = getRangedValue(script2);
					else if ("skMeditation" == script1 ||"SKILL46" == script1 ) pc->baseskill[skMeditation] = getRangedValue(script2);
					else if ("skStealth" == script1 ||"SKILL47" == script1 ) pc->baseskill[skStealth] = getRangedValue(script2);
					else if ("skRemoveTraps" == script1 ||"SKILL48" == script1 ) pc->baseskill[skRemoveTraps] = getRangedValue(script2);
					else if ("DYEHAIR" == script1 )
					{
						int serial;
						uint32_t ci;
						serial=pc->getSerial();
						if (serial > INVALID)
							for (ci=0;ci<pointers::pContMap[serial].size();ci++)
							{
								pItem pii_i=pointers::pContMap[serial][ci];
								if ( pii_i )
									if ((pii_i->layer==LAYER_HAIR) && (pii_i->getContSerial()==serial))
									{
										hairobject=pii_i;
										break;
									}
							}
						pItem phair= hairobject;
						if( phair ) {

							phair->setColor( hex2num(script2) );
							phair->Refresh();
							pc->teleport();
						}
					}
					else if ("DYEBEARD" == script1 )
					{
						int serial;
						uint32_t ci;
						serial=pc->getSerial();
						if (serial > INVALID)
						for (ci=0;ci<pointers::pContMap[serial].size();ci++)
						{
							pItem pii_i=pointers::pContMap[serial][ci];
							if ( pii_i )
								if ((pii_i->layer==LAYER_BEARD) && (pii_i->getContSerial()==serial))
								{
									beardobject=pii_i;
									break;
								}
						}
						if (true)
						{
							pItem pbeard= beardobject;
							if( pbeard ) {
								pbeard->setColor( hex2num(script2) );
								pbeard->Refresh();
								pc->teleport();
							}
						}
					}
					else if( "KILLHAIR" == script1 )
					{
						int serial;
						uint32_t ci;
						serial=pc->getSerial();
						if (serial > INVALID)
						for (ci=0;ci<pointers::pContMap[serial].size();ci++)
						{
							pItem pii_i=pointers::pContMap[serial][ci];
							if ( pii_i )
								if ((pii_i->layer==LAYER_HAIR) && (pii_i->getContSerial()==serial))
								{
									pii_i->Delete();
									break;
								}
						}
					}
					else if ( "KILLBEARD" == script1 )
					{
						int serial;
						uint32_t ci;
						serial=pc->getSerial();
						if (serial > INVALID)
						for (ci=0;ci<pointers::pContMap[serial].size();ci++)
						{
							pItem pii_i=pointers::pContMap[serial][ci];
							if ( pii_i )
								if ((pii_i->layer==LAYER_BEARD) && (pii_i->getContSerial()==serial))
								{
									pii_i->Delete();
									break;
								}
						}
					}
					else if ( "KILLPACK" == script1 )
					{
						int serial;
						uint32_t ci;
						serial=pc->getSerial();
						if (serial > INVALID)
						for (ci=0;ci<pointers::pContMap[serial].size();ci++)
						{
							pItem pii_i=pointers::pContMap[serial][ci];
							if ( pii_i )
								if ((pii_i->layer==LAYER_BACKPACK) && (pii_i->getContSerial()==serial))
								{
									pii_i->Delete();
									break;
								}
						}
					}
					else if ( "ITEM" == script1 )
					{
						char itmnum[1024], amount[1024];
						splitLine( const_cast< char* >(script2.c_str()), itmnum, amount );
						int amt = str2num( amount );
						if( amt == 0 )
							amt=INVALID;
						pItem pi= item::CreateFromScript( itmnum, NULL, amt );
						packnum= pc->getBackpack();

						if (pi)
						{
							pi->setPosition( 50+(rand()%80), 50+(rand()%80), 9);
							if(pi->layer==LAYER_HAIR || pi->layer==LAYER_BEARD)
							{
								pi->setContainer(pc);
							}
							else
							{
								if( packnum ) 
									packnum->AddItem( pi );
							}
							pi->Refresh();//AntiChrist
							pc->teleport();
						}
					}
					else if ( script1 == "SKIN" )
					{
						i=hex2num(script2);
						pc->setColor(i);
						pc->setOldColor(i);
						pc->teleport();
					}
					else if ( "POLY" == script1 )
					{
						x=hex2num(script2);
						pc->setId(x);
						pc->setOldId(x);
						pc->teleport();
					}
					else if ( "ADVOBJ" == script1 )
					{
						pc->advobj = str2num(script2);
					}
				}
			}
			while ((script1[0]!='}') && (++loopexit < MAXLOOPS) );
			safedelete(iter);
	}
	else client->sysmessage("You have already used an advancement object with this character.");
}


//
//
// Aldur
//////////////////////////////////

void objTeleporters(pChar pc)
{
	if ( ! pc ) return;

	sLocation charpos= pc->getPosition();
	

	NxwItemWrapper si;
	si.fillItemsAtXY( charpos );
	for( si.rewind(); !si.isEmpty(); si++ ) {
		pItem pmi=si.getItem();
		if(! pmi )
			continue;

		if (pmi->getPosition().x == charpos.x && pmi->getPosition().y == charpos.y &&
			(abs(pmi->getPosition().z) + 10) >= abs(charpos.z) && (abs(pmi->getPosition().z) - 10) <= abs(charpos.z))
			{
				if ((pmi->type == 60) && (pmi->morex + pmi->morey + pmi->morez >0))
				{
					pc->MoveTo( pmi->morex,pmi->morey,pmi->morez );
					pc->teleport();
				}

				// advancement objects
				if ((pmi->type == 80) && !pc->npc)
				if (pmi->more != 0)
				{
					if (pc->getSerial() == pmi->more )
						advancementobjects(pc, pmi->morex, 0);
				}
				else
					advancementobjects(pc, pmi->morex, 0);

				if ((pmi->type == 81)&&!(pc->npc))
					if ( pmi->more != 0 )
					{
						if (pc->getSerial() == pmi->more)
							advancementobjects(pc, pmi->morex, 1);
					}
					else
						advancementobjects(pc, pmi->morex, 1);
				
				// The above code lets you restrict a gate's use by setting its MORE values to a char's
				// serial #

				// damage objects
				if (!(pc->IsInvul()) && (pmi->type == 85))
				{
					pc->hp = pc->hp - (pmi->morex + RandomNum(pmi->morey, pmi->morez));

					if (pc->hp < 1)
						pc->Kill();
				}
				
				
				//
				//
				// Aldur
				//////////////////////////////////

				// sound objects
				if (pmi->type == 86)
				{
					if ((uint32_t)RandomNum(1, 100) <= pmi->morez)
						pmi->soundEffect2((pmi->morex << 8) + pmi->morey);
				}
			}
	}

}

void teleporters(pChar pc)
{
	if ( ! pc ) return;

	sLocation charpos= pc->getPosition();

	std::multimap<int, sTeleLocations>::iterator iter_tele_locations( tele_locations.find(charpos.x) ),
							iter_tele_locations_end( tele_locations.end() );

	while  ( charpos.x == iter_tele_locations->second.origem.x && iter_tele_locations!=iter_tele_locations_end )
	{
		if( charpos.y == iter_tele_locations->second.origem.y )
		{
			if( iter_tele_locations->second.origem.z == 127 || abs( charpos.z - iter_tele_locations->second.origem.z ) <= 30 )
			{
				if ( !pc->npc )
				{
					// Look for an NPC Araknesh fix animals in teleporters with pg

					NxwCharWrapper sc;
					sc.fillCharsNearXYZ( pc->getPosition(), 4 );
					for( sc.rewind(); !sc.isEmpty(); sc++ )
					{
						pChar pc_i=sc.getChar();
						// That is following this player character
						if ( (pc_i->npc) && (pc_i->ftargserial==pc->getSerial()) )
						{
							// Teleport the NPC along with the player
							pc_i->MoveTo(	iter_tele_locations->second.destination.x,
									iter_tele_locations->second.destination.y,
									iter_tele_locations->second.destination.z );
							pc_i->teleport();
						}
					}
				}

				pc->MoveTo(	iter_tele_locations->second.destination.x,
						iter_tele_locations->second.destination.y,
						iter_tele_locations->second.destination.z );
				pc->teleport();
				return;
			}
		}
		++iter_tele_locations;
	}

}
