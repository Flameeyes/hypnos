/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Implementation of cBody class
*/

#include "objects/cbody.hpp"

cBody::cBody()
{
	memset( layers, 0, sizeof(layers) );
	memset( skills, 0, sizeof(skills) );
	mounting = NULL;
}

/*!
\brief Gets the skill sum
\return the sum of the skills, including the deciaml digit
*/
uint32_t cBody::getSkillSum()
{
	uint32_t sum = 0;
	for ( register int i = 0; i < 49; i++)
		sum += skills[i];

	return sum;
}

/*!
\author Flameeyes
\brief Equip an item
\retval 0 The item is equipped correctly
\retval 1 The item is not equipped because the layer is already used
\retval 2 The event handler canceled the equip
\param pi item to equip
\param drag true if called in wear_item
\todo Change event handling
\todo Move the '#if 0'ed part to cEquippable after rewriting mul reading stuff
*/
const uint8_t cBody::equip(pEquippable pi, bool drag)
{
	cVariantVector params(2);
	param[0] = pi;
	param[1] = getChar();
	
	if ( !pi->handleEvent( eventItemOnEquip, 2, params ) )
		return 2;

	// AntiChrist -- for poisoned items
	if(pi->getPoison())
	{
		if(poison < pi->getPoison())
			poison += pi->getPoison();
	}

	if ( client )
		client->statusWindow(this, true);

	if (drag)
		return 0;

	Layer lay = tiledataStatic->getQuality(pi->getId());
	if (	layers[lay] ||
		lay == layWeapon2H && (layers[layWeapon1H] || layers[layWeapon2H]) ||
		lay == layWeapon1H && layers[layWeapon1H]
	   )
	   	return 1;
	
	pi->setLayer(lay);

	pi->setContainer(this);

	checkSafeStats();
	teleport( teleSendWornItems );

	return 0;
}

/*!
\author Flameeyes
\brief Unequip an item
\return 0 if item unequipped, 1 if bypass called, item not unequipped
\param pi item to unequip
\param drag true when function called in get_item
*/
const uint8_t cBody::unEquip(pItem pi, bool drag)
{
	checkSafeStats();

	cVariantVector params(2);
	param[0] = pi;
	param[1] = getChar();
	
	if ( !pi->handleEvent( eventItemOnUnEquip, 2, params ) )
		return 2;

	// AntiChrist -- for poisoned items
	if(pi->getPoison())
	{
		if(poison < pi->getPoison())
			poison -= pi->getPoison();
	}

	if ( client )
		client->statusWindow(this, true);

	if (drag)
		return 0;

	pi->setContainer( getBackpack(true) );

	nPackets::Sent::ShowItemInContainer pk(pi);
	if(client)
		client->sendPacket(&pk);

	return 0;
}

/*!
\author Flameeyes (based on Endymion's one)
\brief Mount the body on an horse
\param horse The animal to mount
*/
void cBody::mount(pChar horse)
{
}

/*!
\author Flameeyes (Based on Endymion's one)
\brief Unmount the body
*/
void cBody::unmount()
{
}

/*!
\brief Calculates the weight of the body with all equipment and backpacks
\author Flameeyes
\note Replace old weights::NewCalc() function
*/
void cBody::calcWeight()
{
	pContainer bp = getBackpack();
	weight = bp ? bp->getWeightActual() : 0;
	
	for(register int i = 0; i < 0x1E; i++)
		if ( layers[i] && i != layMount )
			weight += layers[i]->getWeightActual();
}

/*!
\brief Check if player is overloaded when walking
\author Flameeyes over unknown work
\todo Clean up, use real stamina variables
*/
bool cBody::overloadedWalking()
{
	const static char steps[4] = { 25, 50, 75, 100 };
	
	int     limit = pc->getStrength()*nSettings::Server::getWeightPerStr()+30,
		percelt = int(((float)pc->weight/(float)limit)*100.0f),
		result;
	bool ret = true;
	uint8_t index,x;
	float amount;
	
	if(getWeight() > limit) index=5; //overweight
	else if(percent==100) index=4; //100% weight
	else for(x=0;x<4;x++) if(percent<steps[x]) { index=x; break; } //less than 100%
		
	/*	if(index<4)
	{
			float stepdiff		= float(steps[index]-steps2[index]),
			percentdiff	= float(percent-steps2[index]),
			stadiff		= ServerScp::g_fStaminaUsage[index+1]-ServerScp::g_fStaminaUsage[index];
			
			amount=ServerScp::g_fStaminaUsage[index]+(percentdiff/stepdiff)*stadiff;
	}
		else 
		*/		amount=ServerScp::g_fStaminaUsage[index];
	
	if(isRunning()) amount*=2; //if running, double the amount of stamina used
	if(getBody()->isMounted()) amount*=(float)server_data.staminaonhorse;
	
	pc->fstm+=amount; //increase the amount of stamina to be subtracted
	
	if(fstm>=1.0f) //if stamina to be removed is less than 1, wait
	{
		result=(int)fstm; //round it
		fstm-=result;
		stm-=result;
		if(stm<=0)
		{
			stm=0;
			ret = false;
		}
		updateStamina();
	}
	
	return ret;
	
}

/*!
\brief Check if player is overloaded when teleporting
\author Flameeyes over Morrolan work
*/
bool cBody::overloadedTeleport()
{
	if ( getWeight() > (getStrength()*nSettings::Server::getWeightPerStr())+30)
	{
		/*! \todo Need to chek what does this...
		pc->mn -= 30;
		if ( pc->mn <= 0 )
			pc->mn = 0;
		*/
		return true;
	}
	return false;
}

/*!
\brief Tells if the body is wearing the given item
\param pi Item to test
\return true if the body is wearing the item, else false
\note Can't be inlined, because in this case cbody will need to include citem.h
*/
const bool cBody::isWearing(pItem pi) const
{
	return this == pi->getContainer();
}

/*!
\brief Gets the body's best skill
\return baseskill's index for the best skill of the character
*/
Skill cBody::bestSkill() const
{
	Skill a = 0;

	for(register int i=0; i < skTrueSkills; i++)
		if ( skills[i] > skills[a] )
			a = i;
	
	return a;
}

/*!
\brief Gets the second or third body's best skill
\param previous index of the previous find best skill
\return baseskill's index for the next best skill of the character
*/
Skill cBody::nextBestSkill(uint8_t previous) const
{
	Skill a = previous ? 0 : 1; // if previous == 0 skip it

	for(register int i = a; i < skTrueSkills; i++)
		if ( skills[i] > skills[a] && i != previous )
			a = i;
	
	return a;
}
