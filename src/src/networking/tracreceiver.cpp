/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "backend/admincmds.h"
#include "networking/tracreceiver.h"
#include "networking/tkiller.h"
#include "networking/exception.h"

#ifdef HAVE_SSTREAM
	#include <sstream>
	using std::ostringstream;
#elif HAVE_SSTREAM_H
	#include <sstream.h>
#endif

/*!
\brief Constructor for tRACReceiver thread

This function register the thread in tRemoteAdmin::threads set and also starts
the thread itself.
*/
tRACReceiver::tRACReceiver(Cabal::TCPSocket *aSock) : tReceiver(aSock)
{
	// Lock the threads set
	tRemoteAdmin::instance->threads_m.lock();
	threads.insert(this);
	tRemoteAdmin::instance->threads_m.unlock();
	// Unlock the threads set
	
	logon = false;
	
	start();
}

/*!
\brief Receiving loop function

This function does all the dirt work for tRACReceiver thread, getting lines and
parsing them.

This function also register the socket as dead in tKiller after the socket is
closed.

\todo Missing all the work in this :)
\todo Add exception handling for error sending data
*/
void *tRACReceiver::run()
{
	if ( ! sock ) return NULL;
	
	try {
		while ( ! sock->closed() )
		{
			if ( ! logon )
				doLogin();
			
			sock->send("# ", 2);
			
			ostringstream sout;
			nAdminCommands::parseCommand(sock->recvLine(), sout);
			
			sock->send(sout.str(), sout.str().lenght());
		}
	} catch(eErrorSending exc) {
		sock->close();
	}
	
	// Lock the threads set
	tRemoteAdmin::instance->threads_m.lock();
	threads.erase(this);
	tRemoteAdmin::instance->threads_m.unlock();
	// Unlock the threads set
	
	tKiller::instance->deadSockets->push(this);
}

/*!
\brief Sends a formatted string to the peer
\param str printf() formatted string

This function does the formatting of a string and then sends it to the client
connected with this socket.

\throw eErrorSending If the socket can't send all the bytes requested
*/
void tRACReceiver::outf(char *str, ...)
{
	va_list argptr;
	char *msg;
	va_start( argptr, txt );
	int len = vasprintf( &msg, txt, argptr );
	va_end( argptr );
	
	int sent = sendAll(str, len);
	free(str);
	
	if ( sent != len )
		throw eErrorSending(len, sent);
}

/*!
\brief Does all the stuff for login an user to the RAC
*/
void tRACReceiver::doLogin()
{
	outf("%c%c%c", IAC, WILL, ECHO);
	
	outf("Hypnos %s [%s]\r\n"
	     "Remote Administration Console\r\n"
	     "Programmed by: %s\r\n"
	     "Based on NoX-Wizard 20031228\r\n"
	     "Web-site : http://hypnos.berlios.de/\r\n"
	     "\r\n"
	     "INFO: character typed for login and password\r\n"
	     "are not echoed, this is not a bug.\r\n"
	     "\r\n"
	     "Login: ",
	     strVersion, getOSVersionString().c_str(), strDevelopers);

	string loginName = sock->recvLine();
	
	outf("Password: ");
	
	string loginPass = sock->recvLine();
	
	if ( !Accounts->AuthenticateRAS(loginName, loginPass) )
	{
		outf("\r\n"
		     "Access Denied.\r\n"
		     "Press any key to get disconnected...\r\n");
		
		LogWarning("Access denied on Remote Console for user '%s'", loginName.c_str());
		sock->recvLine(1);
		sock->close();
		return;
	}
	
	outf("\r\n"
	     "\r\n"
	     "Welcome to the administration console\r\n"
	     "Type HELP to receive help on commands.\r\n"
	     "If you are not authorized to access this system\r\n"
	     "please exit immediatly by typing EXIT\r\n"
	     "or by closing your client. Any other unauthorized\r\n"
	     "action can be persecuted by law.\r\n"
	     "\r\n"
	     "%c%c%c%c%c%c", IAC, DO, ECHO, IAC, WONT, ECHO); // Re-enable local echo for client
	
	LogMessage("Authorised access on Remote Console for user %s", loginName.c_str());
	logon = true;
	return;
}
