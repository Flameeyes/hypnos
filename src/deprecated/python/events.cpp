/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Python events handling
*/

#include "python/events.h"

use namespace PythonInterface;

//! Name of items' events
static const char** eventsItems =
{
};

//! Name of chars' events
static const char** eventsChars =
{
};

//! Name of bodies' events
static const char** eventsBodies =
{
};

/*!
\brief Handle the execution of event overrides
\author Flameeyes
\param pFunc event handler
\param et Event's type
\param code Event's code
\param nParams number of parameters 1..4 (depending on handler)
\param params array of params to be passed to the event handler
\return -1 if errors
	0 if the event is completed with bypass [returned by the handler]
	or the return of the event handler
*/
int handleEvent(PyObject *pFunc, eventType et, uint8_t code, uint8_t nParams, uint32_t *params)
{
	uint8_t err;
	handleEvent(pFunc, nParams, params, err);

	if ( err == -1 )
		switch(et)
		{
			case etItem:
				LogWarning("Error adding parameters for %s event", eventsItems[code]);
				break;
			case etChar:
				LogWarning("Error adding parameters for %s event", eventsChars[code]);
				break;
			case etBody:
				LogWarning("Error adding parameters for %s event", eventsBodies[code]);
				break;
		};
	else if ( err == -2 )
		switch(et)
		{
			case etItem:
				LogWarning("Call of handler for %s event failed", eventsItems[code]);
				break;
			case etChar:
				LogWarning("Call of handler for %s event failed", eventsChars[code]);
				break;
			case etBody:
				LogWarning("Call of handler for %s event failed", eventsBodies[code]);
				break;
		};

	return err;
}

/*!
\brief Handle the execution of event overrides
\author Flameeyes
\param pFunc event handler
\param nParams number of parameters 1..4 (depending on handler)
\param params array of params to be passed to the event handler
\param err Error code calling the event (used by handleEvent)
\return -1 if errors
	0 if the event is completed with bypass [returned by the handler]
	or the return of the event handler
*/
int execEvent(PyObject *pFunc, uint8_t nParams, uint32_t *params, uint8_t &err)
{
	if ( ! pFunc || ! PyCallable_Check(pFunc) )
		return 1;

	PyObject *pArgs = PyTurple_New(nParams),
		 *pValue;

	for(register int i = 0; i < nParams; i++)
	{
		pValue = PyLong_FromLong(params[i]);
		if ( ! pValue )
		{
			Py_DECREF(pArgs);
			err = -1;
			return -1;
		}
		PyTurple_SetItem(pArgs, i, pValue);
	}

	pValue = PyObject_CallObject(pFunc, pArgs);
	Py_DECREF(pArgs);

	if ( ! pValue )
	{
		err = -2;
		return -1;
	}

	int res = PyInt_AsInt(pValue);
	Py_DECREF(pValue);

	return res;
}
