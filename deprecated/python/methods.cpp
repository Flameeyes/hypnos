/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\author Flameeyes & Kheru
\brief Python methods declaration
*/

#ifndef __PY_METHODS__
#define __PY_METHODS__

#include <Python.h>

#define PYNATIVE(_func) \
	static PyObject* _func(PyObject *self, PyObject *args)

#define PYNULL Py_BuildValue("i", NULL)

/*!
\brief Contains all Python related methods, and variables.

Used because if in the future we'll add a new scripting language, we won't
need to rename the function to avoid names' collisions.
*/
namespace PythonInterface
{
#include "misc.cpp"
#include "characters.cpp"

	/*!
	\brief Methods for Hypnos module

	This variable contains the list of methods that are passed to python
	*/
	static PyMethodDef HypnosMethods[] = {
		{"getCurrentTime", getCurrentTime, METH_VARARGS,
			"Returns the current time"},
		{NULL, NULL, 0, NULL}
	};
};

#endif
