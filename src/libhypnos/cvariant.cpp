/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

// With the define below we ask to the standard library to define the
// UINT16_MAX and sibling constants to calc the size of the integer values.
#define __STDC_LIMIT_MACROS

#include "abstraction/tvariant.h"
#include "objects/cchar.h"
#include "objects/citem.h"

//! Inline function to recalc the uint size
inline void tVariant::recalcUIntSize(const uint64_t &val)
{
	if ( uintvalue > UINT32_MAX )
		integerSize = is64;
	else if ( uintvalue > UINT16_MAX )
		integerSize = is32;
	else if ( uintvalue > UINT8_MAX )
		integerSize = is16;
	else
		integerSize = is8;
}

//! Inline function to recalc the sint size
inline void tVariant::recalcSIntSize(const int64_t &val)
{
	if ( sintvalue > INT32_MAX || sintvalue < INT32_MIN )
		integerSize = is64;
	else if ( sintvalue > INT16_MAX || sintvalue < INT16_MIN )
		integerSize = is32;
	else if ( sintvalue > INT8_MAX || sintvalue < INT8_MIN )
		integerSize = is16;
	else
		integerSize = is8;
}

//! Default constructor
tVariant::tVariant()
{
	integerSize = isNotInt;
	assignedType = vtNull;
	pointer = NULL;
}

tVariant::~tVariant()
{
	clear();
}

/*!
\brief Clear the tVariant type

This function is used to reset the tVariant at the null state. It deletes
eventual values stored inside and sets the type to vtNull.

It's called by the by the ~tVariant() destructor and by the assignment
operators to have a clean tVariant where to store the new value.

The only types which won't be deleted are the pointers, all the types.
*/
void tVariant::clear()
{
	switch(assignedType)
	{
	case vtString:
		delete reinterpret_cast<std::string*>(pointer);
		pointer = NULL;
		break;
	
	case vtVector:
		delete reinterpret_cast<tVariantVector*>(pointer);
		pointer = NULL;
		break;
	
	case vtBoolean:
		delete reinterpret_cast<bool*>(pointer);
		pointer = NULL;
		break;
	
	case vtUInt:
		switch(integerSize)
		{
			case is64:
				delete reinterpret_cast<uint64_t*>(pointer);
				break;
			case is32:
				delete reinterpret_cast<uint32_t*>(pointer);
				break;
			case is16:
				delete reinterpret_cast<uint16_t*>(pointer);
				break;
			case is8:
				delete reinterpret_cast<uint8_t*>(pointer);
				break;
			case isNotInt:
				LogCritical("tVariant %p has invalid pointer %p to a not integer size for an unsigned integer type", this, pointer);
		};
		pointer = NULL;
		break;
	
	case vtSInt:
		switch(integerSize)
		{
			case is64:
				delete reinterpret_cast<int64_t*>(pointer);
				break;
			case is32:
				delete reinterpret_cast<int32_t*>(pointer);
				break;
			case is16:
				delete reinterpret_cast<int16_t*>(pointer);
				break;
			case is8:
				delete reinterpret_cast<int8_t*>(pointer);
				break;
			case isNotInt:
				LogCritical("tVariant %p has invalid pointer %p to a not integer size for a signed integer type", this, pointer);
		};
		pointer = NULL;
		break;
		
	case vtPChar:
	case vtPItem:
	case vtPClient:
	case vtPVoid:
	case vtNull:
		/* In all the above cases, the value is stored inside the pointer
		 * attribute, no new instances had been created, so we can
		 * simply set its value to NULL and set the assigned type at
		 * vtNull.
		 */
		 pointer = NULL;
	}
	assignedType = vtNull;
}

/*!
\brief Assignment operator for a string value
\param astr Value to set into the variant instance
\return A reference to the instance itself
\note This method create a new pointer instance
*/
tVariant &tVariant::operator =(const std::string &astr)
{
	clear();
	pointer = new std::string(astr);

	assignedType = vtString;
	integerSize = isNotInt;
	return *this;
}

/*!
\brief Assignment operator for a boolean value
\param aval Value to set into the variant instance
\return A reference to the instance itself
\note This method create a new pointer instance
*/
tVariant &tVariant::operator =(const bool &aval)
{
	clear();
	pointer = new bool(aval);

	assignedType = vtBoolean;
	integerSize = isNotInt;
	return *this;
}

/*!
\brief Assignment operator for an unsigned integer value
\param aval Value to set into the variant instance
\return A reference to the instance itself
\note This method create a new pointer instance

This function takes the biggest integer available but stores it using
the minimum integer size possible.
Please note that doing this we need to use uint64_t when doing sums
and differences.
*/
tVariant &tVariant::operator =(const uint64_t &aval)
{
	clear();
	recalcUIntSize(aval);
	
	switch(integerSize)
	{
	case is64:
		pointer = new uint64_t(aval);
		break;
	case is32:
		pointer = new uint32_t(aval & 0xFFFFFFFF);
		break;
	case is16:
		pointer = new uint16_t(aval & 0xFFFF);
		break;
	case is8:
		pointer = new uint8_t(aval & 0xFF);
		break;
	}
	
	assignedType = vtUInt;
	return *this;
}

/*!
\brief Assignment operator for a signed integer value
\param aval Value to set into the variant instance
\return A reference to the instance itself
\note This method create a new pointer instance

This function takes the biggest integer available but stores it using
the minimum integer size possible.
Please note that doing this we need to use int64_t when doing sums
and differences.
*/
tVariant &tVariant::operator =(const int64_t &aval)
{
	clear();
	recalcSIntSize(aval);
	
	switch(integerSize)
	{
	case is64:
		pointer = new int64_t(aval);
		break;
	case is32:
		pointer = new int32_t(aval & 0xFFFFFFFF);
		break;
	case is16:
		pointer = new int16_t(aval & 0xFFFF);
		break;
	case is8:
		pointer = new int8_t(aval & 0xFF);
		break;
	}
	
	assignedType = vtSInt;
	return *this;
}

/*!
\brief Assignment operator for a pointer value
\param aptr Pointer to set the internal pointer to
\return A reference to the instance itself
*/
tVariant &tVariant::operator =(void *aptr)
{
	pointer = aptr;
	assignedType = vtPVoid;
	integerSize = isNotInt;
	return *this;
}

//! \copydoc tVariant::operator=(void*)
tVariant &tVariant::operator =(pChar aptr)
{
	pointer = aptr;
	assignedType = vtPChar;
	integerSize = isNotInt;
	return *this;
}

//! \copydoc tVariant::operator=(void*)
tVariant &tVariant::operator =(pItem aptr)
{
	pointer = api;
	assignedType = vtPItem;
	integerSize = isNotInt;
	return *this;
}

//! \copydoc tVariant::operator=(void*)
tVariant &tVariant::operator =(pClient aptr)
{
	pointer = aclient;
	assignedType = vtPClient;
	integerSize = isNotInt;
	return *this;
}

tVariant tVariant::operator -() const
{
	switch(assignedType)
	{
	case vtUInt:
		if ( uintvalue > INT64_MAX )
			return tVariant();
		
		return tVariant(-uint64_t(*this));
	case vtSInt:
		return tVariant(-int64_t(*this));
	
	default: // We can't invert the sign of a non-integer
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
		return tVariant(std::string(*this) + std::string(param));
	case vtUInt:
		return tVariant(uint64_t(*this) + uint64_t(param));
	case vtSInt:
		return tVariant(int64_t(*this) + int64_t(param));
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
		return tVariant(uint64_t(*this) - uint64_t(param));
	case vtSInt:
		return tVariant(int64_t(*this) - int64_t(param));
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
		return bool(*this) == bool(param);
	case vtUInt:
		return uint64_t(*this) == uint64_t(param);
	case vtSInt:
		return int64_t(*this) == int64_t(param);
	case vtString:
		return std::string(*this) == std::string(param);
	case vtPVoid:
	case vtPChar:
	case vtPItem:
	case vtPClient:
		return pointer == void*(param);
	default:
		return false;
	}
}

bool tVariant::operator <(const tVariant &param) const
{
	switch(assignedType)
	{
	case vtUInt:
		return uint64_t(*this) < uint64_t(param);
	case vtSInt:
		return int64_t(*this) < int64_t(param);
	case vtString:
		return strcmp( std::string(*this).c_str(), std::string(param).c_str()) < 0;
	default:
		return false;
	}
}

bool tVariant::operator >(const tVariant &param) const
{
	switch(assignedType)
	{
		case vtUInt:
			return uint64_t(*this) > uint64_t(param);
		case vtSInt:
			return int64_t(*this) > int64_t(param);
		case vtString:
			return strcmp( std::string(*this).c_str(), std::string(param).c_str()) > 0;
		default:
			return false;
	}
}

bool tVariant::convertInString()
{
	bool conv;
	std::string temp = toString(&conv);
	
	if ( ! conv )
		return false;
	
	*this = temp;
	return true;
}

bool tVariant::convertInBoolean()
{
	bool conv;
	bool temp = toBoolean(&conv);
	
	if ( ! conv )
		return false;
	
	*this = temp;
	return true;
}

bool tVariant::convertInUInt()
{
	bool conv;
	uint64_t temp = toUInt64(&conv);
	
	if ( ! conv )
		return false;
	
	*this = temp;
	return true;
}

bool tVariant::convertInSInt()
{
	bool conv;
	int64_t temp = toSInt64(&conv);
	
	if ( ! conv )
		return false;
	
	*this = temp;
	return true;
}

bool tVariant::convertInPChar()
{
	bool conv;
	pChar temp = toPChar(&conv);
	
	if ( ! conv )
		return false;
	
	*this = temp;
	return true;
}

bool tVariant::convertInPItem()
{
	bool conv;
	pItem temp = toPItem(&conv);
	
	if ( ! conv )
		return false;
	
	*this = temp;
	return true;
}

bool tVariant::convertInPClient()
{
	bool conv;
	pClient temp = toPClient(&conv);
	
	if ( ! conv )
		return false;
	
	*this = temp;
	return true;
}

bool tVariant::convertInPVoid()
{
	bool conv;
	void *temp = toPVoid(&conv);
	
	if ( ! conv )
		return false;
	
	*this = temp;
	return true;
}

/*!
\brief Translate the variant into a string value.
\param[out] result Will be set true if the conversion was possible, else false.
\return The string which represents the value.
\note For string conversion, the only type from which you can't convert is
	vector.

\b Integer values are converted in string as usual. \b Boolean values are
converted in \em truc or \em false strings. \b Pointers value are converted to
strings as defined by %p format of printf. \b Null values are returned as
\em [nil] string.
*/
std::string tVariant::toString(bool *result) const
{
	if ( result ) *result = true;
	switch( assignedType )
	{
	case vtString:
		return *(reinterpret_cast<std::string*>(pointer));
	case vtSInt:
	{
		char *tmp;
		switch(integerSize)
		{
		case is64:
			asprintf(&tmp, "%lli", *(reinterpret_cast<int64_t*>(pointer)));
			return;
		case is32:
			asprintf(&tmp, "%i", *(reinterpret_cast<int32_t*>(pointer)));
			return;
		case is16:
			asprintf(&tmp, "%hi", *(reinterpret_cast<int16_t*>(pointer)));
			return;
		case is8:
			asprintf(&tmp, "%hhi", *(reinterpret_cast<int8_t*>(pointer)));
			return;
		}
		asprintf(&tmp, "%d", sintvalue);
		std::string ret(tmp);
		free(tmp);
		return ret;
	}
	case vtUInt:
	{
		char *tmp;
		switch(integerSize)
		{
		case is64:
			asprintf(&tmp, "%llu", *(reinterpret_cast<uint64_t*>(pointer)));
			return;
		case is32:
			asprintf(&tmp, "%u", *(reinterpret_cast<uint32_t*>(pointer)));
			return;
		case is16:
			asprintf(&tmp, "%hu", *(reinterpret_cast<uint16_t*>(pointer)));
			return;
		case is8:
			asprintf(&tmp, "%hhu", *(reinterpret_cast<uint8_t*>(pointer)));
			return;
		}
		asprintf(&tmp, "%d", sintvalue);
		std::string ret(tmp);
		free(tmp);
		return ret;
	}
	case vtBoolean:
		return *(reinterpret_cast<bool*>(pointer)) ? std::string("true") : std::string("false");
	
	case vtPChar:
	case vtPItem:
	case vtPClient:
	case vtPVoid:
		{
			char *tmp;
			asprintf(&tmp, "%p", pointer);
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
		switch(integerSize)
		{
		case is64:
			return *(reinterpret_cast<uint64_t*>(pointer));
			return;
		case is32:
			return *(reinterpret_cast<uint32_t*>(pointer));
			return;
		case is16:
			return *(reinterpret_cast<uint16_t*>(pointer));
			return;
		case is8:
			return *(reinterpret_cast<uint8_t*>(pointer));
			return;
		}
	case vtSInt:
		switch(integerSize)
		{
		case is64:
			return *(reinterpret_cast<int64_t*>(pointer));
			return;
		case is32:
			return *(reinterpret_cast<int32_t*>(pointer));
			return;
		case is16:
			return *(reinterpret_cast<int16_t*>(pointer));
			return;
		case is8:
			return *(reinterpret_cast<int8_t*>(pointer));
			return;
		}
	case vtPChar:
	case vtPItem:
	case vtPClient:
	case vtPVoid:
		return pointer;
	case vtString:
		if ( *(reinterpret_cast<std::string*>(pointer)) == "true" ) return true;
		if ( *(reinterpret_cast<std::string*>(pointer)) == "false" ) return false;
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
		return reinterpret_cast<pChar>(pointer);
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
		return reinterpret_cast<pItem>(pointer);
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
		return reinterpret_cast<pClient>(pointer);
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
		return pointer;
	default:
		if ( result ) *result = false;
		return NULL;
	}
}

uint64_t tVariant::toUInt64(bool *result) const
{
	switch( assignedType )
	{
	case vtString:
	{
		char *end;
		uint64_t tmp = strtoull( (*reinterpret_cast<std::string*>(pointer)).c_str(), &end, 0);
		if ( end == (*reinterpret_cast<std::string*>(pointer)).c_str() )
		{
			if ( result ) *result = false;
			return 0;
		}
		
		return tmp;
	}
	case vtUInt:
		switch(integerSize)
		{
		case is64:
			*result = true;
			return *(reinterpret_cast<uint64_t*>(pointer));
		case is32:
			*result = true;
			return *(reinterpret_cast<uint32_t*>(pointer));
		case is16:
			*result = true;
			return *(reinterpret_cast<uint16_t*>(pointer));
		case is8:
			*result = true;
			return *(reinterpret_cast<uint8_t*>(pointer));
		}
	case vtSInt:
		switch(integerSize)
		{
		case is64:
			*result = true;
			return abs(*(reinterpret_cast<int64_t*>(pointer)));
		case is32:
			*result = true;
			return abs(*(reinterpret_cast<int32_t*>(pointer)));
		case is16:
			*result = true;
			return abs(*(reinterpret_cast<int16_t*>(pointer)));
		case is8:
			*result = true;
			return abs(*(reinterpret_cast<int8_t*>(pointer)));
		}
	default:
		if ( result ) *result = false;
		return 0;
	}
}

uint32_t tVariant::toUInt32(bool *result) const
{
	switch( assignedType )
	{
	case vtString:
	{
		char *end;
		uint32_t tmp = strtoul( (*reinterpret_cast<std::string*>(pointer)).c_str(), &end, 0);
		if ( end == (*reinterpret_cast<std::string*>(pointer)).c_str() )
		{
			if ( result ) *result = false;
			return 0;
		}
		
		return tmp;
	}
	case vtUInt:
		switch(integerSize)
		{
		case is64:
			if ( result && *(reinterpret_cast<uint64_t*>(pointer)) > UINT32_MAX )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<uint64_t*>(pointer)) & 0xFFFFFFFF;
		case is32:
			*result = true;
			return *(reinterpret_cast<uint32_t*>(pointer));
		case is16:
			*result = true;
			return *(reinterpret_cast<uint16_t*>(pointer));
		case is8:
			*result = true;
			return *(reinterpret_cast<uint8_t*>(pointer));
		}
	case vtSInt:
		switch(integerSize)
		{
		case is64:
			if ( result && *(reinterpret_cast<int64_t*>(pointer)) > UINT32_MAX )
				*result = false;
			else
				*result = true;
			return abs(*(reinterpret_cast<int64_t*>(pointer))) & 0xFFFFFFFF;
		case is32:
			*result = true;
			return abs(*(reinterpret_cast<int32_t*>(pointer)));
		case is16:
			*result = true;
			return abs(*(reinterpret_cast<int16_t*>(pointer)));
		case is8:
			*result = true;
			return abs(*(reinterpret_cast<int8_t*>(pointer)));
		}
	default:
		if ( result ) *result = false;
		return 0;
	}
}

uint16_t tVariant::toUInt16(bool *result) const
{
	switch( assignedType )
	{
	case vtString:
	{
		char *end;
		uint32_t tmp = strtoul( (*reinterpret_cast<std::string*>(pointer)).c_str(), &end, 0);
		if ( end == (*reinterpret_cast<std::string*>(pointer)).c_str() )
		{
			if ( result ) *result = false;
			return 0;
		}
		
		if ( result && tmp > UINT16_MAX )
			*result = false;
		else
			*result = true;
		return tmp;
	}
	case vtUInt:
		switch(integerSize)
		{
		case is64:
			if ( result && *(reinterpret_cast<uint64_t*>(pointer)) > UINT16_MAX )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<uint64_t*>(pointer)) & 0xFFFF;
		case is32:
			if ( result && *(reinterpret_cast<uint32_t*>(pointer)) > UINT16_MAX )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<uint32_t*>(pointer)) & 0xFFFF;
		case is16:
			*result = true;
			return *(reinterpret_cast<uint16_t*>(pointer));
		case is8:
			*result = true;
			return *(reinterpret_cast<uint8_t*>(pointer));
		}
	case vtSInt:
		switch(integerSize)
		{
		case is64:
			if ( result && *(reinterpret_cast<int64_t*>(pointer)) > UINT16_MAX )
				*result = false;
			else
				*result = true;
			return abs(*(reinterpret_cast<int64_t*>(pointer))) & 0xFFFF;
		case is32:
			if ( result && *(reinterpret_cast<int32_t*>(pointer)) > UINT16_MAX )
				*result = false;
			else
				*result = true;
			return abs(*(reinterpret_cast<int32_t*>(pointer))) & 0xFFFF;
		case is16:
			*result = true;
			return abs(*(reinterpret_cast<int16_t*>(pointer)));
		case is8:
			*result = true;
			return abs(*(reinterpret_cast<int8_t*>(pointer)));
		}
	default:
		if ( result ) *result = false;
		return 0;
	}
}

uint8_t tVariant::toUInt8(bool *result) const
{
	switch( assignedType )
	{
	case vtString:
	{
		char *end;
		uint32_t tmp = strtoul( (*reinterpret_cast<std::string*>(pointer)).c_str(), &end, 0);
		if ( end == (*reinterpret_cast<std::string*>(pointer)).c_str() )
		{
			if ( result ) *result = false;
			return 0;
		}
		
		if ( result && tmp > UINT8_MAX )
			*result = false;
		else
			*result = true;
		return tmp;
	}
	case vtUInt:
		switch(integerSize)
		{
		case is64:
			if ( result && *(reinterpret_cast<uint64_t*>(pointer)) > UINT8_MAX )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<uint64_t*>(pointer)) & 0xFF;
		case is32:
			if ( result && *(reinterpret_cast<uint32_t*>(pointer)) > UINT8_MAX )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<uint32_t*>(pointer)) & 0xFF;
		case is16:
			if ( result && *(reinterpret_cast<uint16_t*>(pointer)) > UINT8_MAX )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<uint16_t*>(pointer)) & 0xFF;
		case is8:
			*result = true;
			return *(reinterpret_cast<uint8_t*>(pointer));
		}
	case vtSInt:
		switch(integerSize)
		{
		case is64:
			if ( result && *(reinterpret_cast<int64_t*>(pointer)) > UINT8_MAX )
				*result = false;
			else
				*result = true;
			return abs(*(reinterpret_cast<int64_t*>(pointer))) & 0xFF;
		case is32:
			if ( result && *(reinterpret_cast<int32_t*>(pointer)) > UINT8_MAX )
				*result = false;
			else
				*result = true;
			return abs(*(reinterpret_cast<int32_t*>(pointer))) & 0xFF;
		case is16:
			if ( result && *(reinterpret_cast<int16_t*>(pointer)) > UINT8_MAX )
				*result = false;
			else
				*result = true;
			return abs(*(reinterpret_cast<int16_t*>(pointer))) & 0xFF;
		case is8:
			*result = true;
			return abs(*(reinterpret_cast<int8_t*>(pointer)));
		}
	default:
		if ( result ) *result = false;
		return 0;
	}
}

int64_t tVariant::toSInt64(bool *result) const
{
	switch( assignedType )
	{
	case vtString:
	{
		char *end;
		uint64_t tmp = strtoll( (*reinterpret_cast<std::string*>(pointer)).c_str(), &end, 0);
		if ( end == (*reinterpret_cast<std::string*>(pointer)).c_str() )
		{
			if ( result ) *result = false;
			return 0;
		}
		
		return tmp;
	}
	case vtSInt:
		switch(integerSize)
		{
		case is64:
			*result = true;
			return *(reinterpret_cast<int64_t*>(pointer));
		case is32:
			*result = true;
			return *(reinterpret_cast<int32_t*>(pointer));
		case is16:
			*result = true;
			return *(reinterpret_cast<int16_t*>(pointer));
		case is8:
			*result = true;
			return *(reinterpret_cast<int8_t*>(pointer));
		}
	case vtUInt:
		switch(integerSize)
		{
		case is64:
			if ( *(reinterpret_cast<uint64_t*>(pointer)) > INT64_MAX )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<uint64_t*>(pointer)) & 0x7FFFFFFF;
		case is32:
			if ( *(reinterpret_cast<uint32_t*>(pointer)) > INT64_MAX )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<uint32_t*>(pointer)) & 0x7FFFFFFF;
		case is16:
			*result = true;
			return *(reinterpret_cast<uint16_t*>(pointer));
		case is8:
			*result = true;
			return *(reinterpret_cast<uint8_t*>(pointer));
		}
	default:
		if ( result ) *result = false;
		return 0;
	}
}

int32_t tVariant::toSInt32(bool *result) const
{
	switch( assignedType )
	{
	case vtString:
	{
		char *end;
		uint64_t tmp = strtol( (*reinterpret_cast<std::string*>(pointer)).c_str(), &end, 0);
		if ( end == (*reinterpret_cast<std::string*>(pointer)).c_str() )
		{
			if ( result ) *result = false;
			return 0;
		}
		
		return tmp;
	}
	case vtSInt:
		switch(integerSize)
		{
		case is64:
			if ( result && *(reinterpret_cast<int64_t*>(pointer)) > INT32_MAX || *(reinterpret_cast<int64_t*>(pointer)) < INT32_MIN )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<int64_t*>(pointer)) & 0x7FFFFFFF;
		case is32:
			*result = true;
			return *(reinterpret_cast<int32_t*>(pointer));
		case is16:
			*result = true;
			return *(reinterpret_cast<int16_t*>(pointer));
		case is8:
			*result = true;
			return *(reinterpret_cast<int8_t*>(pointer));
		}
	case vtUInt:
		switch(integerSize)
		{
		case is64:
			if ( *(reinterpret_cast<uint64_t*>(pointer)) > INT32_MAX )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<uint64_t*>(pointer)) & 0x7FFFFFFF;
		case is32:
			if ( *(reinterpret_cast<uint32_t*>(pointer)) > INT32_MAX )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<uint32_t*>(pointer)) & 0x7FFFFFFF;
		case is16:
			*result = true;
			return *(reinterpret_cast<uint16_t*>(pointer));
		case is8:
			*result = true;
			return *(reinterpret_cast<uint8_t*>(pointer));
		}
	default:
		if ( result ) *result = false;
		return 0;
	}
}

int16_t tVariant::toSInt16(bool *result) const
{
	switch( assignedType )
	{
	case vtString:
	{
		char *end;
		uint64_t tmp = strtol( (*reinterpret_cast<std::string*>(pointer)).c_str(), &end, 0);
		if ( end == (*reinterpret_cast<std::string*>(pointer)).c_str() )
		{
			if ( result ) *result = false;
			return 0;
		}
		
		return tmp;
	}
	case vtSInt:
		switch(integerSize)
		{
		case is64:
			if ( result && *(reinterpret_cast<int64_t*>(pointer)) > INT16_MAX || *(reinterpret_cast<int64_t*>(pointer)) < INT16_MIN )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<int64_t*>(pointer)) & 0x7FFF;
		case is32:
			if ( result && *(reinterpret_cast<int32_t*>(pointer)) > INT16_MAX || *(reinterpret_cast<int64_t*>(pointer)) < INT16_MIN )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<int32_t*>(pointer)) & 0x7FFF;
		case is16:
			*result = true;
			return *(reinterpret_cast<int16_t*>(pointer));
		case is8:
			*result = true;
			return *(reinterpret_cast<int8_t*>(pointer));
		}
	case vtUInt:
		switch(integerSize)
		{
		case is64:
			if ( *(reinterpret_cast<uint64_t*>(pointer)) > INT16_MAX )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<uint64_t*>(pointer)) & 0x7FFF;
		case is32:
			if ( *(reinterpret_cast<uint32_t*>(pointer)) > INT16_MAX )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<uint32_t*>(pointer)) & 0x7FFF;
		case is16:
			if ( *(reinterpret_cast<uint16_t*>(pointer)) > INT16_MAX )
				*result = false;
			else
				*result = true;
			*result = true;
			return *(reinterpret_cast<uint16_t*>(pointer)) & 0x7FFF;
		case is8:
			*result = true;
			return *(reinterpret_cast<uint8_t*>(pointer));
		}
	default:
		if ( result ) *result = false;
		return 0;
	}
}

int8_t tVariant::toSInt8(bool *result) const
{
	switch( assignedType )
	{
	case vtString:
	{
		char *end;
		uint64_t tmp = strtol( (*reinterpret_cast<std::string*>(pointer)).c_str(), &end, 0);
		if ( end == (*reinterpret_cast<std::string*>(pointer)).c_str() )
		{
			if ( result ) *result = false;
			return 0;
		}
		
		return tmp;
	}
	case vtSInt:
		switch(integerSize)
		{
		case is64:
			if ( result && *(reinterpret_cast<int64_t*>(pointer)) > INT16_MAX || *(reinterpret_cast<int64_t*>(pointer)) < INT16_MIN )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<int64_t*>(pointer)) & 0xFFFF;
		case is32:
			if ( result && *(reinterpret_cast<int32_t*>(pointer)) > INT16_MAX || *(reinterpret_cast<int64_t*>(pointer)) < INT16_MIN )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<int32_t*>(pointer)) & 0xFFFF;
		case is16:
			if ( result && *(reinterpret_cast<int16_t*>(pointer)) > INT16_MAX || *(reinterpret_cast<int16_t*>(pointer)) < INT16_MIN )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<int16_t*>(pointer)) & 0xFF;
		case is8:
			*result = true;
			return *(reinterpret_cast<int8_t*>(pointer));
		}
	case vtUInt:
		switch(integerSize)
		{
		case is64:
			if ( *(reinterpret_cast<uint64_t*>(pointer)) > INT16_MAX )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<uint64_t*>(pointer)) & 0x7FFF;
		case is32:
			if ( *(reinterpret_cast<uint32_t*>(pointer)) > INT16_MAX )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<uint32_t*>(pointer)) & 0x7FFF;
		case is16:
			if ( *(reinterpret_cast<uint16_t*>(pointer)) > INT16_MAX )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<uint16_t*>(pointer)) & 0x7FFF;
		case is8:
			if ( *(reinterpret_cast<uint8_t*>(pointer)) > INT16_MAX )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<uint8_t*>(pointer)) & 0x7F;
		}
	default:
		if ( result ) *result = false;
		return 0;
	}
}
