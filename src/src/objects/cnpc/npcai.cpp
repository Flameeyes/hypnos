/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief cNPC's AI's methods
*/

/*!
\author Luxor & Flameeyes
\param target Target of the spell
\param spell Which spell the npc will cast
*/
void cNPC::beginCasting(pChar target, magic::SpellId spell)
{
	if ( ! target ) return;
	
	safedelete( spellTL );
	spellTL = new sTarget( target );
	spelltype = magic::CASTINGTYPE_NPC;
	spell = spell;
	pc->setCasting(true);
	nextact = 1;
	spellaction = 6;
	spelltime = magic::getCastingTime( spell );
	addTempfx( *pc, tempfx::tmpfxSpellParalyze, 0, 0, 0, ( pc->spelltime - getclock() ) / SECS );
	emoteall("*Begins casting a spell*", false);
}

/*!
\author Luxor
*/
void cNPC::checkAI()
{
	if ( !TIMEOUT( nextAiCheck ) )
		return;

	nextAiCheck = (uint32_t) ((double)getclock() + (speed.npcaitime*SECS));

//!\todo Need to be rewrote after arrays are implemented in cVariant
	if( amxevents[ EVENT_CHR_ONCHECKNPCAI ]!=NULL ) {

		NxwCharWrapper sc;
		sc.fillCharsNearXYZ( getPosition(), VISRANGE, true, false );
		uint32_t set = amxSet::create();
		amxSet::copy( set, sc );

		g_bByPass = false;
		amxevents[ EVENT_CHR_ONCHECKNPCAI ]->Call( getSerial(), set, getclock() );

		amxSet::deleteSet( set );

		if (g_bByPass==true)
			return;
	}

	switch(npcaitype)
	{
		case NPCAI_GOOD:
			if( war )
				return;

			if( npcWander == cNPC::WANDER_FLEE )
				return;
				
			if ( ! shopkeeper )
				break;

			NxwCharWrapper sc;
			sc.fillCharsNearXYZ( getPosition(), 3, true, true );
			for( sc.rewind(); !sc.isEmpty(); sc++ ) {
				pChar pj=sc.getChar();
				if (pj == this) continue; //Luxor

				if( pj->dead )
					continue;

				if( pj->hidden )
					continue;
				// Stop talking npcs to each other
				if( pj->IsInnocent() && !pj->npc )
					talkAll("Hello %s, Welcome to my shop, How may i help thee?.", true, pj->getCurrentName().c_str());
			}
			break;
		case NPCAI_HEALER: // good healers
		{
			if ( war )
				return;

			NxwCharWrapper sc;
			sc.fillCharsNearXYZ( getPosition(), 3, true, true );

			for( sc.rewind(); !sc.isEmpty(); sc++ ) {

				pChar pj=sc.getChar();

				if ( !pj || !pj->dead )
			  		continue;

				if (pj == this) continue; //Luxor
			  	if ( !pj->IsInnocent() || pj->IsCriminal() || pj->IsMurderer())
			  	{
			  		if (pj->IsMurderer())
			  			talkAll("I will nay give life to a scoundrel like thee!", 1);
			  		else if (pj->IsCriminal())
						talkAll("I will nay give life to thee for thou art a criminal!", 1);

			  		continue;
			  	}

				playAction(0x10);
				pj->resurrect();
				staticFX(pj, 0x376A, 1, 0, NULL);
				switch(RandomNum(0, 4))
				{
					case 0: talkAll("Thou art dead, but 'tis within my power to resurrect thee. Live!", 1); break;
					case 1: talkAll("Allow me to resurrect thee ghost.  Thy time of true death has not yet come.", 1); break;
					case 2: talkAll("Perhaps thou shouldst be more careful.	Here, I shall resurrect thee.", 1); break;
					case 3: talkAll("Live again, ghost!	Thy time in this world is not yet done.", 1); break;
					case 4: talkAll("I shall attempt to resurrect thee.", 1); break;
				}

			}
		}
		break;
		case NPCAI_EVIL:
		{
			if ( war )
				return;
			if ( npcWander == cNPC::WANDER_FLEE )
				return;

			if (baseskill[skMagery] > 400)
			{
				if ( chance( 50 ) )
				{
					if (hp < getStrength()/2)
						beginCasting(this, magic::spellGreatHeal);
				}
				else
					if (poisoned > 0)
						beginCasting(this, magic::spellCure);
			}
#ifdef SPAR_NEW_WR_SYSTEM
			pointers::pCharVector *pcv = pointers::getCharsNearLocation( this, VISRANGE, pointers::NPC );
			pointers::pCharVectorIt it( pcv->begin() ), end( pcv->end() );
			pChar	pj = 0;
			pChar	pc_target = NULL;
			int32_t	att_value = 0, curr_value = 0;
			while( it != end )
			{
				pj = (*it);
				if ( 	!(
					pj == this ||
					pj->IsInvul() ||
					pj->hidden > 0 ||
					pj->dead ||
					pj->npcaitype == NPCAI_EVIL ||
					pj->npcaitype == NPCAI_HEALER ||
					( SrvParms->monsters_vs_animals == 0 && ((pj->title.size() == 0) && !pj->IsOnline()) ) ||
					( SrvParms->monsters_vs_animals == 1 && chance( SrvParms->animals_attack_chance ) )
					)
				   )
				{
					if( losFrom( pj ) )
					{
						if ( pc_target != 0 )
						{
                                        		curr_value = distFrom( pj ) + pj->hp/3;
							if ( curr_value < att_value )
								pc_target = pj;
						}
						else
						{
							att_value = curr_value = distFrom( pj ) + pj->hp/3;
							pc_target = pj;
						}
					}
				}
				++it;
			}
			if ( pc_target != NULL )
				fight( pc_target );
#else
			NxwCharWrapper sc;
			sc.fillCharsNearXYZ( getPosition(), VISRANGE, true, false );
			pChar pc_target = NULL;
			int32_t att_value = 0, curr_value = 0;
			for( sc.rewind(); !sc.isEmpty(); sc++ ) {

				pChar pj=sc.getChar();
				if (!pj || pj == this )
					continue;

				if (	pj->IsInvul() ||
						pj->hidden > 0 ||
						pj->dead ||
						pj->npcaitype == NPCAI_EVIL ||
						pj->npcaitype == NPCAI_HEALER ||
						( SrvParms->monsters_vs_animals == 0 && ((pj->title.size() == 0) && !pj->IsOnline()) ) ||
						( SrvParms->monsters_vs_animals == 1 && chance( SrvParms->animals_attack_chance ) )
					)
					continue;

				if ( !losFrom( pj ) )
					continue;

				if ( pc_target != NULL ) {
                                        curr_value = distFrom( pj ) + pj->hp/3;
					if ( curr_value < att_value )
						pc_target = pj;
				} else {
					att_value = curr_value = distFrom( pj ) + pj->hp/3;
					pc_target = pj;
				}
			}

                        if ( pc_target != NULL )
				fight( pc_target );
#endif
		}
		break;
		case NPCAI_EVILHEALER:
		{
			if (war) return;

			NxwCharWrapper sc;
			sc.fillCharsNearXYZ( getPosition(), 3, true, true );

			for( sc.rewind(); !sc.isEmpty(); sc++ ) {
				pChar pj=sc.getChar();
				if ( ! pj || !pj->dead )
					continue;
				if (pj == this) continue; //Luxor
				if ( pj->IsInnocent() ) {
					talkAll("I despise all things good. I shall not give thee another chance!", 1);
					continue;
				}
				playAction(0x10);
				pj->resurrect();
				staticFX(pj, 0x3709, 1, 0, NULL);
				switch (RandomNum(0,4))
				{
					case 0:	talkAll("Fellow minion of Mondain, Live!!", 1); break;
					case 1:	talkAll("Thou has evil flowing through your vains, so I will bring you back to life.", 1); break;
					case 2:	talkAll("If I res thee, promise to raise more hell!.", 1); break;
					case 3:	talkAll("From hell to Britannia, come alive!.", 1); break;
					case 4:	talkAll("Since you are Evil, I will bring you back to consciouness.", 1); break;

				}
			}
		}
		break;

		case NPCAI_BEGGAR:
		{
			if (war) return;

			NxwCharWrapper sc;
			sc.fillCharsNearXYZ( getPosition(), 3, true, true );

			for( sc.rewind(); !sc.isEmpty(); sc++ ) {

				pChar pj=sc.getChar();
				if (!pj || pc == this) continue;

				if ( pj->isDead() || !pj->isInnocent() || pj->hidden > 0)
					continue;

				switch (RandomNum(0,2))
				{
					case 0: talkAll("Could thou spare a few coins?", 1); break;
					case 1: talkAll("Hey buddy can you spare some gold?", 1); break;
					case 2: talkAll("I have a family to feed, think of the children.", 1); break;
					default: break;
				}
			}
		}
		break;

		case NPCAI_ORDERGUARD: break;
		case NPCAI_CHAOSGUARD: break;
		case NPCAI_BANKER: break;
		case NPCAI_TELEPORTGUARD:
		case NPCAI_GUARD : 		// in world guards, they dont teleport out...Ripper
		{
			if (war || !(region[region].priv & rgnFlagGuarded))	// this region is not guarded
				return;
			//
			// Sparhawk	Check wether a guard should continue walking towards the character who called him
			//
			int x, y;
			if ( npcWander == cNPC::WANDER_FOLLOW )
			{
				pChar guardcaller = cSerializable::findCharBySerial( ftargserial );

				if ( ! guardcaller || TIMEOUT( antiguardstimer ) || distFrom( guardcaller ) <= 1 )
				{
					npcWander = oldnpcWander;
					oldnpcWander = cNPC::WANDER_NOMOVE;
					ftargserial = INVALID;
				}
				else
				{
					//
					//	Guard is still walking towards char who called
					//	In that case check whether guard can see targets within visual range of caller
					//
					x = guardcaller->getPosition().x;
					y = guardcaller->getPosition().y;
				}
			}
			else
			{
				x = getPosition().x;
				y = getPosition().y;
			}

			NxwCharWrapper sc;
			sc.fillCharsNearXYZ( getPosition(), VISRANGE, true, false );

			for( sc.rewind(); !sc.isEmpty(); sc++ ) {

				pChar character=sc.getChar();
				if ( character )
				{
					if ( character != this &&
					     !character->isDead() &&
					     !character->isHidden() &&
					     losFrom( character )
					   )
					{
						if ( 	character->npcaitype == NPCAI_EVIL ||
							character->npcaitype == NPCAI_MADNESS ||
							character->IsMurderer() ||
							character->IsCriminal()
						   )
						{
							if ( npcWander == cNPC::WANDER_FOLLOW )
							{
								npcWander = oldnpcWander;
								oldnpcWander = cNPC::WANDER_NOMOVE;
								ftargserial = INVALID;
							}

							talkAll("Thou shalt regret thine actions, swine!", 1);
							fight( character );
							return;
						}
					}
				}
			}
		}
		break;
		case NPCAI_TAMEDDRAGON: // Tamed Dragons ..not white wyrm..Ripper
		// so regular dragons attack reds on sight while tamed.
		{
			if (!tamed) return;
			NxwCharWrapper sc;
			sc.fillCharsNearXYZ( getPosition(), 10, true, false );

			for( sc.rewind(); !sc.isEmpty(); sc++ ) {

				pChar pj=sc.getChar();

				if (pj == this) continue; //Luxor
				if( pj && pj->npc && pj->npcaitype==NPCAI_EVIL)
				{
					npcattacktarget(this, pj);
					return;
				}
			}
		}
		break;
		case NPCAI_ATTACKSRED:
		{
			if (war) return;
			NxwCharWrapper sc;
			sc.fillCharsNearXYZ( getPosition(), 10, true, false );

			for( sc.rewind(); !sc.isEmpty(); sc++ ) {

				pChar pj=sc.getChar();
				if ( !pj || pj == this) continue;
				
				if ( pj->IsInvul() || pj->dead || (pj->npcaitype != NPCAI_EVIL && !pj->IsCriminal() && !pj->IsMurderer())) continue;

				npcattacktarget(this, pj);
			}
		}
		break;
		case NPCAI_PLAYERVENDOR: break;
		case NPCAI_PETGUARD:
		{
			if (!tamed) return;
			NxwCharWrapper sc;
			sc.fillCharsNearXYZ( getPosition(), VISRANGE, true, false );

			for( sc.rewind(); !sc.isEmpty(); sc++ ) {

				pChar pj=sc.getChar();
				if (!(pj)) continue;
				if (pj->npc || pj->dead || pj->guarded == false) continue;
				if (getOwner() == pj) {
					if (pj->IsOnline()) {
						pChar pc_attacker = cSerializable::findCharBySerial(pj->attackerserial);
						if ( ! pc_attacker ) return;
						if (distFrom(pc_attacker) <= 10) {
							npcattacktarget(this, pc_attacker);
							return;
						}
					}
				}
			}
		}
		break;
		case 30: // why is this the same as case 50???..Ripper
		case NPCAI_MADNESS://Energy Vortex/Blade Spirit
		{
			if (war) return;
			if (attackerserial == INVALID) {
				NxwCharWrapper sc;
				sc.fillCharsNearXYZ( getPosition(), 10, true, false );

				for( sc.rewind(); !sc.isEmpty(); sc++ ) {

					pChar pj=sc.getChar();
					if ( !pj->IsInvul() && !pj->dead && ! pj->isHidden() && pj->npcaitype != NPCAI_MADNESS) {
						fight(pj);
						return;
					}
				}
			}
		}
		break;
		// Case 60-70 is Skyfires new AI
		case NPCAI_DRAGON1:
		case NPCAI_DRAGON2: //Dragon AI (Skyfire and araknesh)
		{
			NxwCharWrapper sc;
			sc.fillCharsNearXYZ( getPosition(), VISRANGE, true, false );

			for( sc.rewind(); !sc.isEmpty(); sc++ ) {

				pChar pj=sc.getChar();
				if (!(pj)) continue;
				if (pj->dead ||
						pj->IsInvul() ||
						pj->IsGMorCounselor() ||
						pj->npcaitype == NPCAI_EVIL ||
						pj->npcaitype == NPCAI_HEALER ||
						pj->npcaitype == NPCAI_DRAGON1 ||
						pj->npcaitype == NPCAI_DRAGON2 || pj == this) continue;

				if ( ! pj->isHidden() ) {
					switch(RandomNum(0, 6))
					{
						case 0:
							beginCasting(pj, magic::spellCurse);
							talkAll("You are ridiculous", 1);
							break;
						case 1:
							beginCasting(pj, magic::spellFlameStrike);
							talkAll("Die unusefull mortal!", 1);
							break;
						case 2:
							beginCasting(pj, magic::spellParalyze);
							talkAll("What are you doing? Come here and Die!", 1);
							break;
						case 3:
							beginCasting(pj, magic::spellLightning);
							talkAll("Stupid Mortal I'll crush you as a fly", 1);
							break;
						case 4:
							beginCasting(pj, magic::spellLightning);
							talkAll("Stupid Mortal I'll crush you as a fly", 1);
							break;
						case 5:
							beginCasting(pj, magic::spellExplosion);
							talkAll("Die unusefull mortal!", 1);
							break;
						case 6:
							beginCasting(pj, magic::spellExplosion);
							talkAll("Die unusefull mortal!", 1);
							break;
					}
				}
				if (hp < getStrength()/2) {
					talkAll("In Vas Mani", 1);
					beginCasting(this, magic::spellGreatHeal );
				}
				if (poisoned > 0) {
					talkAll("An Nox", 1);
					beginCasting(this, magic::spellCure );
				}
				if ( pj->isDispellable() ) {
					talkAll("An Ort", 1);
					beginCasting(pj, magic::spellDispel );
				}

				if ( !pj->IsHidden() ) npcattacktarget(this, pj);
				return;
			}
		}
		break;
		default:
			WarnOut("cCharStuff::CheckAI-> Error npc %i ( %08x ) has invalid AI type %i\n", getSerial(), getSerial(), npcaitype);
			return;
	}	//switch(npcaitype)
} // checkAI
