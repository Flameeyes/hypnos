/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief PyUO natives for Python - Character Management

\note If nothing specified, the function accepts only 1 params and is the
	character to use the function on.
*/

#include "methods.h"

using namespace PythonInterface;

//! get if the character Yell is a server broadcast
PYNATIVE(chr_canBroadcast)
{
	SI32 serial;
	if ( !PyArg_ParseTuple(args, "i", &serial) )
		return NULL;

	P_CHAR pc = pointers::findCharBySerial(serial);
	VALIDATEPCR( pc, NULL );
	return Py_BuildValue("i", pc->CanBroadcast());
}

//! get if the character can see serial number
PYNATIVE(chr_canSeeSerials)
{
	SI32 serial;
	if ( !PyArg_ParseTuple(args, "i", &serial) )
		return NULL;

	P_CHAR pc = pointers::findCharBySerial(serial);
	VALIDATEPCR( pc, NULL );
	return Py_BuildValue("i", pc->CanSeeSerials());
}

//! get if the character can snoop everywhere always
PYNATIVE(chr_canSnoop)
{
	SI32 serial;
	if ( !PyArg_ParseTuple(args, "i", &serial) )
		return NULL;

	P_CHAR pc = pointers::findCharBySerial(serial);
	VALIDATEPCR( pc, NULL );
	return Py_BuildValue("i", pc->CanSnoop());
}

//! get the total gold in character bank box
PYNATIVE(chr_countBankGold)
{
	SI32 serial;
	if ( !PyArg_ParseTuple(args, "i", &serial) )
		return NULL;

	P_CHAR pc = pointers::findCharBySerial(serial);
	VALIDATEPCR( pc, PYINVALID );
	return Py_BuildValue("i", pc->CountBankGold());
}

//! get the total gold in character backpack
PYNATIVE(chr_countGold)
{
	SI32 serial;
	if ( !PyArg_ParseTuple(args, "i", &serial) )
		return NULL;

	P_CHAR pc = pointers::findCharBySerial(serial);
	VALIDATEPCR( pc, PYINVALID );
	return Py_BuildValue("i", pc->CountGold());
}

/*
\brief disturb the meditation of the character
\return INVALID if not valid character, 0 else
*/
PYNATIVE(chr_countGold)
{
	SI32 serial;
	if ( !PyArg_ParseTuple(args, "i", &serial) )
		return NULL;

	P_CHAR pc = pointers::findCharBySerial(serial);
	VALIDATEPCR( pc, PYINVALID);

	pc->disturbMed();
	return Py_BuildValue("i", 0);
}

/*
\brief get the char's bankbox
\param 1: the character
\param 2: bank box type ( BANKBOX_BANK, BANKBOX_WARE )
\return INVALID if not valid character, else the bank
*/
PYNATIVE(chr_getBankBox)
{
	SI32 serial, bank_type = BANKBOX_BANK;
	if ( !PyArg_ParseTuple(args, "i|i", &serial, &bank_type) )
		return NULL;

	P_CHAR pc = pointers::findCharBySerial(serial);
	VALIDATEPCR( pc, PYINVALID );
        P_ITEM pi = pc->GetBankBox(bank_type);
        VALIDATEPIR( pi, PYINVALID );
        return Py_BuildVAlue("i", pi->getSerial32());
}

