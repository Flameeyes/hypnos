/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "objects/citem/cboath"
#include "objects/cclienth"

cBoat::cBoat() : cItem()
{
}

cBoat::cBoat(uint32_t nserial) : cItem(nserial)
{
}

void cBoat::MoveTo(sLocation newpos)
{
	//!\todo Calc items position to move them as well
	cItem::MoveTo(newpos);
}

/*!
\brief Moves the board one tile in the direction specified
\param client Client who requested the move
\param dir Direction to move the boat (if not specified, move it in the direction it is)
*/
void cBoat::step(pClient client, uint8_t dir)
{
	int8_t tx = 0, ty = 0;
	
	if ( dir == 0xFF ) dir = getDirection();
	
	// This packet pauses the client before move the board
	nPackets::Sent::PauseClient pk(0x01);
	client->sendPacket(&pk);

	switch(dir&0x07)//Which DIR is it going in?
	{
	case 0:
		--ty;
		break;
	case 1:
		++tx;
		--ty;
		break;
	case 2:
		++tx;
		break;
	case 3:
		++tx;
		++ty;
		break;
	case 4:
		++ty;
		break;
	case 5:
		--tx;
		++ty;
		break;
	case 6:
		--tx;
		break;
	case 7:
		--tx;
		--ty;
		break;
	}


	sLocation boatpos = getPosition();
	boatpos.x += tx; boatpos.y += ty;

	if( ( boatpos.x <= XBORDER || boatpos.x >= ((map_width*8)-XBORDER))
		|| (boatpos.y <= YBORDER || boatpos.y >= ((map_height*8)-YBORDER))) //bugfix LB
	{
		type2=0;
		itemtalk(tillerMan,"Arr, Sir, we've hit rough waters!");
		nPackets::Sent::PauseClient pk(0x00);
		client->sendPacket(&pk);
		return;
	}

	if(!good_position(this, boatpos, 0))
	{
		type2=0;
		itemtalk(tillerMan, "Arr, somethings in the way!");
		nPackets::Sent::PauseClient pk(0x00);
		client->sendPacket(&pk);
		return;
	}
	if(collision(this, boatpos,0))
	{
		type2=0;
		itemtalk(tillerMan, "Arr, another ship in the way");
		nPackets::Sent::PauseClient pk(0x00);
		client->sendPacket(&pk);

		return;
	}

	sLocation tillerpos = tillerMan->getPosition();
	tillerpos.x += tx; tillerpos.y += ty;

	sLocation p1pos = plankLeft->getPosition();
	p1pos.x += tx; p1pos.y += ty;

	sLocation p2pos = plankRight->getPosition();
	p2pos.x += tx; p2pos.y += ty;

	sLocation holdpos= hold->getPosition();
	holdpos.x += tx; holdpos.y += ty;

	//Move all the special items
	MoveTo( boatpos );
	tillerMan->MoveTo( tillerpos );
	plankLeft->MoveTo( p1pos );
	plankRight->MoveTo( p2pos );
	hold->MoveTo( holdpos );

//!\todo wait until set hav appropriate function
#if 0
	for (a=0;a<imultisp[serial%HASHMAX].max;a++)  // move all item upside the boat
	{
		c=imultisp[serial%HASHMAX].pointer[a];
		if(c!=-1)
		{
			pItem pi= MAKE_ITEMREF_LOGGED(c,err);
			if(!err)
			{
				mapRegions->remove(pi);
				sLocation itmpos= pi->getPosition();
				itmpos.x+= tx;
				itmpos.y+= ty;
				pi->setPosition( itmpos );
				pi->Refresh();
				mapRegions->add(pi);
			}
		}
	}

	for (a=0;a<cmultisp[serial%HASHMAX].max;a++) // move all char upside the boat
	{
		c=cmultisp[serial%HASHMAX].pointer[a];
		if (c!=-1)
		{
		   pc_c=MAKE_CHARREF_LOGGED(c,err);
		   if (!err)
		   {
			   sLocation charpos= pc_c->getPosition();
			   mapRegions->remove(pc_c);
			   charpos.x+= tx;
			   charpos.y+= ty;
			   pc_c->MoveTo(charpos);
			   pc_c->teleport();
			   mapRegions->add(pc_c);
		   }
		}
	}
#endif

	//! Removes the pause in the client
	nPackets::Sent::PauseClient pk(0x00);
	client->sendPacket(&pk);
	
	Refresh();
	tillerMan->Refresh();
	plankLeft->Refresh();
	plankRight->Refresh();
	hold->Refresh();
}
