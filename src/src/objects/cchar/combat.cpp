/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\brief Combat System Related Stuff
\author Luxor
\note Completely rewritten by Luxor in January 2002
*/

#include "objects/cchar.h"
#include "objects/cnpc.h"
#include "objects/cpc.h"
#include "objects/cclient.h"
#include "objects/citem/cweapon.h"
#include "common_libs.h"
#include "tmpeff.h"
#include "inlines.h"
#include "settings.h"

/*!
\author Luxor
\brief Check if caster loss his concetration
\param damage the damage applied to the char
\return true if the caster has loss his concentration, false if not
*/
inline bool cChar::checkForCastingLoss(int damage)
{
	int chanceToResist = qmin(10, int((body->getSkill(skMeditation)/10.0)-(damage*2.0)));
	if (chance(chanceToResist))
		return false;
	
	pPC tpc = NULL; // This PC
	if ( (tpc = dynamic_cast<pPC>(this)) && tpc->getClient() )
		tpc->getClient()->sysmessage("You break your concentration.");
	
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
	pFunctionHandle evt = NULL;

	if ( events[evtChrOnCombatHit] ) {
		tVariantVector params = tVariantVector(2);
		params[0] = getSerial(); params[1] = pc_def->getSerial();
		events[evtChrOnCombatHit]->setParams(params);
		events[evtChrOnCombatHit]->execute();
		if ( events[evtChrOnCombatHit]->isBypassed() )
			return;
		if( isDead() )	// Killed as result of script action
			return;
	}
	
	bool hit, los;
	int dist, basedamage, damage, def, x;
	Skill fightskill, def_fightskill;
	DamageType dmgtype;

	if (!inWarMode())
		return;

	unHide();
	disturbMed();

	if( pc_def->isHidden() ) return; //last-target bugfix

	los = losFrom(pc_def);

	pWeapon weapon = getBody()->getWeapon();

	fightskill = weapon ? weapon->getCombatSkill() : skWrestling;
	dist = distFrom(pc_def);

	if((dist > 1 && fightskill != skArchery) || !los) return;

	if ( dynamic_cast<pNPC>(pc_def) && dynamic_cast<pPC>(this) ) {
		if ( pc_def->isInvul() )
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

	pWeapon def_Weapon = pc_def->getBody()->getWeapon();
	def_fightskill = def_Weapon ? def_Weapon->getCombatSkill() : skWrestling;

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
        if ( fightskill != skArchery && def_fightskill != skArchery ) { //Melee VS Melee
		chanceToHit = int( ( (fs1+500.0) / ((fs2+500.0)*2.0) )*100.0 - dex2/7.0 + dex1/7.0 );
	} else if ( fightskill == skArchery && def_fightskill == skArchery ) { //Ranged VS Ranged
		chanceToHit = int( (fs1/10.0) - dex2/2.0 + dex1/5.0 );
	} else if ( fightskill == skArchery && def_fightskill != skArchery ) { //Ranged VS Melee
		chanceToHit = int( ((fs1+500.0) / ((fs2+300.0)*2.0)) *100.0 - dex2/6.0 + dex1/5.0 );
	} else if ( fightskill != skArchery && def_fightskill == skArchery ) { //Melee VS Ranged
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

	if ( fightskill == skArchery && isRunning() )
		hit = false;

	if (!hit) {

		if ( events[evtChrOnHitMiss] ) {
			tVariantVector params = tVariantVector(2);
			params[0] = getSerial(); params[1] = pc_def->getSerial();
			events[evtChrOnHitMiss]->setParams(params);
			events[evtChrOnHitMiss]->execute();
			if ( events[evtChrOnHitMiss]->isBypassed() )
				return;
		}

		if (!npc) {
			if ( chance(30) || def_fightskill == skArchery )
				doMissedSoundEffect();
			else {
				pc_def->doCombatSoundEffect( def_fightskill, def_Weapon );
				pc_def->emoteall( "*Parries the attack*", 1 );
			}
		}
		if (fightskill == skArchery) {
			if (chance(33)) {
                                pItem pi = NULL;
				if (weapon->isBow()) {
// 					pi = item::CreateFromScript( "$item_arrow" );
				} else {
// 					pi = item::CreateFromScript( "$item_crossbow_bolt" );
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

	if ( events[evtChrOnHit] ) {
		tVariantVector params = tVariantVector(2);
		params[0] = getSerial(); params[1] = pc_def->getSerial();
		events[evtChrOnHit]->setParams(params);
		events[evtChrOnHit]->execute();
		if ( events[evtChrOnHit]->isBypassed() )
			return;
	}
	
	pFunctionHandle evt = src->getEvent(evtChrOnGetHit);
	if ( evt )
	{
		tVariantVector params = tVariantVector(2);
		params[0] = pc_def->getSerial(); params[1] = getSerial();
		evt->setParams(params);
		evt->execute();
		if ( evt->isBypassed() )
			return;
	}
	
	if ( weapon ) {
		if (chance(5) && weapon->type != ITYPE_SPELLBOOK) {
			weapon->hp--;
			if(weapon->hp <= 0) {
				sysmsg("Your weapon has been destroyed");
				//XAN TO-DO : Insert event handler here ? :)
				weapon->Delete();
			}
		}
	}

	if (pc_def->isInvul()) return;

	checkSkillSparrCheck(skTactics, 0, 1000, pc_def);
	if (pc_def->getBody()->getId()==bodyFemale) pc_def->playSFX(0x014B);
	if (pc_def->getBody()->getId()==bodyMale) pc_def->playSFX(0x0156);
	pc_def->playMonsterSound( SND_DEFEND );

	checkPoisoning(pc_def);	// attacker poisons defender

	if (pc_def->dx > 0) pc_def->unfreeze();

	if (fightskill != skWrestling || npc) {
		basedamage = calcAtt();
	} else {
		basedamage = uint32_t( (skill[skWrestling]/100.0)/2 + RandomNum(1,2) );

		//Luxor (6 dec 2001): Wrestling Disarm & Stun punch
		if ( wresmove == WRESDISARM ) {
			chanceToHit += int( skill[skTactics]/100.0 - pc_def->skill[skTactics]/100.0 );
			chanceToHit += int( str1/10.0 - str2/10.0 );
			if ( chance( chanceToHit ) ) {
				pItem dWeapon=pc_def->getWeapon();
				if (dWeapon!=NULL) {
					sLocation charpos = pc_def->getPosition();

					wresmove = 0;
					dWeapon->setContainer(0);
					dWeapon->MoveTo( charpos );
					dWeapon->Refresh();
				}
			} else {
				wresmove = 0;
				sysmsg("You failed to disarm your opponent!");
			}
		}

		if ( wresmove == WRESSTUNPUNCH ) {
			chanceToHit += int( skill[skTactics]/100.0 - pc_def->skill[skTactics]/100.0 );
			chanceToHit += int( str1/10.0 - str2/10.0 );
			if ( chance( chanceToHit ) ) {
				wresmove = 0;
				tempfx::add(this, pc_def, tempfx::tmpfxSpellParalyze, 0, 0, 0, 7); //paralyze for 7 secs
			} else {
				wresmove = 0;
				sysmsg("You failed to stun your opponent!");
			}
		}
		//Luxor <End>
	}
	pc_def->checkSkill(skTactics, 0, 1000, 1);


	damage = basedamage + (int)(basedamage/100.0 * ((skill[skTactics])/16.0)); //Bonus damage for tactics
	damage += (int)(damage/100.0 * getStrength()/5.0); //Bonus damage for strength
	if (checkSkillSparrCheck(skAnatomy, 0, 1000, pc_def)) { //Bonus damage for anatomy
		if ( skill[skAnatomy] < 1000 ) {
			damage += (int)( damage/100.0 * skill[skAnatomy]/50.0 );
		} else { //GM anatomist
			damage += (int)( damage/100.0 * 30.0 );
		}
	}

	pItem pShield=pc_def->getShield();
	if( pShield ) {
		if ( chance(pc_def->skill[skParrying]/20) ) { // chance to block with shield
			pc_def->checkSkill(skParrying, 0, 1000);
			//pc_def->emoteall( "*Parries the attack*", 1 );
			if (pShield->def!=0 && fightskill!=skArchery) damage -= pShield->def/2; // damage absorbed by shield
			if (pShield->def!=0 && fightskill==skArchery) damage -= pShield->def; // damage absorbed by shield
			if (chance(5)) pShield->hp--;
			if (pShield->hp<=0) {
				pc_def->sysmsg("Your shield has been destroyed");
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
		
		pFunctionHandle evt = src->getEvent(evtItmOnDamage);
		if ( evt )
		{
			tVariantVector params = tVariantVector(4);
			params[0] = weapon->getSerial(); params[1] = pc_def->getSerial();
			params[2] = damage; params[3] = getSerial();
			evt->setParams(params);
			evt->execute();
			if ( evt->isBypassed() )
				return;
		}
	
	}

	//when hit and damage >1, defender fails if casting a spell!
	if (damage > 1 && !pc_def->npc) {
		if (pc_def->casting && pc_def->checkForCastingLoss(damage)) {
			pc_def->spell = magic::spellInvalid;
			pc_def->casting = 0;
			pc_def->spelltime = 0;
			pc_def->unfreeze();
		}
	}

	if( damage > 0 ) {
		//Evaluate damage type
		if (fightskill == skWrestling) dmgtype = damBludgeon;
		if (npc) {
			dmgtype = damagetype;
			damage = int(damage / 3.5);
		}
		dmgtype = weapon ? weapon->damagetype : damPure;

		if (pc_def->ra) {	 // Reactive Armor
			//80% to defender, 10-20% to attacker
			this->damage(int((damage/10.0) + (damage/100.0)*float(RandomNum(1,10))));
            		if ( weapon && weapon->auxdamage ) {
                		pc_def->damage(weapon->auxdamage, weapon->auxdamagetype);
            		}
			pc_def->damage(int(damage - (damage/100.0)*20.0), dmgtype);
			staticFX(pc_def, 0x374A, 0, 15);
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
		pc_def->updateHp();
	}

	if( pc_def->HasHumanBody() ) {
		if (!pc_def->onhorse) pc_def->playAction(0x14);
	}
        if (nTimeOut != 0) {
                timeout = getclock() + nTimeOut;
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

	if ( (!target->npc && !target->isOnline()) || target->isHidden() || target->dead || (target->npc && target->npcaitype==NPCAI_PLAYERVENDOR) )
	{
		undoCombat();
		return;
	}
	if ( !npc && !losFrom(target) ) {
		undoCombat();
		return;
	}

	dist = distFrom(pctarget);

	pFunctionHandle evt = getEvent(cChar::evtChrOnDoCombat);
	if ( evt ) {
		tVariantVector params = tVariantVector(4);
		params[0] = getSerial(); params[1] = pc_def->getSerial(); params[2] = dist; params[3] =  weapon ? weapon->getSerial() : INVALID;
		evt->setParams(params);
		evt->execute();
		if( evt->isBypassed() )
			return;
		if( isDead() ) {	// Killed as result of script action
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
		fightskill = weapon ? weapon->getCombatSkill() : skWrestling;

		if (fightskill==skArchery)
		{
			if (weapon->ammo == 0)   //old ammo system
			{
				weapon->isBow() ? arrowsquant=getAmount(0x0F3F) : arrowsquant=getAmount(0x1BFB);

				if (arrowsquant>0)
					x=1;
				else
					sysmsg("You are out of ammunitions!");
			}
			else   //new ammo system
			{
				if ((getBackpack())->CountItemsByID(weapon->ammo, true))
					x=1;
				else
					sysmsg("You are out of ammunitions!");
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
       					sysmsg("You are too tired to attack.");
					if ( weapon )
					{
						if (weapon->spd==0)
							weapon->spd=35;
						x = (15000 / ((stm+100) * weapon->spd)*SECS);	//Calculate combat delay
					}
					else
					{
						unsigned short wrestling = skill[skWrestling];
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

						x = (15000 / ((stm+100) * j)*SECS);
					}
					timeout = getclock()+x;
       				}

        			stm += SrvParms->attackstamina;

				if (stm > dx)
					stm = dx;

				if (stm < 0)
					stm = 0;
        			updateStamina();
			}	//End stamina maths -----------------------


			//
			// Calculate combat delay
			//
			if ( weapon )
			{
	    			if (weapon->spd==0)
					weapon->spd=35;
				x = (15000 / ((dx+100) * weapon->spd)*SECS);
			}
			else
			{
				unsigned short wrestling = skill[skWrestling];
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
				x = (15000 / ((dx+100) * j)*SECS);
			}
       			timeout = getclock()+x;
			timeout2 = timeout;
			x = j = 0;

			playCombatAction();

			// New ammo system for bows and crossbows by Keldan
			if (fightskill==skArchery)
				if (weapon->ammo == 0)   //old ammo system
				{
					if (weapon->isBow())
					{
						delItems(0x0F3F, 1);
						movingFX( this, target, 0x0F42, 0x08, 0x00, false);
					}
					else
					{
						delItems(0x1BFB, 1);
						movingFX( this, target, 0x1BFE, 0x08, 0x00, false);
					}
				}
				else   //new ammo system
				{
					(getBackpack())->DeleteAmountByID(1, weapon->ammo);
					movingFX( this, target, weapon->ammoFx, 0x08, 0x00, false);
				}

			if ( dist < 2 || fightskill == skArchery )
				(reinterpret_cast<pNPC>this)->simpleAttack(target);

			if (fightskill == skArchery)
				combatHit( pc_def );
			else
				swingtarget = pc_def;
		}	//End -> if (x)

		if (fightskill != skArchery)
			combatHit( pc_def, x);
		return;
	}	//End -> else if (dist<=10 && combatTimerOk())

	if (pc_def->hp < 1)
	{
		pc_def->Kill();
		if (!npc && !pc_def->npc)
		{	//Player vs Player
			if(pc_def->isInnocent() && Guilds->Compare(this,pc_def) == 0 )
			{
				++kills;
				sysmsg("You have killed %i innocent people.", kills);
				if (kills == repsys.maxkills+1)
					sysmsg("You are now a murderer!");
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
						weapon->poisoned = poisonNone;
						sysmsg("The poison you put on the weapon went out.");
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
\param damage Damage inflicted
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
				if (damage < 10) strcpy(temp, "hits you in your Chest!");
				if (damage >=10) strcpy(temp, "lands a terrible blow to your Chest!");
				break;
			case 2:
				if (damage < 10) strcpy(temp, "lands a blow to your Stomach!");
				if (damage >=10) strcpy(temp, "knocks the wind out of you!");
				break;
			default:
				if (damage < 10) strcpy(temp, "hits you in your Ribs!");
				if (damage >=10) strcpy(temp, "broken your Rib?!");
		}
	}
	else if (x<=58)
	{
		if (damage > 1)
		{
			x=2;  // arms
			switch (hitin)
			{
				case 1:	strcpy(temp, "hits you in Left Arm!");	break;
				case 2:	strcpy(temp, "hits you in Right Arm!");	break;
				default:strcpy(temp, "hits you in Right Arm!");
			}
		}
	}
	else if (x<=72)
	{
		x=3;  // head
		switch (hitin)
		{
		case 1:
			if (damage < 10) strcpy(temp, "hits you you straight in the Face!");
			if (damage >=10) strcpy(temp, "lands a stunning blow to your Head!");
			break;
		case 2:
			if (damage < 10) strcpy(temp, "hits you to your Head!"); //kolours - (09/19/98)
			if (damage >=10) strcpy(temp, "smashed a blow across your Face!");
			break;
		default:
			if (damage < 10) strcpy(temp, "hits you you square in the Jaw!");
			if (damage >=10) strcpy(temp, "lands a terrible hit to your Temple!");
		}
	}
	else if (x<=86)
	{
		x=4;  // legs
		switch (hitin)
		{
			case 1:	strcpy(temp, "hits you in Left Thigh!");	break;
			case 2:	strcpy(temp, "hits you in Right Thigh!");	break;
			default:strcpy(temp, "hits you in Groin!");
		}
	}
	else if (x<=93)
	{
		x=5;  // neck
		strcpy(temp, "hits you to your Throat!");
	}
	else
	{
		x=6;  // hands
		switch (hitin)
		{
			case 1:
				if (damage > 1) strcpy(temp, "hits you in Left Hand!");
				break;
			case 2:
				if (damage > 1) strcpy(temp, "hits you in Right Hand!");
				break;
			default:
				if (damage > 1) strcpy(temp, "hits you in Right Hand!");
		}
	}
	pChar pc_attacker = cSerializable::findCharBySerial(attackerserial);
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
						sysmsg("Your %s has been destroyed", pj->getCurrentName().c_str());
					} else
					{
						tile_st tile;
						Map->SeekTile(pj->id(), &tile);
						sysmsg("Your %s has been destroyed", tile.name);
					}
					modifyStrength(-pj->st2);
					dx -= pj->dx2;
					in -= pj->in2;
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
					sysmsg("Your %s has been destroyed", pj->getCurrentName().c_str());
				} else {
					tile_st tile;
					data::seekTile(pj->getId(), tile);
					sysmsg("Your %s has been destroyed", tile.name);
				}
				//LB bugfix !!! -- remove BONUS STATS given by equipped special items
				// LB, lets pray st2,dx2,in2 values are set correctly :)
				modifyStrength(-pj->st2);
				dx -= pj->dx2;
				in -= pj->in2;
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
			if (skill[skWrestling] >= 800 && skill[skArmsLore] >= 800) {
				sysmsg("You prepare yourself for a disarm move.");
				wresmove = 1;	//set wresmove to disarm
			} else {
				wresmove = 0;
			}
			break;

		case WRESSTUNPUNCH:
			if (skill[skWrestling] >= 800 && skill[skAnatomy] >= 800) {
				sysmsg("You prepare yourself for a stunning punch.");
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
\brief Does a combat sound event
\param fightskill fighting skill
\param weapon weapon used
*/
void cChar::doCombatSoundEffect(uint16_t fightskill, pWeapon weapon)
{
	int a=RandomNum(0,3);

	//check for heavy weapon
	if(weapon && weapon->isAxe())
	{
		if (a==0 || a==1) playSFX(0x0236);
		else playSFX(0x0237);
		return;
	}

	switch(fightskill)
	{
		case skArchery:
			playSFX(0x0234);
			break;
		case skFencing:
		case skSwordsmanship:
			if (a==0 || a==1) playSFX(0x023B);
			else playSFX(0x023C);
			break;
		case skMacefighting:
			if (a==0 || a==1) playSFX(0x0232);
			else if (a==2) playSFX(0x0139);
			else playSFX(0x0233);
			break;
		case skWrestling:
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
	pWeapon weapon = getBody()->getWeapon();
	int m = RandomNum(0,3);

	if ( ! weapon ) // fist fighting
	{
		switch (m)
		{
			case 0:		playAction(0x0A);	return; //fist straight-punch
			case 1:		playAction(0x09);	return; //fist top-down
			default:	playAction(0x1F);	return; //default: fist over-head
		}
		return;
	}
	
	if (weapon->isBow()) {
		playAction(0x12); //bow
		return;
	} else if (weapon->isCrossbow() || weapon->isHeavyCrossbow()) {
		playAction(0x13); //crossbow - regular
		return;
	} else if (weapon->isSword()) {
		switch (m) //swords
		{
			case 0:		playAction(0x0D);	return; //side swing
			case 1:		playAction(0x0A);	return; //poke
			default:	playAction(0x09);	return; //top-down swing
		}
	} else if (weapon->isMace1H()) {
		switch (m) //maces
		{
			case 0:		playAction(0x0D);	return;	//side swing
			default:	playAction(0x09);	return; //top-down swing
		}
	} else if (weapon->isMace2H() || weapon->isAxe() || weapon->isSpecialMace()) {
		switch (m)
		{
			case 0:		playAction(0x0D);	return; //2H top-down
			case 1:		playAction(0x0C);	return; //2H swing
			default:	playAction(0x0D);	return; //2H top-down
		}
	} else if (weapon->isFencing1H())	{
		switch (m) //fencing
		{
			case 0:		playAction(0x09);	return; //top-down
			case 1:		playAction(0x0D);	return; //side-swipe
			default:	playAction(0x0A);	return; //default: poke
		}
	} else if (weapon->isFencing2H()) { 	//pitchfork & spear
		switch (m) //pitchfork
		{
			case 0:		playAction(0x0D);	return; //top-down
			default:	playAction(0x0E);	return; //default: 2-handed poke
		}
	}
}

/*!
\author Luxor
\brief Does an animation for a char fighting on horse
*/
void cChar::combatOnHorse()
{
	pWeapon weapon = getWeapon();
	if ( ! weapon )
	{
		playAction(0x1A); //fist fighting
		return;
	}
	
	if (weapon->isBow()) {
		playAction(0x1B);
		return;
	} else if (weapon->isCrossbow() || weapon->isHeavyCrossbow()) {
		playAction(0x1C);
		return;
	} else if(  weapon->isSword() || weapon->isSpecialMace() || weapon->isMaceType() ||
		(weapon->getId() ==0x0FB4 || weapon->getId() ==0x0FB5) || weapon->isFencing1H() ) {
		playAction(0x1A);
		return;
	} else if ( weapon->isAxe() || weapon->isFencing2H() ) {
		playAction(0x1D); //2Handed
		return;
	}
}


/*** Xan : this function is critical, and *SHOULD* be used everytime
 *** an attack request is made, not only for dblclicks in war mode
 ***/

void cChar::attackStuff(pChar victim)
{
	if ( ! victim ) return;

	if( this == victim )
		return;

	if ( events[evtChrOnBeginAttack] ) {
		tVariantVector params = tVariantVector(2);
		params[0] = getSerial(); params[1] = victim->getSerial();
		events[evtChrOnBeginAttack]->setParams(params);
		events[evtChrOnBeginAttack]->execute();
		if ( events[evtChrOnBeginAttack]->isBypassed() )
			return;
	}
	
	evt = victim->getEvent(cChar::evtChrOnBeginDefense);
	if ( evt ) {
		tVariantVector params = tVariantVector(2);
		params[0] = victim->getSerial(); params[1] = getSerial();
		evt->setParams(params);
		evt->execute();
		if( evt->isBypassed() )
			return;
	}

	targserial=victim->getSerial();
	unHide();
	disturbMed();

	if( victim->dead || victim->hp <= 0 )//AntiChrist
	{
		sysmsg("That person is already dead!");
		return;
	}

	if ( victim->npcaitype==NPCAI_PLAYERVENDOR)
	{
		sysmsg("%s cannot be harmed.", victim->getCurrentName().c_str() );
		return;
	}

	nPackets::Sent::AttackAck pk(victim);
	s->sendPacket(&pk);

	if (!( victim->targserial== INVALID))
	{
		victim->attackerserial=getSerial();
		victim->ResetAttackFirst();
	}
	SetAttackFirst();
	attackerserial=victim->getSerial();

        //!\todo once set are done revise this
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

	if ((region[ victim->region].priv & rgnFlagGuarded) && (SrvParms->guardsactive))
	{
		if (isGrey())
			setGrey();

		if (victim->npc==0 && victim->isInnocent() && (!victim->isGrey()) && Guilds->Compare( pc, victim )==0) //REPSYS
		{
			criminal( pc );
			if ( nSettings::Server::hasInstantGuards() )
				npcs::SpawnGuard(pc, victim, getPosition() );
		}
		else if( victim->npc && victim->isInnocent() && !victim->HasHumanBody() && victim->npcaitype!=NPCAI_TELEPORTGUARD )
		{
			criminal( pc );
			if ( nSettings::Server::hasInstantGuards() )
				npcs::SpawnGuard(pc, victim, getPosition() );
		}
		else if( victim->npc && victim->isInnocent() && victim->HasHumanBody() && victim->npcaitype!=NPCAI_TELEPORTGUARD )
		{
			victim->talkAll("Help! Guards! I've been attacked!", true);
			criminal( victim );
			callguards(victim); // Sparhawk must check if npcs can call guards
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
		if ( ! victim->isInnocent() )
			return;
		
		if ( victim->isGrey())
			attacker->SetGrey();
		if (!victim->npc && (!victim->isGrey()) && Guilds->Compare(pc, victim )==0)
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
				victim->talkAll("Help! Guards! Tis a murder being commited!", true);
			}
		}
	}
}
