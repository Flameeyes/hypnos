  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include "nxwcommn.h"
#include "network.h"
#include "sndpkg.h"
#include "itemid.h"
#include "debug.h"
#include "amx/amxcback.h"
#include "set.h"
#include "layer.h"
#include "trade.h"
#include "commands.h"
#include "weight.h"
#include "chars.h"
#include "items.h"
#include "inlines.h"
#include "nox-wizard.h"
#include "utils.h"




// this is a q&d fix for 'sell price higher than buy price' bug (Duke, 30.3.2001)
static bool items_match(P_ITEM pi1,P_ITEM pi2)
{
	VALIDATEPIR(pi1,false);
	VALIDATEPIR(pi2,false);

	if (pi1->getId()==pi2->getId() &&
		pi1->type==pi2->type &&
		!(pi1->getId()==0x14F0 && (pi1->morex!=pi2->morex)) &&	// house deeds only differ by morex
		!(pi1->IsShield() && strcmp(pi1->getSecondaryNameC(),pi2->getSecondaryNameC())) &&	// magic shields only differ by secondary name
		pi1->getColor()==pi2->getColor())		// this enables color checking for armour, but disables dyed clothes :)
		return true;
	return false;

}

void sellaction(NXWSOCKET s)
{
	if ( s < 0 || s >= now )
		return;

	P_CHAR pc=MAKE_CHAR_REF(currchar[s]);
	VALIDATEPC(pc);

	P_ITEM npa=NULL, npb=NULL, npc=NULL;
	int i, amt, value=0, totgold=0;

	if (buffer[s][8]!=0)
	{
		P_CHAR pc_n=pointers::findCharBySerPtr(buffer[s]+3);
		VALIDATEPC(pc_n);

		NxwItemWrapper si;
		si.fillItemWeared( pc_n, true, true, false );
		for( si.rewind(); !si.isEmpty(); si++ )
		{
			P_ITEM pi=si.getItem();
			if(ISVALIDPI(pi)) {
				if (pi->layer == LAYER_TRADE_RESTOCK) npa=pi;	// Buy Restock container
				if (pi->layer == LAYER_TRADE_NORESTOCK) npb=pi;	// Buy no restock container
				if (pi->layer == LAYER_TRADE_BOUGHT) npc=pi;	// Sell container
			}
		}

		// Pre Calculate Total Amount of selling items to STOPS if the items if greater than SELLMAXITEM - Magius(CHE)

		P_ITEM join=NULL;
		uint32_t maxsell=0;
		i=buffer[s][8];
		if (i>256) return;
		for (i=0;i<buffer[s][8];i++)
		{
			amt=ShortFromCharPtr(buffer[s]+9+(6*i)+4);
			maxsell+=amt;
		}
		if (maxsell>SrvParms->sellmaxitem)
		{
			char tmpmsg[256];
			sprintf(tmpmsg,TRANSLATE("Sorry %s but i can buy only %i items at time!"), pc->getCurrentNameC(), SrvParms->sellmaxitem);
			pc_n->talkAll(tmpmsg,0);
			return;
		}

		for (i=0;i<buffer[s][8];i++)
		{
			P_ITEM pSell=pointers::findItemBySerPtr(buffer[s]+9+(6*i));	// the item to sell
			if (!ISVALIDPI(pSell))
				continue;
			amt=ShortFromCharPtr(buffer[s]+9+(6*i)+4);

			// player may have taken items out of his bp while the sell menu was up ;-)
			if (pSell->amount<amt)
			{
				pc_n->talkAll(TRANSLATE("Cheating scum! Leave now, before I call the guards!"),0);
				return;
			}

			// Search the buy restock Container
			if( ISVALIDPI(npa) ) {
				NxwItemWrapper si2;
				si2.fillItemsInContainer( npa, false );
				for( si2.rewind(); !si2.isEmpty(); si2++ )
				{
					P_ITEM pi=si2.getItem();
					if( ISVALIDPI(pi) && items_match(pi,pSell))
						join=pi;
				}
			}

			// Search the sell Container to determine the price
			if( ISVALIDPI(npc) ) {
				NxwItemWrapper si2;
				si2.fillItemsInContainer( npc, false );
				for( si2.rewind(); !si2.isEmpty(); si2++ )
				{
					P_ITEM pi=si2.getItem();
					if( ISVALIDPI(pi) && items_match(pi,pSell))
					{
						value = pi->value;
						value = pSell->calcValue(value);
						if (SrvParms->trade_system==1)
							value=calcGoodValue(s,DEREF_P_ITEM(pSell),value,1); // Fixed for adv trade --- by Magius(CHE) §
						break;	// let's take the first match
					}
				}
			}
			totgold+=(amt*value);	// add to the bill

			if (join!=NULL)	// The item goes to the container with restockable items
			{


				join->amount+=amt;
				join->restock-=amt;
				pSell->ReduceAmount(amt);
			}
			else
			{
				if(ISVALIDPI(npb)) {
					NxwSocketWrapper sw;
					sw.fillOnline( pSell );

					for( sw.rewind(); !sw.isEmpty(); sw++ )
					{
						SendDeleteObjectPkt( sw.getSocket(), pSell->getSerial32() );
					}

					pSell->setContainer( npb );
					if (pSell->amount!=amt)
						Commands::DupeItem(s, DEREF_P_ITEM(pSell), pSell->amount-amt);
				}
			}
		}
		pc->addGold(totgold);
		pc->playSFX( goldsfx(totgold) );
	}

	uint8_t clearmsg[8] = { 0x3B, 0x00, };
	ShortToCharPtr(0x08, clearmsg +1); 				// Packet len
	LongToCharPtr( LongFromCharPtr(buffer[s] +3), clearmsg +3);	// vendorID
	clearmsg[7]=0x00;						// Flag:  0 => no more items  0x02 items following ... 
	Xsend(s, clearmsg, 8);
//AoS/	Network->FlushBuffer(s);
}

///////////////////////////////////////////////////////////////////
// Function name     : tradestart
// Return type       : P_ITEM
// Author            : Luxor
pItem tradestart(pChar pc1, pChar pc2)
{
	if (
		( ! pc1 || ! pc2 ) ||
		( !pc1->getClient() || ! pc2->getClient() ) ||
		( pc1->dead || pc2->dead ) ||
		( pc1->distFrom( pc2 ) > 5 )
	   )
		return NULL;

        pItem bp1 = pc1->getBackpack();
	if ( ! bp1 )
	{
		pc1->sysmsg(TRANSLATE("Time to buy a backpack!"));
		return NULL;
	}
        pItem bp2 = pc2->getBackpack();
	if ( ! bp2 )
	{
		pc1->sysmsg(TRANSLATE("Time to buy a backpack!"));
		return NULL;
	}

	pItem cont1 = item::CreateFromScript( "$item_a_bulletin_board" );
	pItem cont2 = item::CreateFromScript( "$item_a_bulletin_board" );
	if ( ! cont1 || ! cont2 )
		return NULL;

        cont1->setPosition(26, 0, 0);
        cont2->setPosition(26, 0, 0);
        cont1->setContainer(pc1);
        cont2->setContainer(pc2);
	cont1->layer = cont2->layer = 0;
	cont1->type = cont2->type = 1;
	cont1->dye = cont2->dye = 0;
        //TODO find client1 & client2
        client1->addItemToContainer(cont1);
        client2->addItemToContainer(cont1);
        client1->addItemToContainer(cont2);
        client2->addItemToContainer(cont2);

        cont2->moreb1= cont1->getSerial().ser1;
	cont2->moreb2= cont1->getSerial().ser2;
	cont2->moreb3= cont1->getSerial().ser3;
	cont2->moreb4= cont1->getSerial().ser4;
	cont1->moreb1= cont2->getSerial().ser1;
	cont1->moreb2= cont2->getSerial().ser2;
	cont1->moreb3= cont2->getSerial().ser3;
	cont1->moreb4= cont2->getSerial().ser4;
	cont2->morez=0;
	cont1->morez=0;

        uint8_t msg[90];
        msg[0]=0x6F;    //Header Byte
	msg[1]=0;       //Size
	msg[2]=47;      //Size
	msg[3]=0;       //Initiate
        LongToCharPtr(pc2->getSerial32(), msg+4);
	LongToCharPtr(cont1->getSerial32(), msg+8);
	LongToCharPtr(cont2->getSerial32(), msg+12);
	msg[16]=1;
	strcpy((char*)&(msg[17]), pc2->getCurrentNameC());
	Xsend(s1, msg, 47);

        uint8_t msg2[90];
        msg2[0]=0x6F;   //Header Byte
	msg2[1]=0;      //Size
	msg2[2]=47;     //Size
	msg2[3]=0;      //Initiate
	LongToCharPtr(pc1->getSerial32(),msg2+4);
	LongToCharPtr(cont2->getSerial32(), msg2+8);
	LongToCharPtr(cont1->getSerial32(), msg2+12);
	msg2[16]=1;
	strcpy((char*)&(msg2[17]), pc1->getCurrentNameC());
	Xsend(s2, msg2, 47);

        return cont1;
}


void clearalltrades()
{
/*        P_ITEM pi = NULL;
        P_ITEM pj = NULL;
        P_CHAR pc = NULL;
        P_ITEM pack = NULL;
        uint32_t i = 0;
        for (i = 0; i < itemcount; i++) {
                pi = MAKE_ITEM_REF(i);
                if (!ISVALIDPI(pi)) continue;
                if ((pi->type==1) && (pi->getPosition("x")==26) && (pi->getPosition("y")==0) && (pi->getPosition("z")==0) &&
			(pi->id()==0x1E5E))
		{
                        pc = pointers::findCharBySerial(pi->getContSerial());
                        if (ISVALIDPC(pc)) {
                                pack = pc->getBackpack();
                                if (ISVALIDPI(pack)) {
                                    NxwItemWrapper si;
									si.fillItemsInContainer( pi );
									for( si.rewind(); !si.isEmpty(); si++ ) {
										pj = si.getItem();
                                        if( ISVALIDPI(pj)) {
                       						pj->setContainer(pack);
                        				}
									}
                                    pi->deleteItem();
        							InfoOut("trade cleared\n");
                                }
                        }
                }
        }*/
}



/*
void restock(bool total)
{
        //Luxor: new cAllObjects system -> this should be changed soon... too slow!!

	cAllObjectsIter objs;
	P_ITEM pi;
	for( objs.rewind(); !objs.IsEmpty(); objs++ )
	{
		if( isCharSerial( objs.getSerial() ) )
			continue;

		pi = (P_ITEM)(objs.getObject());

		if(!ISVALIDPI(pi) || pi->layer!=0x1A )
			continue;

		NxwItemWrapper si;
		si.fillItemsInContainer( pi, false ); //ndEndy We don't need subcontainer right?
		for( si.rewind(); !si.isEmpty(); si++ ) {
			P_ITEM pj=si.getItem();
			if( !ISVALIDPI(pj) || !pj->restock )
				continue;

			if (total==true)
			{
				pj->amount+=pj->restock;
				pj->restock=0;
			}
			else
			{
				if (pj->restock>0)
				{
					int a=qmin(pj->restock, (pj->restock/2)+1);
					pj->amount+=a;
					pj->restock-=a;
				}
			}
		}
		//ndEndy i dont know why is here and what do..
		//if (SrvParms->trade_system==1) StoreItemRandomValue(pi,-1);// Magius(CHE) (2)
	}
}
*/








cRestockMng::cRestockMng()
{
	timer=uiCurrentTime;
}

void cRestockMng::doRestock()
{


	if( !TIMEOUT( timer ) )
		return;

	if( needrestock.empty() ) {
		rewindList();
	}

	int count= RESTOCK_PER_TIME;

	while( ( --count>0 ) && ( !needrestock.empty() ) ) {

		P_ITEM pi= pointers::findItemBySerial( needrestock.front() );
		this->needrestock.pop();
		if( ISVALIDPI(pi) && pi->layer==LAYER_TRADE_RESTOCK  ) {

			NxwItemWrapper si;
			si.fillItemsInContainer( pi, false ); //ndEndy We don't need subcontainer right?
			for( si.rewind(); !si.isEmpty(); si++ ) {

				P_ITEM pj=si.getItem();
				if( !ISVALIDPI(pj) || ( pj->restock <= 0 ) )
					continue;

				int a=qmin(pj->restock, (pj->restock/2)+1);
				pj->amount+=a;
				pj->restock-=a;

			}

			restocked.push( pi->getSerial32() );

		}

	}

	updateTimer();


}

void cRestockMng::doRestockAll()
{

	rewindList();

	while( !needrestock.empty() ) {

		P_ITEM pi= pointers::findItemBySerial( needrestock.front() );
		this->needrestock.pop();
		if( ISVALIDPI(pi) && pi->layer==LAYER_TRADE_RESTOCK ) {

			NxwItemWrapper si;
			si.fillItemsInContainer( pi, false ); //ndEndy We don't need subcontainer right?
			for( si.rewind(); !si.isEmpty(); si++ ) {

				P_ITEM pj=si.getItem();
				if( !ISVALIDPI(pj) || ( pj->restock <= 0 ) )
					continue;

				pj->amount+=pj->restock;
				pj->restock=0;

			}

			restocked.push( pi->getSerial32() );

		}

	}

	updateTimer();

}

void cRestockMng::addNewRestock( P_ITEM pi )
{
	VALIDATEPI(pi);
	this->needrestock.push( pi->getSerial32() );
}

void cRestockMng::rewindList()
{
	while( !restocked.empty() ) {
		needrestock.push( restocked.front() );
		restocked.pop();
	}
}

void cRestockMng::updateTimer()
{
	if( needrestock.empty() ) //end restock.. next after much time
		timer=uiCurrentTime+ServerScp::g_nRestockTimeRate*60*MY_CLOCKS_PER_SEC;
	else
		timer=uiCurrentTime+CHECK_RESTOCK_EVERY*MY_CLOCKS_PER_SEC;
}



cRestockMng* Restocks = NULL;














