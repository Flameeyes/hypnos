/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "common_libs.h"
#include "networking/network.h"
#include "commands.h"
#include "menu.h"
#include "dragdrop.h"
#include "speech.h"
#include "data.h"
#include "trade.h"
#include "sndpkg.h"
#include "walking.h"
#include "inlines.h"
#include "newbies.h"
#include "skills/skills.h"
#include "encryption/clientcrypt.h"

cNetwork	*Network;


static unsigned int bit_table[257][2] =
{
{0x0002, 0x0000}, {0x0005, 0x001F}, {0x0006, 0x0022}, {0x0007, 0x0034}, {0x0007, 0x0075}, {0x0006, 0x0028}, {0x0006, 0x003B}, {0x0007, 0x0032},
{0x0008, 0x00E0}, {0x0008, 0x0062}, {0x0007, 0x0056}, {0x0008, 0x0079}, {0x0009, 0x019D}, {0x0008, 0x0097}, {0x0006, 0x002A}, {0x0007, 0x0057},
{0x0008, 0x0071}, {0x0008, 0x005B}, {0x0009, 0x01CC}, {0x0008, 0x00A7}, {0x0007, 0x0025}, {0x0007, 0x004F}, {0x0008, 0x0066}, {0x0008, 0x007D},
{0x0009, 0x0191}, {0x0009, 0x01CE}, {0x0007, 0x003F}, {0x0009, 0x0090}, {0x0008, 0x0059}, {0x0008, 0x007B}, {0x0008, 0x0091}, {0x0008, 0x00C6},
{0x0006, 0x002D}, {0x0009, 0x0186}, {0x0008, 0x006F}, {0x0009, 0x0093}, {0x000A, 0x01CC}, {0x0008, 0x005A}, {0x000A, 0x01AE}, {0x000A, 0x01C0},
{0x0009, 0x0148}, {0x0009, 0x014A}, {0x0009, 0x0082}, {0x000A, 0x019F}, {0x0009, 0x0171}, {0x0009, 0x0120}, {0x0009, 0x00E7}, {0x000A, 0x01F3},
{0x0009, 0x014B}, {0x0009, 0x0100}, {0x0009, 0x0190}, {0x0006, 0x0013}, {0x0009, 0x0161}, {0x0009, 0x0125}, {0x0009, 0x0133}, {0x0009, 0x0195},
{0x0009, 0x0173}, {0x0009, 0x01CA}, {0x0009, 0x0086}, {0x0009, 0x01E9}, {0x0009, 0x00DB}, {0x0009, 0x01EC}, {0x0009, 0x008B}, {0x0009, 0x0085},
{0x0005, 0x000A}, {0x0008, 0x0096}, {0x0008, 0x009C}, {0x0009, 0x01C3}, {0x0009, 0x019C}, {0x0009, 0x008F}, {0x0009, 0x018F}, {0x0009, 0x0091},
{0x0009, 0x0087}, {0x0009, 0x00C6}, {0x0009, 0x0177}, {0x0009, 0x0089}, {0x0009, 0x00D6}, {0x0009, 0x008C}, {0x0009, 0x01EE}, {0x0009, 0x01EB},
{0x0009, 0x0084}, {0x0009, 0x0164}, {0x0009, 0x0175}, {0x0009, 0x01CD}, {0x0008, 0x005E}, {0x0009, 0x0088}, {0x0009, 0x012B}, {0x0009, 0x0172},
{0x0009, 0x010A}, {0x0009, 0x008D}, {0x0009, 0x013A}, {0x0009, 0x011C}, {0x000A, 0x01E1}, {0x000A, 0x01E0}, {0x0009, 0x0187}, {0x000A, 0x01DC},
{0x000A, 0x01DF}, {0x0007, 0x0074}, {0x0009, 0x019F}, {0x0008, 0x008D}, {0x0008, 0x00E4}, {0x0007, 0x0079}, {0x0009, 0x00EA}, {0x0009, 0x00E1},
{0x0008, 0x0040}, {0x0007, 0x0041}, {0x0009, 0x010B}, {0x0009, 0x00B0}, {0x0008, 0x006A}, {0x0008, 0x00C1}, {0x0007, 0x0071}, {0x0007, 0x0078},
{0x0008, 0x00B1}, {0x0009, 0x014C}, {0x0007, 0x0043}, {0x0008, 0x0076}, {0x0007, 0x0066}, {0x0007, 0x004D}, {0x0009, 0x008A}, {0x0006, 0x002F},
{0x0008, 0x00C9}, {0x0009, 0x00CE}, {0x0009, 0x0149}, {0x0009, 0x0160}, {0x000A, 0x01BA}, {0x000A, 0x019E}, {0x000A, 0x039F}, {0x0009, 0x00E5},
{0x0009, 0x0194}, {0x0009, 0x0184}, {0x0009, 0x0126}, {0x0007, 0x0030}, {0x0008, 0x006C}, {0x0009, 0x0121}, {0x0009, 0x01E8}, {0x000A, 0x01C1},
{0x000A, 0x011D}, {0x000A, 0x0163}, {0x000A, 0x0385}, {0x000A, 0x03DB}, {0x000A, 0x017D}, {0x000A, 0x0106}, {0x000A, 0x0397}, {0x000A, 0x024E},
{0x0007, 0x002E}, {0x0008, 0x0098}, {0x000A, 0x033C}, {0x000A, 0x032E}, {0x000A, 0x01E9}, {0x0009, 0x00BF}, {0x000A, 0x03DF}, {0x000A, 0x01DD},
{0x000A, 0x032D}, {0x000A, 0x02ED}, {0x000A, 0x030B}, {0x000A, 0x0107}, {0x000A, 0x02E8}, {0x000A, 0x03DE}, {0x000A, 0x0125}, {0x000A, 0x01E8},
{0x0009, 0x00E9}, {0x000A, 0x01CD}, {0x000A, 0x01B5}, {0x0009, 0x0165}, {0x000A, 0x0232}, {0x000A, 0x02E1}, {0x000B, 0x03AE}, {0x000B, 0x03C6},
{0x000B, 0x03E2}, {0x000A, 0x0205}, {0x000A, 0x029A}, {0x000A, 0x0248}, {0x000A, 0x02CD}, {0x000A, 0x023B}, {0x000B, 0x03C5}, {0x000A, 0x0251},
{0x000A, 0x02E9}, {0x000A, 0x0252}, {0x0009, 0x01EA}, {0x000B, 0x03A0}, {0x000B, 0x0391}, {0x000A, 0x023C}, {0x000B, 0x0392}, {0x000B, 0x03D5},
{0x000A, 0x0233}, {0x000A, 0x02CC}, {0x000B, 0x0390}, {0x000A, 0x01BB}, {0x000B, 0x03A1}, {0x000B, 0x03C4}, {0x000A, 0x0211}, {0x000A, 0x0203},
{0x0009, 0x012A}, {0x000A, 0x0231}, {0x000B, 0x03E0}, {0x000A, 0x029B}, {0x000B, 0x03D7}, {0x000A, 0x0202}, {0x000B, 0x03AD}, {0x000A, 0x0213},
{0x000A, 0x0253}, {0x000A, 0x032C}, {0x000A, 0x023D}, {0x000A, 0x023F}, {0x000A, 0x032F}, {0x000A, 0x011C}, {0x000A, 0x0384}, {0x000A, 0x031C},
{0x000A, 0x017C}, {0x000A, 0x030A}, {0x000A, 0x02E0}, {0x000A, 0x0276}, {0x000A, 0x0250}, {0x000B, 0x03E3}, {0x000A, 0x0396}, {0x000A, 0x018F},
{0x000A, 0x0204}, {0x000A, 0x0206}, {0x000A, 0x0230}, {0x000A, 0x0265}, {0x000A, 0x0212}, {0x000A, 0x023E}, {0x000B, 0x03AC}, {0x000B, 0x0393},
{0x000B, 0x03E1}, {0x000A, 0x01DE}, {0x000B, 0x03D6}, {0x000A, 0x031D}, {0x000B, 0x03E5}, {0x000B, 0x03E4}, {0x000A, 0x0207}, {0x000B, 0x03C7},
{0x000A, 0x0277}, {0x000B, 0x03D4}, {0x0008, 0x00C0}, {0x000A, 0x0162}, {0x000A, 0x03DA}, {0x000A, 0x0124}, {0x000A, 0x01B4}, {0x000A, 0x0264},
{0x000A, 0x033D}, {0x000A, 0x01D1}, {0x000A, 0x01AF}, {0x000A, 0x039E}, {0x000A, 0x024F}, {0x000B, 0x0373}, {0x000A, 0x0249}, {0x000B, 0x0372},
{0x0009, 0x0167}, {0x000A, 0x0210}, {0x000A, 0x023A}, {0x000A, 0x01B8}, {0x000B, 0x03AF}, {0x000A, 0x018E}, {0x000A, 0x02EC}, {0x0007, 0x0062},
{0x0004, 0x000D}
};

int MTsend( pClient client, char* xoutbuffer, int len, int boh )
{
	pClient clientent, loopexit=30;
	while( --loopexit>0 )
	{
		sent = send( client[ socket ], xoutbuffer, len, boh );

		if( sent < 0 && errno == 11 )
		{
			timeval timeout;
			timeout.tv_sec= 0;
			timeout.tv_usec= 5000;
			fd_set sockFD;
			FD_ZERO(&sockFD);
			FD_SET( socket, &sockFD);
			if( select( socket, NULL, &sockFD, NULL, &timeout ) == INVALID )
				LogError("Error selecting socket %i (%s)", errno, strerror( errno ) );
		}
		else
			break;
	}
	return sent;
}

void cNetwork::DoStreamCode( pClient clientocket )
{
	pClient clienttatus ;
/*
	FILE *debugout=fopen("d:\\packets.log", "a");
	for ( int i = 0; i < boutlength[socket];++i)
	{
		if ( i % 32 == 0 )
		{
			fprintf(debugout, "\n0x%04x ", i);
		}
		fprintf(debugout, "%02x ", outbuffer[socket][i]);
	}
	fprintf(debugout, "\n------------------------------------------", i);
	fclose(debugout);
*/
	int len = Pack( outbuffer[socket], xoutbuffer, boutlength[socket] );
	// ConOut("Packed %d bytes input to %d bytes out\n", boutlength[socket], len);
	
	pChar pc_currchar= (client)? client->currChar() : NULL;
	if ( clientCrypter[socket] != NULL && clientCrypter[socket]->getCryptVersion() >= CRYPT_3_0_0c )
		clientCrypter[socket] ->encrypt((unsigned char *) &xoutbuffer[0], (unsigned char *) &xoutbuffer[0], len);
	else if (pc_currchar != NULL && pc_currchar->getCrypter() != NULL && pc_currchar->getCrypter()->getCryptVersion() >= CRYPT_3_0_0c )
		pc_currchar->getCrypter()->encrypt((unsigned char *) &xoutbuffer[0], (unsigned char *) &xoutbuffer[0], len);
	
	if ((status = MTsend(socket, xoutbuffer, len, MSG_NOSIGNAL)) == SOCKET_ERROR)
	{
   		LogSocketError("Socket Send error %s\n", errno) ;
	}

}


void cNetwork::FlushBuffer( pClient client ) // Sends buffered data at once
{

	pClient clienttatus ;
	if ( boutlength[ socket ] > 0 )
	{
		if ( clientInfo[ socket ]->compressOut )
		{
			DoStreamCode( socket );
		}
		else
		{
			if((status = MTsend( socket, (char*)outbuffer[socket], boutlength[socket], MSG_NOSIGNAL))==SOCKET_ERROR)
			{
				LogSocketError("Socket Send Error %s\n", errno) ;
			}
		}
		boutlength[socket]=0;
	}
}

void cNetwork::ClearBuffers() // Sends ALL buffered data
{
	for ( int i = 0; i < now; ++i )
		FlushBuffer( i );
}

void cNetwork::xSend( pClient client, const void *point, int length  ) // Buffering send function
{
	if( socket == INVALID || socket > MAXCLIENT )
	{
		LogWarning("XSend called with an invalid socket");
		return;
	}

	if ( boutlength[ socket ] + length > MAXBUFFER )
		FlushBuffer( socket );
	// Still too long
	if ( boutlength[ socket ] + length > MAXBUFFER )
	{
		memcpy( &outbuffer[ socket ][ boutlength[ socket ] ], point, MAXBUFFER );
		boutlength[ socket ] += MAXBUFFER;
		FlushBuffer( socket );
		xSend(socket, (char *)point + (int )MAXBUFFER, length - MAXBUFFER );
	}
	else
	{
		memcpy( &outbuffer[ socket ][ boutlength[ socket ] ], point, length );
		boutlength[ socket ] += length;
	}
}

void cNetwork::xSend(pClient client, wstring& p, bool alsoTermination )
{
	if( socket == INVALID || socket > MAXCLIENT )
	{
		LogWarning("XSend called with an invalid socket");
		return;
	}

//Luxor: if we are in a system which uses 32bit wchar_t, we must truncate them because UO client supports only 16bit unicode chars.
// So let's use uint16_t

	int32_t size = sizeof( uint16_t );
	int32_t length = p.length() * size;
	if ( alsoTermination ) length += size;
	
	if ( boutlength[ socket ] + length > MAXBUFFER )
		FlushBuffer( socket );

	wstring::iterator point( p.begin() ), end( p.end() );
	uint16_t* b = (uint16_t*)&outbuffer[ socket ][ boutlength[ socket ] ];

	int32_t i = 0;
	for( ; point!=end; point++, ++i )
		b[i] = htons(*point);

	if( alsoTermination )
		b[ i ] = 0x0000;

	boutlength[ socket ] += length;
}

void cNetwork::LoginMain(pClient client)
{
	signed long int i;
	uint32_t chrSerial;

	acctno[s]=INVALID;
	int length=0x3e;
	int j;
	unsigned char decryptPacket[MAXBUFFER+1];
	unsigned char cryptPacket[MAXBUFFER+1];
	ClientCrypt * crypter;
	memcpy(&cryptPacket[0], &buffer[s][0],length);
	if ( clientCrypter[s] != NULL )
	{
		crypter = clientCrypter[s] ;
		for ( j = 0 ; j <= CRYPT_3_0_6j;++j)
		{
			memcpy(&cryptPacket[0], &buffer[s][0],length);
			crypter->setLoginCryptKeys(loginKeys [j] [0], loginKeys [j] [1]);
			crypter->setCryptMode(CRYPT_LOGIN);
			crypter->setGameEncryption(j+1);
			crypter->init(&clientSeed[s][0]);
			crypter->decrypt(&cryptPacket[0], decryptPacket, length);
			if ( decryptPacket[0x3e-1] != 0xFF )
				continue;
				
			// crypt key is correct only if Authentication is ok
			memcpy (&cryptPacket[0], decryptPacket, length);
			pSplit((char*)&cryptPacket[31]);
			i = Accounts->verifyPassword((char*)&cryptPacket[1], (char*)pass1);
			if ( i < 0 )
				continue;
			
			memcpy(&buffer[s][0],&cryptPacket[0] ,length);
			// is this client logged in already ?
			if ( ! Accounts->IsOnline(i) )
				break;
				
			// Look for accountno
			for ( int j = 0;j < s;++j)
			{
				if ( acctno[j] != i ) continue;
					
				// Found logged in account
				acctno[j]->disconnect();
				break;
			}
		}
	}
	pSplit((char*)&buffer[s][31]);
	i = Accounts->Authenticate((char*)&buffer[s][1], (char*)pass1);

	if( i >= 0 )
		acctno[s] = i;
	else
	{
		switch(i)  //Let's check for the error message
		{
		case BAD_PASSWORD:
			loginchars[s] = NULL;
			nPackets::Sent::LoginDenied pkNoPass(0x03);
			client->sendPacket(&pkNoPass);
			return;
		case ACCOUNT_BANNED:
			loginchars[s] = NULL;
			nPackets::Sent::LoginDenied pkAcctBlk(0x02);
			client->sendPacket(&pkAcctBlk);
			return;
		case LOGIN_NOT_FOUND:
			if( !SrvParms->auto_a_create )
			{
				loginchars[s] = NULL;
				nPackets::Sent::LoginDenied pkNoAcct(0x00);
				client->sendPacket(&pkNoAcct);
				return;
			} else {
				// Auto create is enable, let's create the new account.
				std::string dummylogin = (char*)&buffer[s][1], dummypass = (char*)&buffer[s][31];

				// Let's check if password isn't blank:
				if (dummypass.empty())
				{
					// User forgot password, let's send a message and return
					nPackets::Sent::LoginDenied pkNoPass(0x03);
					client->sendPacket(&pkNoPass);
					return;
				}
				acctno[s] = Accounts->CreateAccount(dummylogin, dummypass);
			}
		}
	}

	pAccount acc = cAccount::findAccount(name);
	if ( acc->currClient() )
	{
		nPackets::Sent::LoginDenied pkAcctUsed(0x01);
		client->sendPacket(&pkAcctUsed);
		//<Luxor>: Let's kick the current player
		
		//!\todo We actually want to kick already logged in player or not?
		
		acc->currClient()->kick();
		return;
	}

	//ndEndy now better
	if (acctno[s]!=INVALID)
	{
		Accounts->SetEntering(acctno[s]);
		Login2(client);
	}
}

void cNetwork::Login2(pClient client)
{
	//! \todo outbound packet 0xa8 
	static const char msgLogin[] = "Client [%s] connected [first] using Account '%s'.\n";

	uint16_t i, tlen;
	uint32_t ip;
	uint8_t newlist1[6]={ 0xA8, 0x00, };
	uint8_t newlist2[40]={ 0x00, };

	InfoOut( (char*)msgLogin, inet_ntoa(client_addr.sin_addr), &buffer[s][1] );
	if (SrvParms->server_log)
		ServerLog.Write( (char*)msgLogin, inet_ntoa(client_addr.sin_addr), &buffer[s][1] );

	tlen = 6 + (servcount*40);
	ShortToCharPtr(tlen, newlist1 +1);
	newlist1[3]=0xFF;			// System Info flag
	ShortToCharPtr(servcount, newlist1 +4);
	Xsend(s, newlist1, 6);

	for( i = 0; i < servcount; ++i )
	{
		ShortToCharPtr(i+1, newlist2);

		strcpy((char*)&newlist2[2], serv[i][0]);
		newlist2[34]=0x12;		// %Full
		newlist2[35]=0x01;		// Timezone
		ip=inet_addr(serv[i][1]); 	  // Host-Order
		ip = htonl(ip);			  // swap if needs
		LongToCharPtr(ip, newlist2 +36);  //Network order ...
		Xsend(s, newlist2, 40);
	}
//AoS/	Network->FlushBuffer(client);
}


void cNetwork::Relay(pClient client) // Relay player to a certain IP
{
	uint32_t ip;
	int port;

	ip=inet_addr(serv[buffer[s][2]-1][1]);
	port=str2num(serv[buffer[s][2]-1][2]);

	// Enable autodetect unless you bind to a specific ip address
        // otherwise you should lead in some security issue
        if(ServerScp::g_nAutoDetectIP || ServerScp::g_nBehindNat) { //Luxor
		//Xan : plug'n'play mode under windows :)
                //Rik : and for linux too ;) (should run on other bsd compatible systems too)
		socklen_t n = sizeof(sockaddr_in);
                sockaddr_in sa;
		getsockname (client[s], (sockaddr*)(&sa), (socklen_t*)(&n));
		uint32_t oldip = ip;
		//Luxor: REALLY tricky... i should change this soon, but no time to make it better by now :P
		uint32_t pClient clienterverip = sa.sin_addr.s_addr;
		uint32_t clientip = client_addr.sin_addr.s_addr;
//		printf("IP: %d.%d.%d.%d\n", IPPRINTF(ip));
//		printf("SERVER_IP: %d.%d.%d.%d\n", IPPRINTF(serverip));
//		printf("CLIENT_IP: %d.%d.%d.%d\n", IPPRINTF(clientip));
		if (ServerScp::g_nBehindNat) { //if the server is behind a NAT, use the autodetection only if the client is in the same LAN
			if (((serverip&0xFF) == (clientip&0xFF)) && (((serverip>>8)&0xFF) == ((clientip>>8)&0xFF)))
				ip = serverip;
		} else
			ip = serverip;
                if (ip != oldip)
			InfoOut("client %d relayed to IP %d.%d.%d.%d instead of %d.%d.%d.%d\n", s, IPPRINTF(ip), IPPRINTF(oldip));
        }


	uint32_t key = calcserial('a', 'K', 'E', 'Y');

	nPackets::Sent::ConnectToGameServer pk(ip, port, key);
	client->sendPacket(&pk);

	srand(ip+acctno[s]+now+getclock()); // Perform randomize
#ifdef ENCRYPTION
	if ( clientCrypter[s] != NULL )
	{
		ClientCrypt *crypter = clientCrypter[s];
		// unsigned int loginseed = clientip[s][3] + ( clientip[s][2] << 8) + (clientip[s][1] << 16) + ( clientip[s][0] << 24);
		key = 1 + ( 127 << 24 );
		crypter->setCryptSeed(loginseed);
		crypter->setCryptMode(CRYPT_GAME);
		crypter->setEntering(true);
		// memcpy(login03 +7, &clientip[s][0],4);
//		LongToCharPtr (loginseed, login03 +7); // set crypt key
		// if ( clientCrypter[s]->getCryptVersion() < CRYPT_3_0_0c )
//		LongToCharPtr (ip, login03 +1); // set game server ip
	}
#endif
	LongToCharPtr(key, login03 +7);	// New Server Key!
	Xsend(s, login03, 11);
	Network->FlushBuffer(client);
}

void cNetwork::ActivateFeatures(pClient client)
{
	uint16_t features = 0;  //<-- BitMask ?
	// 0x0001 => Button Chat ( T2A ??? )
	// 0x0003 => LBR (+ T2A)
	// 0x801F => AoS and previous .... (AoS + LBR + T2A)
	// 0xFFFF => ... (*ALL* <grin>)

	switch(server_data.feature)
	{
		case 1:	// T2A Features, button chat, popup help ..
			features |= T2A;
			break;
		case 2: // LBR plus previous vers.
			features |= (LBR | T2A);
			break;
		default:  // I don't know, what you want :P
			return;
			break;
	}

	nPackets::Sent::Features pk(features);
	client->sendPacket(&pk);
}

void cNetwork::GoodAuth(pClient client)
{
	uint32_t j;
	uint16_t tlen;
	uint8_t login04a[4]={ 0xA9, 0x09, 0x24, 0x02 }, n = startcount;

	tlen=4+(5*60)+1+(startcount*63) +4;

	ShortToCharPtr(tlen, login04a +1);

	Accounts->OnLogin(acctno[s],s);

	//Endy now much fast
	NxwCharWrapper sc;
	Accounts->GetAllChars( acctno[s], sc );

	ActivateFeatures(client);

	login04a[3] = sc.size(); //Number of characters found
	Xsend(s, login04a, 4);

	j=0;

	uint8_t login04b[60]={ 0, };

	for ( sc.rewind(); !sc.isEmpty(); sc++ )
	{
		pChar pc_a=sc.getChar();
		if( ! pc_a )
			continue;

		strcpy((char*)login04b, pc_a->getCurrentName().c_str());
		Xsend(s, login04b, 60);
		j++;
	}

	uint32_t i=0;
	memset(login04b, 0, 60);
	for ( i=j;i<5;i++)
	{
		Xsend(s, login04b, 60);
	}

	Xsend(s, &n, 1);  // startcount

	uint8_t login04d[63]= { 0, };

	for (i=0;i<startcount;i++)
	{
		memset(login04d, 0, 63);
		login04d[0]=i;
		
		strncpy(login04d+1, nNewbies::startLocations[i]->city.c_str(), 30);
		strncpy(login04d+32, nNewbies::startLocations[i]->place.c_str(), 30);
		Xsend(s, login04d, 63);
	}

	uint8_t tail[4]={0, }; //Fix for new clients, else it stuck on "Connecting ..."

	if(server_data.feature == 2) 	// LBR: NPC Popup Menu   (not currently impl.)
		tail[3] = 0x08;
	Xsend(s, tail, 4);
//AoS/	Network->FlushBuffer(client);
}

void cNetwork::CharList(pClient client) // Gameserver login and character listing
{

	int32_t i;

	acctno[s]=-1;

	pSplit((char*)&buffer[s][35]);
	i = Accounts->Authenticate((char*)&buffer[s][5], (char*)pass1);

	if( i >= 0 )
		acctno[s] = i;
	else
	{
		switch(i)  //Let's check for the error message
		{
		case BAD_PASSWORD:
			loginchars[s] = NULL;
			nPackets::Sent::LoginDenied pkNoPass(0x03);
			client->sendPacket(&pkNoPass);
			return;
		case ACCOUNT_BANNED:
			loginchars[s] = NULL;
			nPackets::Sent::LoginDenied pkAcctBlk(0x02);
			client->sendPacket(&pkAcctBlk);
			return;
		case LOGIN_NOT_FOUND:
			loginchars[s] = NULL;
			nPackets::Sent::LoginDenied pkNoAcct(0x00);
			client->sendPacket(&pkNoAcct);
			return;
		}
	}

	if (acctno[s] >= 0)
		GoodAuth(client);
}

void cNetwork::pSplit (char *pass0) // Split login password into Hypnos password and UO password
{

	int i,loopexit=0;
	i=0;
	pass1[0]=0;
	while ( (pass0[i]!='/') && (pass0[i]!=0) && (++loopexit < MAXLOOPS) ) i++;
	strncpy(pass1,pass0,i);
	pass1[i]=0;
	if (pass0[i]!=0) strcpy(pass2, pass0+i+1);

}

void cNetwork::charplay (pClient client) // After hitting "Play Character" button //Instalog
{
	if ( ! client )
		return;

	loginchars[s] = NULL;

	pChar pc_k=NULL;

	if ( acctno[s] == INVALID )
		return;
	
	int j=0;
	Accounts->SetOffline(acctno[s]);
	NxwCharWrapper sc;
	Accounts->GetAllChars( acctno[s], sc );
	for( sc.rewind(); !sc.isEmpty(); sc++ ) {
		pChar pc_i=sc.getChar();
		if(!pc_i)
			continue;
		if (j==buffer[s][0x44]) {
			pc_k=pc_i;
			break;
		}
		j++;
	}

	if ( pc_k )
	{
		pc_k->setClient(NULL);
		int32_t nSer = pc_k->getSerial();
		for ( int32_t idx = 0; idx < now; idx++ ) {
			if ( pc_k == loginchars[idx] ) {
				//!\todo We need to fix this!!!
				nPackets::Sent::IdleWarning pk(0x5);
				client->sendPacket(&pk);
				client->disconnect();
				idx->disconnect();
				return;
			}
		}

		Accounts->SetOnline(acctno[s], pc_k);
		pc_k->logout=INVALID;

		loginchars[s] = pc_k;

		pc_k->setClient(new cNxwClientObj(client));
		startchar(client);
	}
	else
	{
		nPackets::Sent::IdleWarning pk(0x5);
		client->sendPacket(&pk);
		client->disconnect();
	}
}

void cNetwork::enterchar(pClient client)
{
	pPC pc = NULL;
	if ( ! client || ! ( pc = client->currChar() ) )
		return;
	
	uint8_t techstuff[5]={ 0x69, 0x00, 0x05, 0x01, 0x00 };
	uint8_t world[6]={0xBF, 0x00, 0x06, 0x00, 0x08, 0x00};
	uint8_t modeset[5]={0x72, 0x00, 0x00, 0x32, 0x00};

	if (map_height<300) world[5]=0x02;

	clientInfo[s]->ingame=true;

	Xsend(s, world, 6);
	Network->FlushBuffer(client);

	nPackets::Sent::LoginConfirmation pkLoginConf(pc);
	client->sendPacket(&pkLoginConf);

	pc->war=0;
	Xsend(s, modeset, 5);
	Network->FlushBuffer(client);

	techstuff[3]=0x01;
	Xsend(s, techstuff, 5);
	Network->FlushBuffer(client);

	techstuff[3]=0x02;
	Xsend(s, techstuff, 5);
	Network->FlushBuffer(client);

	techstuff[3]=0x03;
	Xsend(s, techstuff, 5);
	Network->FlushBuffer(client);

	nPackets::Sent::StartGame pkStartGame;
	client->sendPacket(&pkStartGame);

	nPackets::Sent::GameTime pkGameTime;
	client->sendPacket(&pkGameTime);

	pc->spiritspeaktimer=getclock();
	pc->begging_timer=getclock();

	pc->stealth=INVALID;
	if (!(pc->IsGMorCounselor()))
		pc->setHidden(htUnhidden);

	pc->wresmove=0;	//Luxor

	Calendar::commitSeason(pc);
	pc->teleport();
}


//! Send character startup stuff to player
void cNetwork::startchar(pClient client)
{
	pPC pc = NULL;
	if ( ! client || ! ( pc = client->currChar() ) )
		return;
	
	pc->setCrypter(clientCrypter[s]);
	//<Luxor>: possess stuff
	if (pc->possessedSerial != INVALID) {
		pChar pcPos = cSerializable::findCharBySerial(pc->possessedSerial);
		if ( pcPos ) {
			currchar[s] = pcPos->getSerial();
			pcPos->setClient(new cNxwClientObj(client));
			pcPos->setCrypter(clientCrypter[s]);
			pc->setCrypter(NULL);
			pc->setClient(NULL);
			Accounts->SetOffline(pc->account);
			Accounts->SetOnline(pc->account, pcPos);
		} else pc->possessedSerial = INVALID;
	}
	//</Luxor>
	clientCrypter[s]=NULL;
	char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var
	char temp2[TEMP_STR_SIZE]; //xan -> this overrides the global temp var

	AMXEXECSV( pc->getSerial(),AMXT_SPECIALS, 4, AMX_BEFORE);

	enterchar( s );
	Network->FlushBuffer(client);

	client->sysmessage(0x058, "Hypnos %s [%s]", strVersion, OS);
	Network->FlushBuffer(client);

	// log last time signed on
	time_t ltime;
	time( &ltime );

	if (SrvParms->joinmsg)
	{
		if (!strcmp(pc->getCurrentName().c_str(), "pty Slot --")) pc->setCurrentName("A new Character");//AntiChrist
		sysbroadcast("%s entered the realm",pc->getCurrentName().c_str());//message upon entering a server
	}

	// very stupid stuff
	//pc->murderrate=getclock()+repsys.murderdecay*SECS; // LB, bugfix for murder-count getting --'ed each start
	pc->murderrate=getclock()+pc->murdersave*SECS;

	nPackets::Sent::TipsWindow pkMOTD(nMOTD::getMOTD());
	client->sendPacket(&pkMOTD);

	const char * t;
	stringVector::const_iterator vis( clientsAllowed.begin() ), vis_end( clientsAllowed.end() );

	t = (*vis).c_str();
	strcpy(temp,t);

	AMXEXECSV(pc->getSerial(),AMXT_SPECIALS, 4, AMX_AFTER);

	if ( !(strcmp(temp, "ALL") ) )
	{
  	  client->sysmessage("There is NO client version checking active on this shard. The recommanded-dev-team-supported client version for this server version is client version %s though", strSupportedClient);
	  return;

	} else if ( !(strcmp(temp, "SERVER_DEFAULT") ) )
	{
	  client->sysmessage("This shard requires the recommanded-dev-team-supported client version for this server version client version %s", strSupportedClient);
	  return;
	}
	else
	{
	   sprintf(idname, "This shard requires client version[s] %s",temp);
	}

	// remark: although it doesn't look good [without], don't add /n's
	// the (2-d) client doesn't like them

	temp2[0]=0;
	++vis;
	for ( ; vis != vis_end; ++vis)
	{
		t = (*vis).c_str();
		strcpy(temp,t);
		strcat(temp2," or ");
		strcat(temp2,temp);
	}

	strcat(idname, temp2);
	strcat(idname," The Hypnos team recommanded client is ");
	strcat(idname, strSupportedClient);

	client->sysmessage(idname);
	Network->FlushBuffer(client);
}

char cNetwork::LogOut(pClient client)//Instalog
{
	if (s < 0 || s >= now) return 0; //Luxor

	pChar pc = cSerializable::findCharBySerial(currchar[s]);
	if ( ! pc ) return 0;

	uint32_t a, valid=0;
	sLocation charpos= pc->getPosition();
	uint32_t x= charpos.x, y= charpos.y;


	AMXEXECSVNR(pc->getSerial(),AMXT_SPECIALS, 8, AMX_BEFORE);

	pItem pack;
	for(a=0;a<logoutcount;a++)
	{
		if (logout[a].x1<=x && logout[a].y1<=y && logout[a].x2>=x && logout[a].y2>=y)
		{
			valid=1;
			break;
		}
	}

	if(pc->IsGMorCounselor() || pc->account==0) valid=1;

	pMulti p_multi = pc->getMulti();
	if ( !p_multi )
		p_multi = cMulti::getAt( pc->getPosition() );

	if ( p_multi && !valid && (pack = pc->getBackpack()) )
	{ //It they are in a multi... and it's not already valid (if it is why bother checking?)
	
		pItem key = NULL;
		
		while( (key = pack->searchForType(ITYPE_KEY)) )
		{
			if ( p_multi->getSerial() == key->more1.more )
			{
				valid = true;
				break;
			}
		}
	}

	Accounts->SetOffline(pc->account);
	if (valid)//||region[chars[p].region].priv&0x17)
	{
		pc->logout=INVALID; // LB bugfix, was timeout
	} else {
		if ( clientInfo[s]->ingame )
		{
		    pc->logout=getclock()+SrvParms->quittime*SECS;
		}
	}

	NxwSocketWrapper sw;
	sw.fillOnline( pc, true );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		pClient client = sw.getSocket();
		if ( ! client ) return;
		
		client->sendchar(pc, false);
	}
	return valid;
}

/*!
\brief Receive data from a socket
\param client the client to receive from
\param x the number of bytes to receive
\param a the buffer offset
\return the number of actually read bytes
*/
int cNetwork::Receive(pClient client, int x, int a )
{
	if ( (x+a) >= MAXBUFFER) return 0;

	int count,loopexit=0;
	do
	{
		if((count = recv(client[s], (char*)&buffer[s][a], x, MSG_NOSIGNAL))==SOCKET_ERROR)
			LogSocketError("Socket Recv Error %s\n", errno) ;

	}
	while ( (count!=x) && (count>0) && (++loopexit < MAXBUFFER ));

	return count;
}

void cNetwork::sockInit()
{
	int bcode;

	initClients();


	kr=1;
	faul=0;

#ifdef WIN32
	if ( ! winSockInit() )
	{
		keeprun=false;
		error=1;
		kr=0;
		faul=1;
	}
#endif

	a_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (a_socket < 0 )
	{
		ErrOut("ERROR: Unable to create socket\n");
		keeprun=false;
		error=1;
		kr=0;
		faul=1;
		return;
	}

	g_nMainTCPPort = str2num(serv[0][2]);

	len_connection_addr=sizeof (struct sockaddr_in);
	connection.sin_family=AF_INET;
	connection.sin_addr.s_addr=INADDR_ANY;		// All interfaces
	connection.sin_port=htons(g_nMainTCPPort);	// Port

	bcode = bind(a_socket, (struct sockaddr *)&connection, len_connection_addr);

	if (bcode!=0)
	{
		sockManageError(bcode); // OS-Dependant error managing
		
		keeprun=false;
		error=1;
		kr=0;
		faul=1;
		return;
	}

	bcode = listen(a_socket, MAXCLIENT);

	if (bcode!=0)
	{
		ErrOut("ERROR: Unable to set socket in listen mode  - Error code: %i\n",bcode);
		keeprun=false;
		error=1;
		kr=0;
		faul=1;
		return;
	}
	
	// Ok, we need to set this socket (or listening one as non blocking).  The reason is we d a
	// select, and then do an accept.  However, if the client has terminated the connection between the small
	// time from the select and accept, we would block (accept is blocking).  So, set it non blocking
	uint32_t nonzero = 1;
#ifdef WINSOCK
	bcode = WSAAsyncSelect (a_socket, g_HWnd, 0, 0);
#else
	ioctlsocket(a_socket,FIONBIO,&nonzero) ;
#endif

}

void cNetwork::SockClose () // Close all sockets for shutdown
{

	int i;
	closesocket(a_socket);
	for ( i = 0; i < MAXCLIENT; ++i )
		closesocket(client[i]);

}

void cNetwork::CheckConn() // Check for connection requests
{

	char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var
	char temp2[TEMP_STR_SIZE]; //xan -> this overrides the global temp var
	pClient client;
	socklen_t len;

	if ( now > MAXIMUM )
		return;
	
	FD_ZERO(&conn);
	FD_SET(a_socket, &conn);
	nfds=a_socket+1;

	s=select(nfds, &conn, NULL, NULL, &nettimeout);

	if (s>0)
	{
		len=sizeof (struct sockaddr_in);
		client[now] = accept(a_socket, (struct sockaddr *)&client_addr, &len);
		if ((client[now]<0))
		{
			ErrOut("Unknown error at client connection!\n");
			error=1;
			keeprun=true;
			return;
		}
		if ( CheckForBlockedIP( client_addr ) )
		{
			InfoOut("IPBlocking: Blocking IP address [%s] listed in hosts_deny\n", inet_ntoa( client_addr.sin_addr ));

			closesocket(client[now]);
		}
		else
		{
			uint32_t nonzero = 1;
			ioctlsocket(client[now],FIONBIO,&nonzero) ;

			currchar[now] = INVALID;
			cryptedClient[now]=true;
			acctno[now]=-1;
			binlength[now]=0;
			boutlength[now]=0;
			clientInfo[now]= new cClient();
			walksequence[now]=-1;

			clientDimension[now]=2;

			++global_lis; // not 100% correct, but only cosmetical stuff, hence ok not to be 100% correct :>
					// doesnt get correct status if kicked out due to worng pw etc.

			if (global_lis % 2 == 0) sprintf((char*)temp2, "connecting"); else sprintf((char*)temp2, "connected");

			sprintf((char*)temp,"client %i [%s] %s [Total:%i].\n",now,inet_ntoa(client_addr.sin_addr), temp2, now+1);
			InfoOut(temp);

			if (SrvParms->server_log)
				ServerLog.Write("%s", temp);

			++now;
		}
		return;

	}
	else if (s<0)
	{
		ErrOut("select (Conn) failed!\n");
		keeprun=true;
		error=1;
		return;
	}
}

void cNetwork::CheckMessage() // Check for messages from the clients
{

	pClient client, i, oldnow;
	int lp, loops;     //Xan : rewritten to support more than 64 concurrent clients

	oldnow = now;
	loops = now / 64; //xan : we should do loops of 64 players

	for (lp = 0; lp <= loops; lp++)
	{


		FD_ZERO(&all);
		FD_ZERO(&errsock);
		nfds=0;

		for (i=0+(64*lp);i<((lp<loops) ? 64 : oldnow);i++)
		{
			FD_SET(client[i],&all);
			FD_SET(client[i],&errsock);
			if (client[i]+1>nfds) nfds=client[i]+1;

		}

		s=select(nfds, &all, NULL, &errsock, &nettimeout);

		if (s>0)
		{
			for (i=0+(64*lp);i<((lp<loops) ? 64 : oldnow);i++)
			{
				if (FD_ISSET(client[i],&errsock))
				{
					i->disconnect();
				}


				if ((FD_ISSET(client[i],&all))&&(oldnow==now))
				{
					Network->GetMsg(i);
				}
			}
		}
	}
}


cNetwork::cNetwork() // Initialize sockets
{
    sockInit();
#ifdef ENCRYPTION
	for (int i = 0;i< MAXCLIENT+1;++i)
	{
		clientCrypter[i]=0;
	}
#endif
}

int cNetwork::Pack(void *pvIn, void *pvOut, int len)
{

	unsigned char *pIn = (unsigned char *)pvIn;
	unsigned char *pOut = (unsigned char *)pvOut;

	int actByte = 0;
	int bitByte = 0;
	int nrBits;
	unsigned int value;

	if (len<=0) return 0; // ensure termination, got a bit paranoid bout server freezes'

	while(len--)
	{
		nrBits = bit_table[*pIn][0];
		value = bit_table[*pIn++][1];

		while(nrBits--)
		{
			pOut[actByte] = (pOut[actByte] << 1) | (unsigned char)((value >> nrBits) & 0x1);

			bitByte = (bitByte + 1) & 0x07;
			if(!bitByte) actByte++;
		}
	}

	nrBits = bit_table[256][0];
	value = bit_table[256][1];

	if ( nrBits <= 0 ) return 0;
	while(nrBits--)
	{
		pOut[actByte] = (pOut[actByte] << 1) | (unsigned char)((value >> nrBits) & 0x1);

		bitByte = (bitByte + 1) & 0x07;
		if(!bitByte) actByte++;
	}

	if(bitByte)
	{
		while(bitByte < 8)
		{
			pOut[actByte] <<= 1;
			bitByte++;
		}

		actByte++;
	}

	return actByte;

}

#ifdef ENCRYPTION
unsigned char cNetwork::calculateLoginKey(unsigned char loginKey[4], unsigned char packetId)
{
	unsigned pClient clienteed;
	int  m_key [2], index = -1;
	unsigned char out;
	bool found;
	out=packetId;
	seed = (loginKey[0] << 24 ) + (loginKey[1] << 16 ) + (loginKey[2] << 8 ) + (loginKey[3] );
	found = true;
	for ( int i = 0; i < 6; ++i)
	{
		index=i;
		m_key[0] =
				(((~seed) ^ 0x00001357) << 16)
			|   ((seed ^ 0xffffaaaa) & 0x0000ffff);
		m_key[1] =
				((seed ^ 0x43210000) >> 16)
			|   (((~seed) ^ 0xabcdffff) & 0xffff0000);
		unsigned char med = static_cast<unsigned char>(m_key[0]);
	    out = packetId ^ med;
		if ( (out != 0x80) && (out != 0x91) )
		{
			found = false;
			continue;
		}
		else
			break;
	}
	return out;
}
#endif

void cNetwork::GetMsg(pClient client) // Receive message from client
{
	ClientCrypt *crypter=NULL;
	
	pChar pc_currchar= client ? client->currChar() : NULL;
	
	if ( pc_currchar != NULL )
	{
		crypter=pc_currchar->getCrypter();
	}
	else if ( clientCrypter[s] != NULL )
	{
		crypter=clientCrypter[s];
	}

	int count, i, book,length, dyn_length,loopexit=0, fb;
	unsigned char nonuni[512];
	unsigned char packet;
	int  myoffset,  myj, mysize, subcommand;//, subsubcommand ;
	unsigned char mytempbuf[512] ;
	char client_lang[4];

	std::string cpps;
	std::vector<std::string>::const_iterator viter;

	if ( clientInfo[s]->newclient )
	{

		if((count=recv(client[s], (char*)buffer[s], 4, MSG_NOSIGNAL))==SOCKET_ERROR)
		{
			LogSocketError("Socket Recv Error %s\n", errno) ;
		}
		// an encrypted client sends its ip as a seed for the encryption

#ifdef ENCRYPTION
		memcpy(&clientSeed[s][0], &buffer[s][0], 4);
#endif
		struct sockaddr_in tmp_addr;
		socklen_t tmp_addr_len=sizeof(struct sockaddr_in);

		if(getpeername(client[s],(struct sockaddr*)&tmp_addr,&tmp_addr_len)!=SOCKET_ERROR)
			*(uint32_t*)&clientip[s]=tmp_addr.sin_addr.s_addr;
		else
		{
			WarnOut("Unable to determine client's IP [s:%i]\n",s);
			*(uint32_t*)&clientip[s]=0;
		}

		clientInfo[s]->newclient=false;
		clientInfo[s]->firstpacket=true;
	}
	else
	{
#ifdef ENCRYPTION
		int cryptBufferOffset=0;
#endif
		fb = Receive(s, 1, 0);
		if (fb >0)
		{
#ifdef ENCRYPTION
			if ( crypter != NULL )
			{
				crypter->decrypt( &buffer[s][0], &buffer[s][0], 1);
				// crypter->preview( &buffer[s][0], &buffer[s][0], 1);
				cryptBufferOffset++;
			}
#endif
			packet = buffer[s][0];

			length = m_packetLen[packet];

			// Lets assure the data is valid, this stops server freezes caused by receiving nonsense data
			// (remark: useres that dont use ignition do that)
			if ( clientInfo[s]->firstpacket && packet != 0x80 && packet !=0x91 )
			{
#ifdef ENCRYPTION
				// Let's see if the paket is encrypted
				packet = calculateLoginKey(clientSeed[s], packet);
#endif
				if (packet != 0x80 && packet !=0x91 )
				{
#ifdef ENCRYPTION
					// reset crypt memories
					if ( clientCrypter[s] != NULL )
					{
						delete (ClientCrypt * ) crypter ;
						clientCrypter[s]=NULL;
					}
#endif
					client->disconnect();
					InfoOut("received garbage from a client, disconnected it to prevent bad things.\n User probably didnt use ignition or UO-RICE\n");
					return;
				}
#ifdef ENCRYPTION
				else
				{
					ClientCrypt * crypter = new ClientCrypt();
					crypter->setCryptMode(CRYPT_NONE);
					clientCrypter[s] = crypter;
					length = m_packetLen[packet];
				}
#endif
			}


			if (length==PACKET_LEN_NONE)
			{
				FD_ZERO(&all); FD_SET(client[s],&all);nfds=client[s]+1;
				if (select(nfds, &all, NULL, NULL, &nettimeout)>0) Receive(s, MAXBUFFER-2, 0);
				// I think thats supposed to be a re-synch attempt for next msg-in case of getting garbage (LB)
				return;

			}
			int readstat ;

			if (length==PACKET_LEN_DYNAMIC)
			{
				if ((readstat = Receive(s, 2, 1)) > 0)
				{
#ifdef ENCRYPTION
					if ( crypter != NULL )
					{
						unsigned char decryptPacket[3];
						crypter->decrypt(&buffer[s][0]+cryptBufferOffset, decryptPacket, 2);
						cryptBufferOffset+=2;
						buffer[s][1]=decryptPacket[0];
						buffer[s][2]=decryptPacket[1];
					}
#endif
					dyn_length = (int) (  ( (int) buffer[s][1]<<8) + (int) buffer[s][2] );
					length=dyn_length;
					readstat = Receive(s, dyn_length-3, 3) ;
				//ConOut("dyn-length: %i\n",dyn_length);
				} else return;


			} else
			{
				readstat = Receive(s, length-1, 1);
				dyn_length = length; //useless, just to avoid boring warning
			}

			if (readstat > SOCKET_ERROR)
			{
				if (pc_currchar && packet !=0x73 && packet!=0x80 && packet!=0xA4 && packet!=0xA0 && packet!=0x90 && packet!=0x91 ) {
					pc_currchar->clientidletime=SrvParms->inactivitytimeout*SECS+getclock();
				}
        		    // LB, client activity-timestamp !!! to detect client crashes, ip changes etc and disconnect in that case
        		    // 0x73 (idle packet) also counts towards client idle time

				if (pc_currchar)
					AMXEXECSV(pc_currchar->getSerial(),AMXT_NETRCV, packet, AMX_BEFORE);

				//if (packet != PACKET_FIRSTLOGINREQUEST && !pc_currchar) return;
#ifdef ENCRYPTION
				if ( packet == PACKET_LOGINREQUEST && clientCrypter[s] != NULL )
				{
					crypter->decrypt(&buffer[s][0]+cryptBufferOffset, &buffer[s][0]+cryptBufferOffset, 4);
					unsigned int loginseed= buffer[s][4] + ( buffer[s][3] << 8) + (buffer[s][2] << 16) + ( buffer[s][1] << 24);
					// clientCrypter[s]->setCryptSeed(&buffer[s][1]);
					cryptBufferOffset+=4;
				}

				if ( crypter != NULL && crypter->getCryptMode() > 0 )
				{
					crypter->decrypt(&buffer[s][0]+cryptBufferOffset, &buffer[s][0]+cryptBufferOffset, length-cryptBufferOffset);
				}
#endif
				switch(packet)
				{

//-------------------------------------------------------------------------------------------------------------------------------------------------
//Chronodt: removed packet switch because moved to nPacket::Receive. the remainder is left only because of crypting code to be translated to Hypnos
//-------------------------------------------------------------------------------------------------------------------------------------------------


				} // end switch
				if (pc_currchar)
					AMXEXECSV(pc_currchar->getSerial(),AMXT_NETRCV, packet, AMX_AFTER);

			}
			else
			{
			// reset crypt memories
#ifdef ENCRYPTION
				if ( clientCrypter[s] != NULL)
				{
					delete (ClientCrypt * ) clientCrypter[s] ;
					clientCrypter[s]=NULL;
				}
#endif
				client->disconnect();
			}
  		} // end if recv >0
  		else
  		{
			//ConOut("FB: %i perm: %i\n",fb,perm[s]);
			client->disconnect(); // extremly important (and tricky too ;-) !!!
			// osi client closes socket AFTRER the first 4 bytes and re-opens it afterward.
			// this line handles this correctly
		}
 	} // end if newclient[]
}

void cNetwork::LoadHosts_deny()
{
	if( !hosts_deny.empty() )
		hosts_deny.clear();

	cScpIterator* iter = Scripts::HostDeny->getNewIterator("SECTION HOST_DENY");

	if( iter != NULL )
	{
		ip_block_st	ip_block;
		uint32_t		ip_address;
		std::string	sScript1,
				sToken1;
		uint32_t 		siEnd ;

		do
		{
			//let's load a IP addresss/NetMask
			sScript1 = iter->getEntry()->getFullLine();
			siEnd = sScript1.find("/") ;
			sToken1 = sScript1.substr(0,siEnd) ;
			ip_address = inet_addr(sToken1.c_str()) ;

			if (ip_address != INADDR_NONE)
			{
				ip_block.address = ip_address;

				// Get the rest of the string, after the '/' token
				if (siEnd != std::string::npos)
				{
					sToken1 = sScript1.substr(siEnd+1) ;
					ip_address = inet_addr(sToken1.c_str()) ;
					if (ip_address != INADDR_NONE)
						ip_block.mask = ip_address;
					else
						ip_block.mask = static_cast<uint32_t>(~0); // mask is not required. (fills all bits with 1's)
				}
				else
					ip_block.mask = static_cast<uint32_t>(~0);
				hosts_deny.push_back(ip_block);
			}
		}
		while ( sScript1[0] != '}' );
	}
	else
	{
		WarnOut("Can't find SECTION HOST_DENY! \n");
	}
}

bool cNetwork::CheckForBlockedIP(sockaddr_in ip_address)
{
	const pClient clientize = hosts_deny.size();

	for ( int i = 0; i < size; ++i )
		if( (ip_address.sin_addr.s_addr&hosts_deny[i].mask) == (hosts_deny[i].address&hosts_deny[i].mask ))
			return true;

	return false;
}
