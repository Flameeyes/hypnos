/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "common_libs.h"
#include "debug.h"
#include "basics.h"
#include "inlines.h"

void newbieitems(pChar pc)
{
	if ( ! pc ) return;
	
	NXWCLIENT ps=pc->getClient();
	if(ps==NULL)
		return;

	int storeval, itemaddperskill, loopexit = 0;
	uint8_t first, second, third;
	char sect[512];
	char whichsect[105];
	cScpIterator* iter = NULL;

	first = pc->bestSkill();
	second = pc->nextBestSkill(first);
	third = pc->nextBestSkill(second);
	if (pc->baseskill[third] < 190)
		third = 46;

	for (itemaddperskill = 1; itemaddperskill <= 5; itemaddperskill++)
	{
		switch (itemaddperskill)
		{
			// first of all the general section with the backpack, else where we put items?
			case 1: strcpy(whichsect, "SECTION ALLNEWBIES");		break;
			case 2:
				if ( (pc->getId() == BODY_MALE) && (pc->getOldId() == BODY_MALE) )
					strcpy(whichsect, "SECTION MALENEWBIES");
				else
					strcpy(whichsect, "SECTION FEMALENEWBIES");
				break;
			case 3: sprintf(whichsect, "SECTION BESTSKILL %i", first);	break;
			case 4: sprintf(whichsect, "SECTION BESTSKILL %i", second);	break;
			case 5: sprintf(whichsect, "SECTION BESTSKILL %i", third);	break;
			default:
				ErrOut("Switch fallout. newbie.cpp, newbieitems()/n"); // Morrolan
		}

		sprintf(sect, whichsect);
		char script1[1000], script2[1000];
		safedelete(iter);
		iter = Scripts::Newbie->getNewIterator(sect);
		if (iter==NULL) return;

		do
		{
			iter->parseLine(script1,script2);

			if (script1[0] == '@') pc->loadEventFromScript(script1, script2); 	// Sparhawk: Huh loading character events 
												// from newbie item scripts????

			if (script1[0] != '}')
			{
				if (!(strcmp("PACKITEM", script1)))
				{
					std::string itemnum, amount;
					splitLine( script2, itemnum, amount );
					int amt = ( amount != "" )? str2num( amount ) : INVALID; //ndEndy defined amount
					pItem pi_n = item::CreateFromScript( str2num( itemnum ), pc->getBackpack(), amt );
					if ( pi_n ) {
						pi_n->priv |= 0x02; // Mark as a newbie item
					}
					strcpy(script1, "DUMMY");
				}
				else if (!strcmp("BANKITEM", script1))
				{
					std::string itemnum, amount;
					splitLine( script2, itemnum, amount );
					int amt= (amount!="")? str2num( amount ) : INVALID;
					pItem pi = item::CreateFromScript( str2num( itemnum ), pc->GetBankBox(), amt );
					if (pi) {
						pi->priv |= 0x02; // Mark as a newbie item
					}
					strcpy(script1, "DUMMY");
				}
				else if (!strcmp("EQUIPITEM", script1))
				{
					pItem pi = item::CreateFromScript( script2 );
					if (pi)
					{
						pi->priv |= 0x02; // Mark as a newbie item
						pi->setContainer(pc);
						storeval = pi->getScriptID();
					}
					strcpy(script1, "DUMMY");
				}
			}
		}
		while ((script1[0] != '}') &&(++loopexit < MAXLOOPS));
	
		safedelete(iter);
	}
	
	// Give the character some gold
	if ( goldamount > 0 )
	{
		item::CreateFromScript( "$item_gold_coin", pc->getBackpack(), goldamount );
	}
}
