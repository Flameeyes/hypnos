/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator (Utility Library)                              |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in LICENSE file.               |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "libhypnos/cvariant.hpp"
#include "libhypnos/hypstl/vector.hpp"

#define throw_if_not_int case isNotInt: throw eInvalidInteger(this);

namespace nLibhypnos {

//! Inline function to recalc the uint size
inline void cVariant::recalcUIntSize(const uint64_t &val)
{
	if ( val > UINT32_MAX )
		integerSize = is64;
	else if ( val > UINT16_MAX )
		integerSize = is32;
	else if ( val > UINT8_MAX )
		integerSize = is16;
	else
		integerSize = is8;
}

//! Inline function to recalc the sint size
inline void cVariant::recalcSIntSize(const int64_t &val)
{
	if ( val > INT32_MAX || val < INT32_MIN )
		integerSize = is64;
	else if ( val > INT16_MAX || val < INT16_MIN )
		integerSize = is32;
	else if ( val > INT8_MAX || val < INT8_MIN )
		integerSize = is16;
	else
		integerSize = is8;
}

//! Default constructor
cVariant::cVariant()
{
	integerSize = isNotInt;
	assignedType = vtNull;
	pointer = NULL;
}

cVariant::~cVariant()
{
	clear();
}

/*!
\brief Clear the cVariant type

This function is used to reset the cVariant at the null state. It deletes
eventual values stored inside and sets the type to vtNull.

It's called by the by the ~cVariant() destructor and by the assignment
operators to have a clean cVariant where to store the new value.

The only types which won't be deleted are the pointers, all the types.
*/
void cVariant::clear()
{
	switch(assignedType)
	{
	case vtString:
		delete reinterpret_cast<string*>(pointer);
		pointer = NULL;
		break;
	
	case vtVector:
		delete reinterpret_cast<cVariantVector*>(pointer);
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
			
			throw_if_not_int
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
			
			throw_if_not_int
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
cVariant &cVariant::operator =(const string &astr)
{
	clear();
	pointer = new string(astr);

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
cVariant &cVariant::operator =(const bool &aval)
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
cVariant &cVariant::operator =(const uint64_t &aval)
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
	
	throw_if_not_int
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
cVariant &cVariant::operator =(const int64_t &aval)
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
	
	throw_if_not_int
	}
	
	assignedType = vtSInt;
	return *this;
}

/*!
\brief Assignment operator for a pointer value
\param aptr Pointer to set the internal pointer to
\return A reference to the instance itself
*/
cVariant &cVariant::operator =(void *aptr)
{
	pointer = aptr;
	assignedType = vtPVoid;
	integerSize = isNotInt;
	return *this;
}

//! \copydoc cVariant::operator=(void*)
cVariant &cVariant::operator =(pChar aptr)
{
	pointer = aptr;
	assignedType = vtPChar;
	integerSize = isNotInt;
	return *this;
}

//! \copydoc cVariant::operator=(void*)
cVariant &cVariant::operator =(pItem aptr)
{
	pointer = aptr;
	assignedType = vtPItem;
	integerSize = isNotInt;
	return *this;
}

//! \copydoc cVariant::operator=(void*)
cVariant &cVariant::operator =(pClient aptr)
{
	pointer = aptr;
	assignedType = vtPClient;
	integerSize = isNotInt;
	return *this;
}

cVariant cVariant::operator -() const
{
	switch(assignedType)
	{
	case vtUInt:
		if ( uint64_t(*this) > INT64_MAX )
			return cVariant();
		
		return cVariant(-uint64_t(*this));
	case vtSInt:
		return cVariant(-int64_t(*this));
	
	default: // We can't invert the sign of a non-integer
		return cVariant();
	}
}

cVariant cVariant::operator +(const cVariant &param) const
{
	switch(assignedType)
	{
	case vtNull:
		return param;
	case vtString:
		return cVariant(string(*this) + string(param));
	case vtUInt:
		return cVariant(uint64_t(*this) + uint64_t(param));
	case vtSInt:
		return cVariant(int64_t(*this) + int64_t(param));
	default:
		return cVariant();
	}
}

cVariant cVariant::operator -(const cVariant &param) const
{
	switch(assignedType)
	{
	case vtNull:
		return -param;
	case vtUInt:
		return cVariant(uint64_t(*this) - uint64_t(param));
	case vtSInt:
		return cVariant(int64_t(*this) - int64_t(param));
	default:
		return cVariant();
	}
}

cVariant cVariant::operator ++(int unused)
{
	cVariant tmp = *this;
	*this += uint64_t(1);
	return tmp;
}

cVariant cVariant::operator --(int unused)
{
	cVariant tmp = *this;
	*this -= uint64_t(1);
	return tmp;
}

bool cVariant::operator ==(const cVariant &param) const
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
		return string(*this) == string(param);
	case vtPVoid:
	case vtPChar:
	case vtPItem:
	case vtPClient:
		return pointer == (void*)(param);
	default:
		return false;
	}
}

bool cVariant::operator <(const cVariant &param) const
{
	switch(assignedType)
	{
	case vtUInt:
		return uint64_t(*this) < uint64_t(param);
	case vtSInt:
		return int64_t(*this) < int64_t(param);
	case vtString:
		return strcmp( string(*this).c_str(), string(param).c_str()) < 0;
	default:
		return false;
	}
}

bool cVariant::operator >(const cVariant &param) const
{
	switch(assignedType)
	{
		case vtUInt:
			return uint64_t(*this) > uint64_t(param);
		case vtSInt:
			return int64_t(*this) > int64_t(param);
		case vtString:
			return strcmp( string(*this).c_str(), string(param).c_str()) > 0;
		default:
			return false;
	}
}

bool cVariant::convertInString()
{
	bool conv;
	string temp = toString(&conv);
	
	if ( ! conv )
		return false;
	
	*this = temp;
	return true;
}

bool cVariant::convertInBoolean()
{
	bool conv;
	bool temp = toBoolean(&conv);
	
	if ( ! conv )
		return false;
	
	*this = temp;
	return true;
}

bool cVariant::convertInUInt()
{
	bool conv;
	uint64_t temp = toUInt64(&conv);
	
	if ( ! conv )
		return false;
	
	*this = temp;
	return true;
}

bool cVariant::convertInSInt()
{
	bool conv;
	int64_t temp = toSInt64(&conv);
	
	if ( ! conv )
		return false;
	
	*this = temp;
	return true;
}

bool cVariant::convertInPChar()
{
	bool conv;
	pChar temp = toPChar(&conv);
	
	if ( ! conv )
		return false;
	
	*this = temp;
	return true;
}

bool cVariant::convertInPItem()
{
	bool conv;
	pItem temp = toPItem(&conv);
	
	if ( ! conv )
		return false;
	
	*this = temp;
	return true;
}

bool cVariant::convertInPClient()
{
	bool conv;
	pClient temp = toPClient(&conv);
	
	if ( ! conv )
		return false;
	
	*this = temp;
	return true;
}

bool cVariant::convertInPVoid()
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
string cVariant::toString(bool *result) const
{
	if ( result ) *result = true;
	switch( assignedType )
	{
	case vtString:
		return *(reinterpret_cast<string*>(pointer));
	case vtSInt:
	{
		char *tmp;
		switch(integerSize)
		{
		case is64:
			asprintf(&tmp, "%lli", *(reinterpret_cast<int64_t*>(pointer)));
			break;
		case is32:
			asprintf(&tmp, "%i", *(reinterpret_cast<int32_t*>(pointer)));
			break;
		case is16:
			asprintf(&tmp, "%hi", *(reinterpret_cast<int16_t*>(pointer)));
			break;
		case is8:
			asprintf(&tmp, "%hhi", *(reinterpret_cast<int8_t*>(pointer)));
			break;
		
		throw_if_not_int
		}
		string ret(tmp);
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
			break;
		case is32:
			asprintf(&tmp, "%u", *(reinterpret_cast<uint32_t*>(pointer)));
			break;
		case is16:
			asprintf(&tmp, "%hu", *(reinterpret_cast<uint16_t*>(pointer)));
			break;
		case is8:
			asprintf(&tmp, "%hhu", *(reinterpret_cast<uint8_t*>(pointer)));
			break;
		
		throw_if_not_int
		}
		string ret(tmp);
		free(tmp);
		return ret;
	}
	case vtBoolean:
		return *(reinterpret_cast<bool*>(pointer)) ? string("true") : string("false");
	
	case vtPChar:
	case vtPItem:
	case vtPClient:
	case vtPVoid:
		{
			char *tmp;
			asprintf(&tmp, "%p", pointer);
			string ret(tmp);
			free(tmp);
			return ret;
		}
	case vtNull:
		return string("[nil]");
	default:
		if ( result ) *result = false;
		return string("[Undefined]");
	}
}

bool cVariant::toBoolean(bool *result) const
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
		case is32:
			return *(reinterpret_cast<uint32_t*>(pointer));
		case is16:
			return *(reinterpret_cast<uint16_t*>(pointer));
		case is8:
			return *(reinterpret_cast<uint8_t*>(pointer));
		
		throw_if_not_int
		}
	case vtSInt:
		switch(integerSize)
		{
		case is64:
			return *(reinterpret_cast<int64_t*>(pointer));
		case is32:
			return *(reinterpret_cast<int32_t*>(pointer));
		case is16:
			return *(reinterpret_cast<int16_t*>(pointer));
		case is8:
			return *(reinterpret_cast<int8_t*>(pointer));
		
		throw_if_not_int
		}
	case vtPChar:
	case vtPItem:
	case vtPClient:
	case vtPVoid:
		return pointer;
	case vtString:
		if ( *(reinterpret_cast<string*>(pointer)) == "true" ) return true;
		if ( *(reinterpret_cast<string*>(pointer)) == "false" ) return false;
	default:
		// If not true or false
		if ( result ) *result = false;
		return false;
	}
}

pChar cVariant::toPChar(bool *result) const
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

pItem cVariant::toPItem(bool *result) const
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

pClient cVariant::toPClient(bool *result) const
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

void *cVariant::toPVoid(bool *result) const
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

uint64_t cVariant::toUInt64(bool *result) const
{
	switch( assignedType )
	{
	case vtString:
	{
		char *end;
		uint64_t tmp = strtoull( (*reinterpret_cast<string*>(pointer)).c_str(), &end, 0);
		if ( end == (*reinterpret_cast<string*>(pointer)).c_str() )
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
		
		throw_if_not_int
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
		
		throw_if_not_int
		}
	default:
		if ( result ) *result = false;
		return 0;
	}
}

uint32_t cVariant::toUInt32(bool *result) const
{
	switch( assignedType )
	{
	case vtString:
	{
		char *end;
		uint32_t tmp = strtoul( (*reinterpret_cast<string*>(pointer)).c_str(), &end, 0);
		if ( end == (*reinterpret_cast<string*>(pointer)).c_str() )
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
		
		throw_if_not_int
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
		
		throw_if_not_int
		}
	default:
		if ( result ) *result = false;
		return 0;
	}
}

uint16_t cVariant::toUInt16(bool *result) const
{
	switch( assignedType )
	{
	case vtString:
	{
		char *end;
		uint32_t tmp = strtoul( (*reinterpret_cast<string*>(pointer)).c_str(), &end, 0);
		if ( end == (*reinterpret_cast<string*>(pointer)).c_str() )
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
		
		throw_if_not_int
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
		
		throw_if_not_int
		}
	default:
		if ( result ) *result = false;
		return 0;
	}
}

uint8_t cVariant::toUInt8(bool *result) const
{
	switch( assignedType )
	{
	case vtString:
	{
		char *end;
		uint32_t tmp = strtoul( (*reinterpret_cast<string*>(pointer)).c_str(), &end, 0);
		if ( end == (*reinterpret_cast<string*>(pointer)).c_str() )
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
		
		throw_if_not_int
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
		
		throw_if_not_int
		}
	default:
		if ( result ) *result = false;
		return 0;
	}
}

int64_t cVariant::toSInt64(bool *result) const
{
	switch( assignedType )
	{
	case vtString:
	{
		char *end;
		uint64_t tmp = strtoll( (*reinterpret_cast<string*>(pointer)).c_str(), &end, 0);
		if ( end == (*reinterpret_cast<string*>(pointer)).c_str() )
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
		
		throw_if_not_int
		}
	case vtUInt:
		switch(integerSize)
		{
		case is64:
			if ( *(reinterpret_cast<uint64_t*>(pointer)) > INT64_MAX )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<uint64_t*>(pointer)) & 0x7FFFFFFFFFFFFFFFll;
		case is32:
			*result = true;
			return *(reinterpret_cast<uint32_t*>(pointer)) & 0x7FFFFFFF;
		case is16:
			*result = true;
			return *(reinterpret_cast<uint16_t*>(pointer));
		case is8:
			*result = true;
			return *(reinterpret_cast<uint8_t*>(pointer));
		
		throw_if_not_int
		}
	default:
		if ( result ) *result = false;
		return 0;
	}
}

int32_t cVariant::toSInt32(bool *result) const
{
	switch( assignedType )
	{
	case vtString:
	{
		char *end;
		uint64_t tmp = strtol( (*reinterpret_cast<string*>(pointer)).c_str(), &end, 0);
		if ( end == (*reinterpret_cast<string*>(pointer)).c_str() )
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
		
		throw_if_not_int
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
		
		throw_if_not_int
		}
	default:
		if ( result ) *result = false;
		return 0;
	}
}

int16_t cVariant::toSInt16(bool *result) const
{
	switch( assignedType )
	{
	case vtString:
	{
		char *end;
		uint64_t tmp = strtol( (*reinterpret_cast<string*>(pointer)).c_str(), &end, 0);
		if ( end == (*reinterpret_cast<string*>(pointer)).c_str() )
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
		
		throw_if_not_int
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
		
		throw_if_not_int
		}
	default:
		if ( result ) *result = false;
		return 0;
	}
}

int8_t cVariant::toSInt8(bool *result) const
{
	switch( assignedType )
	{
	case vtString:
	{
		char *end;
		uint64_t tmp = strtol( (*reinterpret_cast<string*>(pointer)).c_str(), &end, 0);
		if ( end == (*reinterpret_cast<string*>(pointer)).c_str() )
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
			if ( result && *(reinterpret_cast<int64_t*>(pointer)) > INT8_MAX || *(reinterpret_cast<int64_t*>(pointer)) < INT8_MIN )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<int64_t*>(pointer)) & 0xFFFF;
		case is32:
			if ( result && *(reinterpret_cast<int32_t*>(pointer)) > INT8_MAX || *(reinterpret_cast<int64_t*>(pointer)) < INT8_MIN )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<int32_t*>(pointer)) & 0xFFFF;
		case is16:
			if ( result && *(reinterpret_cast<int16_t*>(pointer)) > INT8_MAX || *(reinterpret_cast<int16_t*>(pointer)) < INT8_MIN )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<int16_t*>(pointer)) & 0xFF;
		case is8:
			*result = true;
			return *(reinterpret_cast<int8_t*>(pointer));
		
		throw_if_not_int
		}
	case vtUInt:
		switch(integerSize)
		{
		case is64:
			if ( *(reinterpret_cast<uint64_t*>(pointer)) > INT8_MAX )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<uint64_t*>(pointer)) & 0x7F;
		case is32:
			if ( *(reinterpret_cast<uint32_t*>(pointer)) > INT8_MAX )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<uint32_t*>(pointer)) & 0x7F;
		case is16:
			if ( *(reinterpret_cast<uint16_t*>(pointer)) > INT8_MAX )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<uint16_t*>(pointer)) & 0x7F;
		case is8:
			if ( *(reinterpret_cast<uint8_t*>(pointer)) > INT8_MAX )
				*result = false;
			else
				*result = true;
			return *(reinterpret_cast<uint8_t*>(pointer)) & 0x7F;
		
		throw_if_not_int
		}
	default:
		if ( result ) *result = false;
		return 0;
	}
}

}
