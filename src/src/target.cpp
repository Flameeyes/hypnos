/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "common_libs.h"
#include "target.h"
#include "inlines.h"
#include "network.h"

uint32_t cTarget::serial_current = 0;

cTarget::cTarget( bool selectLocation )
{
	type = selectLocation;
	serial= ++serial_current;
}

cTarget::~cTarget()
{
}

void cTarget::send(pClient client)
{
	cPacketTargetingCursor< cServerPacket > pkg;
	pkg.type = type;
	pkg.cursor = serial;
	pkg.send( ps );
}

void cTarget::receive(pClient client)
{
	cPacketTargetingCursor<cClientPacket> pkg;
	pkg.receive( ps );

	clicked = pkg.clicked.get();
	model= pkg.model.get();
	loc = Location( pkg.x.get(), pkg.y.get(), pkg.z );
}

bool cTarget::isValid()
{
	if( type=1 && ( ( loc.x==UINVALID16 ) || ( loc.y==UINVALID16 ) ) )
		return false;
	if( type=0 && ( ( clicked==0 ) && ( model== 0 ) ) )
		return false;
	return true;
}

void cTarget::error(pClient client)
{
}

Location cTarget::getLocation()
{
	return loc;
}

uint32_t cTarget::getClicked()
{
	return clicked;
}

uint16_t cTarget::getModel()
{
	return model;
}


cObjectTarget::cObjectTarget() : cTarget( false )
{
}

cObjectTarget::~cObjectTarget()
{
}

bool cObjectTarget::isValid()
{
	return ( type==0 );
}

void cObjectTarget::error(pClient client)
{
	client->sysmessage( "Invalid object" );
}

cCharTarget::cCharTarget() : cObjectTarget()
{
}

cCharTarget::~cCharTarget()
{
}

bool cCharTarget::isValid()
{
	return ( type==0 ) && ( isCharSerial( clicked ) && ( MAKE_CHAR_REF( clicked )!=NULL ) );
}

void cCharTarget::error(pClient client)
{
	client->sysmessage( "Invalid character" );
}

cItemTarget::cItemTarget() : cObjectTarget()
{
}

cItemTarget::~cItemTarget()
{
}

bool cItemTarget::isValid()
{
	return ( type==0 ) && ( isItemSerial( clicked ) && MAKE_ITEM_REF( clicked )!=NULL );
}

void cItemTarget::error(pClient client)
{
	client->sysmessage( "Invalid item" );
}

cLocationTarget::cLocationTarget() : cTarget( true )
{
}

cLocationTarget::~cLocationTarget()
{
}

bool cLocationTarget::isValid()
{
	return ( type==1 ) && ( ( loc.x!=UINVALID16 ) && ( loc.y!=UINVALID16 ) );
}

void cLocationTarget::error(pClient client)
{
	client->sysmessage( "Invalid location" );
}


/*
void amxCallbackOld(pClient client, pTarget t )
{
	if( t->amx_callback==NULL) 
		return;
	
	pChar pc = cSerializable::findCharBySerial( t->getClicked() );
	if( pc ) {
        t->amx_callback->Call( ps->currCharIdx(), pc->getSerial(), INVALID, INVALID, INVALID, INVALID );
        return;
    }

    pItem pi = cSerializable::findItemBySerial( t->getClicked() );
    if( pi ) {
		t->amx_callback->Call( ps->currCharIdx(), INVALID, pi->getSerial(), INVALID, INVALID, INVALID );
        return;
    }

    Location loc = t->getLocation();
	t->amx_callback->Call( ps->currCharIdx(), INVALID, INVALID, loc.x, loc.y, loc.z );
}*/

void amxCallback(pClient client, pTarget t )
{
	if( t->amx_callback==NULL) 
		return;

	int16_t model = t->getModel();
	if( model == 0 )
		model = INVALID;

	/// targ_serial, chr, obj, x, y, z, model, param

	pObject po = objects.findObject( t->getClicked() );
	if( po ) {
		t->amx_callback->Call( t->serial, ps->currCharIdx(), po->getSerial(), INVALID, INVALID, INVALID, model, t->buffer[0] );
		return;
	}
	else {
	    Location loc = t->getLocation();
        t->amx_callback->Call( t->serial, ps->currCharIdx(), INVALID, loc.x, loc.y, loc.z, model, t->buffer[0] );
	}
}


pTarget createTarget( TARG_TYPE type )
{
	switch( type ) {
		case TARG_CHAR:
			return new cCharTarget();
		case TARG_ITEM:
			return new cItemTarget();
		case TARG_OBJ:
			return new cObjectTarget();
		case TARG_LOC:
			return new cLocationTarget();
		case TARG_ALL:
		default:
			return new cTarget();
	}

}











///////////////////////////////////////////////////////////////////////
//
// TARGETLOCATION CLASS
//
///////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
// Function name     : TargetLocation::TargetLocation
// Author            : Xanathar
// Changes           : none yet
void TargetLocation::init(pChar pc)
{
	Location pcpos= pc->getPosition();

	m_pc = pc;
	m_x = pcpos.x;
	m_y = pcpos.y;
	m_z = pcpos.z;
	m_pi = NULL;
	m_piSerial = INVALID;
	m_pcSerial = pc->getSerial();
}
///////////////////////////////////////////////////////////////////
// Function name     : void TargetLocation::init
// Author            : Xanathar
// Changes           : none yet
void TargetLocation::init(pItem pi)
{
	m_pc = NULL;
	if (pi->isInWorld()) {
		m_x = pi->getPosition().x;
		m_y = pi->getPosition().y;
		m_z = pi->getPosition().z;
	} else {
		m_x = m_y = m_z = 0;
	}
	m_pi = pi;
	m_piSerial = pi->getSerial();
	m_pcSerial = INVALID;
}

///////////////////////////////////////////////////////////////////
// Function name     : TargetLocation::init
// Author            : Xanathar
// Changes           : none yet
void TargetLocation::init(int x, int y, int z)
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_pi = NULL;
	m_pc = NULL;
	m_piSerial = m_pcSerial = INVALID;
}

/*!
\brief Recalculate item & char from their serial
\author Xanathar
\todo Is really needed? BTW is actually right?
*/
void TargetLocation::revalidate()
{
	m_pi=cSerializable::findItemBySerial(m_piSerial);

	m_pc=cSerializable::findCharBySerial(m_piSerial);

	if ( m_pi ) {
		m_pi = NULL;
		m_piSerial = INVALID;
	}

	if ( m_pc ) {
		m_pc = NULL;
		m_pcSerial = INVALID;
	}
}

/*!
\brief extends item data to x,y,z and eventually owner char
\brief Xanathar
*/
void TargetLocation::extendItemTarget()
{
	if ( ! m_pc || ! m_pi )
		return;
	
	pObject outmost = m_pi->getOutMostContainer();
	if ( ! m_pi->getOutMostContainer()->isInWorld() )
		outmost = m_pi->getOutMostContainer()->getContainer();
	
	m_x = outmost->getPosition().x;
	m_y = outmost->getPosition().y;
	m_z = outmost->getPosition().z
	
	m_pcSerial = outmost->toBody() ? outmost->toBody()->getChar()->getSerial() : INVALID;
	
	revalidate();
}

///////////////////////////////////////////////////////////////////
// Function name     : TargetLocation::TargetLocation
// Author            : Xanathar
// Changes           : none yet
TargetLocation::TargetLocation( pTarget pp )
{
	Location loc = pp->getLocation();
	init( loc.x, loc.y, loc.z );
	if( pp->type==0 ) {

		pChar pc= cSerializable::findCharBySerial( pp->getClicked() );
		if(pc) {
			init(pc);
			return;
		}

		pItem pi= cSerializable::findItemBySerial( pp->getClicked() );
		if (pi)  {
			init(pi);
			return;
		}
	}
	else if( pp->type==1 ) {
		return;
	}
// Wintermute: Always return a valid location, or spells can't target ground
	m_pc=NULL;
	m_pcSerial=INVALID;
	m_pi=NULL;
	m_piSerial=INVALID;
/*
	m_x=0;
	m_y=0;
	m_z=0;
*/
}


