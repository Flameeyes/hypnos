/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief cChar's talking methods
*/

/*!
\brief easy wrapper to sysmsg
\author Luxor
\param txt as default
\param ... as default
*/
void cChar::sysmsg(const char *txt, ...)
{
	if ( ! getClient() )
		return;
	
	va_list argptr;
	char *msg;
	va_start( argptr, txt );

	vasprintf( &msg, txt, argptr );
	va_end( argptr );
	getClient()->sysmsg(msg);
	free(msg);
}

/*!
\author Luxor
\brief shows speech text to all pcs near the char
\param txt the speech
\param antispam use or not antispam
*/
void cChar::talkAll(char *txt, bool antispam)
{
	NxwSocketWrapper sw;
	sw.fillOnline( this, false );

	for( sw.rewind(); !sw.isEmpty(); sw++ ) {
		pClient ps=sw.getClient();
		if( ps!=NULL )
			talk(ps->toInt(), txt,antispam);
	}

}

/*!
\author Luxor
\brief Shows speech text of a char to the given client
\param client client to send the speech to
\param txt the speech
\param antispam use or not antispam
*/
void cChar::talk(pClient client, char *txt, bool antispam)
{
	if( s < 0 || s >= now )
		return;

	bool machwas= true;

	if( antispam )
	{
		if( TIMEOUT(antispamtimer) )
			antispamtimer=getclock()+SECS*10;
		else
			machwas = false;
	}

	if( machwas )
	{
		uint8_t name[30]={ 0x00, };
		strcpy((char *)name, getCurrentName().c_str());

		saycolor=0x0481;

		if( npcaitype==NPCAI_EVIL )
		{
			saycolor=0x0026;
		}
		else if( npc && !tamed && !guarded && !war )
		{
			saycolor=0x005B;
		}
		cSpeech speech(std::string(txt));	//we must use string constructor or else it is supposed to be an unicode packet
		speech.setColor(saycolor);
		speech.setFont(fonttype);
		speech.setMode(0x00);		// normal speech
		speech.setSpeaker(this);

		nPackets::Sent::Speech pk(speech);
		getClient()->sendPacket(&pk);
	}
}

/*!
\brief Shows emote text of a char
\author Luxor
\param client client to send the emote to
\param txt the emote
\param antispam use or not antispam
\todo document extra parameters
*/
void cChar::emote( pClient client, char *txt, bool antispam, ... )
{
	bool sendEmote = true;
	emotecolor = 0x0026;

	if ( antispam )
	{
		if ( TIMEOUT( antispamtimer ) )
			antispamtimer=getclock()+SECS*10;
		else
			sendEmote = false;
	}

	if ( ! sendEmote )
		return;
	
	va_list argptr;
	char *msg;
	va_start( argptr, antispam );
	vasprintf( &msg, txt, argptr );
	va_end( argptr );

	uint8_t name[30]={ 0x00, };
	strcpy((char *)name, getCurrentName().c_str());

	cSpeech speech(std::string(msg));	//we must use string constructor or else it is supposed to be an unicode packet
	speech.setColor(emotecolor);
	speech.setFont(fonttype);
	speech.setMode(0x02);		// Emote
	speech.setSpeaker(this);

	nPackets::Sent::Speech pk(speech);
	getClient()->sendPacket(&pk);

	free(msg);
}

/*!
\brief Shows emote text of a char to all near pcs
\param txt the emote
\param antispam use or not antispam
\todo document extra parameters
*/
void cChar::emoteall( char *txt, bool antispam, ... )
{
	bool sendEmote = true;

	if ( antispam )
	{
		if ( TIMEOUT( antispamtimer ) )
			antispamtimer=getclock()+SECS*10;
		else
			sendEmote = false;
	}

	if( ! sendEmote )
		return;
		
	va_list argptr;
	char *msg;
	va_start( argptr, antispam );
	vasprintf( &msg, txt, argptr );
	va_end( argptr );

	NxwSocketWrapper sw;
	sw.fillOnline( this, false );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		pClient ps=sw.getClient();
		if( ps!=NULL )
			emote( ps->toInt(), msg, 0 );
	}

	free(msg);
}

/*!
\author Luxor
\brief Shows runic speech text of a char to the given socket
\param client client to send the speech to
\param txt speech
\param antispam use or not antispam
*/
void cChar::talkRunic(pClient client, char *txt, bool antispam)
{
	bool machwas;

	if (s<0) return;

	if (antispam)
	{
		if (TIMEOUT(antispamtimer)) antispamtimer=getclock()+SECS*10;
		else return;
	}

	cSpeech speech(std::string(txt));	//we must use string constructor or else it is supposed to be an unicode packet
	speech.setColor(0x1);
	speech.setFont(0x0008);		// runic font
	speech.setMode(0x00);		// normal speech
	speech.setSpeaker(this);

	nPackets::Sent::Speech pk(speech);
	getClient()->sendPacket(&pk);

}

/*!
\author Luxor
\brief Shows runic speech text to all pcs near the char
\param txt the speech
\param antispam use or not antispam
*/
void cChar::talkAllRunic(char *txt, bool antispam)
{

	NxwSocketWrapper sw;
	sw.fillOnline( this, false );
	for( sw.rewind(); !sw.isEmpty(); sw++ ) {
		pClient ps=sw.getClient();
		if( ps!=NULL )
			talkRunic(ps->toInt(), txt, antispam);
	}
}

