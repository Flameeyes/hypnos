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
#include "version.h"

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
	static const char msgLogin[] = "Client [%s] connected [first] using Account '%s'.\n";

	outInfof( (char*)msgLogin, inet_ntoa(client_addr.sin_addr), &buffer[s][1] );
	if (SrvParms->server_log)
		ServerLog.Write( (char*)msgLogin, inet_ntoa(client_addr.sin_addr), &buffer[s][1] );

	nPackets::Sent::GameServerList pk();
	client->sendPacket(&pk);
}


void cNetwork::Relay(pClient client) // Relay player to a certain IP
{
	uint32_t ip;
	int port;

	ip = inet_addr(serv[buffer[s][2]-1][1]);
	port =str2num(serv[buffer[s][2]-1][2]);

	uint32_t key = calcserial('a', 'K', 'E', 'Y');

	srand(ip+acctno[s]+now+getClockmSecs()); // Perform randomize
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

	nPackets::Sent::ConnectToGameServer pk(ip, port, key);
	client->sendPacket(&pk);
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

	clientInfo[s]->ingame=true;

	nPackets::Sent::SetMap pkSetMap(((map_height<300)? 0x02 : 0x00);
	client->sendPacket(&pkSetMap);

	nPackets::Sent::LoginConfirmation pkLoginConf(pc);
	client->sendPacket(&pkLoginConf);

	pc->war=0;
	nPackets::Sent::WarModeStatus pkWar(pc->war);
	client->sendPacket(&pkWar);

	nPackets::Sent::ChangeTextEmoteColor pkTech1(0x01);
	client->sendPacket(&pkTech1);

	nPackets::Sent::ChangeTextEmoteColor pkTech2(0x02);
	client->sendPacket(&pkTech2);

	nPackets::Sent::ChangeTextEmoteColor pkTech3(0x03);
	client->sendPacket(&pkTech3);

	nPackets::Sent::StartGame pkStartGame;
	client->sendPacket(&pkStartGame);

	nPackets::Sent::GameTime pkGameTime;
	client->sendPacket(&pkGameTime);

	pc->spiritspeaktimer=getClockmSecs();
	pc->begging_timer=getClockmSecs();

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

	client->sysmessage(0x058, "Hypnos %s [%s]", strVersion, getOSVersionString().c_str());

	// log last time signed on
	time_t ltime;
	time( &ltime );

	if (SrvParms->joinmsg)
	{
		if (!strcmp(pc->getCurrentName().c_str(), "pty Slot --")) pc->setCurrentName("A new Character");//AntiChrist
		sysbroadcast("%s entered the realm",pc->getCurrentName().c_str());//message upon entering a server
	}

	// very stupid stuff
	//pc->murderrate=getClockmSecs()+repsys.murderdecay*SECS; // LB, bugfix for murder-count getting --'ed each start
	pc->murderrate=getClockmSecs()+pc->murdersave*SECS;

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
		    pc->logout=getClockmSecs()+SrvParms->quittime*SECS;
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
