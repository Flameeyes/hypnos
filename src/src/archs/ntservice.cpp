/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifdef WIN32

bool g_bNTService = false;

#include "common_libs.hpp"
#include "version.hpp"
#include "archs/ntservice.hpp"
#include "archs/win32registry.hpp"

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

#ifdef HAVE_CONIO_H
#include <conio.h>
#endif

//! The name of the service
char *SERVICE_NAME = "Hypnos Server Emulator";
extern bool keeprun;

BOOL servicePaused;
HANDLE threadHandle;
HANDLE killServiceEvent;
BOOL UpdateSCMStatus (DWORD dwCurrentState,
	                  DWORD dwWin32ExitCode,
	                  DWORD dwServiceSpecificExitCode,
	                  DWORD dwCheckPoint,
	                  DWORD dwWaitHint);
BOOL StartServiceThread();

void terminateService(int i) { exit(i); }

int beepDelay;
DWORD serviceCurrentStatus;

//! Allow ServiceMain to exit
void KillService()
{
	keeprun=FALSE;
	// Set the event that is blocking ServiceMain
	// so that ServiceMain can return
	SetEvent(killServiceEvent);
}

//! Handles the events dispatched by the Service Control Manager.
VOID ServiceCtrlHandler (DWORD controlCode)
{
BOOL success;
   switch(controlCode)
   {
	  // There is no START option because
	  // ServiceMain gets called on a start
	  // Pause the service
	  case SERVICE_CONTROL_PAUSE:
	     if (keeprun && !servicePaused)
	     {
	        // Tell the SCM we're about to Pause.
	        success = UpdateSCMStatus(SERVICE_PAUSE_PENDING, NO_ERROR, 0, 1, 1000);
	        servicePaused = TRUE;
	        SuspendThread(threadHandle);
	        serviceCurrentStatus = SERVICE_PAUSED;
	     }
	     break;
	  // Resume from a pause
	  case SERVICE_CONTROL_CONTINUE:
	     if (keeprun && servicePaused)
	     {
	        // Tell the SCM we're about to Resume.
	        success = UpdateSCMStatus(SERVICE_CONTINUE_PENDING, NO_ERROR, 0, 1, 1000);
	        servicePaused=FALSE;
	        ResumeThread(threadHandle);
	        serviceCurrentStatus = SERVICE_RUNNING;
	     }
	     break;
	  /*// Beep now in response to our special control message.
	  case JW_SERVICE_CONTROL_BEEP_NOW:
	     Beep(500, 500);
	     break;*/
	  // Update the current status for the SCM.
	  case SERVICE_CONTROL_INTERROGATE:
	     // This does nothing, here we will just fall through to the end
	     // and send our current status.
	     break;
	  // For a shutdown, we can do cleanup but it must take place quickly
	  // because the system will go down out from under us.
	  // For this app we have time to stop here, which I do by just falling
	  // through to the stop message.
	  case SERVICE_CONTROL_SHUTDOWN:
	  // Stop the service
	  case SERVICE_CONTROL_STOP:
	     // Tell the SCM we're about to Stop.
	     serviceCurrentStatus = SERVICE_STOP_PENDING;
	     success = UpdateSCMStatus(SERVICE_STOP_PENDING, NO_ERROR, 0, 1, 5000);
	     KillService();
	     return;
	  default:
	      break;
   }
   UpdateSCMStatus(serviceCurrentStatus, NO_ERROR, 0, 0, 0);
}

SERVICE_STATUS_HANDLE serviceStatusHandle;

/*!

ServiceMain is called when the Service Control Manager wants to launch the
service. It should not return until the service has stopped.
*/
VOID ServiceMain(DWORD argc, LPTSTR *argv)
{
   BOOL success;
   // First we must call the Registration function
   serviceStatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME,
	                       (LPHANDLER_FUNCTION) ServiceCtrlHandler);
   if (!serviceStatusHandle)
   {
	  terminateService(GetLastError());
	  return;
   }
   // Next Notify the Service Control Manager of progress
   success = UpdateSCMStatus(SERVICE_START_PENDING, NO_ERROR, 0, 1, 5000);
   if (!success)
   {
	  terminateService(GetLastError());
	  return;
   }
   // Now create the our service termination event to block on
   killServiceEvent = CreateEvent (0, TRUE, FALSE, 0);
   if (!killServiceEvent)
   {
	  terminateService(GetLastError());
	  return;
   }
   // Notify the SCM of progress again
   success = UpdateSCMStatus(SERVICE_START_PENDING, NO_ERROR, 0, 2, 1000);
   if (!success)
   {
	  terminateService(GetLastError());
	  return;
   }
   // Check for a beep delay parameter passed in...
   if (argc == 2)
   {
	  int temp = atoi(argv[1]);
	  if (temp < 1000)
	  {
	     beepDelay = 2000;
	  }
	  else
	  {
	     beepDelay = temp;
	  }
   }
   // Notify the SCM of progress again...
   success = UpdateSCMStatus(SERVICE_START_PENDING, NO_ERROR, 0, 3, 5000);
   if (!success)
   {
	  terminateService(GetLastError());
	  return;
   }
   // Start the service execution thread by calling our StartServiceThread function...
   success = StartServiceThread();
   if (!success)
   {
	  terminateService(GetLastError());
	  return;
   }
   // The service is now running.  Notify the SCM of this fact.
   serviceCurrentStatus = SERVICE_RUNNING;
   success = UpdateSCMStatus(SERVICE_RUNNING, NO_ERROR, 0, 0, 0);
   if (!success)
   {
	  terminateService(GetLastError());
	  return;
   }
   // Now just wait for our killed service signal, and then exit, which
   // terminates the service!
   WaitForSingleObject (killServiceEvent, INFINITE);
   terminateService(0);
}

extern "C" int win32_main(int argc, char *argv[]);

//! This is the main thread of execution for the service while it is running.
DWORD ServiceExecutionThread(LPDWORD param)
{
	win32_main(0,NULL);
	return 0;
}

//! This starts the service by creating its execution thread.
BOOL StartServiceThread()
{
	DWORD id;
	// Start the service's thread
	threadHandle = CreateThread(0, 0,(LPTHREAD_START_ROUTINE) ServiceExecutionThread,0, 0, &id);
	if (threadHandle == 0)
		return FALSE;
		
	keeprun = TRUE;
	return TRUE;
}

//! This function updates the service status for the SCM
BOOL UpdateSCMStatus (DWORD dwCurrentState,
	                  DWORD dwWin32ExitCode,
	                  DWORD dwServiceSpecificExitCode,
	                  DWORD dwCheckPoint,
	                  DWORD dwWaitHint)
{
   BOOL success;
   SERVICE_STATUS serviceStatus;
   // Fill in all of the SERVICE_STATUS fields
   serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
   serviceStatus.dwCurrentState = dwCurrentState;
   // If in the process of something, then accept
   // no control events, else accept anything
   if (dwCurrentState == SERVICE_START_PENDING)
   {
	  serviceStatus.dwControlsAccepted = 0;
   }
   else
   {
	  serviceStatus.dwControlsAccepted =
	     SERVICE_ACCEPT_STOP |
	     SERVICE_ACCEPT_PAUSE_CONTINUE |
	     SERVICE_ACCEPT_SHUTDOWN;
   }
   // if a specific exit code is defines, set up
   // the Win32 exit code properly
   if (dwServiceSpecificExitCode == 0)
   {
	  serviceStatus.dwWin32ExitCode = dwWin32ExitCode;
   }
   else
   {
	  serviceStatus.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
   }
   serviceStatus.dwServiceSpecificExitCode =   dwServiceSpecificExitCode;
   serviceStatus.dwCheckPoint = dwCheckPoint;
   serviceStatus.dwWaitHint = dwWaitHint;
   // Pass the status record to the SCM
   success = SetServiceStatus (serviceStatusHandle, &serviceStatus);
   if (!success)
   {
	  KillService();
   }
   return success;
}

void remain(int argc, char *argv[]);
void initService (int argc, char **argv);

void remain(int argc, char *argv[])
{
	SC_HANDLE myService, scm;
// 	outPlain("NT-Service Installation/Uninstallation\n");
// 	outPlain("Program by Xanathar, Ummon\n\n\n");
	char str[800];
	sprintf(str, "Hypnos %s", strVersion );

// 	open a connection to the SCM
// 	outPlain("\nOpening connection to SCM...");
	scm = OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE);
	if (!scm)
	{
		MessageBox(NULL, "Error : can't connect to Service Control Manager ", "Hypnos Service Manager", MB_ICONSTOP);
		return;
	}

	// Delete the OLD service
	myService = OpenService(scm, "Hypnos", DELETE);
	
	if (myService!=NULL)
	{
		//outPlain("Deleting previous installation...");
		if (!DeleteService(myService))
		{
			char *tmp;
			asprintf(&tmp, "Hypnos Service %s can't be removed. ", strVersion);
			MessageBox(NULL, tmp, "Hypnos Service Manager", MB_ICONEXCLAMATION);
			free(tmp);
		} else {
			CloseServiceHandle(myService);
			CloseServiceHandle(scm);
			
			char *tmp;
			asprintf(&tmp, "Hypnos Service %s successfully removed. ", strVersion);
			MessageBox(NULL, tmp, "Hypnos Service Manager", MB_ICONINFORMATION);
			free(tmp);
			
			setHKLMRegistryString("SOFTWARE\\Hypnos", "Service", "0");
			return;
		}
	}


// 	Install the NEW service
// 	outPlain("Creating new service...");
	myService = CreateService(
		scm, "Hypnos", // the internal service name used by the SCM
		str,  // the external label seen in the Service Control applet
		SERVICE_ALL_ACCESS,  // We want full access to control the service
		SERVICE_WIN32_OWN_PROCESS,  // The service is a single app and not a driver
		SERVICE_DEMAND_START,  // The service will be started by us manually
		SERVICE_ERROR_NORMAL,  // If error during service start, don't misbehave.
		argv[0],
		0, 0, 0, 0, 0);
	if (!myService)
	{
		char *tmp;
		asprintf(&tmp, "Hypnos Service %s can't be installed. ", strVersion);
		MessageBox(NULL, tmp, "Hypnos Service Manager", MB_ICONSTOP);
		free(tmp);
	}
	else
	{
		char *tmp;
		asprintf(&tmp, "Hypnos Service %s successfully installed. ", strVersion);
		MessageBox(NULL, tmp, "Hypnos Service Manager", MB_ICONINFORMATION);
		free(tmp);
	}


	char ss[800];
	GetCurrentDirectory(790, ss);
 	setHKLMRegistryString("SOFTWARE\\Hypnos", "Path", ss);
	setHKLMRegistryString("SOFTWARE\\Hypnos", "Service", "1");

	// clean up
	CloseServiceHandle(myService);
	CloseServiceHandle(scm);
}


static char s_strSrvcMsg[1024];

void goToServiceDir ( void )
{
	char *sz;
	sz = getHKLMRegistryString("SOFTWARE\\Hypnos", "Path");

	SetCurrentDirectory(sz);
	delete sz;

}

int main(int argc, char *argv[])
{


	char *sz;
	sz = getHKLMRegistryString("SOFTWARE\\Hypnos", "Service");

	if (sz!=NULL) {
		ServerScp::g_nDeamonMode = atoi(sz);
		delete sz;
	} else {
		ServerScp::g_nDeamonMode = 0;
	}

/*
	FILE *F;

	F = fopen("server.cfg", "rt");

	if (F==NULL) goToServiceDir();
	else fclose(F);

	loadserverdefaults();
	preloadSections("server.scp");
	preloadSections("shards_server.scp"); */

	if ((argc>1)&&(argv[1][0]=='#'))
	{
		remain(argc,argv);
		return 0;
	}

	if (ServerScp::g_nDeamonMode==0) {
		win32_main(argc, argv);
		return 0;
	}

	g_bNTService = true;

	goToServiceDir();

	SERVICE_TABLE_ENTRY serviceTable[] = {{SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION) ServiceMain},{ NULL, NULL }};

	StartServiceCtrlDispatcher(serviceTable);
	return 0;
}

#endif
