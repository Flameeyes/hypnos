#include <cabal_ssocket.h>
#include <wefts_thread.h>
#include <iostream>

Cabal::ServerSocket *serv;
Wefts::Thread	 *t;

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
			std::cerr << str << std::endl;
			
			if ( str == "!" )
			{
				std::cerr << "Shutdown!" << std::endl;
				serv->close();
				sock->close();
				return NULL;
			}
			
			if ( str == "*" )
			{
				t->stop();
			}
		}
	}
};

class SelectingThread : public Wefts::Thread
{
private:
public:
	SelectingThread()
	{ }
	
	~SelectingThread()
	{
		std::cerr << "Destroyed" << std::endl;
	}
	
	void *run()
	{
		while(1)
		{
			fd_set set;
			FD_ZERO(&set);
			FD_SET(0, &set);
			
			std::cerr << ".";
			int retval = 0;
// 			retval = pselect(1, &set, NULL, NULL, NULL, NULL);
			
			std::string str;
			std::getline(std::cin, str);
			std::cout << '\t' << str << '\n';
		}
	}
};

int EchoThread::count = 0;

int main()
{
	serv = new Cabal::ServerSocket("127.0.0.1", 2593, 30);
	
	std::cout << "\x1B[1;31m" << "Start!" << "\x1B[0m";
	t = new SelectingThread();
	t->start(true);
	
	while(!serv->closed())
	{
		Cabal::TCPSocket *sock = serv->accept();
		if ( sock )
			new EchoThread(sock);
	}
	
	std::cerr << "Shutdown complete" << std::endl;
}
