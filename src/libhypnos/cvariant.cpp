/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Implementation of tVariant type
*/

#include "abstraction/tvariant.h"

//! Inline function to recalc the uint size
inline void tVariant::recalcUIntSize()
{
	if ( uintvalue > INT16_MAX || uintvalue < INT16_MIN )
		integerSize = is32;
	else if ( uintvalue > INT8_MAX || uintvalue < INT8_MIN )
		integerSize = is16;
	else
		integerSize = is8;
}

/*!
\brief Default constructor
*/
tVariant::tVariant()
{
	assignedType = vtNull;
	uintvalue = 0;
	intvalue = 0;
	ptrvalue = NULL;
}

tVariant &tVariant::operator =(const std::string &astr)
{
	stringvalue = astr;
	assignedType = vtString;
	integerSize = isNotInt;
	return *this;
}

tVariant &tVariant::operator =(const bool &aval)
{
	uintvalue = aval;
	assignedType = vtBoolean;
	integerSize = isBool;
	return *this;
}

tVariant &tVariant::operator =(const uint32_t &aval)
{
	uintvalue = aval;
	assignedType = vtUInt;
	if ( aval > UINT16_MAX )
		integerSize = is32;
	else if ( aval > UINT8_MAX )
		integerSize = is16;
	else
		integerSize = is8;
	return *this;
}

tVariant &tVariant::operator =(const int32_t &aval)
{
	sintvalue = aval;
	assignedType = vtSInt;
	if ( aval > INT16_MAX || aval < INT16_MIN )
		integerSize = is32;
	else if ( aval > INT8_MAX || aval < INT8_MIN )
		integerSize = is16;
	else
		integerSize = is8;
	return *this;
}

tVariant &tVariant::operator =(const void *aptr)
{
	ptrvalue = aptr;
	assignedType = vtPVoid;
	integerSize = isNotInt;
	return *this;
}

tVariant &tVariant::operator =(const pChar *apc)
{
	ptrvalue = apc;
	assignedType = vtPChar;
	integerSize = isNotInt;
	return *this;
}

tVariant &tVariant::operator =(const pItem *api)
{
	ptrvalue = api;
	assignedType = vtPItem;
	integerSize = isNotInt;
	return *this;
}

tVariant tVariant::operator +(const tVariant &param) const
{
	switch(assignedType)
	{
		case vtNull:
			return param;
		case vtString:
			return tVariant(stringvalue + param.toString());
		case vtUInt:
			return tVariant(uintvalue + param.toUInt32());
		case vtSInt:
			return tVariant(sintvalue + param.toSInt32());
		default:
			return tVariant();
	}
}

tVariant operator -(const tVariant &param)
{
	switch(assignedType)
	{
		case vtNull:
			return -param;
		case vtUInt:
			return tVariant(uintvalue - param.toUInt32());
		case vtSInt:
			return tVariant(sintvalue - param.toSInt32());
		default:
			return tVariant();
	}
}

tVariant operator +=(const tVariant &param) const;
tVariant operator -=(const tVariant &param) const;

tVariant operator ++(int unused)
{
	tVariant tmp = *this;
	*this += 1;
	return tmp;
}

tVariant operator --(int unused)
{
	tVariant tmp = *this;
	*this -= 1;
	return tmp;
}

bool operator ==(const tVariant &param) const;
bool operator <(const tVariant &param) const;
bool operator >(const tVariant &param) const;
