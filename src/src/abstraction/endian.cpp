  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include "nxwcommn.h"
#include "abstraction/endian.h"

euint16_t::euint16_t()
{
	a=0;
}

euint16_t::euint16_t(uint16_t v)
{
	a=htons( v );
}

euint16_t& euint16_t::operator =( uint16_t v )
{
	a=htons( v );
	return (*this);
}

euint16_t& euint16_t::operator=( euint16_t v )
{
	this->a=v.a;
	return (*this);
}


uint16_t euint16_t::get(  )
{
	return ntohs( this->a );
	//return this->a;
}






euint32_t::euint32_t()
{
	a=0;
}

euint32_t::euint32_t(uint32_t v)
{
	a=htonl( v );
}

euint32_t& euint32_t::operator =( uint32_t v )
{
	a=htonl( v );
	return (*this);
}

euint32_t& euint32_t::operator=( euint32_t v )
{
	this->a=v.a;
	return (*this);
}

uint32_t euint32_t::get(  )
{
	return ntohl( this->a );
}

