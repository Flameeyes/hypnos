/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Win32 Architecture specific
*/

#ifdef WIN32

#ifndef _PYUO_WIN32_
#define _PYUO_WIN32_

#include <winsock.h>
#include <winbase.h>
#include <io.h>
#include <dos.h>
#include <limits.h>
#include <conio.h>
#include <process.h>

char *basename(char *path);

namespace arch {

	char *getHKLMRegistryString(char *key, char *subkey);
	void setHKLMRegistryString(char *key, char *subkey, char *value);
	char *splitPath (char *p);

	void init_deamon();
	void initclock();

	inline bool pollHUPStatus () { return false; }
	inline bool pollCloseRequests () { return false; }
	inline void setup_signals (){ return; }
	inline void start_signal_thread() {return;}

	extern WSADATA wsaData;
	extern WORD wVersionRequested;
	extern long int oldtime, newtime;
} // namespace arch
	
namespace tthreads {

class Mutex
{
private:
	bool m_bLocked;
	bool m_bDebug;
	char* m_szMutexName;
	CRITICAL_SECTION m_cs;
	CRITICAL_SECTION m_cs2;
	inline void init(bool alreadylocked)
	{
		InitializeCriticalSection(&m_cs);
		InitializeCriticalSection(&m_cs2);
		m_bLocked = false;
		m_bDebug = false;
		if (alreadylocked) enter();
	}
public:
	inline Mutex(bool alreadylocked = false) { init(alreadylocked); }
	inline Mutex(char *name, bool alreadylocked = false) { init(alreadylocked); setDebugMode(name); }
	inline ~Mutex()
	{
		if (m_bDebug)
		{
			printf("DBG-MUTEX: Mutex %s destroyed\n", m_szMutexName);
			if (m_szMutexName!=NULL) delete[] m_szMutexName;
        	}
		DeleteCriticalSection(&m_cs);
		DeleteCriticalSection(&m_cs2);
	}
	inline void enter ()
	{
		if (m_bDebug) printf("DBG-MUTEX: Entering mutex %s\n", m_szMutexName);
		EnterCriticalSection(&m_cs2);
		EnterCriticalSection(&m_cs);
		m_bLocked = true;
		LeaveCriticalSection(&m_cs2);
		if (m_bDebug) printf("DBG-MUTEX: Entered mutex %s\n", m_szMutexName);
    	}
	inline void leave ()
	{
		if (m_bDebug) printf("DBG-MUTEX: Leaving mutex %s\n", m_szMutexName);
		m_bLocked = false;
		LeaveCriticalSection(&m_cs);
	}
	inline bool tryEnter ()
	{
		if (m_bDebug) printf("DBG-MUTEX: Try-Locking mutex %s\n", m_szMutexName);
		//if (getOSVersion()==OSVER_WINNT) return TryEnterCriticalSection(&m_cs);
		// Xan : workaround for TryEnterCriticalSection not working in Win9x
		EnterCriticalSection(&m_cs2);
		//we're here, we've control of m_bLocked.
		if (m_bLocked) {
				LeaveCriticalSection(&m_cs2);
				return false;
		}
		// mmm so we'll enter!, thx to critsections are recursive.
		enter();
		LeaveCriticalSection(&m_cs2);
		return true;
	}
	void setDebugMode (char *name)
	{
		m_bDebug = false;
		m_szMutexName = new char[strlen(name)+2];
		strcpy(m_szMutexName, name);
	}
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



#define TTHREAD void
#define EXIT_TTHREAD { return; }

int startTThread( TTHREAD ( *funk )( void * ), void* param = NULL );

};

#endif

typedef int FAR socklen_t ;
#endif