/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "common_libs.h"
#include "version.h"
#include "debug.h"

class ClientCrypt;

#define IPPRINTF(I) ((I)&(0xFF)),((I>>8)&(0xFF)),((I>>16)&(0xFF)),(I>>24)

#define MTMAXBUFFER 65536

typedef struct 
{
	unsigned long address;
	unsigned long mask;
}ip_block_st;

class NetThread 
{
	public:
		char outbuffer[MTMAXBUFFER];
		tthreads::Atomic<int> outhead;
		tthreads::Atomic<int> outtail;
		bool lastopwasinsert;
		tthreads::Mutex mtxrun;
		int realsocket;
		void run();
		void enqueue (char*, int);
		NetThread(int s);
		void set(int s);
};

class cNetwork
{
	public:
		cNetwork();

		void enterchar(int s);
		void startchar(int s);
		void LoginMain(int s);
		void xSend(pClient client, const void *point, int length );
		void xSend(pClient client, unistring& p, bool alsoTermination = true );
		void Disconnect(pClient client);
		void ClearBuffers();
		void CheckConn();
		void CheckMessage();
		void SockClose();
		void FlushBuffer(pClient client);
		void LoadHosts_deny( void );
		bool CheckForBlockedIP(sockaddr_in ip_address);

		int kr,faul; // needed because global varaibles cant be changes in constructores LB

	private:

		std::vector<ip_block_st> hosts_deny;

		void DoStreamCode(pClient client);
		int  Pack(void *pvIn, void *pvOut, int len);
		void Login2(int s);
		void Relay(int s);
		void GoodAuth(int s);
		void charplay (int s);
		void CharList(int s);
		int  Receive(int s, int x, int a);
		void GetMsg(int s);
		char LogOut(pClient client);
		void pSplit(char *pass0);
		void sockInit();
		void ActivateFeatures(pClient client);
		unsigned char calculateLoginKey(unsigned char loginKey [4], unsigned char packetId );
		ClientCrypt * clientCrypter[MAXCLIENT+1]; //! save crypter per client socket
		unsigned char clientSeed[MAXCLIENT+1][4]; 
};

extern class cNetwork	*Network;

NXWCLIENT getClientFromSocket(int s);

#endif
