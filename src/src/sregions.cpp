/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "common_libs.h"
#include "networking/network.h"
#include "sndpkg.h"
#include "sregions.h"
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
						LogWarning("region %i has more than 10 'GUARDNUM', The ones after 10 will not be used\n",i);
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
					else LogError("error in regions.xss. You must write BUYABLE after GOOD <num>!\n");
				}
				else if (!(strcmp("SELLABLE", script1))) // Magius(CHE)
				{
					if (actgood>INVALID) regionRef.goodsell[actgood]=str2num(script2);
					else LogError("error in regions.xss. You must write SELLABLE after GOOD <num>!\n");
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
							LogError("error in regions.xss. You must write RANDOMVALUE NUM2[%i] grater than NUM1[%i].\n",regionRef.goodrnd2[actgood],regionRef.goodrnd1[actgood]);
							regionRef.goodrnd2[actgood]=regionRef.goodrnd1[actgood]=0;
						}
					}
					else LogError("error in regions.xss. You must write RANDOMVALUE after GOOD <num>!\n");
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
			cVariantVector params = cVariantVector(3);
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
			if ( (region[calcreg].priv & rgnFlagGuarded)!=(region[pc->region].priv & rgnFlagGuarded) ||
				(region[calcreg].priv & rgnFlagGuarded && j))
			{
				if (region[calcreg].priv & rgnFlagGuarded)
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

	outInfo("performing weather change...");

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
			//! \todo actually the calendar system for weather variation during the year wasn't a bad idea, but it should NOT involve 3 float multiplications + 3 float to int and 3 int to float conversions for each region every weathercheck
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
		//! \todo a for cycle with all the regions (cregion is the region currently esamined)

	//for (whatever will cycle all region, with cregion as index or iterator)
	{
		Climates climate = region[cregion].climate;
		if (climate == clNone) continue;	//no weather change for a dungeon
		WeatherType current = region[cregion].weatherCurrent;
		sint16_t oldintensity = region[cregion].weatherIntensity;
		sint16_t newintensity = oldintensity;

		//Getting the 4 adiacent regions. getXXXXregion should return invalid if such region does not exist
		uint16_t nregion = getNorthRegion(cregion);
		uint16_t eregion = getEastRegion(cregion);
		uint16_t sregion = getSouthRegion(cregion);
		uint16_t wregion = getWestRegion(cregion);

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
			if (newintensity > 40)
			{
				newintensity = 40;	//If it was so clear, we need at least a few clouds in the sky before it can rain (or snow)
				message = "You see some clouds closing in";
			}
			break;
		case wtCloud:		// intensity 21 - 40
			if (newintensity > 90)		// to have more than a light rain we need heavier clouds first
			{
				newintensity = 70;
				message = "Cloud covering thickens visibly. You hear rumbling in the distance";
			}
			else if (newintensity <= 20) message = "The sky clears off and the sun shines again";
			else if (newintensity > 70)
			{	//snow check
				r = rand()%100;
				if (r < snow) snow = true;
			}
			break;
		case wtStormCloud:	// intensity 41 - 70
			//from this weather type, we can reach every other, since from here it can start raining with any intensity. Or it can snow or even clear up
			if (newintensity <= 40 && newintensity > 20) message = "The sky begins to clear off";
			else if (newintensity <= 20) message = "Strong winds clear away the thick clouds, until the sun shines again";
			else if (newintensity > 70)
			{	//snow check
				r = rand()%100;
				if (r < snow) snow = true;
			}
			break;
		case wtLightRain:	// intensity 71 - 90
			if (newintensity < 71)
			{
				if (newintensity > 20) message = "The rain is stopping";
				else message = "The rain is stopping and the sky clears up";
			}
			else message = "The rain gets stronger";
			r = rand()%100;
			if (climate == clArtic && r < (snow/4)) snow = true;	//If artic climate and raining, check if rain becomes snow (at 1/4 of normal snow chance)
			break;
		case wtMediumRain:	// intensity 91 - 110
			if (newintensity <71)
			{
				if (newintensity > 40) message = "The rain is stopping";
				else
				{
					newintensity = 41;
					message = "The rain is stopping and the sky begins to clear up";
				}
			}
			else message = "The rain gets stronger";
			r = rand()%100;
			if (climate == clArtic && r < (snow/4)) snow = true;	//If artic climate and raining, check if rain becomes snow (at 1/4 of normal snow chance)
			break;
		case wtHeavyRain:	// intensity 111 - 140
			if (newintensity <111)
			{
				if (newintensity > 70) message = "The rain begins to slow down";
				else
				{
					newintensity = 71;
					message = "The rain has stopped falling, but the sky is still covered up";
				}
			}
			else message = "The rain gets stronger";
			r = rand()%100;
			if (climate == clArtic && r < (snow/4)) snow = true;	//If artic climate and raining, check if rain becomes snow (at 1/4 of normal snow chance)
			break;
		case wtLightSnow:	// intensity 71 - 90
			if (newintensity < 71)
			{
				if (newintensity > 20) message = "The snow is stopping";
				else message = "The snow is stopping and the sky clears up";
			}
			else message = "The snow gets stronger";
			r = rand()%100;
			if (climate != clNormal || r > ((100 - snow)/4)) snow = true;	//If normal climate and snowing, check if snow becomes rain (at 1/4 of normal snow_to_rain chance, the complementary of snow chance)
			break;
		case wtMediumSnow:	// intensity 91 - 110
			if (newintensity <71)
			{
				if (newintensity > 40) message = "The snow is stopping";
				else
				{
					newintensity = 41;
					message = "The snow is stopping and the sky begins to clear up";
				}
			}
			else message = "The snow gets stronger";
			r = rand()%100;
			if (climate != clNormal || r > ((100 - snow)/4)) snow = true;	//If normal climate and snowing, check if snow becomes rain (at 1/4 of normal snow_to_rain chance, the complementary of snow chance)
			break;
		case wtHeavySnow:	// intensity 111 - 140
		snow = true;
		if (newintensity <111)
		{
		if (newintensity > 70) message = "The snow begins to slow down";
		else
			{
				newintensity = 71;
				message = "The snow has stopped falling, but the sky is still covered up";
			}
		}
		else message = "The snow gets stronger";
			r = rand()%100;
			if (climate != clNormal || r > ((100 - snow)/4)) snow = true;	//If normal climate and snowing, check if snow becomes rain (at 1/4 of normal snow_to_rain chance, the complementary of snow chance)
			break;
		}


		if (newintensity <= 20) region[cregion].weatherNew = wtSun;
		else if (newintensity <= 40) region[cregion].weatherNew = wtCloud;
		else if (newintensity <= 70) region[cregion].weatherNew = wtStormCloud;
		else if (newintensity <= 90 && !snow) region[cregion].weatherNew = wtLightRain;
		else if (newintensity <= 110 && !snow) region[cregion].weatherNew = wtMediumRain;
		else if (!snow) region[cregion].weatherNew = wtHeavyRain;
		else if (newintensity <= 90 && snow) region[cregion].weatherNew = wtLightSnow;
		else if (newintensity <= 110 && snow) region[cregion].weatherNew = wtMediumSnow;
		else region[cregion].weatherNew = wtHeavySnow;

		region[cregion].weatherIntensityNew = newintensity;

		uint8_t weather = 0;
		is (snow) weather = 0x02;		//snow effect
		if (newintensity <=70) weather = 0xff;	//No weather

		uint8_t intensity = (newintensity > 70) ? newintensity - 70 : newintensity;
		uint8_t intensity2 = intensity;
		bool mixedweather = false;

		if ( ((current == wtLightRain || current == wtMediumRain || current == wtHeavyRain) && snow) ||	   	// Rain has just become snow or...
		     ((current == wtLightSnow || current == wtMediumSnow || current == wtHeavySnow) && !snow))		// Snow has just become rain
		{
			intensity = rand() % intensity; 	// randomize % of snow ...
			intensity2 -= intensity;		// ...and the remaining intensity goes to rain
			weather = 0x2;  	// snow
			weather2 = 0x0; 	// rain
			mixedweather = true;
		}
		//! \todo the limit of light level by weather (it is best to modify existing code in timers.cpp)

		nPackets::Sent::Weather pk( weather, intensity);
		nPackets::Sent::Weather pk2( weather2, intensity2);


		NxwSocketWrapper sw;
		sw.fillOnline( pos );		//!< \todo change with something like fillOnlineInRegion
		for( sw.rewind(); !sw.isEmpty(); sw++ )
		{
			pClient j =sw.getSocket();
			if( j )
			{
				j->sendPacket(&pk);
				if (mixedweather) j->sendPacket(&pk2);
				j->sysmessage(message);
			}
		}

	}
	// End region for cycle. Now we must build another one to update the weatherIntensity and weatherCurrent of all regions
	// if you ask why, think of what would happen if you changed them as you process them, since each region takes a little info
	// from each adiacent region.... :)  (yes, this is a system with a memory :D)

	//! \todo an online iteration of all online chars in region
	//for (whatever will cycle all regions)
	{
		region[cregion].weatherCurrent = region[cregion].weatherNew;
		region[cregion].weatherIntensity = region[cregion].weatherIntensityNew;
	}

	outPlain("[ OK ]\n");

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
