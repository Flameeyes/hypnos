/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Implementation of cPC class
*/

#include "objects/cpc.h"

/*!
\brief PC-related implementation of the cChar::updateFlag() method
\todo Check the comments inside it
*/
bool cPC::updateFlag()
{
	bool flagHasChanged = false;
	if ( kills >= (unsigned) repsys.maxkills )
	{
		if( !IsMurderer() )
			flagHasChanged = true;
		//
		//! \todo - TODO check out logic of next 2 statements (Sparhawk)
		//
		SetMurderer();
		murderrate = (repsys.murderdecay*MY_CLOCKS_PER_SEC)+uiCurrentTime;
	}
	/*else

		if ( crimflag == 0 )
		{
			if( !IsInnocent() )
				flagHasChanged = true;
			SetInnocent();
		}
		else
		{
			if( !IsCriminal() )
				flagHasChanged = true;
			SetCriminal();
		}
		*/
	return flagHasChanged;
}

/*!
\brief Send the update skill packet to the client
\param skill Skill to update
\author Flameeyes
*/
void cChar::updateSkill(UI16 skill)
{
	if ( ! client )
		return;

	cPacketSendUpdateSkill pk(this, skill);

	client->sendPackage(&pk);
}
