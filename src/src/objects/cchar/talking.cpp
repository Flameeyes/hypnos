/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
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
void cChar::sysmsg(const TEXT *txt, ...)
{
	va_list argptr;
	char msg[512];
	va_start( argptr, txt );

	vsnprintf( msg, sizeof(msg)-1,(const char *)txt, argptr );
	va_end( argptr );
	if (getClient() != NULL)
		getClient()->sysmsg(msg);
}

/*!
\author Luxor
\brief shows speech text to all pcs near the char
\param txt the speech
\param antispam use or not antispam
*/
void cChar::talkAll(TEXT *txt, bool antispam)
{
	NxwSocketWrapper sw;
	sw.fillOnline( this, false );

	for( sw.rewind(); !sw.isEmpty(); sw++ ) {
		NXWCLIENT ps=sw.getClient();
		if( ps!=NULL )
			talk(ps->toInt(), txt,antispam);
	}

}

/*!
\author Luxor
\brief Shows speech text of a char to the given char
\param s socket to send the speech to
\param txt the speech
\param antispam use or not antispam
*/
void cChar::talk(NXWSOCKET s, TEXT *txt, bool antispam)
{
	if( s < 0 || s >= now )
		return;

	bool machwas= true;

	if( antispam )
	{
		if( TIMEOUT(antispamtimer) )
			antispamtimer=uiCurrentTime+MY_CLOCKS_PER_SEC*10;
		else
			machwas = false;
	}

	if( machwas )
	{
		UI08 name[30]={ 0x00, };
		strcpy((char *)name, getCurrentNameC());

		saycolor=0x0481;

		if( npcaitype==NPCAI_EVIL )
		{
			saycolor=0x0026;
		}
		else if( npc && !tamed && !guarded && !war )
		{
			saycolor=0x005B;
		}

		SendSpeechMessagePkt(s, getSerial32(), getId(), 0, saycolor, fonttype, name, txt);
	}
}

/*!
\brief Shows emote text of a char
\author Luxor
\param socket socket to send the emote to
\param txt the emote
\param antispam use or not antispam
\todo document extra parameters
*/
void cChar::emote( NXWSOCKET socket, TEXT *txt, bool antispam, ... )
{
	bool sendEmote = true;
	emotecolor = 0x0026;

	if ( antispam )
	{
		if ( TIMEOUT( antispamtimer ) )
			antispamtimer=uiCurrentTime+MY_CLOCKS_PER_SEC*10;
		else
			sendEmote = false;
	}

	if ( sendEmote )
	{
		va_list argptr;
		char msg[512];
		va_start( argptr, antispam );
		vsnprintf( msg, sizeof( msg ) - 1, txt, argptr );
		va_end( argptr );

		UI08 name[30]={ 0x00, };
		strcpy((char *)name, getCurrentNameC());

		SendSpeechMessagePkt(socket, getSerial32(), getId(), 2, emotecolor, fonttype, name, msg);
	}
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
			antispamtimer=uiCurrentTime+MY_CLOCKS_PER_SEC*10;
		else
			sendEmote = false;
	}

	if( sendEmote )
	{
		va_list argptr;
		char msg[512];
		va_start( argptr, antispam );
		vsnprintf( msg, sizeof( msg ) - 1, txt, argptr );
		va_end( argptr );

		NxwSocketWrapper sw;
		sw.fillOnline( this, false );
		for( sw.rewind(); !sw.isEmpty(); sw++ )
		{
			NXWCLIENT ps=sw.getClient();
			if( ps!=NULL )
				emote( ps->toInt(), msg, 0 );
		}
	}

}

/*!
\author Luxor
\brief Shows runic speech text of a char to the given socket
\param s socket to send the speech to
\param txt speech
\param antispam use or not antispam
*/
void cChar::talkRunic(NXWSOCKET s, TEXT *txt, bool antispam)
{
	bool machwas;

	if (s<0) return;

	if (antispam)
	{
		if (TIMEOUT(antispamtimer))
		{
			antispamtimer=uiCurrentTime+MY_CLOCKS_PER_SEC*10;
			machwas = true;
		}
		else
			machwas = false;
	}
	else
		machwas = true;

	if (machwas)
	{
		UI08 name[30]={ 0x00, };
		strcpy((char *)name, getCurrentNameC());

		SendSpeechMessagePkt(s, getSerial32(), getId(), 0, 0x0001, 0x0008, name, txt);
	}
}

/*!
\author Luxor
\brief Shows runic speech text to all pcs near the char
\param txt the speech
\param antispam use or not antispam
*/
void cChar::talkAllRunic(TEXT *txt, bool antispam)
{

	NxwSocketWrapper sw;
	sw.fillOnline( this, false );
	for( sw.rewind(); !sw.isEmpty(); sw++ ) {
		NXWCLIENT ps=sw.getClient();
		if( ps!=NULL )
			talkRunic(ps->toInt(), txt, antispam);
	}
}

