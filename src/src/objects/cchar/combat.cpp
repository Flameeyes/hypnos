   /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

/*!
\brief Combat System Related Stuff
\author Luxor
\note Completely rewritten by Luxor in January 2002
*/

#include "common_libs.h"
#include "itemid.h"
#include "sndpkg.h"
#include "amx/amxcback.h"
#include "layer.h"
#include "tmpeff.h"
#include "npcai.h"
#include "data.h"
#include "set.h"


#include "basics.h"
#include "inlines.h"
#include "classes.h"
#include "utils.h"


/*!
\author Luxor
\brief Check if caster loss his concetration
\param pc the caster
\param damage the damage applied to the char
\return true if the caster has loss his concentration, false if not
*/
static bool checkForCastingLoss(pChar pc, int damage)
{
	if ( ! pc ) return false;
	int chanceToResist = qmin(10, int((pc->body->getSkill(skillMeditation)/10.0)-(damage*2.0)));
	if (chance(chanceToResist)) return false;
	pc->sysmsg(TRANSLATE("You break your concentration."));
	return true;
}

/*!
\author Luxor
\brief Attack a char
\param pc_def defender char
*/
void cChar::combatHit( pChar pc_def, int32_t nTimeOut )
{
	if ( !pc_def ) {
		swingtarget = NULL;
		return;
	}

	if ( amxevents[EVENT_CHR_ONCOMBATHIT] ) {
		g_bByPass = false;
		amxevents[EVENT_CHR_ONCOMBATHIT]->Call( getSerial(), pc_def->getSerial32() );
		if( g_bByPass == true )
			return;
		if( dead )	// Killed as result of script action
			return;
	}

	if( isDead() )	// Killed as result of script action
		return;
	bool hit, los;
	int dist, basedamage, damage, def, x;
	Skill fightskill, def_fightskill;
	DamageType dmgtype;

	if (!inWarMode())
		return;

	unHide();
	disturbMed();

	if( pc_def->IsHidden() ) return; //last-target bugfix

	los = losFrom(pc_def);

	pItem weapon=getWeapon();

	fightskill = weapon ? weapon->getCombatSkill() : WRESTLING;
	dist = distFrom(pc_def);

	if((dist > 1 && fightskill != ARCHERY) || !los) return;

	if ( pc_def->npc && !npc ) {
		if ( pc_def->IsInvul() )
			return;
		pChar pc_target = pc_def->target;
		if ( pc_target ) {
                        int32_t att_value = pc_target->hp/10 + pc_def->distFrom( pc_target ) / 2;
                        int32_t this_value = hp/10 + distFrom( pc_def ) / 2;
                        if ( this_value < att_value ) {
				pc_def->target = this;
				pc_def->attacker = this;
			}
		}
	}

	pItem def_Weapon = pc_def->getWeapon();
	def_fightskill = def_Weapon ? def_Weapon->getCombatSkill() : WRESTLING;

	int fs1, fs2, str1, str2, dex1, dex2;
	str1 = body->getStrength();
	str2 = pc_def->getBody()->getStrength();

	(pc_def->dx < 100) ? dex2 = pc_def->dx : dex2 = 100;
	(dx < 100) ? dex1 = dx : dex1 = 100;
	(skill[fightskill] > 0) ? fs1 = skill[fightskill] : fs1 = 1;
	(pc_def->skill[def_fightskill] > 0) ? fs2 = pc_def->skill[def_fightskill] : fs2 = 1;

        //
        // Luxor: we must calculate the chance depending on which combat situation we are.
        //
        int32_t chanceToHit = 0;
        if ( fightskill != ARCHERY && def_fightskill != ARCHERY ) { //Melee VS Melee
		chanceToHit = int( ( (fs1+500.0) / ((fs2+500.0)*2.0) )*100.0 - dex2/7.0 + dex1/7.0 );
	} else if ( fightskill == ARCHERY && def_fightskill == ARCHERY ) { //Ranged VS Ranged
		chanceToHit = int( (fs1/10.0) - dex2/2.0 + dex1/5.0 );
	} else if ( fightskill == ARCHERY && def_fightskill != ARCHERY ) { //Ranged VS Melee
		chanceToHit = int( ((fs1+500.0) / ((fs2+300.0)*2.0)) *100.0 - dex2/6.0 + dex1/5.0 );
	} else if ( fightskill != ARCHERY && def_fightskill == ARCHERY ) { //Melee VS Ranged
		chanceToHit = int( ((fs1+500.0) / (fs2*2.0)) *100.0 - dex2/7.0 + dex1/7.0 );
	}

	if (npc || pc_def->npc) { //PvM and MvM
		chanceToHit += 20; //20% bonus
	}

	if (chanceToHit < 5) chanceToHit = 5;
	else if (chanceToHit > 95) chanceToHit = 95;
	hit = chance( chanceToHit );

	checkSkillSparrCheck(fightskill, 0, 1000, pc_def);
	swingtarget = NULL;

	if ( fightskill == ARCHERY && isRunning() )
		hit = false;

	if (!hit) {

		if (amxevents[EVENT_CHR_ONHITMISS]) {
			g_bByPass = false;
			amxevents[EVENT_CHR_ONHITMISS]->Call(getSerial(), pc_def->getSerial32());
			if (g_bByPass==true) return;
		}

		if (!npc) {
			if ( chance(30) || def_fightskill == ARCHERY )
				doMissedSoundEffect();
			else {
				pc_def->doCombatSoundEffect( def_fightskill, def_Weapon );
				pc_def->emoteall( "*Parries the attack*", 1 );
			}
		}
		if (fightskill == ARCHERY) {
			if (chance(33)) {
                                pItem pi = NULL;
				if (weapon->IsBow()) {
					pi = item::CreateFromScript( "$item_arrow" );
				} else {
					pi = item::CreateFromScript( "$item_crossbow_bolt" );
				}
				if(pi) {

					pi->MoveTo( pc_def->getPosition() );
					pi->setDecay();
					pi->Refresh();//AntiChrist
				}
			}
		}
		return;
	}

	if (amxevents[EVENT_CHR_ONHIT]) {
		g_bByPass = false;
		amxevents[EVENT_CHR_ONHIT]->Call(getSerial(), pc_def->getSerial32());
		if (g_bByPass==true) return;
	}

	if (pc_def->amxevents[EVENT_CHR_ONGETHIT]) {
		g_bByPass = false;
		pc_def->amxevents[EVENT_CHR_ONGETHIT]->Call(pc_def->getSerial(), getSerial32());
		if (g_bByPass==true) return;
	}

	if ( weapon ) {
		if (chance(5) && weapon->type != ITYPE_SPELLBOOK) {
			weapon->hp--;
			if(weapon->hp <= 0) {
				sysmsg(TRANSLATE("Your weapon has been destroyed"));
				//XAN TO-DO : Insert event handler here ? :)
				weapon->Delete();
			}
		}
	}

	if (pc_def->IsInvul()) return;

	checkSkillSparrCheck(TACTICS, 0, 1000, pc_def);
	if (pc_def->getId()==BODY_FEMALE) pc_def->playSFX(0x014B);
	if (pc_def->getId()==BODY_MALE) pc_def->playSFX(0x0156);
	pc_def->playMonsterSound( SND_DEFEND );

	checkPoisoning(pc_def);	// attacker poisons defender

	if (pc_def->dx > 0) pc_def->unfreeze();

	if (fightskill != WRESTLING || npc) {
		basedamage = calcAtt();
	} else {
		basedamage = uint32_t( (skill[WRESTLING]/100.0)/2 + RandomNum(1,2) );

		//Luxor (6 dec 2001): Wrestling Disarm & Stun punch
		if ( wresmove == WRESDISARM ) {
			chanceToHit += int( skill[TACTICS]/100.0 - pc_def->skill[TACTICS]/100.0 );
			chanceToHit += int( str1/10.0 - str2/10.0 );
			if ( chance( chanceToHit ) ) {
				pItem dWeapon=pc_def->getWeapon();
				if (dWeapon!=NULL) {
					Location charpos = pc_def->getPosition();

					wresmove = 0;
					dWeapon->setContainer(0);
					dWeapon->MoveTo( charpos );
					dWeapon->Refresh();
				}
			} else {
				wresmove = 0;
				sysmsg( TRANSLATE("You failed to disarm your opponent!") );
			}
		}

		if ( wresmove == WRESSTUNPUNCH ) {
			chanceToHit += int( skill[TACTICS]/100.0 - pc_def->skill[TACTICS]/100.0 );
			chanceToHit += int( str1/10.0 - str2/10.0 );
			if ( chance( chanceToHit ) ) {
				wresmove = 0;
				tempfx::add(this, pc_def, tempfx::SPELL_PARALYZE, 0, 0, 0, 7); //paralyze for 7 secs
			} else {
				wresmove = 0;
				sysmsg( TRANSLATE("You failed to stun your opponent!") );
			}
		}
		//Luxor <End>
	}
	pc_def->checkSkill(TACTICS, 0, 1000, 1);


	damage = basedamage + (int)(basedamage/100.0 * ((skill[TACTICS])/16.0)); //Bonus damage for tactics
	damage += (int)(damage/100.0 * getStrength()/5.0); //Bonus damage for strenght
	if (checkSkillSparrCheck(ANATOMY, 0, 1000, pc_def)) { //Bonus damage for anatomy
		if ( skill[ANATOMY] < 1000 ) {
			damage += (int)( damage/100.0 * skill[ANATOMY]/50.0 );
		} else { //GM anatomist
			damage += (int)( damage/100.0 * 30.0 );
		}
	}

	pItem pShield=pc_def->getShield();
	if( pShield ) {
		if ( chance(pc_def->skill[PARRYING]/20) ) { // chance to block with shield
			pc_def->checkSkill(PARRYING, 0, 1000);
			//pc_def->emoteall( "*Parries the attack*", 1 );
			if (pShield->def!=0 && fightskill!=ARCHERY) damage -= pShield->def/2; // damage absorbed by shield
			if (pShield->def!=0 && fightskill==ARCHERY) damage -= pShield->def; // damage absorbed by shield
			if (chance(5)) pShield->hp--;
			if (pShield->hp<=0) {
				pc_def->sysmsg(TRANSLATE("Your shield has been destroyed"));
				pShield->Delete();
			}
		}
	}

	//Luxor: Armor absorption system
	x = pc_def->combatHitMessage(damage);
	def = pc_def->calcDef(x);
	if (!pc_def->npc)
		damage -= RandomNum(def, uint32_t(def*2.5)); //PC armor system
	else
		damage -= RandomNum(def/2, def); //NPC armor system
	if (damage<0) damage=0;
	//End armor absorption system

	if (!pc_def->npc) damage = (int)(damage / float(SrvParms->npcdamage));
	if (damage<0) damage=0;

	if (damage>0 && weapon ) {
		if ((weapon->amxevents[EVENT_IONDAMAGE]!=NULL)) {
			g_bByPass = false;
			damage = weapon->amxevents[EVENT_IONDAMAGE]->Call(weapon->getSerial(), pc_def->getSerial32(), damage, getSerial32());
			if (g_bByPass==true) return;
		}
	}

	//when hit and damage >1, defender fails if casting a spell!
	if (damage > 1 && !pc_def->npc) {
		int sd = 0;
		if (pc_def->getClient() != NULL) sd = pc_def->getClient()->toInt();
		if (pc_def->casting && checkForCastingLoss(pc_def, damage)) {
			pc_def->spell = magic::SPELL_INVALID;
			pc_def->casting = 0;
			pc_def->spelltime = 0;
			pc_def->unfreeze();
		}
	}

	if( damage > 0 ) {
		//Evaluate damage type
		if (fightskill == WRESTLING) dmgtype = DAMAGE_BLUDGEON;
		if (npc) {
			dmgtype = damagetype;
			damage = int(damage / 3.5);
		}
		dmgtype = weapon ? weapon->damagetype : DAMAGE_PURE;

		if (pc_def->ra) {	 // Reactive Armor
			//80% to defender, 10-20% to attacker
			this->damage(int((damage/10.0) + (damage/100.0)*float(RandomNum(1,10))));
            		if ( weapon && weapon->auxdamage ) {
                		pc_def->damage(weapon->auxdamage, weapon->auxdamagetype);
            		}
			pc_def->damage(int(damage - (damage/100.0)*20.0), dmgtype);
			pc_def->staticFX(0x374A, 0, 15);
		} else {
			pc_def->damage(damage, dmgtype);
		        if ( weapon && weapon->auxdamage ) {
	                	pc_def->damage(weapon->auxdamage, weapon->auxdamagetype);
            		}
		}
	}	//End -> if (damage > 0)
	if (!npc)
		doCombatSoundEffect(fightskill, weapon);

	if (pc_def->hp < 0) {
		pc_def->hp=0;
		pc_def->updateStats(0);
	}

	if( pc_def->HasHumanBody() ) {
		if (!pc_def->onhorse) pc_def->playAction(0x14);
	}
        if (nTimeOut != 0) {
                timeout = uiCurrentTime + nTimeOut;
        }
}

/*!
\brief Abort combat sequence
\author Sparhawk
*/
void cChar::undoCombat()
{
	/*if ( war ) //Luxor
		toggleCombat();*/
	timeout = 0;
	attacker = NULL;
	target = NULL;
	setAttackFirst(false);
}

/*!
\brief Does combat sequence
\author Luxor
*/
void cChar::doCombat()
{
	if (
		!inWarMode() ||
		isDead() ||
		isHiddenBySpell() ||
		isFrozen() ||
		( rtti() == rtti::cPC && (reinterpret_cast<cPC>this)->isOnline() )
	   )
	{
		undoCombat();
		return;
	}

	int	dist,
		fightskill,
		x = 0,
		j = 0,
		arrowsquant = 0;
	pItem	weapon = getWeapon();

	if( !target )
	{
		undoCombat();
		return;
	}

	if ( (!target->npc && !target->IsOnline()) || target->IsHidden() || target->dead || (target->npc && target->npcaitype==NPCAI_PLAYERVENDOR) )
	{
		undoCombat();
		return;
	}
	if ( !npc && !losFrom(target) ) {
		undoCombat();
		return;
	}

	dist = distFrom(pctarget);

	if ( amxevents[EVENT_CHR_ONDOCOMBAT] ) {
		g_bByPass = false;
		amxevents[EVENT_CHR_ONDOCOMBAT]->Call( getSerial(), pc_def->getSerial32(), dist, weapon ? weapon->getSerial32() : INVALID );
		if( g_bByPass == true )
		{
			return;
		}
		if( dead )	// Killed as result of script action
		{
			undoCombat();
			return;
		}
	}

	if ( npc )
		npcs::npcMagicAttack( this, target );

	if ( dist > VISRANGE )
	{
		if ( attacker )
		{
			attacker->ResetAttackFirst();
			attacker->attackerserial=INVALID;
		}
		undoCombat();
		return;
	}
	else if ( combatTimerOk() )
	{
		fightskill = weapon ? weapon->getCombatSkill() : WRESTLING;

		if (fightskill==ARCHERY)
		{
			if (weapon->ammo == 0)   //old ammo system
			{
				weapon->IsBow() ? arrowsquant=getAmount(0x0F3F) : arrowsquant=getAmount(0x1BFB);

				if (arrowsquant>0)
					x=1;
				else
					sysmsg(TRANSLATE("You are out of ammunitions!"));
			}
			else   //new ammo system
			{
				if ((getBackpack())->CountItemsByID(weapon->ammo, true))
					x=1;
				else
					sysmsg(TRANSLATE("You are out of ammunitions!"));
			}
		}
		else if (dist < 2 )
		{
			x=1;
		}

		if (x)
		{
			//Stamina maths-----------------------------------
			if(abs(SrvParms->attackstamina) > 0 && !IsGM())
			{
				if((SrvParms->attackstamina < 0) &&( stm < abs(SrvParms->attackstamina)))
				{
       					sysmsg(TRANSLATE("You are too tired to attack."));
					if ( weapon )
					{
						if (weapon->spd==0)
							weapon->spd=35;
						x = (15000 / ((stm+100) * weapon->spd)*MY_CLOCKS_PER_SEC);	//Calculate combat delay
					}
					else
					{
						unsigned short wrestling = skill[WRESTLING];
						if(wrestling>800)
							j = 50;
						else if(wrestling>600)
							j = 45;
						else if(wrestling>400)
							j = 40;
						else if(wrestling>200)
							j = 35;
						else
							j = 30;

						x = (15000 / ((stm+100) * j)*MY_CLOCKS_PER_SEC);
					}
					timeout = uiCurrentTime+x;
       				}

        			stm += SrvParms->attackstamina;

				if (stm > dx)
					stm = dx;

				if (stm < 0)
					stm = 0;
        			updateStats(2);
			}	//End stamina maths -----------------------


			//
			// Calculate combat delay
			//
			if ( weapon )
			{
	    			if (weapon->spd==0)
					weapon->spd=35;
				x = (15000 / ((dx+100) * weapon->spd)*MY_CLOCKS_PER_SEC);
			}
			else
			{
				unsigned short wrestling = skill[WRESTLING];
				if(wrestling>200)
				{
					j = 35;
				}
				else if(wrestling>400)
				{
					j = 40;
				}
				else if(wrestling>600)
				{
					j = 45;
				}
				else if(wrestling>800)
				{
					j = 50;
				}
				else
				{
					j = 30;
				}
				x = (15000 / ((dx+100) * j)*MY_CLOCKS_PER_SEC);
			}
       			timeout = uiCurrentTime+x;
			timeout2 = timeout;
			x = j = 0;

			playCombatAction();

			// New ammo system for bows and crossbows by Keldan
			if (fightskill==ARCHERY)
				if (weapon->ammo == 0)   //old ammo system
				{
					if (weapon->IsBow())
					{
						delItems(0x0F3F, 1);
						movingeffect3( getSerial(), targserial, 0x0F, 0x42, 0x08, 0x00, 0x00,0,0,0,0);
					}
					else
					{
						delItems(0x1BFB, 1);
						movingeffect3( getSerial(), targserial, 0x1B, 0xFE, 0x08, 0x00, 0x00,0,0,0,0);
					}
				}
				else   //new ammo system
				{
					(getBackpack())->DeleteAmountByID(1, weapon->ammo);
					movingeffect3( getSerial(), targserial, (weapon->ammoFx>>8)&0xFF, weapon->ammoFx & 0xFF, 0x08, 0x00, 0x00,0,0,0,0);
				}

			if ( dist < 2 || fightskill == ARCHERY )
				(reinterpret_cast<pNPC>this)->simpleAttack(target);

			if (fightskill == ARCHERY)
				combatHit( pc_def );
			else
				swingtarget = pc_def;
		}	//End -> if (x)

		if (fightskill != ARCHERY)
			combatHit( pc_def, x);
		return;
	}	//End -> else if (dist<=10 && combatTimerOk())

	if (pc_def->hp < 1)
	{
		pc_def->Kill();
		if (!npc && !pc_def->npc)
		{	//Player vs Player
			if(pc_def->IsInnocent() && Guilds->Compare(this,pc_def) == 0 )
			{
				++kills;
				sysmsg(TRANSLATE("You have killed %i innocent people."), kills);
				if (kills == repsys.maxkills+1)
					sysmsg(TRANSLATE("You are now a murderer!"));
			}
			if (SrvParms->pvp_log)
			{
				LogFile pvplog("PvP.log");
				pvplog.Write("%s was killed by %s!\n", pc_def->getCurrentName().c_str(), getCurrentName().c_str());
			}
		}
		if (npc)
		{
			toggleCombat();
		}
	}
}

/*!
\author Luxor
\brief Checks for poisoning attack
\param pc_def Defender character
*/
void cChar::checkPoisoning(pChar pc_def)
{
	if ( npc ) { //NPC poisoning
		if ( poison && pc_def->poisoned < poison && chance(33) ) {
			pc_def->applyPoison( static_cast<PoisonType>(poison) );
		}
	} else { //PC poisoning
		pItem weapon = getWeapon();
		if ( weapon && weapon->poisoned > 0 && pc_def->poisoned < weapon->poisoned ) {
			if ( chance(33) ) {
				pc_def->applyPoison( static_cast<PoisonType>(weapon->poisoned) );
				if ( chance(80) ) {
					if ( weapon->poisoned > 1 )
						weapon->poisoned = static_cast<PoisonType>(weapon->poisoned -1);
					else {
						weapon->poisoned = POISON_NONE;
						sysmsg( TRANSLATE("The poison you put on the weapon went out.") );
					}
					(poison > 1) ? poison = static_cast<PoisonType>(poison-1) : poison = 0;
				}
			}
		}
	}
}

/*!
\author Luxor
\brief Determines part of body hit and show to pc_def the hit message (?)
\param damaage Damage inflicted
\return part of body hit
*/
int cChar::combatHitMessage(int32_t damage)
{
	char temp[TEMP_STR_SIZE];
	int hitin;
	int x = rand()%100;// determine area of body hit
	if (SrvParms->combathitmessage != 1)
	{
		if (x<=44) x=1; // body
		else if (x<=58) x=2; // arms
		else if (x<=72) x=3; // head
		else if (x<=86) x=4; // legs
		else if (x<=93) x=5; // neck
		else x=6; // hands
		return x; //NumberSix
	}

	temp[0] = '\0';
	hitin = rand()%2;
	if (x<=44)
	{
		x=1;       // body
		switch (hitin)
		{
			case 1:
				//later take into account dir facing attacker during battle
				if (damage < 10) strcpy(temp, TRANSLATE("hits you in your Chest!"));
				if (damage >=10) strcpy(temp, TRANSLATE("lands a terrible blow to your Chest!"));
				break;
			case 2:
				if (damage < 10) strcpy(temp, TRANSLATE("lands a blow to your Stomach!"));
				if (damage >=10) strcpy(temp, TRANSLATE("knocks the wind out of you!"));
				break;
			default:
				if (damage < 10) strcpy(temp, TRANSLATE("hits you in your Ribs!"));
				if (damage >=10) strcpy(temp, TRANSLATE("broken your Rib?!"));
		}
	}
	else if (x<=58)
	{
		if (damage > 1)
		{
			x=2;  // arms
			switch (hitin)
			{
				case 1:	strcpy(temp, TRANSLATE("hits you in Left Arm!"));	break;
				case 2:	strcpy(temp, TRANSLATE("hits you in Right Arm!"));	break;
				default:strcpy(temp, TRANSLATE("hits you in Right Arm!"));
			}
		}
	}
	else if (x<=72)
	{
		x=3;  // head
		switch (hitin)
		{
		case 1:
			if (damage < 10) strcpy(temp, TRANSLATE("hits you you straight in the Face!"));
			if (damage >=10) strcpy(temp, TRANSLATE("lands a stunning blow to your Head!"));
			break;
		case 2:
			if (damage < 10) strcpy(temp, TRANSLATE("hits you to your Head!")); //kolours - (09/19/98)
			if (damage >=10) strcpy(temp, TRANSLATE("smashed a blow across your Face!"));
			break;
		default:
			if (damage < 10) strcpy(temp, TRANSLATE("hits you you square in the Jaw!"));
			if (damage >=10) strcpy(temp, TRANSLATE("lands a terrible hit to your Temple!"));
		}
	}
	else if (x<=86)
	{
		x=4;  // legs
		switch (hitin)
		{
			case 1:	strcpy(temp, TRANSLATE("hits you in Left Thigh!"));	break;
			case 2:	strcpy(temp, TRANSLATE("hits you in Right Thigh!"));	break;
			default:strcpy(temp, TRANSLATE("hits you in Groin!"));
		}
	}
	else if (x<=93)
	{
		x=5;  // neck
		strcpy(temp, TRANSLATE("hits you to your Throat!"));
	}
	else
	{
		x=6;  // hands
		switch (hitin)
		{
			case 1:
				if (damage > 1) strcpy(temp, TRANSLATE("hits you in Left Hand!"));
				break;
			case 2:
				if (damage > 1) strcpy(temp, TRANSLATE("hits you in Right Hand!"));
				break;
			default:
				if (damage > 1) strcpy(temp, TRANSLATE("hits you in Right Hand!"));
		}
	}
	pChar pc_attacker = pointers::findCharBySerial(attackerserial);
	if ( pc_attacker) {
		sysmsg("%s %s",pc_attacker->getCurrentName().c_str(), temp);
	}

	return x;
}

/*!
\author Luxor
\brief Calculates defense of the given body part
\param x Body part hit
\return defense of the part
*/
int cChar::calcDef(int32_t x)
{
	if (npc) return def;

	char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var

	pItem pj = NULL;
	int total = 0;
	int armordef = 0;

	NxwItemWrapper si;
	si.fillItemWeared( this, false, true, true );
	for( si.rewind(); !si.isEmpty(); si++ )
	{
		pItem pi=si.getItem();
		if (!pi) continue;
		if (pi->layer > LAYER_1HANDWEAPON && pi->layer < LAYER_MOUNT) {
			if (pi->def>0)
			{
				int hpPerc = int(float(pi->hp)/float(pi->maxhp)*100.0);
				armordef = qmax( 1, int(pi->def/100.0*hpPerc) );

				switch (pi->layer)
				{
					case 5:
					case 13:
					case 17:
					case 20:
					case 22:
						if (x==1) {
							total=armordef;
							pj=pi;
						}
						break;
					case 19:
						if (x==2) {
							total=armordef;
							pj=pi;
						}
						break;
					case 6:
						if (x==3) {
							total=armordef;
							pj=pi;
						}
						break;
					case 3:
					case 4:
					case 12:
					case 23:
					case 24:
						if (x==4) {
							total=armordef;
							pj=pi;
						}
						break;
					case 10:
						if (x==5) {
							total=armordef;
							pj=pi;
						}
						break;
					case 7:
						if (x==6) {
							total=armordef;
							pj=pi;
						}
						break;
					default:
						break;
				}
				if (x==0) total+=armordef;
			}
		}
	}

	if (x==0) return total;

	if (pj) {
		/*
		//	Sparhawk:	TODO implement scriptable cChar static function isDamageProneLayer()
		//			function checks wether item on layer can be damaged
		//			TODO damagetype should be considered before reducing hp.
		//
		if( isDamageProneLayer( pj->layer ) )
		{
			if( chance( 5 ) )
			{
				--pj->hp; //Take off a hit point
				if( pj->hp <= 0 )
				{
					if ( strncmp(pj->getCurrentName().c_str(), "#", 1) )
					{
						sprintf(temp,TRANSLATE("Your %s has been destroyed"),pj->getCurrentName().c_str());
					} else
					{
						tile_st tile;
						Map->SeekTile(pj->id(), &tile);
						sprintf(temp,TRANSLATE("Your %s has been destroyed"),tile.name);
					}
					modifyStrength(-pj->st2);
					dx -= pj->dx2;
					in -= pj->in2;
					sysmsg( temp );
					pj->deleteItem();
				}
			}
		}
		*/
		//Dont damage wears beard hair and backpack
		if(pj->layer!=0x0B && pj->layer!=0x10 && pj->layer!=0x15 && pj->layer!=0x4 && pj->layer!=0x5 && pj->layer!=0x6
			&& pj->layer!=0xC && pj->layer!=0x11 && pj->layer!=0x14 && pj->layer!=0x16) {
			if(chance(5))
				pj->hp--; //Take off a hit point
			if(pj->hp<=0) {
				if ( strncmp(pj->getCurrentName().c_str(), "#", 1) ) {
					sprintf(temp,TRANSLATE("Your %s has been destroyed"),pj->getCurrentName().c_str());
				} else {
					tile_st tile;
					data::seekTile(pj->getId(), tile);
					sprintf(temp,TRANSLATE("Your %s has been destroyed"),tile.name);
				}
				//LB bugfix !!! -- remove BONUS STATS given by equipped special items
				// LB, lets pray st2,dx2,in2 values are set correctly :)
				modifyStrength(-pj->st2);
				dx -= pj->dx2;
				in -= pj->in2;
				sysmsg(temp);
				pj->Delete();
			}
		}
	}

	if (getClient() != NULL) getClient()->statusWindow(this,true);

	if (total<2) total=2;
	return total*3;
}

/*!
\author Luxor
\brief Sets a wrestling move, including the skills check
\author Luxor
\param move Move's id
*/
void cChar::setWresMove(int32_t move)
{
	switch (move)
	{
		case WRESDISARM:
			if (skill[WRESTLING] >= 800 && skill[ARMSLORE] >= 800) {
				sysmsg(TRANSLATE("You prepare yourself for a disarm move."));
				wresmove = 1;	//set wresmove to disarm
			} else {
				wresmove = 0;
			}
			break;

		case WRESSTUNPUNCH:
			if (skill[WRESTLING] >= 800 && skill[ANATOMY] >= 800) {
				sysmsg(TRANSLATE("You prepare yourself for a stunning punch."));
				wresmove = 2;	//set wresmove to stun punch
			} else {
				wresmove = 0;
			}
			break;

		default:
			wresmove = 0;
			break;
	}
}

/*!
\author Luxor
\brief Calculates total ataack power
\return character's attack
*/
int cChar::calcAtt()
{
	if(npc) {
		if (lodamage < 1 || hidamage < 1) return 1;
		return RandomNum(lodamage, hidamage);
	}

	pItem pi = getWeapon();
	//if(pi==NULL)
	//	return 0;
	if ( ! pi ) return skill[WRESTLING]/100;

	return RandomNum(pi->lodamage, pi->hidamage);
}

/*!
\author Luxor
\brief Does a combat sound event
\param fightskill fighting skill
\param weapon weapon used
*/
void cChar::doCombatSoundEffect(uint16_t fightskill, pItem weapon)
{
	bool heavy=false;
	int a=RandomNum(0,3);

	//check for heavy weapon
	if (weapon && weapon->IsAxe())
		heavy=true;

	if(heavy)
	{
		if (a==0 || a==1) playSFX(0x0236);
		else playSFX(0x0237);
		return;
	}

	switch(fightskill)
	{
		case ARCHERY:
			playSFX(0x0234);
			break;
		case FENCING:
		case SWORDSMANSHIP:
			if (a==0 || a==1) playSFX(0x023B);
			else playSFX(0x023C);
			break;
		case MACEFIGHTING:
			if (a==0 || a==1) playSFX(0x0232);
			else if (a==2) playSFX(0x0139);
			else playSFX(0x0233);
			break;
		case WRESTLING:
			if (a==0) playSFX(0x0135);
			else if (a==1) playSFX(0x0137);
			else if (a==2) playSFX(0x013D);
			else playSFX(0x013B);
			break;
		default:
			playSFX(0x013D);
	}
}

/*!
\author Luxor
\brief Does an animation for a char fighting on feet
*/
void cChar::combatOnFoot()
{
	pItem weapon = getWeapon();
	int m = RandomNum(0,3);

	if (weapon) {
//		short weapId = weapon->id(); // unused variable

		if (weapon->IsBow()) {
			playAction(0x12); //bow
			return;
		} else if (weapon->IsCrossbow() || weapon->IsHeavyCrossbow()) {
			playAction(0x13); //crossbow - regular
			return;
		} else if (weapon->IsSword()) {
			switch (m) //swords
			{
				case 0:		playAction(0x0D);	return; //side swing
				case 1:		playAction(0x0A);	return; //poke
				default:	playAction(0x09);	return; //top-down swing
			}
		} else if (weapon->IsMace1H()) {
			switch (m) //maces
			{
				case 0:		playAction(0x0D);	return;	//side swing
				default:	playAction(0x09);	return; //top-down swing
			}
		} else if (weapon->IsMace2H() || weapon->IsAxe() || weapon->IsSpecialMace()) {
			switch (m)
			{
				case 0:		playAction(0x0D);	return; //2H top-down
				case 1:		playAction(0x0C);	return; //2H swing
				default:	playAction(0x0D);	return; //2H top-down
			}
		} else if (weapon->IsFencing1H())	{
			switch (m) //fencing
			{
				case 0:		playAction(0x09);	return; //top-down
				case 1:		playAction(0x0D);	return; //side-swipe
				default:	playAction(0x0A);	return; //default: poke
			}
		} else if (weapon->IsFencing2H()) { 	//pitchfork & spear
			switch (m) //pitchfork
			{
				case 0:		playAction(0x0D);	return; //top-down
				default:	playAction(0x0E);	return; //default: 2-handed poke
			}
		}
	} else { //fist fighting
		switch (m)
		{
			case 0:		playAction(0x0A);	return; //fist straight-punch
			case 1:		playAction(0x09);	return; //fist top-down
			default:	playAction(0x1F);	return; //default: fist over-head
		}
	}
}

/*!
\author Luxor
\brief Does an animation for a char fighting on horse
*/
void cChar::combatOnHorse()
{
	pItem weapon = getWeapon();
	if (weapon) {
		short weapId = weapon->getId();

		if (weapon->IsBow()) {
			playAction(0x1B);
			return;
		} else if (weapon->IsCrossbow() || weapon->IsHeavyCrossbow()) {
			playAction(0x1C);
			return;
		} else if(  weapon->IsSword() || weapon->IsSpecialMace() || weapon->IsMaceType() || (weapId ==0x0FB4 || weapId ==0x0FB5) || weapon->IsFencing1H() ) {
			playAction(0x1A);
			return;
		} else if ( weapon->IsAxe() || weapon->IsFencing2H() ) {
			playAction(0x1D); //2Handed
			return;
		}
	} else {
		playAction(0x1A); //fist fighting
		return;
	}
}


/*** Xan : this function is critical, and *SHOULD* be used everytime
 *** an attack request is made, not only for dblclicks in war mode
 ***/

void cChar::attackStuff(pChar victim)
{
	if ( ! victim ) return;

	if( getSerial() == victim->getSerial32() )
		return;

	if ( amxevents[EVENT_CHR_ONBEGINATTACK]) {
		g_bByPass = false;
		amxevents[EVENT_CHR_ONBEGINATTACK]->Call( getSerial(), victim->getSerial32() );
		if (g_bByPass==true) return;
	}

	if ( victim->amxevents[EVENT_CHR_ONBEGINDEFENSE]) {
		g_bByPass = false;
		victim->amxevents[EVENT_CHR_ONBEGINDEFENSE]->Call( victim->getSerial(), getSerial32() );
		if (g_bByPass==true) return;
	}
	/*
	runAmxEvent( EVENT_CHR_ONBEGINATTACK, getSerial(), victim->getSerial32() );
	if (g_bByPass==true)
		return;
	victim->runAmxEvent( EVENT_CHR_ONBEGINDEFENSE, victim->getSerial(), getSerial32() );
	if (g_bByPass==true)
		return;
	*/
	targserial=victim->getSerial();
	unHide();
	disturbMed();

	if( victim->dead || victim->hp <= 0 )//AntiChrist
	{
		sysmsg( TRANSLATE("That person is already dead!") );
		return;
	}

	if ( victim->npcaitype==NPCAI_PLAYERVENDOR)
	{
		sysmsg( TRANSLATE("%s cannot be harmed."), victim->getCurrentName().c_str() );
		return;
	}

        //!\TODO: modify this to send a packet
	SndAttackOK(s, victim->getSerial());	//keep the target highlighted


	if (!( victim->targserial== INVALID))
	{
		victim->attackerserial=getSerial();
		victim->ResetAttackFirst();
	}
	SetAttackFirst();
	attackerserial=victim->getSerial();


        //!\TODO once set are done revise this
	if( victim->guarded )
	{
		NxwCharWrapper sc;
		sc.fillOwnedNpcs( victim, false, false );
		for ( sc.rewind(); !sc.isEmpty(); sc++ )
		{
			pChar guard = sc.getChar();
			if ( guard )
				if ( guard->npcaitype == NPCAI_PETGUARD && ( distFrom( guard )<= 10 ) )
					npcattacktarget(pc, guard);
		}
	}

	if ((region[ victim->region].priv & RGNPRIV_GUARDED) && (SrvParms->guardsactive))
	{
		if (isGrey())
			setGrey();

		if (victim->npc==0 && victim->isInnocent() && (!victim->IsGrey()) && Guilds->Compare( pc, victim )==0) //REPSYS
		{
			criminal( pc );
			if (ServerScp::g_nInstantGuard==1)
				npcs::SpawnGuard(pc, victim, getPosition() );
		}
		else if( victim->npc && victim->isInnocent() && !victim->HasHumanBody() && victim->npcaitype!=NPCAI_TELEPORTGUARD )
		{
			criminal( pc );
			if (ServerScp::g_nInstantGuard==1)
				npcs::SpawnGuard(pc, victim, getPosition() );
		}
		else if( victim->npc && victim->isInnocent() && victim->HasHumanBody() && victim->npcaitype!=NPCAI_TELEPORTGUARD )
		{
			victim->talkAll( TRANSLATE("Help! Guards! I've been attacked!"), 1);
			criminal( victim );
			callguards(DEREF_pChar(victim)); // Sparhawk must check if npcs can call guards
		}
		else if( victim->npc && victim->npcaitype==NPCAI_TELEPORTGUARD)
		{
			criminal( pc );
			npcattacktarget(victim, pc);
		}
		else if ((victim->npc || victim->tamed) && !victim->war && victim->npcaitype!=NPCAI_TELEPORTGUARD)
		{
			victim->fight( pc );
		}
		else
		{
			victim->setNpcMoveTime();
		}
		//emoteall( "You see %s attacking %s!", 1, getCurrentName().c_str(), victim->getCurrentName().c_str() );
	}
	else	// not a guarded area
	{
		if ( victim->IsInnocent())
		{
			if ( victim->IsGrey())
				attacker->SetGrey();
			if (!victim->npc && (!victim->IsGrey()) && Guilds->Compare(pc, victim )==0)
			{
				criminal( pc );
			}
			else if (victim->npc && victim->tamed)
			{
				criminal( pc );
				npcattacktarget(victim, pc);
			}
			else if (victim->npc)
			{
				criminal( pc );
				npcattacktarget(victim, pc);
				if (victim->HasHumanBody() )
				{
					victim->talkAll(TRANSLATE("Help! Guards! Tis a murder being commited!"), 1);
				}
			}
		}
	}

}

