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

		pValue = PyObject_CallObject(pFunc, pArgs);
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
