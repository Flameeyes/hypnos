/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "common_libs.h"
#include "tmpeff.h"
#include "sndpkg.h"
#include "inlines.h"
#include "skills/skills.h"
#include "misc.h"

namespace tempfx {

uint32_t_SLIST tempfxCheck;

/*!
\author Luxor
*/
void tempeffectson()
{
	if ( tempfxCheck.empty() )
		return;

	pObject po = NULL;

	uint32_t_SLIST::iterator it( tempfxCheck.begin() );
	for ( ; it != tempfxCheck.end(); ) {
		po = objects.findObject( (*it) );

                if ( po == NULL ) {
			it = tempfxCheck.erase( it );
			continue;
	        }

	        if ( !po->hasTempfx() ) {
			it = tempfxCheck.erase( it );
			continue;
	        }

	        po->tempfxOn();

	        it++;
	}
}

/*!
\author Luxor
*/
void tempeffectsoff()
{
        if ( tempfxCheck.empty() )
		return;

	pObject po = NULL;

	uint32_t_SLIST::iterator it( tempfxCheck.begin() );
        for ( ; it != tempfxCheck.end(); ) {
                po = objects.findObject( (*it) );

                if ( po == NULL ) {
			it = tempfxCheck.erase( it );
			continue;
	        }

	        if ( !po->hasTempfx() ) {
			it = tempfxCheck.erase( it );
			continue;
	        }

	        po->tempfxOff();
	        it++;
	}
}

/*!
\author Luxor
*/
void checktempeffects()
{
        if ( tempfxCheck.empty() )
		return;

	pObject po = NULL;

	uint32_t_SLIST::iterator it( tempfxCheck.begin() );
        for ( ; it != tempfxCheck.end(); ) {
                po = objects.findObject( (*it) );

                if ( po == NULL ) {
			it = tempfxCheck.erase( it );
			continue;
	        }

	        if ( !po->hasTempfx() ) {
			it = tempfxCheck.erase( it );
			continue;
	        }

	        po->checkTempfx();
	        it++;
	}
}

/*!
\author Luxor
\brief Calls a custom tempfx defined in small code
*/
static void callCustomTempFx(pObject poSrc, pObject poDest, int mode, int amxcallback, int more1, int more2, int more3)
{
	if(!poSrc || !poDest) return;

	if (amxcallback <= -2) return;

	int more = (more3<<16) + (more2<<8) + more1;
	AmxFunction::g_prgOverride->CallFn(amxcallback, poSrc->getSerial(), poDest->getSerial(), more, mode);
}

/*!
\author Luxor
*/
bool isSrcRepeatable(int num)
{
	if ( num < 0 || num >= MAX_TEMPFX_INDEX )
		return false;

	switch(num)
	{
		case SPELL_INCOGNITO:
		case SPELL_INVISIBILITY:
		case SPELL_POLYMORPH:
		case DRINK_FINISHED:
			return false;

		default:
			return true;
	}

	return false;
}

/*!
\author Luxor
*/
bool isDestRepeatable(int num)
{
	if ( num < 0 || num >= MAX_TEMPFX_INDEX )
		return false;

	switch(num)
	{
		case spellClumsy:
		case spellFeebleMind:
		case spellWeaken:
		case spellAgility:
		case spellStrenght:
		case spellCunning:
		case SPELL_BLESS:
		case SPELL_CURSE:
		case SPELL_INCOGNITO:
		case SPELL_POLYMORPH:
		case SPELL_INVISIBILITY:
		case CRIMINAL:
		case FIELD_DAMAGE:
			return false;

		default:
			return true;
	}

	return false;
}


/*!
\author Luxor
*/
int32_t getTempFxTime(pChar src, int num, int more1, int more2, int more3)
{
	int dur = 0;

	if ( num < 0 || num >= MAX_TEMPFX_INDEX )
		return 0;

	switch (num)
	{
		case FIELD_DAMAGE:
			dur = 2;
			break;
		case SPELL_PARALYZE:
			if(!src) return 0;
			dur = src->skill[skMagery]/100;
			break;

		case SPELL_LIGHT:
			if(!src) return 0;
			dur = src->skill[skMagery]*10;
			break;

		case spellClumsy:
		case spellFeebleMind:
		case spellWeaken:
		case spellAgility:
		case spellStrenght:
		case spellCunning:
		case SPELL_BLESS:
		case SPELL_CURSE:
		case SPELL_REACTARMOR:
		case spellProtection:
			if(!src) return 0;
			dur = src->skill[skMagery]/10;
			break;

		case SPELL_POLYMORPH:
			dur = polyduration;
			break;

		case SPELL_INCOGNITO:
			dur = 90;
			break;

		case SPELL_INVISIBILITY:
			dur = 90;
			break;

		case ALCHEMY_GRIND:
			dur = more2;
			break;

		case ALCHEMY_END:
			dur = 12;
			break;

		case AUTODOOR:
			dur = 10;
			break;

		case TRAINDUMMY:
			dur = 5;
			break;

		case EXPLOTIONMSG:
			dur = more2;
			break;

		case EXPLOTIONEXP:
			dur = 4;
			break;

		case LSD:
			dur = 90;
			break;

		case HEALING_HEAL:
			dur = 0;
			break;

		case HEALING_CURE:
			dur = 0;
			break;

		case HEALING_RESURRECT:
			dur = 0;
			break;

		case POTION_DELAY:
			dur = 0;
			break;

		case GM_HIDING:
			dur = more1;
			break;

		case GM_UNHIDING:
			dur = more1;
			break;

		case HEALING_DELAYHEAL:
			dur = more3;
			break;

		case COMBAT_PARALYZE:
			dur = 5;
			break;

		case COMBAT_CONCUSSION:
			dur = 30;
			break;

		case AMXCUSTOM:
			dur = 0;
			break; //Luxor's note: AMXCUSTOM will never get into this function :P

		case GREY:
			dur = 240;
			break;

		case CRIMINAL:
			dur = repsys.crimtime;
			break;

		case DRINK_FINISHED:
			dur = 3;
			break;

		default:
			dur = 0;
			break;
	}

	return dur;
}

/*!
\author Luxor
*/
void cTempfx::start()
{
	pChar src = cSerializable::findCharBySerial(m_nSrc);
	pChar dest = cSerializable::findCharBySerial(m_nDest);

	if ( !dest )
		return;

	switch(m_nNum)
	{
		case FIELD_DAMAGE:
			dest->damage(m_nMore1, static_cast<DamageType>(m_nMore2));
			dest->playSFX(0x0208);
			break;
		case SPELL_PARALYZE:
			if (dest->resistFreeze())
				return;
			dest->freeze();
			break;

		case SPELL_LIGHT:
			dest->fixedlight = worldbrightlevel;
			if (dest->getClient())
				dolight(dest->getClient()->toInt(), worldbrightlevel);
			break;

		case spellClumsy:
			if (dest->dx < m_nMore1)
				m_nMore1 = dest->dx;
			dest->dx -= m_nMore1;
			dest->stm = min (dest->dx, dest->stm);
			if (dest->getClient())
                		client->statusWindow(dest,true);  //!< \todo check second argument
			break;

		case spellFeebleMind:
			if (dest->in < m_nMore1)
				m_nMore1 = dest->in;
			dest->in -= m_nMore1;
			if (dest->getClient())
                		client->statusWindow(dest,true);  //!< \todo check second argument
			break;

		case spellWeaken:
			if (dest->getStrength() < m_nMore1)
				m_nMore1 = dest->getStrength();
			dest->modifyStrength(-m_nMore1);
			if (dest->getClient())
                		client->statusWindow(dest,true);  //!< \todo check second argument
			break;

		case spellAgility:
			dest->dx += m_nMore1;
			dest->stm += m_nMore1;
			if (dest->getClient())
                		client->statusWindow(dest,true);  //!< \todo check second argument
			break;

		case spellStrenght:
			dest->modifyStrength(m_nMore1);
			dest->hp += m_nMore1;
			if (dest->getClient())
                		client->statusWindow(dest,true);  //!< \todo check second argument
			break;

		case spellCunning:
			dest->in += m_nMore1;
			dest->mn += m_nMore1;
			if (dest->getClient())
                		client->statusWindow(dest,true);  //!< \todo check second argument
			break;

		case SPELL_BLESS:
			dest->modifyStrength(m_nMore1);
			dest->dx += m_nMore2;
			dest->in += m_nMore3;

			dest->hp += m_nMore1;
			dest->stm += m_nMore2;
			dest->mn += m_nMore3;
			if (dest->getClient())
                		client->statusWindow(dest,true);  //!< \todo check second argument
			break;

		case SPELL_CURSE:
			if (dest->getStrength() < m_nMore1)
				m_nMore1 = dest->getStrength();
			if (dest->dx < m_nMore2)
				m_nMore2 = dest->dx;
			if (dest->in < m_nMore3)
				m_nMore3 = dest->in;
			dest->modifyStrength(-m_nMore1);
			dest->dx -= m_nMore2;
			dest->in -= m_nMore3;
			if (dest->getClient())
                		client->statusWindow(dest,true);  //!< \todo check second argument
			break;

		case SPELL_INVISIBILITY:
			dest->setHidden(htBySpell);
			dest->morph(0);
			break;

		case ALCHEMY_GRIND:
			break;

		case ALCHEMY_END:
			break;

		case AUTODOOR:
			break;

		case TRAINDUMMY:
			break;

		case SPELL_REACTARMOR:
			dest->ra = 1;
			break;

		case EXPLOTIONMSG:
			break;

		case EXPLOTIONEXP:
			break;

		case SPELL_POLYMORPH:
			if (dest->morphed)
				dest->morph();  //if the char is morphed, unmorph him
			dest->setCrimGrey(nSettings::Reputation::getPolymorphAction());
			dest->morph( m_nMore1, -1, -1, -1, -1, -1, NULL, true );
			dest->polymorph = true;
			break;

		case SPELL_INCOGNITO:
			//Luxor's incognito code :)
			if (dest->morphed)
				dest->morph();	//if the char is morphed, unmorph him
			dest->setIncognito(true);
			uint16_t body, skincolor, hairstyle, haircolor, beardstyle, beardcolor, x;

			//--Sex--
			body = change(50) ? bodyMale : bodyFemale;
			//--Skin color--
			x=rand()%6;
			switch(x)
			{
				case 0:	skincolor = 0x83EA; break;
				case 1: skincolor = 0x8405; break;
				case 2:	skincolor = 0x83EF; break;
				case 3: skincolor = 0x83F5; break;
				case 4:	skincolor = 0x841C; break;
				case 5:	skincolor = 0x83FB; break;
				default: skincolor = 0x83FB;break;
			}
			//--Hair Style--
			x=rand()%10;
			switch(x)
			{
				case 0:	hairstyle = 0x203B; break;
				case 1: hairstyle = 0x203C; break;
				case 2:	hairstyle = 0x203D; break;
				case 3: hairstyle = 0x2044; break;
				case 4:	hairstyle = 0x2045; break;
				case 5:	hairstyle = 0x2046; break;
				case 6:	hairstyle = 0x2047; break;
				case 7:	hairstyle = 0x2048; break;
				case 8:	hairstyle = 0x2049; break;
				case 9:	hairstyle = 0x204A; break;
				default: hairstyle = 0x204A;break;
			}
			//--Beard Style--
			if (body == bodyMale)
			{
				x=rand()%7;
				switch(x)
				{
					case 0:	beardstyle = 0x203E; break;
					case 1: beardstyle = 0x203F; break;
					case 2:	beardstyle = 0x2040; break;
					case 3: beardstyle = 0x2041; break;
					case 4:	beardstyle = 0x204B; break;
					case 5:	beardstyle = 0x204C; break;
					case 6:	beardstyle = 0x204D; break;
					default: beardstyle = 0x204D;break;
				}
			}
			//--Hair color--
			x=rand()%6;
			switch(x)
			{
				case 0:	haircolor = 0x83EA; break;
				case 1: haircolor = 0x8405; break;
				case 2:	haircolor = 0x83EF; break;
				case 3: haircolor = 0x83F5; break;
				case 4:	haircolor = 0x841C; break;
				case 5:	haircolor = 0x83FB; break;
				default: haircolor = 0x83FB;break;
			}
			//--Beard color--
			x=rand()%6;
			switch(x)
			{
				case 0:	beardcolor = 0x83EA; break;
				case 1: beardcolor = 0x8405; break;
				case 2:	beardcolor = 0x83EF; break;
				case 3: beardcolor = 0x83F5; break;
				case 4:	beardcolor = 0x841C; break;
				case 5:	beardcolor = 0x83FB; break;
				default: beardcolor = 0x83FB;break;
			}
			
			{
				std::string newname;
				if( body == bodyMale )
				{
					std::string value("1");
					newname = cObject::getRandomScriptValue( std::string("RANDOMNAME"), value );
				}
				else
				{
					std::string value("2");
					newname = cObject::getRandomScriptValue( std::string("RANDOMNAME"), value );
				}
				dest->morph(body, skincolor, hairstyle, haircolor, beardstyle, beardcolor, newname.c_str(), true);
			}
			break;

		case LSD:
			/*if (dest->getClient() == NULL) return;
			index = dest->getClient()->toInt();
			dest->sysmsg("Hmmm, tasty, LSD");
			clientInfo[index]->lsd = true;
			dest->hp = dest->st;
			dest->mn = dest->in;
			dest->getClient()->sendchar(dest, false);*/
			break;

		case spellProtection:
			dest->nxwflags[0] |= cChar::flagSpellProtection;
			break;

		case GM_HIDING:
			break;

		case GM_UNHIDING:
			break;

		case HEALING_DELAYHEAL:
			if ( src ) {
				if (src->war)
					src->sysmsg("You cannot heal while you are in a fight.");
				if (!m_nMore2)
					src->sysmsg("You start healing...");
				else
					src->sysmsg("You continue to heal...");
			}
			break;

		case AMXCUSTOM:
			callCustomTempFx(src, dest, MODE_START, m_nAmxcback, m_nMore1, m_nMore2, m_nMore3);
			break;

		case GREY:
			dest->nxwflags[0] |= cChar::flagGrey;
			break;

		case COMBAT_PARALYZE:
			dest->freeze();
			break;

		case COMBAT_CONCUSSION:
			if (dest->in < m_nMore1)
				m_nMore1 = dest->in;
			dest->in -= m_nMore1;
			if (dest->getClient())
                		client->statusWindow(dest,true);  //!< \todo check second argument
			break;

		case CRIMINAL:
			dest->SetCriminal();
			dest->sysmsg( "You are now a criminal!");
			break;

		case SPELL_TELEKINESYS:
			dest->nxwflags[0] |= cChar::flagSpellTelekinesys;
			break;

		default:
			break;
	}

	//if (dest) item::CheckEquipment(dest);
}

/*!
\author Luxor
*/
int8_t cTempfx::checkForExpire()
{
	if ( !TIMEOUT(m_nExpireTime) )
		return 0;

	executeExpireCode();

        pObject po = objects.findObject( m_nDest );
	if ( !po )
		return INVALID;

	return 1;
}

/*!
\author Luxor
*/
void cTempfx::executeExpireCode()
{
	pChar src = cSerializable::findCharBySerial(m_nSrc);
	pChar dest = cSerializable::findCharBySerial(m_nDest);
	pItem pi_dest = cSerializable::findItemBySerial(m_nDest);

	switch(m_nNum)
	{
		case SPELL_PARALYZE:
			if ( ! dest ) return;
			if (dest->isFrozen())
				dest->unfreeze( true );
			break;

		case SPELL_LIGHT:
			if ( ! dest ) return;
			dest->fixedlight = 0xFF;
			if (dest->getClient())
				dolight(dest->getClient()->toInt(), worldbrightlevel);
			break;

		case spellClumsy:
			if ( ! dest ) return;
			dest->dx += m_nMore1;
			if (dest->getClient())
                		client->statusWindow(dest,true);  //!< \todo check second argument
			break;

		case spellFeebleMind:
			if ( ! dest ) return;
			dest->in += m_nMore1;
			if (dest->getClient())
                		client->statusWindow(dest,true);  //!< \todo check second argument
			break;

		case spellWeaken:
			if ( ! dest ) return;
			dest->modifyStrength(m_nMore1);
			if (dest->getClient())
                		client->statusWindow(dest,true);  //!< \todo check second argument
			break;

		case spellAgility:
			if ( ! dest ) return;
			dest->dx -= m_nMore1;
			dest->stm = min(dest->stm, dest->dx);
			if (dest->getClient())
                		client->statusWindow(dest,true);  //!< \todo check second argument
			break;

		case spellStrenght:
			if ( ! dest ) return;
			dest->modifyStrength(-m_nMore1);
			dest->hp = min(dest->hp, (int32_t)dest->getStrength());
			if (dest->getClient())
                		client->statusWindow(dest,true);  //!< \todo check second argument
			break;

		case spellCunning:
			if ( ! dest ) return;
			dest->in -= m_nMore1;
			dest->mn = min(dest->mn, dest->in);
			if (dest->getClient())
                		client->statusWindow(dest,true);  //!< \todo check second argument
			break;

		case SPELL_BLESS:
			if ( ! dest ) return;
			dest->modifyStrength(-m_nMore1);
			dest->dx -= m_nMore2;
			dest->in -= m_nMore3;
			dest->hp = min(dest->hp, (int32_t)dest->getStrength());
			dest->stm = min(dest->stm, dest->dx);
			dest->mn = min(dest->mn, dest->in);
			if (dest->getClient())
                		client->statusWindow(dest,true);  //!< \todo check second argument
			break;

		case SPELL_CURSE:
			if ( ! dest ) return;
			dest->modifyStrength(m_nMore1);
			dest->dx += m_nMore2;
			dest->in += m_nMore3;
			if (dest->getClient())
                		client->statusWindow(dest,true);  //!< \todo check second argument
			break;

		case SPELL_INVISIBILITY:
			if ( ! dest ) return;
			if (dest->isHiddenBySpell()) {
				dest->setHidden(htUnhidden);
				dest->morph();
				dest->playSFX(0x203);
			}
			break;


		case ALCHEMY_GRIND:
			if ( ! dest ) return;
			if (m_nMore1 == 0)
			{
				if (m_nMore2 != 0)
					dest->emoteall("*%s continues grinding.*", 1, dest->getCurrentName().c_str());

				dest->playSFX(0x242);
			}
			break;


		case ALCHEMY_END:
			if ( !src || !pi_dest ) return;

			Skills::CreatePotion(src, m_nMore1, m_nMore2, pi_dest);
			break;

		case AUTODOOR:
			if(!pi_dest) return;

			if (pi_dest->dooropen == 0)
				break;
			pi_dest->dooropen = 0;
			dooruse(INVALID, pi_dest);
			break;

		case TRAINDUMMY:
			if(!pi_dest) return;

			if (pi_dest->getId() == 0x1071)
			{
				pi_dest->setId(0x1070);
				pi_dest->gatetime = 0;
				pi_dest->Refresh();
			}
			else if (pi_dest->getId()==0x1075)
			{
				pi_dest->setId(0x1074);
				pi_dest->gatetime = 0;
				pi_dest->Refresh();
			}
			break;

		case SPELL_REACTARMOR:
			if (!dest) return;
			dest->ra = 0;
			break;

		case EXPLOTIONMSG:
			if (!dest) return;
			dest->sysmsg("%i", m_nMore3);
			break;

		case EXPLOTIONEXP:
			if ( !src || !pi_dest ) return;

			if (src->getClient())
				pi_dest->explode(src->getClient()->toInt());
			break;

		case SPELL_POLYMORPH:
			if (!dest) return;
			dest->morph();
			dest->polymorph = false;
			break;

		case SPELL_INCOGNITO:
			if ( ! dest ) return;
			dest->morph();
			dest->setIncognito(false);
			break;

		case LSD:
			/*if (dest->getClient() == NULL) return;
			index = dest->getClient()->toInt();
			clientInfo[index]->lsd = false;
			dest->sysmsg("LSD has worn off");
			dest->stm = 3;
			dest->mn = 3;
			dest->hp /= 7;
			dest->getClient()->sendchar(dest, false);
			all_items(index);
			pChar p_nearchar;
			forEachCharNearby(dest->x, dest->y, 15, p_nearchar) {
				if ((p_nearchar!=NULL)) {
					p_chearchar->teleport();
				}
			} nextIndex(p_nearchar);*/
			break;

		case spellProtection:
			if ( ! dest ) return;
			dest->nxwflags[0] &= ~cChar::flagSpellProtection;
			break;

		case DRINK_EMOTE:
			if (!src) return;
			src->emote(src->getSocket(),"*glu*",1);
			break;

		case DRINK_FINISHED:
			if ( !src || !pi_dest ) return;

			usepotion(src, pi_dest);
			break;

		case GM_HIDING:
			if (!dest) return;
			dest->sysmsg("You have hidden yourself well.");
			dest->setHidden(htBySkill);
			dest->teleport( teleNone );
			break;

		case GM_UNHIDING:
			if (!dest) return;
			dest->unHide();
			dest->sysmsg("You are now visible.");
			break;

		case HEALING_DELAYHEAL:
			if ( !src || !dest ) return;

			if (src->war) {
				src->sysmsg("You cannot heal while you are in a fight.");
				return;
			}
			dest->hp = min(dest->hp + m_nMore1, (int32_t)dest->getStrength());
			dest->sysmsg("After receiving some healing, you feel better.");
			dest->updateHp();
			if (!m_nMore2)
				add(src, dest, m_nNum, m_nMore1 +1, 1, m_nMore3);
			break;

		case AMXCUSTOM:
			if ( !src || !dest ) return;

			callCustomTempFx(src, dest, MODE_END, m_nAmxcback, m_nMore1, m_nMore2, m_nMore3);
			break;

		case GREY:
			if (!dest) return;
			dest->nxwflags[0] &= ~cChar::flagGrey;
			break;

		case CRIMINAL:
			if(!dest) return;
			dest->SetInnocent();
			dest->sysmsg("You are no longer a criminal.");
			break;

		case SPELL_TELEKINESYS:
			if(!dest) return;
			dest->nxwflags[0] &= ~cChar::flagSpellTelekinesys;
			break;

		default:
			break;
	}



	if (dest)
		dest->checkEquipment();
}


/*!
\author Luxor
*/
void cTempfx::activate()
{
	pChar src = cSerializable::findCharBySerial(m_nSrc);
	pChar dest = cSerializable::findCharBySerial(m_nDest);

	if ( !dest ) return;

	switch(m_nNum)
	{
		case SPELL_PARALYZE:
			dest->freeze();
			break;

		case SPELL_LIGHT:
			dest->fixedlight = worldbrightlevel;
			break;

		case spellClumsy:
			dest->dx -= m_nMore1;
			break;

		case spellFeebleMind:
			dest->in -= m_nMore1;
			break;

		case spellWeaken:
			dest->modifyStrength(-m_nMore1);
			break;

		case spellAgility:
			dest->dx += m_nMore1;
			break;

		case spellStrenght:
			dest->modifyStrength(m_nMore1);
			break;

		case spellCunning:
			dest->in += m_nMore1;
			break;

		case SPELL_BLESS:
			dest->modifyStrength(m_nMore1);
			dest->dx += m_nMore2;
			dest->in += m_nMore3;
			break;

		case SPELL_CURSE:
			dest->modifyStrength(-m_nMore1);
			dest->dx -= m_nMore2;
			dest->in -= m_nMore3;
			break;

		case SPELL_INVISIBILITY:
                        break;

		case AMXCUSTOM:
			callCustomTempFx(src, dest, MODE_ON, m_nAmxcback, m_nMore1, m_nMore2, m_nMore3);
			break;

		case GREY:
			dest->nxwflags[0] |= NCF0_GREY;
			break;

		case CRIMINAL:
			dest->SetCriminal();
			break;

		case SPELL_TELEKINESYS:
			dest->nxwflags[0] |= NCF0_TELEKINESYS;
			break;

		default:
			break;
	}


}

/*!
\author Luxor
*/
void cTempfx::deactivate()
{
	pChar src = cSerializable::findCharBySerial(m_nSrc);
	pChar dest = cSerializable::findCharBySerial(m_nDest);

	if ( !dest )
		return;

	switch(m_nNum)
	{
		case SPELL_PARALYZE:
			if (dest->isFrozen())
				dest->unfreeze( true );
			break;

		case SPELL_LIGHT:
			dest->fixedlight = 0xFF;
			break;

		case spellClumsy:
			dest->dx += m_nMore1;
			break;

		case spellFeebleMind:
			dest->in += m_nMore1;
			break;

		case spellWeaken:
			dest->modifyStrength(m_nMore1);
			break;

		case spellAgility:
			dest->dx -= m_nMore1;
			break;

		case spellStrenght:
			dest->modifyStrength(-m_nMore1);
			break;

		case spellCunning:
			dest->in -= m_nMore1;
			break;

		case SPELL_BLESS:
			dest->modifyStrength(-m_nMore1);
			dest->dx -= m_nMore2;
			dest->in -= m_nMore3;
			break;

		case SPELL_CURSE:
			dest->modifyStrength(m_nMore1);
			dest->dx += m_nMore2;
			dest->in += m_nMore3;
			break;

		case SPELL_INVISIBILITY:
			break;

		case AMXCUSTOM:
			callCustomTempFx(src, dest, MODE_OFF, m_nAmxcback, m_nMore1, m_nMore2, m_nMore3);
			break;

		case GREY:
			dest->nxwflags[0] &= ~NCF0_GREY;
			break;

		case CRIMINAL:
			dest->SetInnocent();
			break;
		case SPELL_TELEKINESYS:
			dest->nxwflags[0] &= ~NCF0_TELEKINESYS;
			break;

		default:
			break;
	}


}


/*!
\author Luxor
\brief Tells if a tempfx is valid
*/
bool cTempfx::isValid()
{
	if ( m_nNum < 0 || m_nNum >= MAX_TEMPFX_INDEX )
		return false;

	if ( m_nNum == AMXCUSTOM && m_nAmxcback <= INVALID )
		return false;

	pObject src = objects.findObject(m_nSrc);
	pObject dest = objects.findObject(m_nDest);

	if ( !src || !dest )
		return false;

	return true;
}

/*!
\author Luxor
\brief cTempfx constructor
*/
cTempfx::cTempfx( uint32_t nSrc, SERIAL nDest, int32_t num, int32_t dur, int32_t more1, int32_t more2, int32_t more3, int32_t amxcback )
{
	m_nSrc = INVALID;
	m_nDest = INVALID;
	m_nNum = INVALID;
	m_nMode = INVALID;
	m_nExpireTime = 0;
	m_nAmxcback = INVALID;
	m_nMore1 = INVALID;
	m_nMore2 = INVALID;
	m_nMore3 = INVALID;
	m_bDispellable = false;
	m_bSrcRepeatable = isSrcRepeatable( num );
	m_bDestRepeatable = isDestRepeatable( num );

	//
	//	Set serials
	//
	if ( cSerializable::isCharSerial(nSrc) ) {
		if ( !cSerializable::findCharBySerial(nSrc) )
			return;
	}

	if ( cSerializable::isItemSerial(nSrc) ) {
		if ( !cSerializable::findItemBySerial(nSrc) )
			return;
	}

	if ( cSerializable::isCharSerial(nDest) ) {
		if ( !cSerializable::findCharBySerial(nDest) )
			return;
	}

	if ( cSerializable::isItemSerial(nDest) ) {
		if ( !cSerializable::findItemBySerial(nDest) )
			return;
	}

	m_nSrc = nSrc;
	m_nDest = nDest;

	if ( num < 0 || num >= MAX_TEMPFX_INDEX )
		return;

	m_nNum = num;

	//
	//	If a duration is given, use it. Otherwise, use the standard value.
	//
	if ( dur > 0 )
		m_nExpireTime = getclock() + (dur*SECS);
	else
		m_nExpireTime = getclock() + (getTempFxTime(cSerializable::findCharBySerial(m_nSrc), num, more1, more2, more3)*SECS);

	if ( m_nNum == AMXCUSTOM && amxcback <= INVALID )
		return;

	m_nMore1 = more1;
	m_nMore2 = more2;
	m_nMore3 = more3;
	m_nAmxcback = amxcback;
}

/*!
\author Luxor
\brief	Adds a temp effect
*/
bool add(pObject src, pObject dest, int num, unsigned char more1, unsigned char more2, unsigned char more3, short dur, int amxcback)
{
	if( !src || !dest ) return false;

	return dest->addTempfx( *src, num, (int)more1, (int)more2, (int)more3, (int)dur, amxcback );
}

/*!
\author Luxor
*/
void addTempfxCheck( uint32_t serial )
{
	pObject po = objects.findObject( serial );
	if(!po) return;

	if ( find( tempfxCheck.begin(), tempfxCheck.end(), serial ) != tempfxCheck.end() )
		return;

	tempfxCheck.push_front( serial );
}

} //namespace



