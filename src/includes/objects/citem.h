/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __OBJECTS_CITEM_H__
#define __OBJECTS_CITEM_H__

#include "common_libs.h"
#include "objects/cobject.h"
#include "objects/cserializable.h"
#include "magic.h"
#include "settings.h"

//! Item class
class cItem : public cObject, public cSerializable
{
protected:
	static uint32_t nextSerial;
	uint32_t getNewSerial();

	virtual uint16_t eventsNumber() const
	{ return evtItmMax; }
//@{
/*!
\name Constructors and Operators
*/
public:
	//! Redefinition of = operator for cItem class
        cItem& operator=(const cItem& b);

	cItem(uint32_t serial);
        cItem();
	~cItem();
//@}
/*	//! Events for securable items
	enum {
		evtSecOnLockPick = evtCntMax,
		evtSecMax
	};*/

	//! Events for items
	enum {
		evtOnStart = evtGenericMax,
		evtItmOnDamage,
		evtItmOnClick,
		evtItmOnDblClick,
		evtItmOnPutInBackpack,
		evtItmOnDropInLand,
		evtItmOnCheckCanUse,
		evtItmOnTransfer,
		evtItmOnStolen,
		evtItmOnPoisoned,
		evtItmOnDecay,
		evtItmOnRemoveTrap,
		evtItmOnLockPick,
		evtItmOnWalkOver,
		evtItmOnPutInContainer,
		evtItmOnTakeFromContainer,
		evtItmMax
	};

//@{
/*!
\name Main properties
*/
protected:
	int32_t			hp;	//!< Number of hit points an item has.
	int32_t			maxhp;	//!< Max number of hit points an item can have.
public:
	void			Refresh();
	const std::string getName();
	const std::string getRealItemName();
	virtual const std::string getPopupHelp() const;

	inline const int32_t getHP() const
	{ return hp; }

	inline void setHP(int32_t newhp)
	{ hp = newhp; }

	inline const int32_t getMaxHP() const
	{ return maxhp; }

	inline void setMaxHP(int32_t newhp)
	{ maxhp = newhp; }
//@}

//@{
/*!
\name Look and Sound
*/
protected:
	uint16_t animid;	//!< animation id

	int8_t magic;		//!< 0=Default as stored in client, 1=Always movable, 2=Never movable, 3=Owner movable.
				//!< \todo Change to enum
	int8_t visible;		//!< 0=Normally Visible, 1=Owner & GM Visible, 2=GM Visible
				//!< \todo Change to enum
	uint8_t dir;		//!< Item's direction (?)

public:
	void playSFX(uint16_t sound);

	//! Gets the direction of the item (?)
	const uint8_t getDirection() const
	{ return dir; }
	
	void setDirection(uint8_t newdir);

	void talk(const std::string &msg);

	void dyeItem(pClient client, uint16_t color); //!< Rehue an item
//@{
/*!
\name Item Identifiers
\brief Static (and non static) functions to identify items. Most inlines
\todo Change them to use between() function instead of the unreadable current
	checking code
*/

	inline static const bool isCorpse(uint16_t id)
	{ return id==0x2006; }

	inline static const bool isTree(uint16_t id)
	{
		return (id==0x0CD0 || id==0x0CD3 || id==0x0CD6 ||
			id==0x0CD8 || id==0x0CDA || id==0x0CDD ||
			id==0x0CE0 || id==0x0CE3 || id==0x0CE6 ||
			(id>=0x0CCA && id<=0x0CCD) ||
			(id>=0x12B8 && id<=0x12BB) ||
			id==0x0D42 || id==0x0D43 || id==0x0D58 ||
			id==0x0D59 || id==0x0D70 || id==0x0D85 ||
			id==0x0D94 || id==0x0D95 || id==0x0D98 ||
			id==0x0Da4 || id==0x0Da8 || id==0x0D58);
	}

	//! this is used in SwordTarget() to give kindling.
	inline static const bool isTree2(uint16_t id)
	{
		return (id==0x0CD0 || id==0x0CD3 || id==0x0CD6 ||
			id==0x0CD8 || id==0x0CDA || id==0x0CDD ||
			id==0x0CE0 || id==0x0CE3 || id==0x0CE6 ||
			(id>=0x0CCA && id<=0x0CCD) ||
			(id>=0x12B8 && id<=0x12BB) );
	}

	inline static const bool isLog(uint16_t id)
	{ return ( id>=0x1BDD && id<=0x1BE2 ); }

	inline static const bool isShaft(uint16_t id)
	{ return ( id>=0x1BD4 && id<=0x1BD6 ); }

	inline static const bool isFeather(uint16_t id)
	{ return ( id>=0x1BD1 && id<=0x1BD3 ); }

	static const bool isHouse(uint16_t id);

// Non-static ItemID functions

//	const magic::FieldType isFieldSpellItem() const;

	inline const bool isCorpse() const
	{ return isCorpse( getId() ); }

	//! predefined spells
	inline const bool isSpellScroll() const
	{ return (getId()>0x1F2C && getId()<0x1F6D); }

	//! includes the scrolls with a variable name
	inline const bool isSpellScroll72() const
	{ return (getId() >= 0x1F2D && getId() <= 0x1F72); }

	//! this is used in AxeTarget()
	inline const bool isTree() const
	{ return isTree( getId() ); }

	//! this is used in SwordTarget() to give kindling.
	inline const bool isTree2() const
	{ return isTree2( getId() ); }

	inline const bool isInstrument() const
	{
		return (getId()==0x0E9C || getId()==0x0E9D ||
			getId()==0x0E9E || getId()==0x0EB1 ||
			getId()==0x0EB2 || getId()==0x0EB3 ||
			getId()==0x0EB4);
	}

	inline const bool isArrow() const
	{ return (getId()==0x0F3F||getId()==0x0F42); }

	inline const bool isBolt() const
	{ return (getId()==0x1BFB||getId()==0x1BFE); }

	inline const bool isChaosOrOrderShield() const
	{ return (getId()>=0x1BC3 && getId()<=0x1BC5); }

	inline const bool isShield() const
	{
		return ((getId()>=0x1B72 && getId()<=0x1B7B) ||
			isChaosOrOrderShield());
	}

	inline const bool isLog() const
	{ return isLog(getId()); }

	inline const bool isShaft() const
	{ return isShaft(getId()); }

	inline const bool isBoard() const
	{ return between(getId(), (uint16_t)0x1BD7, (uint16_t)0x1BDC); }

	inline const bool isFeather() const
	{ return isFeather(getId()); }

	inline const bool isCutLeather() const
	{
		return (getId()==0x1067 || getId()==0x1068 ||
			getId()==0x1081 || getId()==0x1082 );
	}

	inline const bool isHide() const
	{ return ( getId()==0x1078 || getId()==0x1079 ); }

	inline const bool isBoltOfCloth() const
	{ return between(getId(), (uint16_t)0x0F95, (uint16_t)0x0F9C); }

	inline const bool isCutCloth() const
	{ return ( getId()>=0x1766 && getId()<=0x1768 ); }

	inline const bool isCloth() const
	{
		return ((getId()>=0x175D && getId()<=0x1765)
			|| isCutCloth() );
	}

	inline const bool isChest() const
	{
		return (( getId()>=0x09A8 && getId()<=0x09AB) ||
			( getId()>=0x0E40 && getId()<=0x0E43) ||
			( getId()==0x0E7C) || (getId()==0x0E7D) ||
			( getId()==0x0E80));
	}

	inline const bool isForge() const
	{
		return  ( getId()==0x0FB1 ||
			( getId()>=0x197A && getId()<=0x19A9 ) );
	}

	inline const bool isAnvil() const
	{ return ( getId()==0x0FAF || getId()==0x0FB0 ); }

	inline const bool isCookingPlace() const
	{
		return ((getId()>=0x0DE3 && getId()<=0x0DE9)||
			(getId()==0x0FAC || getId()==0x0FB1)||
			(getId()>=0x197A && getId()<=0x19B6)||
			(getId()>=0x0461 && getId()<=0x0480)||
			(getId()>=0x0E31 && getId()<=0x0E33)||
			(getId()==0x19BB || getId()==0x1F2B)||
			(getId()>=0x092B && getId()<=0x0934)||
			(getId()>=0x0937 && getId()<=0x0942)||
			(getId()>=0x0945 && getId()<=0x0950)||
			(getId()>=0x0953 && getId()<=0x095E)||
			(getId()>=0x0961 && getId()<=0x096C) );
	}

	inline const bool isDagger() const
	{ return ( getId()==0x0F51 || getId()==0x0F52 ); }

	inline const bool isFish() const
	{ return ( getId()>=0x09CC && getId()<=0x09CF ); }

	inline const bool isFishWater() const
	{
		return ((getId()==0x1798) || (getId()==0x179B) ||
			(getId()==0x179C) || (getId()==0x1799) ||
			(getId()==0x1797) || (getId()==0x179A) ||
			(getId()==0x346E) || (getId()==0x346F) ||
			(getId()>=0x3470) && (getId()<=0x3485) ||
			(getId()>=0x3494) && (getId()<=0x349F) ||
			(getId()>=0x34A0) && (getId()<=0x34AB) ||
			(getId()>=0x34B8) && (getId()<=0x34BB) ||
			(getId()>=0x34BD) && (getId()<=0x34BF) ||
			(getId()>=0x34C0) || (getId()<=0x34C2) ||
			(getId()>=0x34C3) && (getId()<=0x34C5) ||
			(getId()>=0x34C7) && (getId()<=0x34CA) ||
			(getId()>=0x34D1) && (getId()<=0x34D5));
	}

	inline const bool isSign() const
	{
		return ((getId()==0x0B95 || getId()==0x0B96) ||
			(getId()>=0x0BA3 && getId()<=0x0C0E) ||
			(getId()==0x0C43 || getId()==0x0C44));
	}

	inline const bool isBrassSign() const
	{ return ((getId()==0x0BD1 || getId()==0x0BD2)); }

	inline const bool isWoodenSign() const
	{ return ((getId()==0x0BCF || getId()==0x0BD0)); }

	inline const bool isGuildSign() const
	{ return ((getId() >= 0x0BD3 && getId() <= 0x0C0A)); }

	inline const bool isTradeSign() const
	{
		return ((getId()==0x0B95 || getId()==0x0B96) ||
			(getId() >= 0x0BA3 && getId() <= 0x0BCE));
	}

	inline const bool isBankSign() const
	{ return ((getId() >= 0x0C0B && getId() <= 0x0C0C)); }

	inline const bool isTheatreSign() const
	{ return ((getId() >= 0x0C0D && getId() <= 0x0C0E)); }

	inline const bool isHouse() const
	{ return isHouse( getId() ); }

	//! Tells if an item is a dynamic spawner
	inline const bool isSpawner() const
	{ return ( type >= 61 && type <= 65 ) || type == 59 || type == 125; }

//@}

//@{
/*!
\name Flags
*/
protected:
	static const uint64_t flagPileable	= 0x0000000000000001ull; //!< Can the item be piled?
	static const uint64_t flagCanDecay	= 0x0000000000000002ull; //!< Can the item decay?
	static const uint64_t flagNewbie	= 0x0000000000000004ull; //!< Is the item newbie?
	static const uint64_t flagDispellable	= 0x0000000000000008ull; //!< Can the item be dispelled?
	static const uint64_t flagUseAnimID	= 0x0000000000000010ull; //!< The item uses animID
	static const uint64_t flagDyeable	= 0x0000000000000020ull; //!< Can an item be dyed with dyekit?

public:
	inline const bool isPileable() const
	{ return flags & flagPileable; }

	inline void setPileable(bool set = true)
	{ setFlag(flags, flagPileable, set); }

	inline const bool canDecay() const
	{ return flags & flagCanDecay; }

	inline void setDecay(bool set = true)
	{ setFlag(flags, flagCanDecay, set); }

	inline const bool isNewbie() const
	{ return flags & flagNewbie; }

	inline void setNewbie(bool set = true)
	{ setFlag(flags, flagNewbie, set); }

	inline const bool isDispellable() const
	{ return flags & flagDispellable; }

	inline void setDispellable(bool set = true)
	{ setFlag(flags, flagDispellable, set); }
	
	inline const bool useAnimID() const
	{ return flags & flagUseAnimID; }
	
	inline void setUseAnimID(bool set = true)
	{ setFlag(flags, flagUseAnimID, set); }
	
	inline const bool isDyeable() const
	{ return flags & flagDyeable; }
	
	inline void setDyeable(bool set = true)
	{ setFlag(flags, flagDyeable, set); }
//@}

//@{
/*!
\name Containers

An item can either be in world or in a container. For container this time we
don't intend a cContainer instance, because the container of an item can also be
a body, if the item is equipped.

This complicate the container's stuff because we must have functions which works
with pContainer and ones which works with pObject.
*/
protected:
	pObject cont;
	pObject oldcont;

public:
	pItem getOutMostCont( uint16_t rec=50 );
	pBody getPackOwner();
	pChar getCurrentOwner(bool searchBank = true);	//!< returns pChar to item who has item, either equipped or in the pack (and subcontainers too) else NULL

	inline const pObject getContainer() const
	{ return cont; }

	virtual void setContainer(pObject obj);

	inline const pObject getOldContainer() const
	{ return oldcont; }

	inline void setOldContainer(pObject obj)
	{ oldcont = obj; }

	inline const bool isSecureContainer() const
	{ return type==8 || type==13 || type==64; }

	int32_t	 secureIt; // secured chests
	void putInto( pItem pi );
//@}

//@{
/*!
\name More values

These variables are used to store informations in the item without add new
members.
Please note that in Hypnos, moreb stands for 'byte', and not for the second
more variable. The two mores are more1 and more2. There's also morex, morey
and morez which are used to save locations.

\deprecated We shouldn't use this anymore, subclasses must be created when
	more values must be stored.
*/
	union tMore {
		uint32_t more;

		struct {
		uint8_t moreb1, moreb2, moreb3, moreb4;
		};
	};
	
	tMore more1;	//!< First more value
	tMore more2;	//!< Secondo more value

	uint16_t morex;
	uint16_t morey;
	int8_t morez;
//@}

	
//@{
/*!
\name Amount
*/
protected:
	uint16_t amount;	//!< Amount of items in pile
	uint16_t amount2;	//!< Used to track things like number of yards left in a roll of cloth

public:
	int32_t ReduceAmount(const int16_t amount);
	int32_t IncreaseAmount(const int16_t amount);

	//! sets the amount of piled items
	inline void setAmount(const uint16_t amt)
	{ amount = amt; Refresh(); }

	inline const uint16_t getAmount()
	{ return amount; }

	int32_t DeleteAmount(int amount, short id, short color=-1);

	//! \todo maybe add quality check
	inline const bool isCombinableWith(pItem it)
	{ return isPileable() && it->isPileable() &&
		 getId() == it->getId() &&
		 getColor() == it->getColor(); }

//@}

//@{
/*!
\name Weight
*/
protected:
	uint32_t		weight;

public:
	float			getWeight();
	virtual const float	getWeightActual();
//@}

//@{
/*!
\name Position
*/
public:
	virtual void MoveTo(sLocation newloc) = 0;

	inline const bool isInWorld() const
	{ return !cont; }

	inline void MoveTo(int32_t x, int32_t y, int8_t z)
	{ MoveTo( sLocation(x, y, z) ); }
	
	virtual sLocation getWorldLocation() const;
//@}

//@{
/*!
\name Weapon and armour related

\deprecated All this should be moved inside cWeapon or cArmor, or cEquippable,
	if common between them.
*/
	uint32_t	att;		//!< Item attack
	uint32_t	def;		//!< Item defense
	DamageType	damagetype;	//!< for different damage types system
	DamageType	auxdamagetype;	//!< Additional damage :]
	int32_t		auxdamage;	//!< Additional damage :]
	int32_t		lodamage;	//!< Minimum Damage weapon inflicts
	int32_t		hidamage;	//!< Maximum damage weapon inflicts
	int32_t		wpsk;		//!< The skill needed to use the item
	int32_t		spd;		//!< The speed of the weapon
	int32_t		itmhand;	//!< ITEMHAND system - AntiChrist

	int32_t		st2;		//!< The strength the item gives

	int32_t		dx2;		//!< The dexterity the item gives

	int32_t		in2;		//!< The intelligence the item gives
	PoisonType	poisoned;	//!< type of poison that poisoned item
	uint32_t	ammo;		//!< Ammo used (firing weapon)
	uint32_t	ammoFx;		//!< Flying ammo animation (firing weapon)
//@}

//@{
/*!
\name Magic Related
*/
	uint32_t	gatetime;
	int32_t		gatenumber;
	int8_t		offspell;
	
	virtual bool getMarkedLocation(sLocation &loc);
	virtual bool recallOn(pClient client);
	virtual bool gateOn(pClient client);
//@}

//@{
/*!
\name Corpse related
\deprecated Move them to cContainer at least, or create cCorpse
*/
protected:
	std::string	murderer;	//!< char's name who killed the char (forensic ev.)
	int32_t		murdertime;	//!< when the people has been killed \todo Should be moved to time_t?
public:
	inline const std::string getMurderer() const
	{ return murderer; }
	
	inline void setMurderer(const std::string &newmurderer)
	{ murderer = newmurderer; }
//@}

//@{
/*!
\name Creation related
\author Magius (CHE)

\todo Change all this into a struct and place here a pointer to that struct, so
	if an item hasn't this stuff we don't waste too much space.
*/
	int32_t		smelt;		//!< for item smelting
	/*!
	\brief for rank system, this value is the LEVEL of the item from 1 to 10.

	Simply multiply the rank*10 and calculate the MALUS this item has from the original.<br>
	for example: RANK 5 ---> 5*10=50% of malus<br>
	this item has same values decreased by 50%..<br>
	RANK 1 ---> 1*10=10% this item has 90% of malus!<br>
	RANK 10 --> 10*10=100% this item has no malus! RANK 10 is automatically setted if you select RANKSYSTEM 0.<br>
	Vars: LODAMAGE,HIDAMAGE,ATT,DEF,HP,MAXHP<br>
	*/
	int32_t		rank;

	std::string	creator;	//!< Store the name of the player made this item
	int32_t		good;		//!< Store type of GOODs to trade system! (Plz not set as UNSIGNED)
	int32_t		rndvaluerate;	//!< Store the value calculated base on RANDOMVALUE in region.scp.

	/*!
	\brief Store the skills used to make this item

	Note by Magius: Value range to -ALLSKILLS-1 to ALLSKILLS+1<br>
	To calculate skill used to made this item:<br>
	if is a positive value, substract 1 it.<br>
		Ex) madewith=34 , 34-1=33 , 33=skStealing<br>
	if is a negative value, add 1 from it and invert value.<br>
		Ex) madewith=-34 , -34+1=-33 , Abs(-33)=33=skStealing.<br>
	0 = NULL<br>

	So... a positive value is used when the item is made by a
	player with 95.0+ at that skill. Infact in this way when
	you click on the item appear its name and the name of the
	creator. A Negative value if the player is not enought
	skilled!
	*/
	int32_t		madewith;
	std::string	vendorDescription;//!< vendor description
//@}

//@{
/*!
\name Spawn
\todo Create cSpawn and move there all the spawn stuff
*/
	uint32_t	spawnserial;
	uint32_t	spawnregion;
//@}

//@{
/*!
\name Buy & Sell
*/
protected:
	int32_t		value;		//!< Price shopkeeper sells item at.
	int32_t		restock;	//!< Number up to which shopkeeper should restock this item
public:
	const int32_t calcValue(int32_t bvalue);
//@}

//@{
/*!
\name Item Use
\todo All the minUsingSkill must be changed to a parameterized functions to
	allow \c n values.
*/
protected:
	bool ToolWearOut(pClient client);			//!< Check for tool consumption. Used in doubleClick
	virtual void doubleClicked(pClient client);		//!< After an accepted doubleclick, call this virtual
        uint16_t minUsingStrength;				//!< Minimum strength to use item
        uint16_t minUsingIntelligence;				//!< Minimum inteligence to use item
        uint16_t minUsingDexterity;				//!< Minimum dexterity to use item
        Skill minUsingSkill[3];					//!< holds up to 3 skills to be checked for usability. if skInvalid no skill check is done
        uint16_t minUsingSkillValue[3];				//!< holds the 3 skill values of skills Minimum in the minUsingSkill array

public:
	void singleClick(pClient client);			//!< Single click on item
	bool usableWhenLockedDown(pPC pc = NULL);		//!< Item can be used when locked down (e.g. in a house)
	bool checkItemUsability(pChar pc, int type);		//!< If item can be used with "type" method by pc  \todo update this function with the new usability/ equippability function here and in cEquippable 
	void doubleClick(pClient client);			//!< Use of item by doubleclicking on it

	inline void setMinUsingStrength(uint16_t newStrength)
        { minUsingStrength = newStrength; }

	inline void setMinUsingDexterity(uint16_t newDexterity)
        { minUsingDexterity = newDexterity; }

	inline void setMinUsingIntelligence(uint16_t newIntelligence)
        { minUsingIntelligence = newIntelligence; }

	inline uint16_t getMinUsingStrength()
        { return minUsingStrength; }

	inline uint16_t getMinUsingDexterity()
        { return minUsingDexterity; }

	inline uint16_t getMinUsingIntelligence()
        { return minUsingIntelligence; }

	inline Skill getMinUsingSkill1()
        { return minUsingSkill[1]; }

        inline uint16_t getMinUsingSkillValue1()
	{ return minUsingSkillValue[1]; }

	inline Skill getMinUsingSkill2()
        { return minUsingSkill[2]; }

        inline uint16_t getMinUsingSkillValue2()
	{ return minUsingSkillValue[2]; }

	inline Skill getMinUsingSkill3()
        { return minUsingSkill[3]; }

        inline uint16_t getMinUsingSkillValue3()
	{ return minUsingSkillValue[3]; }


//@}

//@{
/*!
\name Special Use

\deprecated Many of these things must be removed from there or moved out of this
	section.
*/
	uint32_t	type;		//!< For things that do special things on doubleclicking
	uint32_t	type2;
	int32_t		carve;		//!< for new carve system
	int32_t		wipe;		//!< Should this item be wiped with the /wipe command
					//!< \todo This should be changed to a flag
	uint32_t	time_unused;	//!< used for house decay and possibly for more in future, gets saved
	uint32_t	timeused_last;	//!< helper attribute for time_unused, doesnt get saved
//@}

/********************************
	Effect related
********************************/
//	int32_t		glow; // LB identifies glowing objects
//	int8_t		glow_c1; // for backup of old color
//	int8_t		glow_c2;
//	int8_t		glow_effect;
	int8_t		doordir; // Reserved for doors
	bool		dooropen;
	void		explode(pClient client);

protected:
	uint32_t	decaytime;

public:
	virtual bool doDecay(bool dontDelete = false);

	inline const void setDecayTime( const uint32_t delay = /*getClockmSecs()+*/(nSettings::Server::getDecayTimer()*SECS) )
	{ decaytime = delay; }

	inline const uint32_t getDecayTime() const
	{ return decaytime; }

	uint32_t distFrom( pChar pc );
	uint32_t distFrom( pItem pi );

	inline void setAnimid(uint16_t id)
	{ animid = id; }

	inline const uint16_t getAnimid() const
	{ return animid ? animid : getId(); }

public:
	virtual void	Delete();
};

#endif
