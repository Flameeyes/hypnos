/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "nxwcommn.h"
#include "sndpkg.h"
#include "amx/amxcback.h"
#include "magic.h"
#include "set.h"
#include "itemid.h"
#include "race.h"
#include "debug.h"
#include "pointer.h"
#include "range.h"
#include "data.h"
#include "boats.h"
#include "archive.h"
#include "items.h"
#include "chars.h"
#include "inlines.h"
#include "basics.h"
#include "utils.h"

/*!
\brief Add item by ID
\author Anthalir - Rewrote by Flameeyes
\param id id of the item to add
\param nAmount amount of items to add
\param cName name of the item to add
\param color color of the item
\param where Location to add the item to
*/
static pItem cItem::addByID(SI32 id, UI16 nAmount, const char *cName, UI16 color, Location where)
{
	pItem pi = item::spawnItemByIdInternal(nAmount, cName, id, color);
	if ( where.x != 0xFFFF )
	{
		z = getHeight(where);
		pi->moveTo(where);
		pi->Refresh();
	}
	return pi;
}

/*!
\brief Tells if an id is a house
\param id id to check
*/
static const bool cItem::isHouse(UI16 id)
{
	if (id < 0x0040) return false;

	if ( (id >= 0x0064) && (id <= 0x007f) ) return true;

	if( id==0x0bb8 || id==0x1388 )
		return true;

	switch(id&0xFF)
	{
		case 0x87:
		case 0x8c:
		case 0x8d:
		case 0x96:
		case 0x98:
		case 0x9a:
		case 0x9c:
		case 0x9e:
		case 0xa0:
		case 0xa2:
			return true;
	}

	return false;
}

/*!
\todo rewrite it, this will not work!
*/
static void cItem::loadWeaponsInfo()
{
	cScpIterator* iter = NULL;
	char script1[1024];
	char script2[1024];
	UI16 id=0xFFFF;
	UI16 type=weaponSword1H;

	int loopexit=0;
	do
	{
		safedelete(iter);
		iter = Scripts::WeaponInfo->getNewIterator("SECTION WEAPONTYPE %i", type );
		if( iter==NULL ) continue;

		do
		{
			iter->parseLine(script1, script2);
			if ((script1[0]!='}')&&(script1[0]!='{'))
			{
				if (!strcmp("ID", script1)) {
					id = str2num(script2);
					weaponinfo[id]=type;
				}
			}

		}
		while ( (script1[0]!='}') && (++loopexit < MAXLOOPS) );

		type++;
	}while ( (strcmp("EOF", script1)) && (++loopexit < MAXLOOPS) );

	safedelete(iter);
}

/*!
\brief Say if an ID is a weapon of the specified type
\param id id of the weapon
\param type mask of weapon types to tests
*/
static const bool cItem::isWeaponLike( UI16 id, UI16 type )
{
	WeaponMap::iterator iter( weaponinfo.find( id ) );
	if( iter==weaponinfo.end() )
		return false;
	else
		return ( iter->second & type );
}

/*!
\brief Constructor for new item
*/

cItem::cItem()
{
	cItem(nextSerial());
}


/*!
\brief Constructor with serial known
\todo a check to ensure nextSerial() will still be valid even after adding this one 
*/
cItem::cItem( SERIAL ser )
{

	setSerial32( ser );
	setOwnerSerial32Only(INVALID);
	setMultiSerial32Only(INVALID);//Multi serial

	setCurrentName("#");
	setSecondaryName("#");
	setScriptID( 0 );
	murderer = string("");
	creator = string("");
	incognito=false;//AntiChrist - incognito
	madewith=0; // Added by Magius(CHE)
	rank=0; // Magius(CHE)
	good=-1; // Magius(CHE)
	rndvaluerate=0; // Magius(CHE) (2)
	setId( 0x0001 ); // Item visuals as stored in the client
	setPosition(100, 100, 0);
	setOldPosition( getPosition() );
	setColor( 0x0000 ); // Hue
	cont = NULL;
	oldcont = NULL;
	layer=oldlayer=0; // Layer if equipped on paperdoll
	scriptlayer=0;	//Luxor
	itmhand=0; // Layer if equipped on paperdoll
	type=0; // For things that do special things on doubleclicking
	type2=0;
	offspell=0;
	weight=0;
	more1=0; // For various stuff
	more2=0;
	more3=0;
	more4=0;
	moreb1=0;
	moreb2=0;
	moreb3=0;
	moreb4=0;
	morex=0;
	morey=0;
	morez=0;
	amount=1; // Amount of items in pile
	amount2=0; //Used to track things like number of yards left in a roll of cloth
	doordir=0; // Reserved for doors
	dooropen=0;
	pileable=0; // Can item be piled
	dye=0; // Reserved: Can item be dyed by dye kit
	corpse=0; // Is item a corpse
	carve=-1;//AntiChrist-for new carving system
	att=0; // Item attack
	def=0; // Item defense
	fightskill=INVALID_SKILL; //Luxor: skill used by item
	reqskill[0]=0; //Luxor: skill value required by item (skillnum = fightskill)
	reqskill[1]=0;
	damagetype=DAMAGE_PURE; //Luxor: damage types system
	auxdamagetype=DAMAGE_PURE; //Luxor: damage types system
	auxdamage=0;
	lodamage=0; //Minimum Damage weapon inflicts
	hidamage=0; //Maximum damage weapon inflicts
	smelt=0; // for smelting items
	secureIt=0; // secured chests
	wpsk=0; //The skill needed to use the item
	hp=0; //Number of hit points an item has.
	maxhp=0; // Max number of hit points an item can have.
	st=0; // The strength needed to equip the item
	st2=0; // The strength the item gives
	dx=0; // The dexterity needed to equip the item
	dx2=0; // The dexterity the item gives
	in=0; // The intelligence needed to equip the item
	in2=0; // The intelligence the item gives
	spd=0; //The speed of the weapon
	wipe=0; //Should this item be wiped with the /wipe command
	magic=0; // 0=Default as stored in client, 1=Always movable, 2=Never movable, 3=Owner movable.
	gatetime=0;
	gatenumber=INVALID;

	visible=0; // 0=Normally Visible, 1=Owner & GM Visible, 2=GM Visible
	spawnserial=INVALID;
	spawnregion=INVALID;
	dir=0; // Direction, or light source type.
	priv=0; // Bit 0, decay off/on.  Bit 1, newbie item off/on.  Bit 2 Dispellable
	decaytime = 0;
	value=0; // Price shopkeeper sells item at.
	restock=0; // Number up to which shopkeeper should restock this item
	trigger=0; //Trigger number that item activates
	trigtype=0; //Type of trigger
	tuses=0;    //Number of uses for trigger
	poisoned=POISON_NONE; //AntiChrist -- for poisoning skill
	murdertime=0; //AntiChrist -- for corpse -- when the people has been killed
//	glow=0;
//	glow_effect=0;
//	glow_c1=0;
//	glow_c2=0;
	time_unused=0;
	timeused_last=getclock();
	animSetId(0x0000); // elcabesa animation
	ammo=0;
	ammoFx=0;

	/* Luxor: damage resistances */
	int i;
	for (i=0;i<MAX_RESISTANCE_INDEX;i++)
		resists[i]=0;

	for (int X=0; X<ALLITEMEVENTS; X++)
		amxevents[X] = NULL;
	//desc[0]=0x00;
	vendorDescription = std::string("");
	setDecayTime(); //Luxor

}

/*!
\author Luxor
\brief operator = for the cItem class, let's dupe :)
\todo dupe books fix
*/
cItem& cItem::operator=(cItem& b)
{
        // NAMES
        setCurrentName(b.getCurrentName());
        setRealName(b.getRealName());

        setScriptID(b.getScriptID());
        creator = b.creator;
        incognito = b.incognito;
        madewith = b.madewith;
        rank = b.rank;
        good = b.good;
        rndvaluerate = b.rndvaluerate;
        //setMultiSerial32(b.getMultiSerial32());
        setId( b.getId() );
        //setPosition(b.getPosition());
        //setOldPosition(b.getOldPosition());
        setColor( b.getColor() );
        layer = b.layer;
        oldlayer = b.oldlayer;
        scriptlayer = b.scriptlayer;
        itmhand = b.itmhand;
        type = b.type;
        type2 = b.type2;
        offspell = b.offspell;
        weight = b.weight;
        more1 = b.more1;
        more2 = b.more2;
        more3 = b.more3;
        more4 = b.more4;
        moreb1 = b.moreb1;
        moreb2 = b.moreb2;
        moreb3 = b.moreb3;
        moreb4 = b.moreb4;
        morex = b.morex;
        morey = b.morey;
        morez = b.morez;
        amount = b.amount;
        amount2 = b.amount2;
        doordir = b.doordir;
        dooropen = b.dooropen;
        dye = b.dye;
        corpse = b.corpse;
        carve = b.carve;
        att = b.att;
        def = b.def;
        fightskill = b.fightskill;
        reqskill[0] = b.reqskill[0];
        reqskill[1] = b.reqskill[1];
        damagetype = b.damagetype;
        auxdamagetype = b.auxdamagetype;
        auxdamage = b.auxdamage;
        lodamage = b.lodamage;
	hidamage = b.hidamage;
        smelt = b.smelt;
        secureIt = b.secureIt;
        wpsk = b.wpsk;
        hp = b.hp;
        maxhp = b.maxhp;
        st = b.st;
        st2 = b.st2;
        dx = b.dx;
        dx2 = b.dx2;
        in = b.in;
        in2 = b.in2;
        spd = b.spd;
        wipe = b.wipe;
        magic = b.magic;
        gatetime = b.gatetime;
        gatenumber = b.gatenumber;
        decaytime = b.decaytime;
        //setOwnerSerial32(b.getOwnerSerial32());
        visible = b.visible;
        spawnserial = INVALID;
        spawnregion = INVALID;
        dir = b.dir;
        priv = b.priv;
        value = b.value;
        restock = b.restock;
        trigger = b.trigger;
        trigtype = b.trigtype;
        tuses = b.tuses;
        poisoned = b.poisoned;
        murderer = b.murderer;
        murdertime = b.murdertime;
        time_unused = b.time_unused;
        timeused_last = b.timeused_last;
        animid1 = b.animid1;
        animid2 = b.animid2;
	ammo = b.ammo;
	ammoFx = b.ammoFx;

        UI32 i;
        for ( i = 0; i < MAX_RESISTANCE_INDEX; i++ )
                resists[i] = b.resists[i];

	for ( i=0; i < ALLITEMEVENTS; i++ ) {
		amxevents[i] = b.amxevents[i];
		/*
		AmxEvent* event = b.getAmxEvent( i );
		if ( event == NULL )
			continue;

		setAmxEvent( i, event->getFuncName(), !(event->shouldBeSaved()) );*/
	}
	vendorDescription = b.vendorDescription;
	amxVS.copyVariable(getSerial32(), b.getSerial32());

        return *this;
}


//
// Class methods
//
void cItem::archive()
{
	std::string saveFileName( SrvParms->savePath + SrvParms->itemWorldfile + SrvParms->worldfileExtension );
	std::string timeNow( getNoXDate() );
	for( int i = timeNow.length() - 1; i >= 0; --i )
		switch( timeNow[i] )
		{
			case '/' :
			case ' ' :
			case ':' :
				timeNow[i]= '-';
		}
	std::string archiveFileName( SrvParms->archivePath + SrvParms->itemWorldfile + timeNow + SrvParms->worldfileExtension );


	if( rename( saveFileName.c_str(), archiveFileName.c_str() ) != 0 )
	{
		LogWarning("Could not rename/move file '%s' to '%s'\n", saveFileName.c_str(), archiveFileName.c_str() );
	}
	else
	{
		InfoOut("Renamed/moved file '%s' to '%s'\n", saveFileName.c_str(), archiveFileName.c_str() );
	}
}

void cItem::safeoldsave()
{
	std::string oldFileName( SrvParms->savePath + SrvParms->itemWorldfile + SrvParms->worldfileExtension );
	std::string newFileName( SrvParms->savePath + SrvParms->itemWorldfile + SrvParms->worldfileExtension + "$" );
	remove( newFileName.c_str() );
	rename( oldFileName.c_str(), newFileName.c_str() );
}

//
// Object methods
cItem::~cItem()
{

}

void cItem::getPopupHelp(char *str)
{
	if (isInstrument())
		sprintf(str, TRANSLATE("This item is a musical instrument. You can use it for bardic skills like enticement or provocation"));
	else if (type == ITYPE_DOOR)
		sprintf(str, TRANSLATE("This a door. To open or close it, double click on it."));
	else if (type == ITYPE_LOCKED_DOOR)
		sprintf(str, TRANSLATE("This a locked door. To open or close it, click on the proper key and target it."));
	else if (type == ITYPE_FOOD)
		sprintf(str, TRANSLATE("This food you can eat when you're hungry. To eat, double click the food, but beware poisoned food!"));
	else if (type == ITYPE_RUNE)
		sprintf(str, TRANSLATE("This is a rune for use with recall, mark and gate travel spells"));
	else if (type == ITYPE_RESURRECT)
		sprintf(str, TRANSLATE("If you dye (or are dead) you can double click this item to resurrect!"));
	else if (type == ITYPE_KEY)
		sprintf(str, TRANSLATE("This is a key you can use (double click) to open doors"));
	else if (type == ITYPE_SPELLBOOK)
		sprintf(str, TRANSLATE("This is the spellbook, where you can write down your own spells for later use"));
	else if (type == ITYPE_POTION)
		sprintf(str, TRANSLATE("This is a potion! You can drink that when you need its effects... but beware of poison potions!"));
}

/*!
\author Flameeyes
\brief Set the item's container
\param obj New (actual) container
\note This function insert the item in the container's list or in the layers
	of the wearing body.
*/
void cItem::setContainer(pObject obj)
{
	if ( obj && obj->rtti() != rtti::cContainer && obj->rtti() != rtti::cBody && obj->rtti() != rtti::cMsgBoard)
		return;

	oldcont = cont;
	cont = obj;

     	if ( cont && cont->rtti() == rtti::cMsgBoard ) return; //Only container addition is needed for msgboards

	if ( ! obj )
		setDecayTime();

	if ( cont && cont->rtti() == rtti::cContainer )
		(reinterpret_cast<pContainer>cont)->insertItem(this);
	else if (cont && cont->rtti() == rtti::cBody )
		(reinterpret_cast<pBody>cont)->setLayerItem(layer, this);
}

/*!
\author Luxor
\brief execute decay on the item
\return true if decayed (so deleted), false else
*/
bool cItem::doDecay()
{
	if ( !canDecay() )
		return false;

	if ( magic == 4/* || magic == 2*/ )
		return false;

	if ( !isInWorld() )
		return false;

	if ( TIMEOUT( decaytime ) )
	{


		if ( amxevents[EVENT_IONDECAY] !=NULL )
		{
			g_bByPass = false;
			amxevents[EVENT_IONDECAY]->Call(getSerial32(), DELTYPE_DECAY);
			if ( g_bByPass == true )
				return false;
		}
		/*
		g_bByPass = false;
		runAmxEvent( EVENT_IONDECAY, getSerial32(), DELTYPE_DECAY );
		if ( g_bByPass == true )
			return false;
		*/

		//Multis
		if ( !isFieldSpellItem() && !corpse )
		{
			if ( getMultiSerial32() == INVALID )
			{
				P_ITEM pi_multi = findmulti(getPosition());
				if ( ISVALIDPI(pi_multi) )
				{
					if ( pi_multi->more4 == 0 )
					{
						setDecayTime();
						SetMultiSerial(pi_multi->getSerial32());
						return false;
					}
				}
			}
			else
			{
				setDecayTime();
				return false;
			}
		}
		//End Multis

		if( type == ITYPE_CONTAINER || ( !SrvParms->lootdecayswithcorpse && corpse ) )
		{
			NxwItemWrapper si;
			si.fillItemsInContainer( this, false );
			for( si.rewind(); !si.isEmpty(); si++ )
			{
				P_ITEM pj = si.getItem();
				if( ISVALIDPI(pj) )
				{
					pj->setContainer(0);
					pj->MoveTo( getPosition() );
					pj->setDecayTime();
					pj->Refresh();
				}
			}
		}
		Delete();
		return true;
	}
	else
		return false;
}


void cItem::explode(NXWSOCKET  s)
{
	if (s < 0 || s > now) return;	//Luxor

	unsigned int dmg=0,len=0;

	P_CHAR pc_current=MAKE_CHAR_REF(currchar[s]);
	VALIDATEPC(pc_current);

	if(!isInWorld())
		return;

	type=0; //needed for recursive explosion

	//Luxor - recursive explosions!! :DD
	NxwItemWrapper si;
	si.fillItemsNearXYZ( getPosition(), 5, true );
    for( si.rewind(); !si.isEmpty(); si++ ) {
		P_ITEM p_nearbie=si.getItem();
		if(ISVALIDPI(p_nearbie) && p_nearbie->type == ITYPE_POTION && p_nearbie->morey == 3) { //It's an explosion potion!
			p_nearbie->explode(s);
    	}
    }
	//End Luxor recursive explosions

	staticeffect2(this, 0x36, 0xB0, 0x10, 0x80, 0x00);
	soundeffect3(this, 0x0207);

	len=morex/250; //4 square max damage at 100 alchemy
	switch (morez)
	{
		case 1:dmg=RandomNum( 5,10) ;break;
		case 2:dmg=RandomNum(10,20) ;break;
		case 3:dmg=RandomNum(20,40) ;break;
		default:
			ErrOut("Switch fallout. NoX-Wizard.cpp, explodeitem()\n"); //Morrolan
			dmg=RandomNum(5,10);
	}

	if (dmg<5) dmg=RandomNum(5,10);	// 5 points minimum damage
	if (len<2) len=2;	// 2 square min damage range

	NxwCharWrapper sc;
	sc.fillCharsNearXYZ( getPosition(), len, true );
	for( sc.rewind(); !sc.isEmpty(); sc++ ) {

		P_CHAR pc=sc.getChar();
		if( ISVALIDPC(pc) ) {
			pc->damage( dmg+(2-pc->distFrom(this)), DAMAGE_FIRE );
		}
	}

	Delete();

}

/*
\author Duke
\brief reduce the amount of piled items
\param amt amount to subtract to item amount

Reduces the given item's amount by 'amt' and deletes it if necessary and returns 0.
If the request could not be fully satisfied, the remainder is returned
*/
SI32 cItem::ReduceAmount(const SI16 amt)
{
	UI32 rest=0;
	if( amount > amt )
	{
		amount-=amt;
		Refresh();
	}
	else
	{
		rest=amt-amount;
		Delete();
	}
	return rest;

}
/*!
\brief increase the amount of piled items
\param amt amount to add to item amount
*/
SI32 cItem::IncreaseAmount(const SI16 amt)
{
	amount+= amt;
	Refresh();
	return amount;
}

const magic::FieldType cItem::isFieldSpellItem() const
{
	if( (getId()==0x3996) || (getId()==0x398C) ) return magic::fieldFire;
	if( (getId()==0x3915) || (getId()==0x3920) ) return magic::fieldPoison;
	if( (getId()==0x3979) || (getId()==0x3967) ) return magic::fieldParalyse;
	if( (getId()==0x3956) || (getId()==0x3946) ) return magic::fieldEnergy;

	return fieldInvalid;
}

void cItem::SetMultiSerial(SI32 mulser)
{
	if (getMultiSerial32()!=INVALID)	// if it was set, remove the old one
		pointers::delFromMultiMap(this);

	setMultiSerial32Only(mulser);

	if (getMultiSerial32()!=INVALID)		// if there is multi, add it
		pointers::addToMultiMap(this);
}

/*!
\author Anthalir
*/
void cItem::MoveTo(Location newloc)
{
#ifdef SPAR_I_LOCATION_MAP
	setPosition( newloc );
	pointers::updateLocationMap(this);
#else
	mapRegions->remove(this);
	setPosition( newloc );
	mapRegions->add(this);
#endif
}

/*
\brief Check if two item are similar so pileable
\author Endymion
\todo add amx vars and events
\note if same item is compared, false is returned
*/
inline bool operator ==( cItem& a, cItem& b ) {
	return  a.isPileable() && b.isPileable()  &&
		( a.getSerial32() != b.getSerial32() ) &&
		( a.getScriptID() == b.getScriptID() ) &&
		( a.getId() == b.getId() ) &&
		( a.getColor() == b.getColor() ) &&
		( a.poisoned == b.poisoned );
}

/*
\brief Check if two item are not similar so not pileable
\author Endymion
*/
inline bool operator !=( cItem& a, cItem& b ) {
	return !(a==b);
}

/*!
\author Flameeyes
\brief Handle the right event for this item
\param code Code of the event
\param nParams number of params for the event handler
\param params params for the event handler
*/
int cItem::handleEvent(UI08 code, UI08 nParams, UI32 *params)
{
	return PythonInterface::handleEvent(
		events[code], etItem, code,
		nParams, params
		);
}

/*!
\author Xanathar
\brief gets the real name of an item (removing #'s)
\return the real name of an item
\note See also cItem::getName
*/
const std::string cItem::getRealItemName()
{
	if ( current_name == "#" )
	{
		tile_st tile;
		data::seekTile(getId(), tile);
		return std::string(tile.name);
	} else
		return current_name;
}

/*!
\brief Gets the actual name of the item
\return The true name of the item, also if the current_name is #
*/
const std::string cItem::getName()
{
	tile_st tile;
	int len, mode, used, ok, namLen;
	std::string name;

	if ( current_name != "#" )
		return current_name;

	data::seekTile(getId(), tile);
	if (tile.flags&TILEFLAG_PREFIX_AN)
		name = "an ";
	else if (tile.flags&TILEFLAG_PREFIX_A)
		name = "a ";

	mode=0;
	used=0;
	len=strlen((char *) tile.name);
	for(register int j = 0; j < len; j++)
	{
		ok=0;
		if ((tile.name[j]=='%')&&(mode==0)) mode=2;
		else if ((tile.name[j]=='%')&&(mode!=0)) mode=0;
		else if ((tile.name[j]=='/')&&(mode==2)) mode=1;
		else if (mode==0) ok=1;
		else if ((mode==1)&&(pi->amount==1)) ok=1;
		else if ((mode==2)&&(pi->amount>1)) ok=1;
		if (ok)
		{
			name += tile.name[j];
			if (mode) used=1;
		}
	}

	return name;
}

/*!
\brief the weight of the single item
\return the weigth
\note May have to seek it from mul files
*/
R32 cItem::getWeight()
{

	if (getId() == ITEMID_GOLD)
		return (R32)SrvParms->goldweight;

	R32 itemweight=0.0;

	if (weight>0) //weight is defined in scripts for this item
		itemweight=(R32)weight;
	else
	{
		tile_st tile;
		data::seekTile(getId(), tile);
		if (tile.weight==0) // can't find weight
		{
			if(type != ITYPE_FOOD)
				itemweight = 2.0;	// not food weighs .02 stone

			else
				itemweight = 100.0;	//food weighs 1 stone
		}
		else //found the weight from the tile, set it for next time
		{
			weight=(tile.weight*100); // set weight so next time don't have to search
			itemweight = (R32)(weight);
		}

	}
	return itemweight;
}

/*!
\brief Gets the actual weight of the item, counting all the amount of it
\note This is a virtual method, when called for cContainer items, all the items inside it
      will be also added to the weight.
*/
const R32 cItem::getWeightActual()
{
	return (amount>1)? getWeight()*amount : getWeight();
}

bool LoadItemEventsFromScript (P_ITEM pi, char *script1, char *script2)
{

#define CASEITEMEVENT( NAME, ID ) 	else if (!(strcmp(NAME,script1))) pi->amxevents[ID] = newAmxEvent(script2);

	if (!strcmp("@ONSTART",script1))	{
		pi->amxevents[EVENT_IONSTART] = newAmxEvent(script2);
		newAmxEvent(script2)->Call(pi->getSerial32(), -1);
	}
	CASEITEMEVENT("@ONDAMAGE", EVENT_IONDAMAGE)
	CASEITEMEVENT("@ONEQUIP", EVENT_IONEQUIP)
	CASEITEMEVENT("@ONUNEQUIP", EVENT_IONUNEQUIP)
	CASEITEMEVENT("@ONCLICK", EVENT_IONCLICK)
	CASEITEMEVENT("@ONDBLCLICK", EVENT_IONDBLCLICK)
	CASEITEMEVENT("@ONCHECKCANUSE", EVENT_IONCHECKCANUSE)
	CASEITEMEVENT("@ONPUTINBACKPACK", EVENT_IPUTINBACKPACK)
	CASEITEMEVENT("@ONDROPINLAND", EVENT_IDROPINLAND)
	CASEITEMEVENT("@ONTRANSFER", EVENT_IONTRANSFER)
	CASEITEMEVENT("@ONSTOLEN", EVENT_IONSTOLEN)
	CASEITEMEVENT("@ONPOISONED", EVENT_IONPOISONED)
	CASEITEMEVENT("@ONDECAY", EVENT_IONDECAY)
	CASEITEMEVENT("@ONREMOVETRAP", EVENT_IONREMOVETRAP)
	CASEITEMEVENT("@ONLOCKPICK", EVENT_IONLOCKPICK)
	CASEITEMEVENT("@ONWALKOVER", EVENT_IONWALKOVER)
	CASEITEMEVENT("@ONPUTITEM", EVENT_IONPUTITEM)
	CASEITEMEVENT("@ONTAKEFROMCONTAINER", EVENT_ITAKEFROMCONTAINER)
	else if (!(strcmp("@ONCREATION",script1))) newAmxEvent(script2)->Call(pi->getSerial32(),-1);
	else return false;
	return true;
}

/*!
\author Luxor
\brief gets the combat skill of an item
\return the combat skill used by the object
*/
Skill cItem::getCombatSkill()
{
	if (fightskill != INVALID_SKILL) return fightskill;
	else if (IsSwordType())		return SWORDSMANSHIP;
	else if (IsMaceType() || IsSpecialMace())		return MACEFIGHTING;
	else if (IsFencingType())	return FENCING;
	else if (IsBowType())		return ARCHERY;
	return WRESTLING;
}

/*!
\author Luxor
\brief deletes the item
*/
void cItem::Delete()
{
	if (spawnregion!=INVALID )
		Spawns->removeObject( spawnregion, this );

	if( isSpawner() || spawnserial!=INVALID )
		Spawns->removeSpawnDinamic( this );

	cPacketSendDeleteObj pk(serial);

	NxwSocketWrapper sw;
	sw.fillOnline( this );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		NXWSOCKET j=sw.getSocket();
		if (j!=INVALID)
			client->sendPacket(&pk);
	}

	// - remove from pointer arrays
	pointers::delItem(this);	//Luxor

	if (type==ITYPE_BOOK && (morex==666 || morey==999) && morez)
		// make sure that the book is really there
		if ( Books::books.find(morez) != Books::books.end() )
			Books::books.erase( Books::books.find(morez) );
        // if a new book gets deleted also delete the corresponding map element

	safedelete(this);
}

/*!
\author Luxor and AntiChrist
\brief Refreshes the item
*/
void cItem::Refresh()
{

	if( cont == this )
	{
		ErrOut("item %s [serial: %i] has dangerous container value, autocorrecting...\n", getCurrentNameC(), getSerial32());
		setContainer(NULL);
	}

	NxwSocketWrapper sw;
	sw.fillOnline();
	for ( sw.rewind(); !sw.isEmpty(); sw++ ) {
		NXWCLIENT ps_w = sw.getClient();
		if ( ps_w != NULL )
			ps_w->sendRemoveObject(static_cast<P_OBJECT>(this));
	}

	//first check: let's check if it's on the ground....
	if(isInWorld())
	{

		NxwSocketWrapper sw;
		sw.fillOnline( this );
		for( sw.rewind(); !sw.isEmpty(); sw++ )
		{
			NXWSOCKET a=sw.getSocket();
			if(a!=INVALID)
				senditem(a, this);
		}
		return;
	}

	//if not, let's check if it's on a char or in a pack

	if( isCharSerial(cont->getSerial()) )//container is a player...it means it's equipped on a character!
	{
		// elcabesa this is like a wearit() function, we can use here
		pChar charcont= (pChar)cont;

		NxwSocketWrapper sw;
		sw.fillOnline( charcont, false );
		for( sw.rewind(); !sw.isEmpty(); sw++ )
		{
			NXWSOCKET a=sw.getSocket();
			if(a!=INVALID)
				wearIt(a, this);
		}
		return;
	}
	else//container is an item...it means we have to use sendbpitem()!!
	{
		NxwSocketWrapper sw;
		sw.fillOnline();
		for( sw.rewind(); !sw.isEmpty(); sw++ )
		{
			NXWSOCKET a=sw.getSocket();
			if(a!=INVALID)
				sendbpitem(a, this);	//NOTE: there's already the inrange check
							//in the sendbpitem() function, so it's unuseful
							//to do a double check!!
		}
	}
}

/*!
\brief Get the out most container
\author Endymion
\return outer container
\param rec not need to use, only internal for have a max number or recursion
\note max recursion = 50
*/
pItem cItem::getOutMostCont( SI16 rec )
{
	if ( rec<0	// too many recursions
		|| (isInWorld()) // in the world
		|| (cont->rtti() == rtti::cBody ) )//weared
		return this;

	return cont->getOutMostCont( --rec );
}

/*!
\brief Get the owner of this pack ( automatic recurse for out most pack )
\author Flameeyes
\return pointer to pack owner
*/
pBody cItem::getPackOwner()
{
	pItem omcont = getOutMostCont();

	if(omcont->isInWorld())
		return NULL;
	else
		return cont->getContainer();
}

/*!
\brief Get item's distance from the given char
\author Flameeyes
\return distance ( if error is returned VERY_VERY_FAR )
\param pc the char
*/
UI32 cItem::distFrom( pChar pc )
{
	if ( ! pc )
		return VERY_VERY_FAR;

	return pc->distFrom(this);
}

/*!
\brief Get item's distance from the given item
\author Endymion - rewrite by Flameeyes
\return distance ( if invalid is returned VERY_VERY_FAR )
\param pi the item
\note it check also if is subcontainer, or weared. so np call freely
*/
UI32 cItem::distFrom( pItem pi )
{
	if ( ! pi )
		return VERY_VERY_FAR;

	pItem omc = pi->getOutMostCont();
	if ( ! omc )
		return VERY_VERY_FAR;

	pItem mycont = getOutMostCont();
	if ( ! mycont )
		return VERY_VERY_FAR;

	if(omc->isInWorld() ) {
		if( myomc->isInWorld() )
			return (UI32)dist(myomc->getPosition(),omc->getPosition());
		else { //this is weared
			if(isCharSerial(omc->getContainer()->getSerial()))
			{ //can be weared
				pChar pc = omc->getContainer();
				if ( ! pc )
					return VERY_VERY_FAR;

				return (UI32)dist(pc->getPosition(),omc->getPosition());
			}
			else return VERY_VERY_FAR; //not world, not weared.. and another omc can't be
		}
	}
	else { //omc is weared
		if(isCharSerial(omc->getContainer()->getSerial()))
			pChar pc_i = omc->getContainer();
			if ( ! pc_i )
				return VERY_VERY_FAR;
			if( myomc->isInWorld() )
				return (UI32)dist(pc_i->getPosition(),myomc->getPosition());
			else
				return pc_i->distFrom(myomc);
		}
		else return VERY_VERY_FAR;

	}
}

/*!
\brief Calculate the value of the item
\param bvalue base value
*/
const SI32 cItem::calcValue(SI32 bvalue)
{
	int mod=10;

	if (type==19)
	{
		if (morex>500) mod=mod+1;
		if (morex>900) mod=mod+1;
		if (morex>1000) mod=mod+1;
		if (morez>1) mod=mod+(3*(pi->morez -1));
		bvalue=(bvalue*mod)/10;
	}

	// Lines added for Rank System by Magius(CHE)
	if (rank>0 && rank<10 && SrvParms->rank_system==1)
	{
		bvalue=(int) (pi->rank*bvalue)/10;
	}
	if (bvalue<1) bvalue=1;
	// end addon

	// Lines added for Trade System by Magius(CHE) (2)
	if (rndvaluerate<0) rndvaluerate=0;
	if (rndvaluerate!=0 && SrvParms->trade_system==1) {
		bvalue+=(int) (bvalue*rndvaluerate)/1000;
	}
	if (bvalue<1) bvalue=1;
	// end addon

	return bvalue;
}
