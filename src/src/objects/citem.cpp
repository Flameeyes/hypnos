/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "common_libs.h"
#include "sndpkg.h"
#include "magic.h"
#include "races/race.h"
#include "pointer.h"
#include "data.h"
#include "boats.h"
#include "inlines.h"
#include "basics.h"
#include "utils.h"

#include <wefts_mutex.h>

uint32_t cItem::nextSerial = 0x40000000;

/*!
\brief Gets the next serial
\see cSerializable
*/
uint32_t cItem::getNewSerial()
{
	// This is here to avoid serials collisions
	static Wefts::Mutex m;
	
	m.lock();
	bool firstpass = true;
	uint32_t fserial = nextSerial;
	uint32_t nserial;
	do {
		nserial = nextSerial++;
		if ( nserial >= 0x80000000 )
			nserial = 0x40000000;
		if ( ! firstpass && nserial == fserial )
		{
			LogCritical("Too much items created!!!! No more serials free");
		}
		firstpass = false;
	} while ( cSerializable::findBySerial(nserial) );

	m.unlock();
	return nserial;
}

/*!
\brief Add item by ID
\author Anthalir - Rewrote by Flameeyes
\param id id of the item to add
\param nAmount amount of items to add
\param cName name of the item to add
\param color color of the item
\param where sLocation to add the item to
*/
static pItem cItem::addByID(int32_t id, uint16_t nAmount, const char *cName, uint16_t color, sLocation where)
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
static const bool cItem::isHouse(uint16_t id)
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
\brief Constructor for new item
*/
cItem::cItem()
	: cSerializable(getNewSerial())
{
	resetData();
}

/*!
\brief Constructor with serial known
*/
cItem::cItem(uint32_t ser)
	: cSerializable(ser)
{
	resetData();
}

cItem::resetData()
{
	flags = 0ull;
	// Hypnos OK

	setOwnerSerial32Only(INVALID);
	setMultiSerial32Only(INVALID);//Multi serial
	
	setCurrentName("#");
	setSecondaryName("#");
	murderer = string("");
	creator = string("");
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
	more = 0; // For various stuff
	moreb = 0;
	morex=0;
	morey=0;
	morez=0;
	amount=1; // Amount of items in pile
	amount2=0; //Used to track things like number of yards left in a roll of cloth
	doordir=0; // Reserved for doors
	dooropen=0;
	pileable=0; // Can item be piled
	corpse=0; // Is item a corpse
	carve=-1;//AntiChrist-for new carving system
	att=0; // Item attack
	def=0; // Item defense
	fightskill=INVALID_SKILL; //Luxor: skill used by item
	reqskill[0]=0; //Luxor: skill value required by item (skillnum = fightskill)
	reqskill[1]=0;
	damagetype=damPure; //Luxor: damage types system
	auxdamagetype=damPure; //Luxor: damage types system
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
        minUsingStrength = 0;  // Minimum strength to use item
        minUsingIntelligence = 0; // Minimum inteligence to use item
        minUsingDexterity = 0; // Minimum dexterity to use item
        minUsingSkill[3]= { UINVALID16, UINVALID16, UINVALID16 }; // holds up to 3 skills to be checked for usability. if INVALID no skill check is done
        minUsingSkillvalue[3] = {0,0,0};
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
	poisoned=poisonNone; //AntiChrist -- for poisoning skill
	murdertime=0; //AntiChrist -- for corpse -- when the people has been killed
	time_unused=0;
	timeused_last=getclock();
	animSetId(0x0000); // elcabesa animation
	ammo=0;
	ammoFx=0;

	/* Luxor: damage resistances */
	int i;
	for (i=0;i<MAX_RESISTANCE_INDEX;i++)
		resists[i]=0;

	//desc[0]=0x00;
	vendorDescription = std::string("");
	setDecayTime(); //Luxor
}

/*!
\author Luxor
\brief operator = for the cItem class, let's dupe :)
\todo dupe books fix
*/
cItem& cItem::operator=(const cItem& b)
{
	flags = b.flags;
	cEventThrower::operator=(b);
	// Hypnos OK!
        
	// NAMES
        setCurrentName(b.getCurrentName());
        setRealName(b.getRealName());
	
        creator = b.creator;
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
        more = b.more;
        moreb  = b.moreb;
        morex = b.morex;
        morey = b.morey;
        morez = b.morez;
        amount = b.amount;
        amount2 = b.amount2;
        doordir = b.doordir;
        dooropen = b.dooropen;
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

        uint32_t i;
        for ( i = 0; i < MAX_RESISTANCE_INDEX; i++ )
                resists[i] = b.resists[i];

	vendorDescription = b.vendorDescription;

        return *this;
}

//
// Object methods
cItem::~cItem()
{

}

const std::string cItem::getPopupHelp() const
{
	if (isInstrument())
		return std::string("This item is a musical instrument. You can use it for bardic skills like enticement or provocation");
	else if (type == ITYPE_DOOR)
		return std::string("This a door. To open or close it, double click on it.");
	else if (type == ITYPE_LOCKED_DOOR)
		return std::string("This a locked door. To open or close it, click on the proper key and target it.");
	else if (type == ITYPE_FOOD)
		return std::string("This food you can eat when you're hungry. To eat, double click the food, but beware poisoned food!");
	else if (type == ITYPE_RUNE)
		return std::string("This is a rune for use with recall, mark and gate travel spells");
	else if (type == ITYPE_RESURRECT)
		return std::string("If you die (or are dead) you can double click this item to be resurrected!");
	else if (type == ITYPE_KEY)
		return std::string("This is a key you can use (double click) to open doors");
	else if (type == ITYPE_SPELLBOOK)
		return std::string("This is the spellbook, where you can write down your own spells for later use");
	else if (type == ITYPE_POTION)
		return std::string("This is a potion! You can drink that when you need its effects... but beware of poison potions!");
}

/*!
\author Flameeyes
\brief Set the item's container
\param obj New (actual) container
\note This function insert the item in the container's list
*/
void cItem::setContainer(pObject obj)
{
	oldcont = cont;
	cont = obj;
	
	if ( ! obj )
		setDecayTime();
	
	pContainer itemcont = NULL;
	if ( (itemcont = dynamic_cast<pContainer>(cont)) )
		itemcont->insertItem(this);
}

/*!
\author Luxor & Flameeyes
\brief execute decay on the item
\param dontDelete Should be called by inherited classes to not delete the item.
\return true if decayed (so deleted), false else
*/
bool cItem::doDecay(bool dontDelete = false)
{
	if ( !canDecay() )
		return false;

	if ( magic == 4/* || magic == 2*/ )
		return false;

	if ( !isInWorld() )
		return false;

	if ( ! TIMEOUT( decaytime ) )
		return false;


	if ( events[evtItmOnDecay] ) {
		tVariantVector params = tVariantVector(2);
		params[0] = getSerial(); params[1] = deleteDecay;
		events[evtItmOnDecay]->setParams(params);
		events[evtItmOnDecay]->execute();
		if ( events[evtItmOnDecay]->isBypassed() )
			return;
	}

	//Multis
	if ( !isFieldSpellItem() && !corpse )
	{
		if ( getMultiSerial32() == INVALID )
		{
			pItem pi_multi = findmulti(getPosition());
			if ( pi_multi )
			{
				if ( pi_multi->more4 == 0 )
				{
					setDecayTime();
					SetMultiSerial(pi_multi->getSerial());
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

	if ( ! dontDelete )
		Delete();
	return true;
}

void cItem::explode(pClient client)
{
	if (s < 0 || s > now) return;	//Luxor

	unsigned int dmg=0,len=0;

	pChar pc_current=cSerializable::findCharBySerial(currchar[s]);
	if ( ! pc_current ) return;

	if(!isInWorld())
		return;

	type=0; //needed for recursive explosion

	//Luxor - recursive explosions!! :DD
	NxwItemWrapper si;
	si.fillItemsNearXYZ( getPosition(), 5, true );
	for( si.rewind(); !si.isEmpty(); si++ )
	{
		pItem p_nearbie=si.getItem();
		if( p_nearbie && p_nearbie->type == ITYPE_POTION && p_nearbie->morey == 3)
		{ //It's an explosion potion!
			p_nearbie->explode(s);
		}
	}
	//End Luxor recursive explosions

	staticeffect2(this, 0x36, 0xB0, 0x10, 0x80, 0x00);
	playSFX(0x0207);

	len=morex/250; //4 square max damage at 100 alchemy
	switch (morez)
	{
		case 1:dmg=RandomNum( 5,10) ;break;
		case 2:dmg=RandomNum(10,20) ;break;
		case 3:dmg=RandomNum(20,40) ;break;
		default:
			ErrOut("Switch fallout. citem.cpp, explode()\n"); //Morrolan
			dmg=RandomNum(5,10);
	}

	if (dmg<5) dmg=RandomNum(5,10);	// 5 points minimum damage
	if (len<2) len=2;	// 2 square min damage range

	NxwCharWrapper sc;
	sc.fillCharsNearXYZ( getPosition(), len, true );
	for( sc.rewind(); !sc.isEmpty(); sc++ ) {

		pChar pc=sc.getChar();
		if( pc ) {
			pc->damage( dmg+(2-pc->distFrom(this)), damFire );
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
int32_t cItem::ReduceAmount(const int16_t amt)
{
	uint32_t rest=0;
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
int32_t cItem::IncreaseAmount(const int16_t amt)
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

void cItem::SetMultiSerial(int32_t mulser)
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
void cItem::MoveTo(sLocation newloc)
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

/*!
\brief Gets the location of a serial instance in the world
\author Flameeyes
\note This function returns the location of the player with the object or with
the outmost container for it.
*/
sLocation cItem::getWorldLocation() const
{
	pObject inworld = NULL;
	pItem outmost = getOutMostCont();
	if ( ! outmost )
		return sLocation(0,0,0);
	
	inworld = outmost->isInWorld() ? outmost : outmost->getContainer();
	return inworld->getPosition();
}

/*!
\brief Check if two item are similar so pileable
\author Endymion
\note if same item is compared, false is returned
*/
inline bool operator ==( cItem& a, cItem& b ) {
	return  a.isPileable() && b.isPileable()  &&
		( a.getSerial() != b.getSerial() ) &&
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
float cItem::getWeight()
{

	if (getId() == ITEMID_GOLD)
		return (float)SrvParms->goldweight;

	float itemweight=0.0;

	if (weight>0) //weight is defined in scripts for this item
		itemweight=(float)weight;
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
			itemweight = (float)(weight);
		}

	}
	return itemweight;
}

/*!
\brief Gets the actual weight of the item, counting all the amount of it
\note This is a virtual method, when called for cContainer items, all the items inside it
      will be also added to the weight.
*/
const float cItem::getWeightActual()
{
	return (amount>1)? getWeight()*amount : getWeight();
}

bool LoadItemEventsFromScript (pItem pi, char *script1, char *script2)
{

#if 0
#define CASEITEMEVENT( NAME, ID ) 	else if (!(strcmp(NAME,script1))) pi->amxevents[ID] = newAmxEvent(script2);

	if (!strcmp("@ONSTART",script1))	{
		pi->amxevents[EVENT_IONSTART] = newAmxEvent(script2);
		newAmxEvent(script2)->Call(pi->getSerial(), -1);
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
	else if (!(strcmp("@ONCREATION",script1))) newAmxEvent(script2)->Call(pi->getSerial(),-1);
	else return false;
	return true;
#endif
}

/*!
\author Luxor
\brief gets the combat skill of an item
\return the combat skill used by the object
*/
Skill cItem::getCombatSkill()
{
	if (fightskill != INVALID_SKILL) return fightskill;
	else if (IsSwordType())		return skSwordsmanship;
	else if (IsMaceType() || IsSpecialMace())		return skMacefighting;
	else if (IsFencingType())	return FENCING;
	else if (IsBowType())		return skArchery;
	return skWrestling;
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

	nPackets::Sent::DeleteObj pk(serial);

	NxwSocketWrapper sw;
	sw.fillOnline( this );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		pClient j =sw.getSocket();
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
		ErrOut("item %s [serial: %i] has dangerous container value, autocorrecting...\n", getCurrentName().c_str(), getSerial());
		setContainer(NULL);
	}

	NxwSocketWrapper sw;
	sw.fillOnline();
	for ( sw.rewind(); !sw.isEmpty(); sw++ ) {
		pClient ps_w = sw.getClient();
		if ( ps_w != NULL )
			ps_w->sendRemoveObject(static_cast<pObject>(this));
	}

	//first check: let's check if it's on the ground....
	if(isInWorld())
	{

		NxwSocketWrapper sw;
		sw.fillOnline( this );
		for( sw.rewind(); !sw.isEmpty(); sw++ )
		{
			if ( sw.getClient() )
				sw.getClient()->senditem(this);
		}
		return;
	}

	//if not, let's check if it's on a char or in a pack

	if( cSerializable::isCharSerial(cont->getSerial()) )//container is a player...it means it's equipped on a character!
	{
		// elcabesa this is like a wearit() function, we can use here
		pChar charcont= (pChar)cont;

		NxwSocketWrapper sw;
		sw.fillOnline( charcont, false );
		for( sw.rewind(); !sw.isEmpty(); sw++ )
		{
			if ( sw.getClient() )
				sw.getClient()->wearIt(this);
		}
		return;
	}
	else//container is an item...it means we have to use sendbpitem()!!
	{
		NxwSocketWrapper sw;
		sw.fillOnline();
		for( sw.rewind(); !sw.isEmpty(); sw++ )
		{
			if ( sw.getClient() )
				sw.getClient()->showItemInContainer(this);
				//NOTE: there's already the inrange check
				//in the showItemInContainer() function, so it's unuseful
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
pItem cItem::getOutMostCont( uint16_t rec )
{
	if ( rec<0	// too many recursions
		|| isInWorld() // in the world
		|| cont->toBody() )//weared
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
\brief Get the owner of this item. item may be equipped or in the backpack. If searchBank is true, bank is also searched
\author Flameeyes
\return pointer to pack owner
*/

pChar getCurrentOwner(bool searchBank)
{
	pItem cont = getOutMostCont(); 		// this returns a container equipped on char or a container in the world. If it is equipped, cont == this
        if (cont->isInWorld()) return NULL;     // if it is in the world it has no current owner
	if (cont == this) return (dynamic_cast<pBody> cont->getContainer())->getChar();
        pEquippableContainer econt = dynamic_cast<pEquippableContainer> cont;
       	if (!econt) return NULL;   //I have no idea of what "this" is if at this point econt is NULL :)
        if (!searchBank && econt->getLayer() == layBank) return NULL;
	return (dynamic_cast<pBody> cont->getContainer())->getChar();
}

/*!
\brief Get item's distance from the given char
\author Flameeyes
\return distance ( if error is returned VERY_VERY_FAR )
\param pc the char
*/
uint32_t cItem::distFrom( pChar pc )
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
uint32_t cItem::distFrom( pItem pi )
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
			return (uint32_t)dist(myomc->getPosition(),omc->getPosition());
		else { //this is weared
			if(cSerializable::isCharSerial(omc->getContainer()->getSerial()))
			{ //can be weared
				pChar pc = omc->getContainer();
				if ( ! pc )
					return VERY_VERY_FAR;

				return (uint32_t)dist(pc->getPosition(),omc->getPosition());
			}
			else return VERY_VERY_FAR; //not world, not weared.. and another omc can't be
		}
	}
	else { //omc is weared
		if(cSerializable::isCharSerial(omc->getContainer()->getSerial()))
			pChar pc_i = omc->getContainer();
			if ( ! pc_i )
				return VERY_VERY_FAR;
			if( myomc->isInWorld() )
				return (uint32_t)dist(pc_i->getPosition(),myomc->getPosition());
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
const int32_t cItem::calcValue(int32_t bvalue)
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

void cItem::setDirection(uint8_t newdir)
{
	newdir %= 8;
	dir = newdir;
}

/*!
\brief Plays the given sound to the near clients
\param sound Sound to play
\todo Port to the new proximity system
*/
void cItem::playSFX(uint16_t sound)
{
	nPackets::Sent::SoundFX pk(sound, getPosition(), false);

	NxwSocketWrapper sw;
	sw.fillOnline( pi );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		pClient ps_i=sw.getClient();
		if( ! ps_i ) continue;
		
		ps_i->sendPacket(&pk);
	}
}

/*!
\brief Plays the given sound to the given client
\param client Client to send the sound to
\param sound Sound to play
*/
void cItem::playSFX(pClient client, uint16_t sound)
{
	nPackets::Sent::SoundFX pk(sound, getPosition(), false);
	client->sendPacket(&pk);
}
