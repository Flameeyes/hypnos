/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#ifndef __TRADE_H__
#define __TRADE_H__

#define RESTOCK_PER_TIME 20
#define CHECK_RESTOCK_EVERY 3

class cRestockMng {

private:

	uint32_t timer;
	queue< uint32_t > needrestock;
	queue< uint32_t > restocked;

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
