/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Unix Architecture specific
*/

#ifndef _PYUO_UNIX_
#define _PYUO_UNIX_

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netdb.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <libgen.h>

#define ioctlsocket ioctl

#include <signal.h>
#include <sys/errno.h>
#include <arpa/inet.h>
#define closesocket(s)	close(s)
#include <sys/utsname.h>

#define SOCKET_ERROR -1

#if defined(__OpenBSD__) || defined(__FreeBSD__)
    #include <pthread.h>
#endif

namespace arch {
	extern bool pollHUPStatus ();
	extern bool pollCloseRequests ();
	extern void setup_signals ();
	extern void start_signal_thread();

	void init_deamon();
	void initclock();
} // namespace arch

char *strlwr(char *);
char *strupr(char *);

void Sleep(unsigned long msec);


namespace tthreads {

class Mutex {
  private:
        bool m_bLocked;
        bool m_bDebug;
        char* m_szMutexName;
    	pthread_mutex_t* m_mutex;
   inline void init(bool alreadylocked) {
    	    m_mutex = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
    	    pthread_mutex_init (m_mutex, NULL);
        m_bLocked = false;
        m_bDebug = false;
        if (alreadylocked) enter();
    }
  public:
    inline Mutex(bool alreadylocked = false) { init(alreadylocked); }
    inline Mutex(char *name, bool alreadylocked = false) { init(alreadylocked); setDebugMode(name); }
    inline ~Mutex() {
        if (m_bDebug) {
            printf("DBG-MUTEX: Mutex %s destroyed\n", m_szMutexName);
            if (m_szMutexName!=NULL) delete[] m_szMutexName;
        }
    	    pthread_mutex_destroy (m_mutex);
        	free(m_mutex);
    }
    inline void enter () {
        if (m_bDebug) printf("DBG-MUTEX: Entering mutex %s\n", m_szMutexName);
    	    pthread_mutex_lock(m_mutex);
        m_bLocked = true;
        if (m_bDebug) printf("DBG-MUTEX: Entered mutex %s\n", m_szMutexName);
    }
    inline void leave () {
        if (m_bDebug) printf("DBG-MUTEX: Leaving mutex %s\n", m_szMutexName);
        m_bLocked = false;
        	pthread_mutex_unlock(m_mutex);
    }
    inline bool tryEnter () {
        if (m_bDebug) printf("DBG-MUTEX: Try-Locking mutex %s\n", m_szMutexName);
            return pthread_mutex_trylock(m_mutex)==0;
    }
    void setDebugMode (char *name) { m_bDebug = false; m_szMutexName = new char[strlen(name)+2]; strcpy(m_szMutexName, name);}
};




template <typename T> class Atomic {
  protected:
    Mutex m_mutex;
    volatile T m_val;
    inline void enterIfEq(const T& compval)  { while(true) { m_mutex.enter(); if (m_val==compval) return; m_mutex.leave(); Sleep(1); } }
    inline void enterIfNEq(const T& compval) { while(true) { m_mutex.enter(); if (m_val!=compval) return; m_mutex.leave(); Sleep(1); } }
  public:
	inline T setIfEq  (const T& newval, const T& compval = 0) { enterIfEq(compval); T old = m_val; m_val = newval; m_mutex.leave(); return old;}
	inline T setIfNEq (const T& newval, const T& compval = 0) { enterIfNEq(compval); T old = m_val; m_val = newval; m_mutex.leave();  return old;}
	inline T decIfEq  (const T& newval = 1, const T& compval = 0) { enterIfEq(compval); T old = m_val; m_val -= newval; m_mutex.leave();  return old;}
	inline T decIfNEq (const T& newval = 1, const T& compval = 0) { enterIfNEq(compval); T old = m_val; m_val -= newval; m_mutex.leave();  return old;}
	inline T incIfEq  (const T& newval = 1, const T& compval = 0) { return dec (-newval, compval); }
	inline T incIfNEq (const T& newval = 1, const T& compval = 0) { return dec (-newval, compval); }
	inline T set      (const T& newval) { m_mutex.enter(); T old = m_val; m_val = newval; m_mutex.leave();  return old;}
    inline T dec      (const T& newval = 1) { m_mutex.enter(); T old = m_val; m_val -= newval; m_mutex.leave();  return old;}
    inline T inc      (const T& newval = 1) { return dec(-newval); }

    inline Atomic<T>& operator++() { inc(); return *this;}
    inline Atomic<T>& operator--() { dec(); return *this;}
    inline Atomic<T>& operator+=(const T& x) { inc(x);  return *this;}
    inline Atomic<T>& operator-=(const T& x) { dec(x);  return *this;}
    inline Atomic<T>& operator=(const T& newval) { set(newval);  return *this;}

    inline operator T() { return m_val; }
    inline bool operator==(const T& v) { return (m_val==v); }
};


class Semaphore : public Atomic<int> {
public :
    Semaphore(int n = 1) { set(n); }
    inline void enter() { decIfNEq(); }
    inline bool tryEnter() {
        m_mutex.enter();
        if (m_val!=0) {
            m_val--;
            m_mutex.leave();
            return true;
        }
        m_mutex.leave();
        return false;
    }
    void exit() { inc(); }
};



#define TTHREAD void*
#define EXIT_TTHREAD { return NULL; }

int startTThread( TTHREAD ( *funk )( void * ), void* param = NULL );

};

#endif