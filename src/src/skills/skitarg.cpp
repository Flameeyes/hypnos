/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "common_libs.h"
#include "targeting.h"
#include "tmpeff.h"
#include "constants.h"
#include "npcai.h"
#include "map.h"
#include "skills/skills.h"
#include "basics.h"
#include "inlines.h"
#include "backend/scripting.h"
#include "objects/cclient.h"
#include "objects/cchar.h"
#include "objects/cpc.h"
#include "objects/cbody.h"
#include "objects/citem.h"

bool CheckInPack(pClient client, pItem pi)
{
	pItem pPack = client->currChar()->getBody()->getBackpack(true);
	if ( ! pPack ) return false;
	
	if ( pi->getContainer() != pPack )
	{
		client->sysmessage("You can't use material outside your backpack");
		return false;
	}
	return true;
}

void nSkills::target_removeTraps(pClient client, pTarget t )
{
	pChar pc = client->currChar();
	pItem pi = dynamic_cast<pItem>( t->getClicked() );
	if ( !pc || !pi ) return;

	pFunctionHandle evt = pi->getEvent(cItem::evtItmOnRemoveTrap);
	if ( evt )
	{
		tVariantVector params = tVariantVector(2);
		params[0] = pi->getSerial(); params[1] = pc->getSerial();
		evt->setParams(params);
		evt->execute();
	} else {
		client->sysmessage("There are no traps on this object");
		if ((rand()%3)==0)
			pc->checkSkill( skRemoveTraps, 0, 750); //ndEny is good?
	}
}

void nSkills::target_tailoring(pClient client, pTarget t )
{
	pChar pc = client->currChar();
	pItem pi = dynamic_cast<pItem>( t->getClicked() );

	if ( !pc || !pi ) return;

    AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,skTailoring,AMX_BEFORE);

    if( pi->magic==4)
		return;

	if( pi->isCutLeather() || pi->isCutCloth() || pi->isHide() )
    {
		if (CheckInPack(client, pi))
        {
			if(pc->getAmount(pi->getId())<1)
			{
				client->sysmessage("You don't have enough material to make anything.");
				return;
			}

            if(	pi->isCutLeather()  ) {
				if( tannering == NULL )
					tannering = new AmxFunction( AMXTANNERING );
				if( tannering != NULL )
					tannering->Call( pc->getSerial(), pi->getSerial() );
			}
            else {
				if( tailoring == NULL )
					tailoring = new AmxFunction( AMXskTailoring );
				if( tailoring != NULL )
					tailoring->Call( pc->getSerial(), pi->getSerial() );
			}

        }

    }
	else client->sysmessage("You cannot use that material for tailoring.");

    AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,skTailoring,AMX_AFTER);
}

void nSkills::target_fletching(pClient client, pTarget t )
{
	pChar pc = client->currChar();
	pItem pi = dynamic_cast<pItem>( t->getClicked() );

	if ( !pc || !pi ) return;

    AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,skBowcraft,AMX_BEFORE);
    
	if ( pi->magic!=4) // Ripper
	{
		if (CheckInPack(client,pi))
		{
			MakeMenu(pc,60,skBowcraft, cSerializable::findItemBySerial( t->buffer[0] ), pi );
		}
	} else
		client->sysmessage("You cannot use that for fletching.");

    AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,skBowcraft,AMX_AFTER);
}

void nSkills::target_bowcraft(pClient client, pTarget t )
{
	pChar pc = client->currChar();
	if ( !pc ) return;

	pc->playAction(pc->isMounting() ? 0x1C : 0x0D);

	pItem pi=dynamic_cast<pItem>( t->getClicked() );
	if ( ! pi ) return;

	AMXEXECSVTARGET(pc->getSerial(),AMXT_SKITARGS,skBowcraft,AMX_BEFORE);

	pc->playAction(pc->isMounting() ? 0x1C : 0x0D);
	if ( pi->magic!=4) // Ripper
	{

		if( pi->isLog() || pi->isBoard() )
		{
			if (CheckInPack(client,pi))
			{
				MakeMenu(pc,65,skBowcraft,pi);
			}
		}
	}

	AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,skBowcraft,AMX_AFTER);
}

////////////////////
// name:    target_carpentry()
// history: unknown, rewritten by Duke, 25.05.2000
// purpose: sets up appropriate Makemenu when player targets logs or boards
//          after dclick on carpentry tool
//
//          If logs are targetted, Makemenu 19 is called to produce boards
//          If boards, MM 20 is called for furniture etc.
//
void nSkills::target_carpentry(pClient client, pTarget t )
{
	pChar pc = client->currChar();
	pItem pi = dynamic_cast<pItem>( t->getClicked() );

	if ( !pc || !pi ) return;

    AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,skCarpentry,AMX_BEFORE);

	if ( pi->magic!=4)
	{
		if( pi->isLog() || pi->isBoard() ) // logs or boards
		{
			if (CheckInPack(client,pi))
			{
				short mm = pi->isLog() ? 19 : 20; // 19 = Makemenu to create boards from logs
				MakeMenu(pc,mm,skCarpentry,pi);
			}
		}
	}
	else
		client->sysmessage("You cannot use that material for carpentry.");

    AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,skCarpentry,AMX_AFTER);
}

/*!
\todo use or remove it!
*/
static bool ForgeInRange(pClient client)
{
	pPC pc = client->currChar();
	if ( !pc ) return false;
	
	NxwItemWrapper si;
	si.fillItemsNearXYZ( pc->getBody()->getPosition(), 3, false );
	for( si.rewind(); !si.isEmpty(); si++ )
	{
		pItem pi=si.getItem();
		if( pi && pi->isForge())
			return true;
	}
	return false;
}

static bool AnvilInRange(pClient client)
{
	pPC pc = client->currChar();
	if ( ! pc ) return false;

	NxwItemWrapper si;
	si.fillItemsNearXYZ( pc->getPosition(), 3, false );
	for( si.rewind(); !si.isEmpty(); si++ )
	{
		pItem pi=si.getItem();
		if(pi->isAnvil())
			return true;
	}
	return false;
}

/*!
\author Duke
\date 28/03/2000
\brief Little helper function for nSkills::target_smith()
\param client the client
\param pi pointer to material item
\param ma maximum amount
\param mm makemenu number to invoke from create.scp
\param matname name of the metal

checks for anvil in reach and enough material and invokes appropriate makemenu
*/
static void AnvilTarget( pClient client, pItem pi, int ma, int mm, char* matname)
{
	pChar pc = client->currChar();
	if ( !pc || !pi ) return;

    
	if (!AnvilInRange(client))
		client->sysmessage("The anvil is too far away.");
    
	else {
		if (CheckInPack(client,pi))
			nSkills::MakeMenu(pc,mm,skBlacksmithing,pi);
	}
}

//////////////////////////
// Function:    Smith
// History:     unknown, colored ore added by Cork,
//              28 March 2000 revamped by Duke
//              16.9.2000 removed array access and revamped s a bit more (Duke)
//
// Purpose:     checks if targeted material is ore of some kind,
//              checks if anvil is in reach and invokes appropriate Makemenu
// Remarks:     the ingottype var is problematic in a multiplayer environment!!
//
extern int ingottype;

void nSkills::target_smith(pClient client, pTarget t )
{
    pItem pi = dynamic_cast<pItem>( t->getClicked() );
	if ( !pi ) return;

    if (pi->magic!=4) // Ripper
    {
        if (!CheckInPack(client,pi))
			return;

        if (pi->getId()==0x1BEF || pi->getId()==0x1BF2)   // is it an ingot ?
        {
			AnvilTarget(client, pi, 1, AmxFunction::g_prgOverride->CallFn( AmxFunction::g_prgOverride->getFnOrdinal(AMXINGOTMAKEMENU), pi->getColor()), NULL);
			return;
        }
    }
    client->sysmessage("You cannot use that material for blacksmithing");
}

struct Ore
{
    short color;
    short minskill; // minimum skill to handle that ore
    short quota;    // relative(!) chance to mine that ore (quota/sum of all quotas)
    char *name;
};
const struct Ore OreTable[] =   // MUST be sorted by minskill
{
//  {0x0000,  0,630,"Iron"},
    {0x0386,650,126,"Shadow"},
    {0x02C3,700,112,"Merkite"},
    {0x046E,750, 98,"Copper"},
    {0x0961,790, 84,"Silver"},
    {0x02E7,800, 70,"Bronze"},
    {0x0466,850, 56,"Golden"},
    {0x0150,900, 42,"Agapite"},
    {0x022F,950, 28,"Verite"},
    {0x0191,990, 14,"Mythril"}
};
const short NumberOfOres = sizeof(OreTable)/sizeof(Ore);

/*static const Ore* getColorFound(short skill)
{
    int i,totalQuotas=0,minersQuota=0;

    for (i=0;i<NumberOfOres;i++)
    {
        if(OreTable[i].minskill<=skill)     // get his part of quotas depending on his skill
            minersQuota += OreTable[i].quota;
        else
            break;
    }
    if (minersQuota < 1) return NULL;
    int r = rand()%minersQuota;

    for (i=0;i<NumberOfOres;i++)
    {
        totalQuotas += OreTable[i].quota;   // summarize quotas until we find the ore
        if (r < totalQuotas)
            return &OreTable[i];
        if(OreTable[i].minskill > skill)
        {
            ErrOut("something went wrong with mining");
            break;
        }
    }
    return &OreTable[0];    // shouldn't get here, but return iron as default anyway
}*/

/////////////////
// name:    TryToMine
// history: by Duke, 31 March 2000
// Purpose: helper function for nSkills::Mine()
//          checks if the player's mining skill meets the requirements
//          if yes, he'll get the specified ore
//          There's a *second* CheckSkill done here. It was like this,
//          so I left it like this. That's a gameplay issue.
//
/*static bool TryToMine(    int s,                  // current char's socket #
                        int minskill,           // minimum skill required for ore color
                        unsigned char id1, unsigned char id2,       // item ID of ingot to be created
                        unsigned char col1,unsigned char col2,  // color
                        char *orename)          // first letter should be uppercase
{
    SERIAL cc = currchar[s];
    pChar pc = MAKE_CHARREF_LRV(cc, false);

    if(pc->skill[skMining] >= minskill)
    {
        char tmp[100];
        sprintf(tmp,"%s Ore",orename);
        item::SpawnItem(s,cc,1,tmp,1,id1,id2,col1,col2,1,1);

        sysmessage(s,"You place some %c%s ore in your pack.",tolower(*orename),orename+1);
        return true;
    }
    return false;
}
*/ // XAN : SEEMS UNUSED

void nSkills::target_tree(pClient client, pTarget t )
{
    pChar pc = client->currChar();

    AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,skLumberjacking,AMX_BEFORE);

    static uint32_t logtime[max_res_x][max_res_y];//see mine for values...they were 1000 also here
    static uint32_t logamount[max_res_x][max_res_y];
    int a, b;
    unsigned int c;
    unsigned long int curtime=getclock();

	if( pc->isMounting() ) {
		client->sysmessage("You cannot do this on a horse");
		return;
	}

	sLocation charpos = pc->getPosition();
	sLocation location = t->getPosition();


    if( dist( charpos, location )>2 )
    {
        client->sysmessage("You are to far away to reach that");
        return;
    }

	pItem weapon = pc->getWeapon();
	if( !( weapon && ( weapon->isAxe() || weapon->isSword() ) ) )
	{
		client->sysmessage("You must have a weapon in hand in order to chop.");
		return;
	}

    if (resource.logstamina<0 && abs(resource.logstamina)>pc->stm)
    {
        client->sysmessage("You are too tired to chop.");
        return;
    }

    pc->stm+=resource.logstamina;
    if(pc->stm<0) pc->stm=0;
    if(pc->stm>pc->dx) pc->stm=pc->dx;
    pc->updateStamina();

    if(resource.logarea<10) resource.logarea=10; //New -- Zippy

    if(logtime[0][0]==0)//First time done since server started
    {
        logtime[0][0]=17;//lucky number ;-)
        logamount[0][0]=resource.logs;
        LogMessage("Lumberjacking startup, setting tree values and times...");

        //for(a=1;a<410;a++)
        for(a=1;a<max_res_x;a++)//AntiChrist bug fix
        {
            for(b=1;b<max_res_y;b++)
            {
                logamount[a][b]=resource.logs;
                SetTimerSec(&logtime[a][b],static_cast<short>(resource.logtime));
            }
        }
        LogMessage("[DONE]");
    }

	pc->facexy( location.x, location.y );

    a= charpos.x / resource.logarea; //Zippy
    b= charpos.y / resource.logarea;

    if(a>=max_res_x || b>=max_res_y) return;

    if(logtime[a][b]<=curtime)
    {
        for(c=0;c<resource.logs;c++)//Find howmany 10 min periods have been by, give 1 more for each period.
        {
            if((logtime[a][b]+(c*resource.logtime*SECS))<=curtime && logamount[a][b]<resource.logs)
                logamount[a][b]+=resource.lograte;//AntiChrist
            else break;
        }
        SetTimerSec(&logtime[a][b],static_cast<short>(resource.logtime));
    }

    if(logamount[a][b]>resource.logs) logamount[a][b]=resource.logs;

    if(logamount[a][b]<=0)
    {
        client->sysmessage("There is no more wood here to chop.");
        return;
    }

    pItem packnum = pc->getBackpack();
    if( ! packnum ) {
    	client->sysmessage("No backpack to store logs");
		return;
	}

    pc->playAction( pc->isMounting() ? 0x1C : 0x0D );
    pc->playSFX(0x013E);

    if (!pc->checkSkill(skLumberjacking, 0, 1000))
    {
        client->sysmessage("You chop for a while, but fail to produce any usable wood.");
        if(logamount[a][b]>0 && rand()%2==1)
			logamount[a][b]--;//Randomly deplete resources even when they fail 1/2 chance you'll loose wood.
        return;
    }

    if(logamount[a][b]>0)
		logamount[a][b]--;

    AmxFunction::g_prgOverride->CallFn( AmxFunction::g_prgOverride->getFnOrdinal(AMXskLumberjacking), s);

    AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,skLumberjacking,AMX_AFTER);
}

void nSkills::GraveDig(pClient client) // added by Genesis 11-4-98
{
	pChar pc = client->currChar();
	if ( ! pc ) return;

    int nAmount, nFame;
    char iID=0;

    pc->IncreaseKarma(-2000); // Karma loss no lower than the -2 pier

    pc->playAction( pc->isMounting() ? 0x1A : 0x0b );
    pc->playSFX(0x0125);
    if(!pc->checkSkill(skMining, 0, 800))
    {
        client->sysmessage("You sifted through the dirt and found nothing.");
        return;
    }

    nFame = pc->GetFame();
    pc->playAction( pc->isMounting() ? 0x1A : 0x0b );
    pc->playSFX(0x0125);
    int nRandnum=rand()%13;
    switch(nRandnum)
    {
    case 2:
        npcs::SpawnRandomMonster(pc,"UNDEADLIST","1000"); // Low level Undead - Random
        client->sysmessage("You have disturbed the rest of a vile undead creature.");
        break;
    case 4:
		{
			pItem pi=item::SpawnRandomItem(s,"ITEMLIST","1001"); // Armor and shields - Random
			if(pi)
				if((pi->getId()>=7026)&&(pi->getId()<=7035))
					client->sysmessage("You unearthed an old shield and placed it in your pack");
				else
		            client->sysmessage("You have found an old piece armor and placed it in your pack.");
		}
        break;
    case 5:
        //Random treasure between gems and gold
        nRandnum=rand()%2;
        if(nRandnum)
        { // randomly create a gem and place in backpack
            pItem pi=item::SpawnRandomItem(s,"ITEMLIST","999");
            if(pi)
				client->sysmessage("You place a gem in your pack.");
        }
        else
        { // Create between 1 and 15 goldpieces and place directly in backpack
            nAmount=1+(rand()%15);
	    pc->addGold(nAmount);
            pc->playSFX( goldsfx(nAmount) );
            if (nAmount==1)
                client->sysmessage("You unearthed %i gold coin.", nAmount);
            else
                client->sysmessage("You unearthed %i gold coins.", nAmount);
        }
        break;
    case 6:
        if(nFame<500)
            npcs::SpawnRandomMonster(pc,"UNDEADLIST","1000"); // Low level Undead - Random
        else
            npcs::SpawnRandomMonster(pc,"UNDEADLIST","1001"); // Med level Undead - Random
        client->sysmessage("You have disturbed the rest of a vile undead creature.");
        break;
    case 8:
	{
		pItem pi=item::SpawnRandomItem(s,"ITEMLIST","1000");
		if(pi)
			client->sysmessage("You unearthed a old weapon and placed it in your pack.");
	}
        break;
    case 10:
        if(nFame<1000)
            npcs::SpawnRandomMonster(pc,"UNDEADLIST","1001"); // Med level Undead - Random
        else
            npcs::SpawnRandomMonster(pc,"UNDEADLIST","1002"); // High level Undead - Random
        client->sysmessage("You have disturbed the rest of a vile undead creature.");
        break;
    case 12:
        if(nFame>1000)
            npcs::SpawnRandomMonster(pc,"UNDEADLIST","1002"); // High level Undead - Random
        else
            npcs::SpawnRandomMonster(pc,"UNDEADLIST","1001"); // Med level Undead - Random
        client->sysmessage("You have disturbed the rest of a vile undead creature.");
        break;
    default:
        nRandnum=rand()%2;
        pItem pBone = NULL;
        switch(nRandnum)
        {
            case 1:
                nRandnum=rand()%12;
                switch(nRandnum)
                {
                    case 0: iID=0x11; break;
                    case 1: iID=0x12; break;
                    case 2: iID=0x13; break;
                    case 3: iID=0x14; break;
                    case 4: iID=0x15; break;
                    case 5: iID=0x16; break;
                    case 6: iID=0x17; break;
                    case 7: iID=0x18; break;
                    case 8: iID=0x19; break;
                    case 9: iID=0x1A; break;
                    case 10: iID=0x1B; break;
                    case 11: iID=0x1C; break;
                }
                pBone = item::CreateFromScript( "$item_bone", pc->getBackpack() );
                pBone->setId( 0x1B00 + iID );
                client->sysmessage("You have unearthed some old bones and placed them in your pack.");
                break;
            default: // found an empty grave
               	client->sysmessage("This grave seems to be empty.");
        }
    }
}



//////////////////////////
// Function:    SmeltOre
// History:     unknown, colored ore by Cork,
//              31 March 2000 totally revamped by Duke
//              16.9.2000 removed array access and revamped a bit more (Duke)
//
// Purpose:     checks if targeted item is a forge and in range
//              then executes the smelting function with the appropriate
//              minskill and ingot type
// Remarks:     NOTE: ingot color is different from ore color for gold, silver & copper!
//
void nSkills::target_smeltOre(pClient client, pTarget t )
{
	pChar pc = client->currChar();
	pItem pi = dynamic_cast<pItem>( t->getClicked() );

	if ( !pc || !pi ) return;

	if ( pi->magic!=4) // Ripper
	{
		if( pi->isForge() )
		{
			if( !pc->hasInRange(pi, 3) )        //Check if the forge is in range
				client->sysmessage("You cant smelt here.");
			else
			{
				pItem pix=cSerializable::findItemBySerial( t->buffer[0] );
				if (!pix) return;

				AmxFunction::g_prgOverride->CallFn( AmxFunction::g_prgOverride->getFnOrdinal(AMXSMELTORE), pc->getSerial(), pix->getColor(), pix->getSerial());
			}
		}
	}

	pc->getBody()->calcWeight();
	client->statusWindow(pc,true);  //!< \todo check second argument
}

void nSkills::target_wheel(pClient client, pTarget t )	//Spinning wheel
{
	pChar pc = client->currChar();
	pItem pi = dynamic_cast<pItem>( t->getClicked() );
	if ( !pc || ! pi ) return;

	int mat = t->buffer[0];

	if( !(pi->getId() >= 0x10A4 && pi->getId() <= 0x10A6) || ! pc->hasInRange(pi, 3) )
	{
		client->sysmessage("You cant tailor here.");
		return;
	}
	
	if (!pc->checkSkill(skTailoring, 0, 1000))
	{
		client->sysmessage("You failed to spin your material.");
		return;
	}

	pItem pti = cSerializable::findItemBySerial( t->buffer[1] );
	if ( ! pti ) return;
	
	client->sysmessage("You have successfully spun your material.");
	
	pti->setCurrentName("");
	pti->amount *= 3;
	pti->setId( mat == YARN ? 0x0E1D : 0x0FA0 );
	
	pti->setCanDecay(true);
	pti->Refresh();
}

void nSkills::target_loom(pClient client, pTarget t )
{
	pChar pc = client->currChar();
	pItem pi = dynamic_cast<pItem>( t->getClicked() );
	if ( !pc || !pi ) return;

	if ( pi->magic == 4 || !( pi->getId() >= 0x105F && pi->getId() <= 0x1066 ) || ! pc->hasInRange(pi, 3) )
	{
		client->sysmessage("You can't tailor here.");
		return;
	}
	
	pItem pti = cSerializable::findItemBySerial( t->buffer[0] );
	if ( ! pti ) return;

	if(pti->amount<5)
	{
		client->sysmessage("You do not have enough material to make anything!");
		return;
	}

	if (!pc->checkSkill(skTailoring, 300, 1000))
	{
		client->sysmessage("You failed to make cloth.");
		client->sysmessage("You have broken and lost some material!");
		pti->ReduceAmount( 1+(rand() % (pti->amount)));
		pti->Refresh();
		return;
	}

	switch( pti->getId() )
	{
	case 0x0E1E: // Yarn
	case 0x0E1D:
	case 0x0E1F:
		client->sysmessage("You have made your cloth.");
		pti->setCurrentName("#");
		pti->setId( 0x175D );
		pti->setCanDecay(true);
		pti->setAmount((pti->amount-1)*10);
		break;
	case 0x0FA0: // Thread
	case 0x0FA1:
		client->sysmessage("You have made a bolt of cloth.");
		pti->setCurrentName("#");
		pti->setId( 0x0F95 );
		pti->setCanDecay(true);
		pti->setAmount(pti->amount*0.25);
		break;
	}

	pti->Refresh();
}

////////////
// Name:    CookOnFire
// By:      Ripper & Duke, 07/20/00
// Purpose: so you can use raw meat on fire
//
void nSkills::target_cookOnFire(pClient client, pTarget t )
{
	pChar pc = client->currChar();
	pItem pi = dynamic_cast<pItem>( t->getClicked() );
	if ( !pc || !pi ) return;

	uint16_t id = t->buffer[0];
	std::string matname = t->buffer_str[0];
	
	if ( pi->magic == 4 )
		return;

        pItem piRaw = cSerializable::findItemBySerial( t->buffer[1] );
	if ( !piRaw || !pc->isInBackpack(piRaw) || !pi->isCookingPlace() || !pc->hasInRange(pi, 3) )
	{
		client->sysmessage("You cannot cook here");
		return;
	}
                    
	pc->playSFX(0x01DD);   // cooking sound
	if (!pc->checkSkill(skCooking, 0, 1000))
	{
		piRaw->ReduceAmount(1+(rand() %(piRaw->amount)));
		client->sysmessage("You failed to cook the %s and drop some into the ashes.", matname.c_str());
		return;
	}
	
	pItem pi=item::CreateFromScript( "$item_raw_fish", pc->getBackpack(), piRaw->amount );
	if ( ! pi ) return;

	pi->setCurrentName("");
	pi->setId( id );
	pi->type = ITYPE_FOOD;
	pi->Refresh();
	piRaw->Delete();
	client->sysmessage("You have cooked the %s,and it smells great.", matname.c_str());
}


/*

*/

/*!
\author Luxor

I decided to base this on how OSI will be changing detect hidden.
The greater your skill, the more of a range you can detect from target position.
Hiders near the center of the detect circle are easier to detect than ones near
the edges of the detect circle. Also low skill detecters cannot find high skilled
hiders as easily as low skilled hiders.
*/
void nSkills::target_detectHidden(pClient client, pTarget t )
{
	pChar pc = client->currChar();
	if ( !pc ) return;

	AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,skDetectingHidden,AMX_BEFORE);

	sLocation location = t->getPosition();

	int32_t nSkill = pc->skill[skDetectingHidden];
	int32_t nRange = int32_t( VISRANGE * nSkill/2000.0 );
	int32_t nLow = 0;
	sLocation lCharPos = pc->getPosition();

	if ( int32_t(dist(lCharPos, location)) > 15 ) {
		client->sysmessage( "You cannot see for hidden objects so far.");
		return;
	}

	NxwCharWrapper sw;
	bool bFound = false;
	pChar pc_curr = NULL;
	pPC pc_curr_PC = NULL;
	int32_t nDist = 0;
	sw.fillCharsNearXYZ( location, nRange, true, true );

	for( sw.rewind(); !sw.isEmpty(); sw++ ) {
		pc_curr = sw.getChar();
		if ( !pc_curr )
			continue;

		if ( pc_curr->isHiddenBySkill() && !pc->isPermaHidden() )
		{
			nDist = int32_t( dist(lCharPos, pc_curr->getPosition()) );
			nLow = int32_t( (nDist * 20.0) + (pc_curr->skill[HIDING] / 2.0) );
			if ( nLow < 0 )
				nLow = 0;
			else if ( nLow > 999 )
				nLow = 999;
			if ( pc->checkSkill(skDetectingHidden, nLow, 1000) ) {
				pc_curr->unHide();
	
				pc_curr_PC = dynamic_cast<pPC>(pc_curr);
				if(!pc_curr_PC)
					continue;

				pc_curr_PC->getClient()->sysmessage( "You have been revealed!" );

				client->sysmessage( "You revelaled %s", pc_curr->getCurrentName().c_str() );
				bFound = true;
			}
		}
	}

	if( !bFound )
		client->sysmessage( "You fail to find anyone." );

	AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,skDetectingHidden,AMX_AFTER);
}

////////////////////////
// name:    HealingSkillTarget
// history: unknown, revamped by Duke, 4.06.2000
//
void nSkills::target_healingSkill(pClient client, pTarget t )
{
	pChar ph = client->currChar();
	pChar pp = dynamic_cast<pChar>( t->getClicked() );
	if ( ! ph || ! pp ) return;

	int j;
	pItem pib = cSerializable::findItemBySerial( t->buffer[0] );    // item index of bandage
	if ( ! pib ) return;

	AMXEXECSVTARGET( ph->getSerial(),AMXT_SKITARGS,HEALING,AMX_BEFORE);
	
	if (!SrvParms->bandageincombat ) {
		//pChar pc_att=cSerializable::findCharBySerial(ph->attackerserial);
		if( ph->war/* || pp->war || ( pc_att && pc_att->war)*/)
		{
			client->sysmessage("You can't heal while in a fight!");
			return;
		}
	}

	if( !ph->hasInRange(pp, 1) )
	{
		client->sysmessage("You are not close enough to apply the bandages.");
		return;
	}

	if( ((pp->getId() != BODY_MALE) || (pp->getId() != BODY_FEMALE)) && !pp->tamed) //Used on non-human and controls if tamed

        if ( ph->isInnocent() && ph != pp )
        {
		ph->helpStuff(pp);
        }

	if ( pp->isDead() )
	{
		if (ph->baseskill[HEALING] < 800 || ph->baseskill[skAnatomy]<800)
			client->sysmessage("You are not skilled enough to resurrect");
		else {
			if(ph->checkSkill(HEALING,800,1000) && ph->checkSkill(skAnatomy,800,1000) ) {
				pp->resurrect();
				client->sysmessage("Because of your skill, you were able to resurrect the ghost.");
			} else
				client->sysmessage("You failed to resurrect the ghost");

			SetTimerSec(&ph->objectdelay, nSettings::Actions::getObjectsDelay()+nSettings::Actions::getBandageDelay());
			pib->ReduceAmount(1);
			return;
		}
	}

	if (pp->poisoned>0)
	{
		if (ph->baseskill[HEALING]<=600 || ph->baseskill[skAnatomy]<=600)
		{
			client->sysmessage("You are not skilled enough to cure poison.");
			client->sysmessage("The poison in your target's system counters the bandage's effect.");
		} else {
			if (ph->checkSkill( HEALING,600,1000) && ph->checkSkill(skAnatomy,600,1000))
			{
				pp->poisoned=poisonNone;
				client->sysmessage("Because of your skill, you were able to counter the poison.");
			} else
				client->sysmessage("You fail to counter the poison");

		}

		pib->ReduceAmount(1);
		SetTimerSec(&ph->objectdelay,nSettings::Actions::getObjectsDelay()+nSettings::Actions::getBandageDelay());
		return;
	}

	if(pp->hp >= pp->getStrength())
	{
		client->sysmessage("That being is not damaged");
		return;
	}

	if(pp->HasHumanBody()) //Used on human
	{
		if (!ph->checkSkill(HEALING,0,1000))
		{
			client->sysmessage("You apply the bandages, but they barely help!");
			pp->hp++;
		}
		else
		{
			j=ph->skill[HEALING]/100*2 + 1 + rand()%2;      // a GM healer gives 42-44,
			j+=ph->skill[skAnatomy]/100*2 + 1 + rand()%2;     // a 20.0 healer 10-12. Ok ?
			//pp->hp = qmin(pp->st, j+pp->hp);
			//updatestats(i, 0);
			//sysmessage(s,"You apply the bandages and the patient looks a bit healthier.");
			int iMore1 = 0;
			(j+pp->hp > pp->getStrength()) ? iMore1 = pp->getStrength() - pp->hp : iMore1 = j;
			if(pp->getSerial()==ph->getSerial())
				tempfx::add(ph, ph, tempfx::HEALING_DELAYHEAL, iMore1/2,0,10);//allow a delay
			else
				tempfx::add(ph, pp, tempfx::HEALING_DELAYHEAL, iMore1/2,0,4);// added suggestion by Ramases //-Fraz- must be checked

		}

	}
	else //Bandages used on a non-human
	{
		if (!ph->checkSkill(skVeterinary,0,1000))
			client->sysmessage("You are not skilled enough to heal that creature.");
		else
		{
			j=((3*ph->skill[skVeterinary])/100) + rand()%6;
			pp->hp=qmin(pp->getStrength(), j+pp->hp);
			pp->updateHp();
			client->sysmessage("You apply the bandages and the creature looks a bit healthier.");
		}
	}

	SetTimerSec(&ph->objectdelay, nSettings::Actions::getObjectsDelay() + nSettings::Actions::getBandageDelay());
	pib->ReduceAmount(1);
	
	
	AMXEXECSVTARGET( ph->getSerial(),AMXT_SKITARGS,HEALING,AMX_AFTER);
}

void nSkills::target_armsLore(pClient client, pTarget t )
{
	pChar pc = client->currChar();
	pItem pi = dynamic_cast<pItem>( t->getClicked() );
	if ( !pc || !pi ) return;

	int total;
	float totalhp;
	char p2[100];
	
	AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,skArmsLore,AMX_BEFORE);

    
	if ( (pi->def==0 || pi->pileable)
	   && ((pi->lodamage==0 && pi->hidamage==0) && (pi->rank<1 || pi->rank>9)))
	{
		client->sysmessage("That does not appear to be a weapon.");
		return;
	}

	if(pc->isGM())
	{
		client->sysmessage("Attack [%i] Defense [%i] Lodamage [%i] Hidamage [%i]", pi->att, pi->def, pi->lodamage, pi->hidamage);
		return;
	}

	if (!pc->checkSkill( skArmsLore, 0, 250))
		client->sysmessage("You are not certain...");
	else {
		if( pi->maxhp==0)
			client->sysmessage(" Sorry this is a old item and it doesn't have maximum hp");
		else {
			totalhp= (float) pi->hp/pi->maxhp;
			strcpy(temp,"This item ");
			if  (totalhp>0.9)     strcpy(p2, "is brand new.");
			else if (totalhp>0.8) strcpy(p2, "is almost new.");
			else if (totalhp>0.7) strcpy(p2, "is barely used, with a few nicks and scrapes.");
			else if (totalhp>0.6) strcpy(p2, "is in fairly good condition.");
			else if (totalhp>0.5) strcpy(p2, "suffered some wear and tear.");
			else if (totalhp>0.4) strcpy(p2, "is well used.");
			else if (totalhp>0.3) strcpy(p2, "is rather battered.");
			else if (totalhp>0.2) strcpy(p2, "is somewhat badly damaged.");
			else if (totalhp>0.1) strcpy(p2, "is flimsy and not trustworthy.");
			else                  strcpy(p2, "is falling apart.");
			strcat(temp,p2);
			char temp2[33];
			sprintf(temp2," [%.1f %%]",totalhp*100);
			strcat(temp,temp2);  // Magius(CHE) 
		}

		if (pc->checkSkill(skArmsLore, 250, 510))

		{
            if (pi->hidamage)
            {
                total = (pi->hidamage + pi->lodamage)/2;
                if  ( total > 26) strcpy(p2, " Would be extraordinarily deadly.");
                else if ( total > 21) strcpy(p2, " Would be a superior weapon.");
                else if ( total > 16) strcpy(p2, " Would inflict quite a lot of damage and pain.");
                else if ( total > 11) strcpy(p2, " Would probably hurt your opponent a fair amount.");
                else if ( total > 6)  strcpy(p2, " Would do some damage.");
                else if ( total > 3)  strcpy(p2, " Would do minimal damage.");
                else              strcpy(p2, " Might scratch your opponent slightly.");
                strcat(temp,p2);

                if (pc->checkSkill( skArmsLore, 500, 1000))
                {
                    if  (pi->spd > 35) strcpy(p2, " And is very fast.");
                    else if (pi->spd > 25) strcpy(p2, " And is fast.");
                    else if (pi->spd > 15) strcpy(p2, " And is slow.");
                    else               strcpy(p2, " And is very slow.");
                    strcat(temp,p2);
                }
            }
            else
            {
                if  (pi->def> 12) strcpy(p2, " is superbly crafted to provide maximum protection.");
                else if (pi->def> 10) strcpy(p2, " Offers excellent protection.");
                else if (pi->def> 8 ) strcpy(p2, " is a superior defense against attack.");
                else if (pi->def> 6 ) strcpy(p2, " Serves as a sturdy protection.");
                else if (pi->def> 4 ) strcpy(p2, " Offers some protection against blows.");
                else if (pi->def> 2 ) strcpy(p2, " Provides very little protection.");
                else if (pi->def> 0 ) strcpy(p2, " Provides almost no protection.");
                else              strcpy(p2, " Offers no defense against attackers.");
                strcat(temp,p2);
            }
        }
        client->sysmessage(temp);

        if (!(pi->rank<1 || pi->rank>10 || SrvParms->rank_system==0))
        {
            if (pc->checkSkill(skArmsLore, 250, 500))
            {
                switch(pi->rank)
                {
			//!\todo Here we can use static const char*
                    case 1: strcpy(p2, "It seems an item with no quality!"); break;
                    case 2: strcpy(p2, "It seems an item very below standard quality!"); break;
                    case 3: strcpy(p2, "It seems an item below standard quality!"); break;
                    case 4: strcpy(p2, "It seems a weak quality item!"); break;
                    case 5: strcpy(p2, "It seems a standard quality item!"); break;
                    case 6: strcpy(p2, "It seems a nice quality item!"); break;
                    case 7: strcpy(p2, "It seems a good quality item!"); break;
                    case 8: strcpy(p2, "It seems a great quality item!"); break;
                    case 9: strcpy(p2, "It seems a beautiful quality item!"); break;
                    case 10:strcpy(p2, "It seems a perfect quality item!"); break;
                }
                client->sysmessage(p2);
            }
        }
    }
    AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,skArmsLore,AMX_AFTER);

}

void nSkills::target_itemId(pClient client, pTarget t )
{
	pChar pc = client->currChar();
	pItem pi = dynamic_cast<pItem>( t->getClicked() );

	if ( !pc || !pi ) return;

    AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,ITEMID,AMX_BEFORE);
    
	if( pi->magic!=4) // Ripper
	{
        	if (!pc->checkSkill( ITEMID, 0, 250))
			client->sysmessage("You can't quite tell what this item is...");
		else {
            
			if(pi->corpse)
			{
				client->sysmessage("You have to use your forensics evalutation skill to know more on this corpse.");
				return;
			}
			
			if (pc->checkSkill( ITEMID, 250, 500))
			if (pi->getSecondaryNameC() && (strcmp(pi->getSecondaryNameC(),"#"))) pi->setCurrentName(pi->getSecondaryNameC()); // Item identified! -- by Magius(CHE)

			if( !strncmp(pi->getCurrentName().c_str(), "#", 1) )
				pi->getName(temp2);
			else
				strcpy(temp2, pi->getCurrentName().c_str());

			client->sysmessage("You found that this item appears to be called: %s", temp2);

            // Show Creator by Magius(CHE)
            if (pc->checkSkill( ITEMID, 250, 500))
            {
                if (!pi->creator.empty())
                {
                    if (pi->madewith>0) client->sysmessage("It is %s by %s",skillinfo[pi->madewith-1].madeword,pi->creator.c_str());
                    else if (pi->madewith<0) client->sysmessage("It is %s by %s",skillinfo[0-pi->madewith-1].madeword,pi->creator.c_str());
                    else client->sysmessage("It is made by %s",pi->creator.c_str());
                } else client->sysmessage("You don't know its creator!");
            } else client->sysmessage("You can't know its creator!");
            // End Show creator

            if (!pc->checkSkill( ITEMID, 250, 500))
            {
                client->sysmessage("You can't tell if it is magical or not.");
            }
            else
            {
                if(pi->type != ITYPE_WAND)
                {
                    client->sysmessage("This item has no hidden magical properties.");
                }
                else
                {
                    if (!pc->checkSkill( ITEMID, 500, 1000))
                    {
                        client->sysmessage("This item is enchanted with a spell, but you cannot determine which");
                    }
                    else
                    {
                        if (!pc->checkSkill( ITEMID, 750, 1100))
                        {
                            client->sysmessage("It is enchanted with the spell %s, but you cannot determine how many charges remain.",spellname[(8*(pi->morex-1))+pi->morey-1]);
                        }
                        else
                        {
                            client->sysmessage("It is enchanted with the spell %s, and has %d charges remaining.",spellname[(8*(pi->morex-1))+pi->morey-1],pi->morez);
                        }
                    }
                }
            }
        }
    }

    AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,ITEMID,AMX_AFTER);
}


void nSkills::target_tame(pClient client, pTarget t )
{
	pChar pc = client->currChar();
	pChar target = dynamic_cast<pChar>( t->getClicked() );

	if ( !pc || !target ) return;

	bool tamed = false;

	if(! line_of_sight(INVALID, pc->getPosition(), target->getPosition(), losWallsChimneys | losDoors | losRoofingFlat))
		return;

	AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,skTaming,AMX_BEFORE);

	if(buffer[s][7]==0xFF) return;

	if (target->npc==1 && ( pc->distFrom(target) <= 3))
	{
		if ( (target->taming > 1000) || (target->taming ==0) )//Morrolan default is now no tame
		{
			client->sysmessage("You can't tame that creature.");
			return;
		}

		if( (target->tamed) && pc->isOwnerOf(target) )
		{
			client->sysmessage("You already control that creature!" );
			return;
		}

		if( target->tamed )
		{
			client->sysmessage("That creature looks tame already." );
			return;
		}

		for(int a=0;a<3;a++)
		{
			switch(rand()%4)
			{
				case 0: pc->talkAll( "I've always wanted a pet like you.", false); break;
				case 1: pc->talkAll( "Will you be my friend?", false); break;
				case 2: pc->talkAll( "Here %s.", false, target->getCurrentName().c_str()); break;
				case 3: pc->talkAll( "Good %s.", false, target->getCurrentName().c_str()); break;
				default:
					LogError("switch reached default");
			}
		}

		if ( (!pc->checkSkill(skTaming, 0, 1000)) || (pc->skill[skTaming] < target->taming) )
		{
			client->sysmessage("You were unable to tame it.");
			return;
		}

		pc->talk(s, "It seems to accept you as it's master!",0);
		tamed = true;
		target->setOwner(pc);

		if((target->getId()==0x000C) || (target->getId()==0x003B))
		{
			if(target->getColor() != 0x0481)
			{
				target->npcaitype=NPCAI_TAMEDDRAGON;
			}
		}
	}

	if (!tamed) client->sysmessage("You can't tame that!");

	AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,skTaming,AMX_AFTER);
}


void nSkills::target_animalLore(pClient client, pTarget t )
{
	pChar pc = client->currChar();
	pChar target = dynamic_cast<pChar>( t->getClicked() );

	if ( !pc || !target ) return;

	// blackwind distance fix
	if( target->distFrom(pc) >= 10 )
	{
		client->sysmessage("You need to be closer to find out more about them" );
		return;
	}

	AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,skAnimalLore,AMX_BEFORE);

	if (target->isGMorCounselor())
	{
		client->sysmessage("Little is known of these robed gods.");
		return;
	}
	else if( target->HasHumanBody() ) // Used on human
	{
		client->sysmessage("The human race should use dvorak!");
	}
	else // Lore used on a non-human
	{
        	if (target->checkSkill( skAnimalLore, 0, 1000))
        	{
			pChar target_owner = target->getOwner();
			if(!target_owner) return;

			sprintf(temp, "Attack [%i] Defense [%i] Taming [%i] Hit Points [%i] is Loyal to: [%s]", target->att, target->def, target->taming/10, target->hp, target->isTamed() ? target_owner->getCurrentName().c_str() : "himself" );
			target->emote(s,temp,1);
        	}
        	else
        	{
            		client->sysmessage("You can not think of anything relevant at this time.");
        	}
	}

    AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,skAnimalLore,AMX_AFTER);
}

void nSkills::target_forensics(pClient client, pTarget t )
{
	pChar pc = client->currChar();
	pItem pi = dynamic_cast<pItem>( t->getClicked() );
	if ( !pc || !pi ) return;

	AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,skForensics,AMX_BEFORE);

	int curtim=getclock();


	if (!pi->corpse) {
    		client->sysmessage("That does not appear to be a corpse.");
    		return;
	}

	if(pc->isGM()) {
    		client->sysmessage("The %s is %i seconds old and the killer was %s.", pi->getCurrentName().c_str(), (curtim-pi->murdertime)/SECS, pi->murderer.c_str());
	} else {
		if (!pc->checkSkill( skForensics, 0, 500)) client->sysmessage("You are not certain about the corpse.");
		else
		{
			const char *tmp = NULL;

			static const char strFew[] = "few";
			static const char strMany[] = "many";
			static const char strManyMany[] = "many many";
			
			if ( (curtim-pi->murdertime)/SECS > 180 )
				tmp = strManyMany;
			else if ( (curtim-pi->murdertime)/SECS > 60 )
				tmp = strMany;
			else
				tmp = strFew;

			client->sysmessage("The %s is %s seconds old.", pi->getCurrentName().c_str(), tmp);

			if (!pc->checkSkill( skForensics, 500, 1000) || pi->murderer.empty())
				client->sysmessage("You can't say who was the killer.");
			else
				pc->sysmessage("The killer was %s.", pi->murderer.c_str());
		}
	}

	AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,skForensics,AMX_AFTER);
}



/*!
\brief Poison target
\author AntiChrist, rewritten by Endymion
\param client the client
\param t the target
\note pi->morez is the poison type
*/
void target_poisoning2(pClient client, pTarget t )
{
	pChar pc = client->currChar();
	pItem poison = cSerializable::findItemBySerial(t->buffer[0]);
	if ( !pc || !poison ) return;

	AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,skPoisoning,AMX_BEFORE);

	if(poison->type!=ITYPE_POTION || poison->morey!=6)
	{
		client->sysmessage("That is not a valid poison!");
		pc->objectdelay = 0;
		return;
	}

	if ( !CheckInPack(client, poison) )
	{
		pc->objectdelay = 0;
		return;
	}

	pItem pi = dynamic_cast<pItem>( t->getClicked() );
	if(!pi) {
		client->sysmessage("You can't poison that item.");
		pc->objectdelay = 0;
		return;
	}

	if ( !CheckInPack(client, pi) )
	{
		pc->objectdelay = 0;
		return;
	}

	if (!pi->isFencing1H() && !pi->isSword() && !pi->isArrow() && !pi->isBolt())
	{
		client->sysmessage("You cannot poison that item");
		pc->objectdelay = 0;
		return;
	}

	if ( (pi->isArrow()||pi->isBolt()) && pi->amount>5)
	{
		client->sysmessage("Too many items you can poison at least 5 items");
		pc->objectdelay = 0;
		return;
	}

	pFunctionHandle evt = pi->getEvent(cItem::evtItmOnPoisoned);
	if ( evt )
	{
		tVariantVector params = tVariantVector(3);
		params[0] = pi->getSerial(); params[1] = client;
		params[2] = poison->morez;
		evt->setParams(params);
		evt->execute();
		if ( evt->isBypassed() )
			return;
	}
	
	int success=0;
	switch(poison->morez)
	{
        case 0: break;
        case 1: success=pc->checkSkill( skPoisoning, 0, 500);	break; //lesser poison
        case 2:
			{
				success=pc->checkSkill( skPoisoning, 251, 651);
				if (pc->skill[skPoisoning]<650) success=0;
				break;//poison
			}
        case 3:
			{
				success=pc->checkSkill( skPoisoning, 851, 1051);
				if (pc->skill[skPoisoning]<850) success=0;
				break;//greater poison
			}
        case 4:
			{
				success=pc->checkSkill( skPoisoning, 1201, 1401);
				if (pc->skill[skPoisoning]<950) success=0;
				break;//deadly poison
			}
        default:
            LogError("switch reached default");
            return;
	}

	if (poison->morez!=0) {
		pc->playSFX( 0x0247);
		if(success!=0)
		{
			if(pi->poisoned<(PoisonType)poison->morez) pi->poisoned=(PoisonType)poison->morez;
			client->sysmessage("You successfully poison that item.");
		}
		else
		{
			client->sysmessage("You fail to apply the poison.");
			pi->hp-=poison->morez;
			pi->poisoned=poisonNone;
			if(pi->hp<=0)
			{
				client->sysmessage("Your weapon has been destroyed");
				//<Luxor>
				nPackets::Sent::DeleteObj pk(pi);
				client->sendPacket(&pk);
				pi->Delete();
			}
		}
	}

	poison->Delete();
	//spawn an empty bottle after poisoning
	pItem emptybottle = new cItem(cItem::nextSerial());

	emptybottle->setId( 0x0F0E );
	emptybottle->pileable=1;
	emptybottle->MoveTo( pc->getPosition() );
	emptybottle->priv|=0x01;
	emptybottle->Refresh();

	AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,skPoisoning,AMX_AFTER);
}

void nSkills::target_poisoning(pClient client, pTarget t )
{
	pItem poison = dynamic_cast<pItem>( t->getClicked() );
	if(!poison) return;

	pTarget targ = clientInfo[client->toInt()]->newTarget( new cItemTarget() );
	targ->code_callback=target_poisoning2;
	targ->buffer[0]=poison->getSerial();
	targ->send( client );

	client->sysmessage( "What item do you want to poison?") ;
}

void nSkills::target_tinkering(pClient client, pTarget t )
{
	pChar pc = client->currChar();
	pItem pi = dynamic_cast<pItem>( t->getClicked() );
	if ( !pc || !pi ) return;

	AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,skTinkering,AMX_BEFORE);

	if ( pi->magic == 4 || ( pi->getId() != 0x1BEF && pi->getId() 1= 0x1BF2 && pi->isLog() ) )
	{
		client->sysmessage("You cannot use that material for tinkering.");
		AMXEXECSVTARGET(pc->getSerial(),AMXT_SKITARGS,skTinkering,AMX_AFTER);
	}

	
	if ( ! CheckInPack(client,pi) )
		return;
		
	int amt;
	amt = pc->CountItems(pi->getId(), pi->getColor());
	if(amt<2)
	{
		client->sysmessage("You don't have enough ingots to make anything.");
		return;
	}
	
	if ( ! pi->isLog() )
	{
		MakeMenu(pc,80,skTinkering,pi);
		return;
	}
	
	if (amt<4)
	{
		client->sysmessage("You don't have enough log's to make anything.");
		return;
	}
	
	MakeMenu(pc,70,skTinkering,pi);
}

//////////////////////////////////
// name:    cTinkerCombine
// history: by Duke, 3.06.2000
// Purpose: handles the combining of two (tinkering-)items after the user
//          dclicked one and then targeted the second item
//
//          It's a base class for three derived classes that handle the
//          old interfaces
//
class cTinkerCombine    // Combining tinkering items
{
protected:
    char* failtext;
//    short badsnd1;
//    short badsnd2;
    short badsnd;
    short itembits;
    short minskill;
    short id2;
public:
    cTinkerCombine(short badsnd=0x0051, char *failmsg="You break one of the parts.")
    {
        this->badsnd=badsnd;
        failtext=failmsg;
        itembits=0;
        minskill=100;
    }
    /*
    virtual void delonfail(SOCK s)      {deletematerial(s, itemmake[s].needs/2);}
    virtual void delonsuccess(SOCK s)   {deletematerial(s, itemmake[s].needs);}
    virtual void failure(SOCK s)        {delonfail(s);playbad(s);failmsg(s);}
    */
    virtual void failmsg(pClient client)         {client->sysmessage(failtext);}
    virtual void playbad(pClient client)         {client->playSFX(badsnd);}
    virtual void playgood(pClient client)        {client->playSFX(0x002A);}
    virtual void checkPartID(short id)  {;}
    virtual bool decide()               {return (itembits == 3) ? true : false;}
    virtual void createIt(pClient client)        {;}
    static cTinkerCombine* factory(short combinetype);
    
	virtual void DoIt(pClient client, pTarget t )
	{
		pItem piClick = cSerializable::findItemBySerial( t->buffer[0] );

		if(!piClick)
		{
			client->sysmessage("Original part no longer exists");
			return;
		}

		pItem piTarg = dynamic_cast<pItem>( t->getClicked() );
		if (piTarg==NULL || piTarg->magic==4)
		{
			client->sysmessage("You can't combine these.");
			return;
		}

		// make sure both items are in the player's backpack
		pItem pPack=Check4Pack(client);
		if (!pPack) return;

		if ( (piTarg->getContSerial() != pPack->getSerial()) ||
		     (piClick->getContSerial() != pPack->getSerial()) )
		{
			client->sysmessage("You can't use material outside your backpack");
			return;
		}

        // make sure the parts are of correct IDs AND they are different
        checkPartID( piClick->getId() );
        checkPartID( piTarg->getId() );
        
		if (!decide())
			client->sysmessage("You can't combine these.");
        
		else {
			pChar pc = client->currChar();

			if (pc->skill[skTinkering]<minskill)
			{
				client->sysmessage("You aren't skilled enough to even try that!");
				return;
			}

			if( !pc->checkSkill( skTinkering, minskill, 1000 ) )
			{
				failmsg(client);
				pItem piLoser= rand()%2 ? piTarg : piClick;
				piLoser->ReduceAmount(1);
				playbad(client);
			} else {
				client->sysmessage("You combined the parts");
				piClick->ReduceAmount(1);
				piTarg->ReduceAmount(1);        // delete both parts
				createIt(client);                        // spawn the item
				playgood(client);
			}
		}
	}
};

class cTinkCreateAwG : public cTinkerCombine
{
public:
    cTinkCreateAwG() : cTinkerCombine() {}
    virtual void checkPartID(short id)
    {
        if (id==0x105B || id==0x105C) itembits |= 0x01; // axles
        if (id==0x1053 || id==0x1054) itembits |= 0x02; // gears
    }
    virtual void createIt(pClient client)
    {
		pChar pc = client->currChar();
		if (!pc) return;

		item::CreateFromScript( "$item_axles_with_gears", pc->getBackpack() );
    }
};

class cTinkCreateParts : public cTinkerCombine
{
public:
	cTinkCreateParts() : cTinkerCombine() {}

    virtual void checkPartID(short id)
    {
        if (id==0x1051 || id==0x1052) itembits |= 0x01; // axles with gears
        if (id==0x1055 || id==0x1056) itembits |= 0x02; // hinge
        if (id==0x105D || id==0x105E) itembits |= 0x04; // springs
    }
    virtual bool decide()
    {
        if (itembits == 3) {id2=0x59; minskill=300; return true;}   // sextant parts
        if (itembits == 5) {id2=0x4F; minskill=400; return true;}   // clock parts
        return false;
    }
    
	virtual void createIt(pClient client)
	{
		pChar pc = client->currChar();
		if (!pc) return;

		if (id2 == 0x4F)
			item::CreateFromScript( "$item_clock_parts", pc->getBackpack() );
		else
			item::CreateFromScript( "$item_sextant_parts", pc->getBackpack() );
	}
};

class cTinkCreateClock : public cTinkerCombine
{
public:
	cTinkCreateClock() : cTinkerCombine() {}

    virtual void checkPartID(short id)
    {
        if (id==0x104D || id==0x104E) itembits |= 0x01; // clock frame
        if (id==0x104F || id==0x1050) itembits |= 0x02; // clock parts
    }
    virtual bool decide()   {minskill=600; return cTinkerCombine::decide();}
    
	virtual void createIt(pClient client)
	{
		pChar pc = client->currChar();
		if (!pc) return;

		item::CreateFromScript( "$item_clock", pc->getBackpack() );
	}
};

#define cTC_AwG     11
#define cTC_Parts   22
#define cTC_Clock   33

cTinkerCombine* cTinkerCombine::factory(short combinetype)
{
    switch (combinetype)
    {
        case cTC_AwG:   return new cTinkCreateAwG();
        case cTC_Parts: return new cTinkCreateParts();    // clock/sextant parts
        case cTC_Clock: return new cTinkCreateClock();    // clock
        default:        return new cTinkerCombine();      // a generic handler
    }
}

void nSkills::target_tinkerAxel(pClient client, pTarget t )
{
    cTinkerCombine *ptc = cTinkerCombine::factory(cTC_AwG);
    ptc->DoIt(client, t);
}

void nSkills::target_tinkerAwg(pClient client, pTarget t )
{
    cTinkerCombine *ptc = cTinkerCombine::factory(cTC_Parts);
    ptc->DoIt(client, t);
}

void nSkills::target_tinkerClock(pClient client, pTarget t )
{
    cTinkerCombine *ptc = cTinkerCombine::factory(cTC_Clock);
    ptc->DoIt(client, t);
}

void nSkills::target_repair(pClient client, pTarget t )
{
	pChar pc = client->currChar();
	pItem pi = dynamic_cast<pItem>( t->getClicked() );
	if ( !pc || !pi ) return;

    
	uint16_t smithing = pc->baseskill[skBlacksmithing];

    
	if (smithing < 500)
	{
		client->sysmessage("* Your not skilled enough to repair items.*");
		return;
	}

	if ( pi->magic == 4)
		return;
	
	if (!CheckInPack(s,pi))
		return;

	if (!pi->hp)
	{
		client->sysmessage(" That item cant be repaired.");
		return;
	}

	if(!AnvilInRange(client))
	{
		client->sysmessage(" Must be closer to the anvil.");
		return;
	}

	if (pi->hp>=pi->maxhp)
	{
		client->sysmessage(" That item is at full strength.");
		return;
	}


	int16_t dmg=4;    // damage to maxhp
	if      ((smithing>=900)) dmg=1;
	else if ((smithing>=700)) dmg=2;
	else if ((smithing>=500)) dmg=3;

	if (pc->checkSkill(skBlacksmithing, 0, 1000))
	{
		pi->maxhp-=dmg;
		pi->hp=pi->maxhp;
		client->sysmessage(" * the item has been repaired.*");
	} else {
		pi->hp-=2;
		pi->maxhp-=1;
		client->sysmessage(" * You fail to repair the item. *");
		client->sysmessage(" * You weaken the item.*");
	}
}
