/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\author Flameeyes & Kheru
\brief Python methods declaration
*/

#ifndef __PY_METHODS__
#define __PY_METHODS__

/*!
\brief Contains all Python related methods, and variables.

Used because if in the future we'll add a new scripting language, we won't
need to rename the function to avoid names' collisions.
*/
namespace PythonInterface
{
	/*!
	\brief Methods for PyUO module

	This variable contains the list of methods that are passed to python
	*/
	static PyMethodDef PyUOMethods[] = {
		{NULL, NULL, 0, NULL}
	};
};

#endif
