/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "common_libs.h"
#include "network.h"
#include "sndpkg.h"
#include "calendar.h"
#include "sregions.h"
#include "basics.h"
#include "inlines.h"

void loadregions()//New -- Zippy spawn regions
{
	int i, noregion, l=0, a=0,loopexit=0;
	char sect[512];
	int actgood=INVALID; // Magius(CHE)
	cScpIterator* iter = NULL;
	char script1[1024];
	char script2[1024];

	for (i=0;i<256;i++)
	{
		region_st &regionRef = region[i];
		
		regionRef.inUse = false;
		regionRef.midilist=0;
		regionRef.priv=0;
		regionRef.drychance=0;
		regionRef.keepchance=0;
		regionRef.wtype = 0;
		regionRef.snowchance=0;
		regionRef.rainchance=0;
		regionRef.forcedseason = INVALID;
		regionRef.ignoreseason = false;
		regionRef.name[0]=0;
		noregion=0;
		for (a=0;a<10;a++)
		{
			regionRef.guardnum[a]=RandomNum(1000,1001);
		}
		for (a=0;a<100;a++)		// added by Magius(CHE)
		{
			regionRef.goodsell[a]=0;
			regionRef.goodbuy[a]=0;
			regionRef.goodrnd1[a]=0;
			regionRef.goodrnd2[a]=0;
		}
		a=0;		// end added by Magius(CHE)

		sprintf(sect, "SECTION REGION %i", i);
		safedelete(iter); //as the name implies, this is safe :P, Xan
		iter = Scripts::Regions->getNewIterator(sect);

		if (iter==NULL) {
			noregion=1;
			continue; //-> goes next loop!
		}
		regionRef.inUse = true;
		
		loopexit=0;
		do
		{
			iter->parseLine(script1, script2);
			if ((script1[0]!='}')&&(script1[0]!='{'))
			{
				if (!(strcmp("GUARDNUM",script1)))
				{
					if (a<10)
					{
						regionRef.guardnum[a]=str2num(script2);
						a++;
					}
					else
					{
						WarnOut("region %i has more than 10 'GUARDNUM', The ones after 10 will not be used\n",i);
					}
				}
				else if (!(strcmp("NAME",script1)))
				{
					strcpy(regionRef.name,script2);
					actgood=INVALID; // Magius(CHE)
				}
				// Dupois - Added April 5, 1999
				// To identify whether this region is escortable or not.
				else if (!(strcmp("ESCORTS",script1)))
				{
					// Load the region number in the global array of valid escortable regions
					if ( str2num(script2) == 1 )
					{
						// Store the region index into the valid escort region array
						validEscortRegion[escortRegions] = i;
						escortRegions++;
						//ConOut( "NoX-Wizard: loadregions() %i regions loaded so far\n", escortRegions );
					}
				} // End - Dupois
				else if (!(strcmp("GUARDOWNER",script1)))	strcpy(regionRef.guardowner,script2);
				else if (!(strcmp("MIDILIST",script1)))		regionRef.midilist=str2num(script2);
				else if (!(strcmp("GUARDED",script1)))
				{
					if (str2num(script2)) regionRef.priv|=0x01;
				}
				else if (!(strcmp("MAGICDAMAGE",script1)))
				{
					if ((str2num(script2))) regionRef.priv|=0x40; // bugfix LB 12-march-
					// changes from 0=magicdamge,1=no magic damage
					// to			1=			 0=
				}
				else if (!(strcmp("NOMAGIC",script1)))
				{
					if ((str2num(script2))) regionRef.priv|=0x80;
				}
				else if (!(strcmp("MARK",script1)))
				{
					if (str2num(script2)) regionRef.priv|=0x02;
				}
				else if (!(strcmp("GATE",script1)))
				{
					if (str2num(script2)) regionRef.priv|=0x04;
				}
				else if (!(strcmp("RECALL",script1)))
				{
					if (str2num(script2)) regionRef.priv|=0x08;
				}
				else if (!(strcmp("SNOWCHANCE", script1)))
				{
					regionRef.snowchance=str2num(script2);
				}
				else if (!(strcmp("RAINCHANCE", script1)))
				{
					regionRef.rainchance=str2num(script2);
				}
				//xan : quick&dirty weather system :)
				else if (!(strcmp("DRYCHANCE", script1)))
				{
					regionRef.drychance=str2num(script2);
				}
				else if (!(strcmp("KEEPCHANCE", script1)))
				{
					regionRef.keepchance=str2num(script2);
				}
				else if (!(strcmp("FORCESEASON", script1)))
				{
					regionRef.forcedseason =str2num(script2);
				}
				else if (!(strcmp("IGNOREMONTHMULTIPLIERS", script1)))
				{
					regionRef.ignoreseason =true;
				}
				else if (!(strcmp("GOOD", script1))) // Magius(CHE)
				{
					actgood=str2num(script2);
				}
				else if (!(strcmp("BUYABLE", script1))) // Magius(CHE)
				{
					if (actgood>INVALID) regionRef.goodbuy[actgood]=str2num(script2);
					else ErrOut("error in regions.xss. You must write BUYABLE after GOOD <num>!\n");
				}
				else if (!(strcmp("SELLABLE", script1))) // Magius(CHE)
				{
					if (actgood>INVALID) regionRef.goodsell[actgood]=str2num(script2);
					else ErrOut("error in regions.xss. You must write SELLABLE after GOOD <num>!\n");
				}
				else if (!(strcmp("RANDOMVALUE", script1))) // Magius(CHE) (2)
				{
					if (actgood>INVALID) {
						gettokennum(script2, 0);
						regionRef.goodrnd1[actgood]=str2num(gettokenstr);
						gettokennum(script2, 1);
						regionRef.goodrnd2[actgood]=str2num(gettokenstr);
						if (regionRef.goodrnd2[actgood]<regionRef.goodrnd1[actgood])
						{
							ErrOut("error in regions.xss. You must write RANDOMVALUE NUM2[%i] grater than NUM1[%i].\n",regionRef.goodrnd2[actgood],regionRef.goodrnd1[actgood]);
							regionRef.goodrnd2[actgood]=regionRef.goodrnd1[actgood]=0;
						}
					}
					else ErrOut("error in regions.xss. You must write RANDOMVALUE after GOOD <num>!\n");
				}
				else if (!(strcmp("X1", script1)))
				{
					location[l].x1=str2num(script2);
				}
				else if (!(strcmp("X2", script1)))
				{
					location[l].x2=str2num(script2);
				}
				else if (!(strcmp("Y1", script1)))
				{
					location[l].y1=str2num(script2);
				}
				else if (!(strcmp("Y2", script1)))
				{
					location[l].y2=str2num(script2);
					location[l].region=i;
					l++;
				}
			}
		}
		while (script1[0]!='}' && !noregion && (++loopexit < MAXLOOPS) );
	}


	locationcount=l;
	logoutcount=0;
	//Instalog

	strcpy(sect, "SECTION INSTALOG");

	safedelete(iter); //as the name implies, this is safe :P, Xan
	iter = Scripts::Regions->getNewIterator(sect);

	if (iter==NULL) return;

	loopexit=0;
	do
	{
		iter->parseLine(script1, script2);
		if(!(strcmp(script1,"X1"))) logout[logoutcount].x1=str2num(script2);
		if(!(strcmp(script1,"Y1"))) logout[logoutcount].y1=str2num(script2);
		if(!(strcmp(script1,"X2"))) logout[logoutcount].x2=str2num(script2);
		if(!(strcmp(script1,"Y2")))
		{
			logout[logoutcount].y2=str2num(script2);
			logoutcount++;
		}
	} while ( (script1[0]!='}') && (++loopexit < MAXLOOPS) );

	safedelete(iter);


}

//<Anthalir>
short calcRegionFromXY(sLocation pos)
{
	return calcRegionFromXY(pos.x, pos.y);
}
//</Anthalir>

short calcRegionFromXY(int x, int y)
{
	int i;
	for (i=0;i<locationcount;i++)
	{
		if (location[i].x1<=x && location[i].y1<=y && location[i].x2>=x &&
			location[i].y2>=y)
		{
			return location[i].region;
		}
	}

	return INVALID;


}

void checkregion(pChar pc)
{
	if ( ! pc ) return;

	pClient client;
	pPC tmp;

	int calcreg, j;

	calcreg=calcRegionFromXY( pc->getPosition() );
	if (calcreg!= pc->region)
	{
		
		pFunctionHandle evt = pc->getEvent(cChar::evtChrOnRegionChange);
		if(evt) {
			tVariantVector params = tVariantVector(3);
			params[0] = pc->getSerial(); params[1] = pc->region; params[2] = calcreg;
			evt->setParams(params);
			evt->execute();
		}

		client = (tmp = dynamic_cast<pPC>(pc))? tmp->getClient() : NULL;
		if (client)
		{
			pweather(client);
			Calendar::commitSeason(pc);
			if (region[ pc->region ].name[0]!=0)
			{
				client->sysmessage("You have left %s.", region[ pc->region].name);
			}
			if (region[calcreg].name[0]!=0)
			{
				client->sysmessage("You have entered %s.", region[calcreg].name);
			}
			j=strcmp(region[calcreg].guardowner, region[pc->region].guardowner);
			if ( (region[calcreg].priv & RGNPRIV_GUARDED)!=(region[pc->region].priv & RGNPRIV_GUARDED) ||
				(region[calcreg].priv & RGNPRIV_GUARDED && j))
			{
				if (region[calcreg].priv & RGNPRIV_GUARDED)
				{
					if (region[calcreg].guardowner[0]==0)
						client->sysmessage("You are now under the protection of the guards.");
					else
						client->sysmessage("You are now under the protection of %s guards.", region[calcreg].guardowner);
				}
				else
				{
					if (region[pc->region].guardowner[0]==0)
						client->sysmessage("You are no longer under the protection of the guards.");
					else
						client->sysmessage("You are no longer under the protection of %s guards.", region[pc->region].guardowner);
				}
			}
		}

		pc->region=calcreg;

		if (client)
			 client->playMidi();
	}
}


/*!
\brief Do periodical checks for weather change
\author Xanatar
*/
void check_region_weatherchange ()
{
	int r,sn=0, rn=0, dr=0, sm, i;

	InfoOut("performing weather change...");

	//! \todo revisit this part
	for (i=0;i<256;i++)
	{
		region_st &regionRef = region[i];
		if ((regionRef.keepchance==0)&&(regionRef.drychance==0)) continue;
		r = rand()%100;
		if ((r<=regionRef.keepchance)||(regionRef.keepchance==100)) continue;
		//we're here, let's change the weeeeather
		dr = (regionRef.wtype==0) ? 0 : regionRef.drychance;
		rn = (regionRef.wtype==1) ? 0 : regionRef.rainchance;
		sn = (regionRef.wtype==2) ? 0 : regionRef.snowchance;
		if (!regionRef.ignoreseason)
		{
			dr = static_cast<int>(static_cast<float>(dr) * Calendar::g_fCurDryMod);
			rn = static_cast<int>(static_cast<float>(rn) * Calendar::g_fCurRainMod);
			sn = static_cast<int>(static_cast<float>(sn) * Calendar::g_fCurSnowMod);
		}
		sm = dr+rn+sn;
		r = rand()%sm;
		if (r < dr) regionRef.wtype = 0;
		else if (r < (rn+dr)) regionRef.wtype = 1;
		else regionRef.wtype = 2;
	}


	// Chronodt 17/8/04 - begun additional weathercode
	//! \todo sobstitute temporary region subnames with the true ones when regions redone

	WeatherType current = region[cregion].weatherCurrent;
	sint16_t oldintensity = region[cregion].weatherIntensity;
	sint16_t newintensity = oldintensity;

	//Getting the 4 adiacent regions. getXXXXregion should return invalid if such region does not exist
	uint16_t nregion = getNorthregion(cregion);
	uint16_t eregion = getEastregion(cregion);
	uint16_t sregion = getSouthregion(cregion);
	uint16_t wregion = getWestregion(cregion);

	// modifying the intensity by surrounding squares (obiously the old status)
	newintensity += getIntensityModifier(cregion, nregion);
	newintensity += getIntensityModifier(cregion, eregion);
	newintensity += getIntensityModifier(cregion, sregion);
	newintensity += getIntensityModifier(cregion, wregion);

	// adding intensity by random raincheck (based on region configuration)
	//! \todo modify these with new region parameters
	uint8_t dry  = region[cregion].drychance;
	uint8_t rain = region[cregion].rainchance;
	uint8_t snow = region[cregion].snowchance;
	uint8_t r = rand()%100;
	if (r < dry) newintensity -= (dry - r) * 5;
	r = rand()%100;
	if (r < rain) newintensity += (rain - r) * 5;

	if (newintensity > 140) newintensity = 140;
	if (newintensity < 0) newintensity = 0;

	bool snow = false;
	std::string message = "";

	//! \todo insert a maxlight to limit light in bad beather (but nightvision should counter this) 

	switch (current)	//select message type to send clients based on previous weather in region. Also checking if weather change is too sudden, and if so lessen the impact :D
	{
	case wtSun:		// intensity 0 - 20
		if (newintensity >= 35)
		{
			newintensity = 69;	//If it was so clear, we need at least a few clouds in the sky before it can rain (or snow)
			message = "You see some clouds closing in";
		}
		break;
	case wtCloud:		// intensity 21 - 40
		if (newintensity >= 105)		// to have more than a light rain we need heavier clouds first
		{
			newintensity = 69;
			message = "Cloud covering thickens visibly. You hear rumbling in the distance";
		}
		else if (newintensity < 35) message = "The sky clears off and the sun shines again";
		else if (newintensity >= 70)
		{	//snow check
			r = rand()%100;
			if (r < snow) snow = true
		}
		break;
	case wtStormCloud:	// intensity 41 - 70
		//from this weather type, we can reach every other, since from here it can start raining with any intensity. Or it can snow but even clear up
		if (newintensity < 70 && newintensity > 35) message = "The sky begins to clear off";
		else if (newintensity < 35) message = "Strong winds clear away all the clouds, until the sun shines again";
		else if (newintensity > 70)
		{	//snow check
			r = rand()%100;
			if (r < snow) snow = true
		}
	case wtLightRain:	// intensity 71 - 90
	case wtMediumRain:	// intensity 91 - 110
	case wtHeavyRain:	// intensity 111 - 140
	case wtLightSnow:	// intensity 71 - 90
		snow = true;
	case wtMediumSnow:	// intensity 91 - 110
		snow = true;
	case wtHeavySnow:	// intensity 111 - 140
		snow = true;
	}


	//! \todo weather control in nox wasn't bad, not much to modify, but still we must update it to hypnos
	//! \todo actually the calendar system for weather variation during the year wasn't a bad idea, but it should NOT involve 3 float multiplications + 3 float to int and 3 int to float conversions for each region every weathercheck
	if (region[wregion].wtype==0) packet[2] = 0x00;
	if (region[wregion].wtype==1) packet[1] = 0x00;
	if (region[wregion].wtype==2) { packet[1] = 0x02; packet[3] = 0xEC; }

	Xsend(s, packet, 4);
//AoS/	Network->FlushBuffer(s);
#endif


	//here : we should commit weather changes to players
   wtype=0;

	for (i=0;i<now;i++) { if (clientInfo[i]->ingame) { weather(i); } }
	for (i=0;i<now;i++) { if (clientInfo[i]->ingame) { pweather(i); } }

   ConOut("[ OK ]\n");

}


sint16_t getIntensityModifier(uint16_t region1, uint16_t region2)
{
	if (region1 == INVALID || region2 == INVALID) return 0;
	Climates climate1 = region[region1].climate;
	Climates climate2 = region[region2].climate;
	if (climate1 == clNone || climate2 == clNone) return 0;	//dungeons don't influence nearby areas' weather

	uint8_t weather1 = region[region1].weatherCurrent;
	uint8_t weather2 = region[region2].weatherCurrent;

	sint16_t delta = (region[region1].weatherIntensity - region[region1].weatherIntensity) /10;
	if (climate1 == climate2) return delta; 	//if the 2 regions have the same climate, they just "try" to level weather intensity

	//if flow continues here, the 2 regions have different climates
	switch(climate1)
	{
	case clNormal:
		if (climate2 == clArtic) return delta + 2;	// humidity usually is higher in the hotter region
		if (climate2 == clTropical) return delta - 3;	// humid winds to slightly colder climates :D
		if (climate2 == clDry) return delta + 5;	// deserts usually have few precipitations :D
		break;
	case clArtic:
		if (climate2 == clNormal) return delta - 2;	// humidity usually is higher in the hotter region
		if (climate2 == clTropical) return delta - 6;	// vastly different climates. very cold wind vs very hot and humid ones.... hurricanes anyone? :P
		if (climate2 == clDry) return delta + 1;	// deserts and artic areas are both almost equally dry
		break;
	case clTropical:
		if (climate2 == clNormal) return delta + 3;	// mostof the precipitations go to the tropical area
		if (climate2 == clArtic) return delta + 10;	// vastly different climates. very cold wind vs very hot and humid ones.... hurricanes anyone? :P
		if (climate2 == clDry) return delta + 8;	// deserts and tropical areas usually are nice neighbor... simply all the rain goes away from the desert :D
		break;
	case clDry:
		if (climate2 == clNormal) return delta - 5;	// deserts usually have few precipitations. they drain humidity from surrounding areas :D
		if (climate2 == clArtic) return delta - 1;	// deserts and artic areas are both almost equally dry
		if (climate2 == clTropical) return delta - 8;	// deserts and tropical areas usually are nice neighbor... simply all the rain goes away from the desert :D
		break;
	}
	//should never arrive here
	return 0;
}


