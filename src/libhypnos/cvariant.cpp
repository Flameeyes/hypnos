/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Implementation of tVariant type
*/

// Request limits
#define __STDC_LIMIT_MACROS

#include "abstraction/tvariant.h"
#include "objects/cchar.h"
#include "objects/citem.h"

//! Inline function to recalc the uint size
inline void tVariant::recalcUIntSize()
{
	if ( uintvalue > UINT16_MAX )
		integerSize = is32;
	else if ( uintvalue > UINT8_MAX )
		integerSize = is16;
	else
		integerSize = is8;
}

//! Inline function to recalc the sint size
inline void tVariant::recalcSIntSize()
{
	if ( sintvalue > INT16_MAX || sintvalue < INT16_MIN )
		integerSize = is32;
	else if ( sintvalue > INT8_MAX || sintvalue < INT8_MIN )
		integerSize = is16;
	else
		integerSize = is8;
}

//! Default constructor
tVariant::tVariant()
{
	assignedType = vtNull;
	uintvalue = 0;
	sintvalue = 0;
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
	recalcUIntSize();
	return *this;
}

tVariant &tVariant::operator =(const int32_t &aval)
{
	sintvalue = aval;
	assignedType = vtSInt;
	recalcSIntSize();
	return *this;
}

tVariant &tVariant::operator =(void *aptr)
{
	ptrvalue = aptr;
	assignedType = vtPVoid;
	integerSize = isNotInt;
	return *this;
}

tVariant &tVariant::operator =(pChar apc)
{
	ptrvalue = apc;
	assignedType = vtPChar;
	integerSize = isNotInt;
	return *this;
}

tVariant &tVariant::operator =(pItem api)
{
	ptrvalue = api;
	assignedType = vtPItem;
	integerSize = isNotInt;
	return *this;
}

tVariant &tVariant::operator =(pClient aclient)
{
	ptrvalue = aclient;
	assignedType = vtPClient;
	integerSize = isNotInt;
	return *this;
}

tVariant tVariant::operator -() const
{
	switch(assignedType)
	{
		case vtUInt:
			if ( uintvalue > INT32_MAX )
				return tVariant();
			
			return tVariant(-uintvalue);
		case vtSInt:
			return tVariant(-sintvalue);
		
		default:
			return tVariant();
	}
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

tVariant tVariant::operator -(const tVariant &param) const
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

tVariant tVariant::operator ++(int unused)
{
	tVariant tmp = *this;
	*this += 1;
	return tmp;
}

tVariant tVariant::operator --(int unused)
{
	tVariant tmp = *this;
	*this -= 1;
	return tmp;
}

bool tVariant::operator ==(const tVariant &param) const
{
	switch(assignedType)
	{
		case vtNull:
			return param.isNull();
		case vtBoolean:
			return uintvalue == param.toBoolean();
		case vtUInt:
			return uintvalue == param.toUInt32();
		case vtSInt:
			return sintvalue == param.toSInt32();
		case vtString:
			return stringvalue == param.toString();
		case vtPVoid:
		case vtPChar:
		case vtPItem:
		case vtPClient:
			return ptrvalue == param.toPVoid();
		default:
			return false;
	}
}

bool tVariant::operator <(const tVariant &param) const
{
	switch(assignedType)
	{
		case vtUInt:
			return uintvalue < param.toUInt32();
		case vtSInt:
			return sintvalue < param.toSInt32();
		case vtString:
			return strcmp(stringvalue.c_str(), param.toString().c_str()) < 0;
		default:
			return false;
	}
}

bool tVariant::operator >(const tVariant &param) const
{
	switch(assignedType)
	{
		case vtUInt:
			return uintvalue > param.toUInt32();
		case vtSInt:
			return sintvalue > param.toSInt32();
		case vtString:
			return strcmp(stringvalue.c_str(), param.toString().c_str()) > 0;
		default:
			return false;
	}
}

bool tVariant::convertInString()
{
	stringvalue = toString();
	assignedType = vtString;
	integerSize = isNotInt;
	return true;
}

bool tVariant::convertInBoolean()
{
	bool conv;
	bool temp = toBoolean(&conv);
	
	if ( ! conv )
		return false;
	
	uintvalue = temp;
	assignedType = vtBoolean;
	integerSize = isBool;
	return true;
}

bool tVariant::convertInUInt()
{
	bool conv;
	uint32_t temp = toUInt32(&conv);
	
	if ( ! conv )
		return false;
	
	uintvalue = temp;
	assignedType = vtUInt;
	recalcUIntSize();
	return true;
}

bool tVariant::convertInSInt()
{
	bool conv;
	int32_t temp = toSInt32(&conv);
	
	if ( ! conv )
		return false;
	
	sintvalue = temp;
	assignedType = vtSInt;
	recalcSIntSize();
	return true;
}

bool tVariant::convertInPChar()
{
	bool conv;
	pChar temp = toPChar(&conv);
	
	if ( ! conv )
		return false;
	
	ptrvalue = temp;
	assignedType = vtPChar;
	integerSize = isNotInt;
	return true;
}

bool tVariant::convertInPItem()
{
	bool conv;
	pItem temp = toPItem(&conv);
	
	if ( ! conv )
		return false;
	
	ptrvalue = temp;
	assignedType = vtPItem;
	integerSize = isNotInt;
	return true;
}

bool tVariant::convertInPClient()
{
	bool conv;
	pClient temp = toPClient(&conv);
	
	if ( ! conv )
		return false;
	
	ptrvalue = temp;
	assignedType = vtPClient;
	integerSize = isNotInt;
	return true;
}

bool tVariant::convertInPVoid()
{
	bool conv;
	void *temp = toPVoid(&conv);
	
	if ( ! conv )
		return false;
	
	ptrvalue = temp;
	assignedType = vtPVoid;
	integerSize = isNotInt;
	return true;
}

/*!
\note Conversion to string is always possible: vtNull is reported as [nil]
*/
std::string tVariant::toString(bool *result) const
{
	if ( result ) *result = true;
	switch( assignedType )
	{
		case vtString:
			return stringvalue;
		case vtSInt:
			{
				char *tmp;
				asprintf(&tmp, "%d", sintvalue);
				std::string ret(tmp);
				free(tmp);
				return ret;
			}
		case vtUInt:
			{
				char *tmp;
				asprintf(&tmp, "%u", uintvalue);
				std::string ret(tmp);
				free(tmp);
				return ret;
			}
		case vtBoolean:
			return uintvalue ? std::string("true") : std::string("false");
		
		case vtPChar:
		case vtPItem:
		case vtPClient:
		case vtPVoid:
			{
				char *tmp;
				asprintf(&tmp, "%p", ptrvalue);
				std::string ret(tmp);
				free(tmp);
				return ret;
			}
		case vtNull:
			return std::string("[nil]");
		default:
			if ( result ) *result = false;
			return std::string("[Undefined]");
	}
}

bool tVariant::toBoolean(bool *result) const
{
	if ( result ) *result = true;
	switch( assignedType )
	{
		case vtNull:
			return false;
		case vtBoolean:
		case vtUInt:
			return uintvalue;
		case vtSInt:
			return sintvalue;
		case vtPChar:
		case vtPItem:
		case vtPClient:
		case vtPVoid:
			return ptrvalue;
		case vtString:
			if ( stringvalue == "true" ) return true;
			if ( stringvalue == "false" ) return false;
		default:
			// If not true or false
			if ( result ) *result = false;
			return false;
	}
}

pChar tVariant::toPChar(bool *result) const
{
	if ( result ) *result = true;
	switch( assignedType )
	{
		case vtPChar:
		case vtPVoid:
			return reinterpret_cast<pChar>(ptrvalue);
		default:
			if ( result ) *result = false;
			return NULL;
	}
}

pItem tVariant::toPItem(bool *result) const
{
	if ( result ) *result = true;
	switch( assignedType )
	{
		case vtPItem:
		case vtPVoid:
			return reinterpret_cast<pItem>(ptrvalue);
		default:
			if ( result ) *result = false;
			return NULL;
	}
}

pClient tVariant::toPClient(bool *result) const
{
	if ( result ) *result = true;
	switch( assignedType )
	{
		case vtPClient:
		case vtPVoid:
			return reinterpret_cast<pClient>(ptrvalue);
		default:
			if ( result ) *result = false;
			return NULL;
	}
}

void *tVariant::toPVoid(bool *result) const
{
	switch( assignedType )
	{
		case vtPItem:
		case vtPChar:
		case vtPClient:
		case vtPVoid:
			if ( result ) *result = true;
			return ptrvalue;
		default:
			if ( result ) *result = false;
			return NULL;
	}
}

uint32_t tVariant::toUInt32(bool *result) const
{
	switch( assignedType )
	{
		case vtString:
			{
				char *end;
				uint32_t tmp = strtoul( stringvalue.c_str(), &end, 0);
				if ( end == stringvalue.c_str() )
				{
					if ( result ) *result = false;
					return 0;
				}
				
				return tmp;
			}
		case vtUInt:
			return uintvalue;
		case vtSInt:
			{
				if ( sintvalue < 0 )
				{
					if ( result ) *result = false;
					return 0;
				}
				
				return static_cast<uint32_t>(sintvalue);
			}
		
		default:
			if ( result ) *result = false;
			return 0;
	}
}

uint16_t tVariant::toUInt16(bool *result) const
{
	uint32_t tmp = 0;
	switch( assignedType )
	{
		case vtString:
			{
				char *end;
				tmp = strtoul( stringvalue.c_str(), &end, 0);
				if ( end == stringvalue.c_str() )
				{
					if ( result ) *result = false;
					return 0;
				}
			}
			break;
		case vtUInt:
			tmp = uintvalue;
			break;
		case vtSInt:
			{
				if ( sintvalue < 0 )
				{
					if ( result ) *result = false;
					return 0;
				}
				
				tmp = static_cast<uint32_t>(sintvalue);
			}
			break;
		
		default:
			if ( result ) *result = false;
			return 0;
	}
	
	if ( tmp > UINT16_MAX )
	{
		if ( result ) *result = false;
		return 0;
	}
	
	if ( result ) *result = true;
	return static_cast<uint16_t>(tmp);
}

uint8_t tVariant::toUInt8(bool *result) const
{
	uint32_t tmp = 0;
	switch( assignedType )
	{
		case vtString:
			{
				char *end;
				tmp = strtoul( stringvalue.c_str(), &end, 0);
				if ( end == stringvalue.c_str() )
				{
					if ( result ) *result = false;
					return 0;
				}
			}
			break;
		case vtUInt:
			tmp = uintvalue;
			break;
		case vtSInt:
			{
				if ( sintvalue < 0 )
				{
					if ( result ) *result = false;
					return 0;
				}
				
				tmp = static_cast<uint32_t>(sintvalue);
			}
			break;
		
		default:
			if ( result ) *result = false;
			return 0;
	}
	
	if ( tmp > UINT8_MAX )
	{
		if ( result ) *result = false;
		return 0;
	}
	
	if ( result ) *result = true;
	return static_cast<uint8_t>(tmp);
}

int32_t tVariant::toSInt32(bool *result) const
{
	switch( assignedType )
	{
		case vtString:
			{
				char *end;
				int32_t tmp = strtol( stringvalue.c_str(), &end, 0);
				if ( end == stringvalue.c_str() )
				{
					if ( result ) *result = false;
					return 0;
				}
				
				return tmp;
			}
		case vtSInt:
			return sintvalue;
		case vtUInt:
			{
				if ( uintvalue > INT32_MAX )
				{
					if ( result ) *result = false;
					return 0;
				}
				
				return static_cast<int32_t>(uintvalue);
			}
		
		default:
			if ( result ) *result = false;
			return 0;
	}
}

int16_t tVariant::toSInt16(bool *result) const
{
	int32_t tmp = 0;
	switch( assignedType )
	{
		case vtString:
			{
				char *end;
				tmp = strtol( stringvalue.c_str(), &end, 0);
				if ( end == stringvalue.c_str() )
				{
					if ( result ) *result = false;
					return 0;
				}
			}
			break;
		case vtSInt:
			tmp = sintvalue;
			break;
		case vtUInt:
			{
				if ( uintvalue > INT32_MAX )
				{
					if ( result ) *result = false;
					return 0;
				}
				
				tmp = static_cast<int32_t>(uintvalue);
			}
			break;
		
		default:
			if ( result ) *result = false;
			return 0;
	}
	
	if ( tmp > INT16_MAX || tmp < INT16_MIN )
	{
		if ( result ) *result = false;
		return 0;
	}
	
	if ( result ) *result = true;
	return static_cast<int16_t>(tmp);
}

int8_t tVariant::toSInt8(bool *result) const
{
	int32_t tmp = 0;
	switch( assignedType )
	{
		case vtString:
			{
				char *end;
				tmp = strtol( stringvalue.c_str(), &end, 0);
				if ( end == stringvalue.c_str() )
				{
					if ( result ) *result = false;
					return 0;
				}
			}
			break;
		case vtSInt:
			tmp = sintvalue;
			break;
		case vtUInt:
			{
				if ( uintvalue > INT32_MAX )
				{
					if ( result ) *result = false;
					return 0;
				}
				
				tmp = static_cast<int32_t>(uintvalue);
			}
			break;
		
		default:
			if ( result ) *result = false;
			return 0;
	}
	
	if ( tmp > INT8_MAX || tmp < INT8_MIN )
	{
		if ( result ) *result = false;
		return 0;
	}
	
	if ( result ) *result = true;
	return static_cast<int8_t>(tmp);
}
