#include <cabal_ssocket.h>
#include <wefts_thread.h>
#include <iostream>

Cabal::ServerSocket *serv;

class EchoThread : public Wefts::Thread
{
private:
	Cabal::TCPSocket *sock;
	static int count;
public:
	EchoThread(Cabal::TCPSocket *aSock)
	{
		count++;
		sock = aSock;
		start();
	}
	
	void *run()
	{
		if ( ! sock ) return NULL;
		
		while ( ! sock->closed() )
		{
			std::string str = sock->recvLine();
			std::cerr << count << '\t' << str << std::endl;
			
			if ( str == "!" )
			{
				std::cerr << "Shutdown!" << std::endl;
				serv->close();
				sock->close();
				return NULL;
			}
		}
	}
};

int EchoThread::count = 0;

int main()
{
	serv = new Cabal::ServerSocket("127.0.0.1", 2593, 30);
	
	int s = serv->rawSocket();
	int flags;
	flags = fcntl( s , F_GETFL );
	if ( flags | O_NONBLOCK )
		std::cerr << "Socket non blocking" << std::endl;
	else
		std::cerr << "Socket blocking" << std::endl;
	
	while(!serv->closed())
	{
		std::cerr << ".";
		Cabal::TCPSocket *sock = serv->accept();
		if ( sock )
		{
			new EchoThread(sock);
		}
	}
	
	std::cerr << "Shutdown complete" << std::endl;
}
