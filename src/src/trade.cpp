/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "common_libs.h"
#include "objects/citem.h"
#include "objects/cpc.h"

// this is a q&d fix for 'sell price higher than buy price' bug (Duke, 30.3.2001)
static bool items_match(pItem pi1,pItem pi2)
{
	if ( ! pi1 || ! pi2 ) return false;

	if (pi1->getId()==pi2->getId() &&
		pi1->type==pi2->type &&
		!(pi1->getId()==0x14F0 && (pi1->morex!=pi2->morex)) &&	// house deeds only differ by morex
		!(pi1->isShield() && pi1->getSecondaryName() == pi2->getSecondaryName()) &&	// magic shields only differ by secondary name
		pi1->getColor()==pi2->getColor())		// this enables color checking for armour, but disables dyed clothes :)
		return true;
	return false;

}

/*
void restock(bool total)
{
        //Luxor: new cAllObjects system -> this should be changed soon... too slow!!

	cAllObjectsIter objs;
	pItem pi;
	for( objs.rewind(); !objs.IsEmpty(); objs++ )
	{
		if( cSerializable::isCharSerial( objs.getSerial() ) )
			continue;

		pi = (pItem)(objs.getObject());

		if(!pi || pi->layer!=0x1A )
			continue;

		NxwItemWrapper si;
		si.fillItemsInContainer( pi, false ); //ndEndy We don't need subcontainer right?
		for( si.rewind(); !si.isEmpty(); si++ ) {
			pItem pj=si.getItem();
			if( !pj || !pj->restock )
				continue;

			if ( total )
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
	timer=getclock();
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

		pItem pi= cSerializable::findItemBySerial( needrestock.front() );
		this->needrestock.pop();
		if( pi && pi->layer==LAYER_TRADE_RESTOCK  ) {

			NxwItemWrapper si;
			si.fillItemsInContainer( pi, false ); //ndEndy We don't need subcontainer right?
			for( si.rewind(); !si.isEmpty(); si++ ) {

				pItem pj=si.getItem();
				if( !pj || ( pj->restock <= 0 ) )
					continue;

				int a=qmin(pj->restock, (pj->restock/2)+1);
				pj->amount+=a;
				pj->restock-=a;

			}

			restocked.push( pi->getSerial() );

		}

	}

	updateTimer();
}

void cRestockMng::doRestockAll()
{
	rewindList();
	
	while( !needrestock.empty() ) {

		pItem pi= cSerializable::findItemBySerial( needrestock.front() );
		this->needrestock.pop();
		if( pi && pi->layer==LAYER_TRADE_RESTOCK ) {

			NxwItemWrapper si;
			si.fillItemsInContainer( pi, false ); //ndEndy We don't need subcontainer right?
			for( si.rewind(); !si.isEmpty(); si++ ) {

				pItem pj=si.getItem();
				if( !pj || ( pj->restock <= 0 ) )
					continue;

				pj->amount+=pj->restock;
				pj->restock=0;

			}

			restocked.push( pi->getSerial() );

		}

	}

	updateTimer();
}

void cRestockMng::addNewRestock( pItem pi )
{
	if ( ! pi ) return;
	this->needrestock.push( pi->getSerial() );
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
		timer=getclock()+ServerScp::g_nRestockTimeRate*60*SECS;
	else
		timer=getclock()+CHECK_RESTOCK_EVERY*SECS;
}

cRestockMng* Restocks = NULL;
