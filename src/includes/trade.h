/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#ifndef __TRADE_H__
#define __TRADE_H__

void sellaction(int s);
pItem tradestart(pChar pc1, pChar pc2);
void clearalltrades();
void trademsg(int s);



typedef struct {
	int layer;
	pItem item;
	int amount;
} buyeditem;

#define RESTOCK_PER_TIME 20
#define CHECK_RESTOCK_EVERY 3

#include <queue>

class cRestockMng {

private:

	TIMERVAL timer;
	std::queue< uint32_t > needrestock;
	std::queue< uint32_t > restocked;

	void rewindList();
	void updateTimer();


public:
	
	cRestockMng();


	void doRestock();
	void doRestockAll();

	void addNewRestock( pItem pi );

};

extern cRestockMng* Restocks;


#endif
