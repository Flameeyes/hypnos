/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief GM Pages management implementation
*/

namespace Pages {

	/*!
	\brief Gets the next page
	\todo Rewrite it merging counselor and gm calls
	*/
	void cmdNextCall(pClient cli)
	{
		// Type is the same as it is in showgmqueue()
		
		int i;
		int x=0;
		
		P_CHAR pc_currchar = MAKE_CHAR_REF(currchar[s]);
		
		if(pc_currchar->callnum!=0)
		{
			donewithcall(s, type);
		}
		
		if(type==1) //Player is a GM
		{
			for(i=1;i<MAXPAGES;i++)
			{
				if(gmpages[i].handled==0)
				{
					P_CHAR pj=pointers::findCharBySerial( gmpages[i].serial.serial32 );
					if(ISVALIDPC(pj))
					{
						Location dest= pj->getPosition();
						
						pc_currchar->sysmsg("");
						pc_currchar->sysmsg("Transporting to next call: %s", gmpages[i].name);
						pc_currchar->sysmsg("Problem: %s.", gmpages[i].reason);
						pc_currchar->sysmsg("Serial number: %x", gmpages[i].serial.serial32);
						pc_currchar->sysmsg("Paged at %s.", gmpages[i].timeofcall);
						gmpages[i].handled=1;
						pc_currchar->MoveTo( dest );
						pc_currchar->callnum=i;
						pc_currchar->teleport();
						x++;
					}// if
					if(x>0)break;
				}// if
			}// for
			if(x==0) pc_currchar->sysmsg("The GM queue is currently empty");
		} //end first IF
		else //Player is only a counselor
		{
			x=0;
			for(i=1;i<MAXPAGES;i++)
			{
				if(counspages[i].handled==0)
				{
					P_CHAR pj=pointers::findCharBySerial( counspages[i].serial.serial32 );
					if(ISVALIDPC(pj))
					{
						Location dest= pj->getPosition();
						
						pc_currchar->sysmsg("");
						pc_currchar->sysmsg("Transporting to next call: %s", counspages[i].name);
						pc_currchar->sysmsg("Problem: %s.", counspages[i].reason);
						pc_currchar->sysmsg("Serial number: %x", counspages[i].serial.serial32);
						pc_currchar->sysmsg("Paged at %s.", counspages[i].timeofcall);
						counspages[i].handled=1;
						pc_currchar->MoveTo( dest );
						pc_currchar->callnum=i;
						pc_currchar->teleport();
						x++;
						break;
					}// if
				}// else
				if(x>0)break;
			}//for
			if(x==0) pc_currchar->sysmsg("The Counselor queue is currently empty");
		}//if
	}	
} // namespace Pages