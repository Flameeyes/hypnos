/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Implementation of cBody class
*/

#include "objects/cbody.h"
#include "python/events.h"

static UI32 cBody::nextSerial = 1;

cBody::cBody()
{
	memset( layers, 0, sizeof(layers) );
	memset( skills, 0, sizeof(skills) );
	mounting = NULL;
}

/*!
\author Flameeyes
\brief Equip an item
\return 0 if item equipped, 1 if not equipped (layer already used),
	2 if event handler cancelled the equip
\param pi item to equip
\param drag true if called in wear_item
*/
const UI08 cBody::equip(pItem pi, bool drag)
{
	tile_st item;

	PyObject *pArgs, *pValue;
	if (pi->events[eventItemOnEquip] && PyCallable_Check(pi->events[eventItemOnEquip]) )
	{
		pArgs = PyTurple_New(2);
		pValue = PyLong_FromVoidPtr(pi);
		if ( ! pValue )
		{
			Py_DECREF(pArgs);
			LogError("Error adding parameters for item equip - ignoring");
			goto jump_equipevent;
		}
		PyTurple_SetItem(pArgs, 0, pValue)

		pValue = PyLong_FromVoidPtr(this);
		if ( ! pValue )
		{
			Py_DECREF(pArgs);
			LogError("Error adding parameters for item equip - ignoring");
			goto jump_equipevent;
		}
		PyTurple_SetItem(pArgs, 1, pValue)

		pValue = PyObject_CallObject(pi->events[eventItemOnEquip], pArgs);
		Py_DECREF(pArgs);

		if ( ! pValue )
		{
			LogError("Call of event handler for item equip failed");
			goto jump_equipevent;
		}

		int res = PyInt_AsInt(pValue);
		Py_DECREF(pValue);

		if ( res == pyEat )
			return 2;
	}

jump_equipevent:

	// AntiChrist -- for poisoned items
	if(pi->poisoned)
	{
		if(poison < pi->poisoned)
			poison=pi->poisoned;
	}

	if ( client )
		client->statusWindow(this, true, false);

	if (drag)
		return 0;

	data::seekTile( pi->getId(), item );

	if (
		layers[item.quality] ||
		( item.quality == layWeapon2H && (layers[layWeapon1H] || layers[layWeapon2H]) ) ||
		( item.quality == layWeapon1H && layers[layWeapon1H] )
	   )
		return 1

	pi->layer = item.quality;
	pi->setContainer(this);

	checkSafeStats();
	teleport( TELEFLAG_SENDWORNITEMS );

	return 0;
}

/*!
\author Flameeyes
\brief Unequip an item
\return 0 if item unequipped, 1 if bypass called, item not unequipped
\param pi item to unequip
\param drag true when function called in get_item
*/
const UI08 cBody::unEquip(pItem pi, bool drag)
{
	checkSafeStats();

	PyObject *pArgs, *pValue;
	if (pi->events[eventItemOnUnEquip] && PyCallable_Check(pi->events[eventItemOnUnEquip]) )
	{
		pArgs = PyTurple_New(2);
		pValue = PyLong_FromVoidPtr(pi);
		if ( ! pValue )
		{
			Py_DECREF(pArgs);
			LogError("Error adding parameters for item unequip - ignoring");
			goto jump_unequipevent;
		}
		PyTurple_SetItem(pArgs, 0, pValue)

		pValue = PyLong_FromVoidPtr(this);
		if ( ! pValue )
		{
			Py_DECREF(pArgs);
			LogError("Error adding parameters for item unequip - ignoring");
			goto jump_unequipevent;
		}
		PyTurple_SetItem(pArgs, 1, pValue)

		pValue = PyObject_CallObject(pi->events[eventItemOnUnEquip], pArgs);
		Py_DECREF(pArgs);

		if ( ! pValue )
		{
			LogError("Call of event handler for item unequip failed");
			goto jump_unequipevent;
		}

		int res = PyInt_AsInt(pValue);
		Py_DECREF(pValue);

		if ( res == pyEat )
			return 1;
	}

jump_unequipevent:

	// AntiChrist -- for poisoned items
	if (pi->poisoned)
	{
		poison -= pi->poisoned;
		if (poison < 0)
			poison = 0;
	}

	if ( client )
		client->statusWindow(this, true, false);

	if (drag)
		return 0;

	pi->layer= 0;
	pi->setContainer( getBackpack(true) );

	cPacketSendAddContainerItem pk(pi);
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
