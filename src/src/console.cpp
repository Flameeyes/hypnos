/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2003 - NoX-Wizard Project                                  |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "common_libs.h"
#include "version.h"
#include "arch/console.h"

static FILE *s_fileStdOut = NULL;

void setWinTitle(char *str, ...)
{
	if (ServerScp::g_nDeamonMode!=0) return;

	char *temp; //xan -> this overrides the global temp var
	va_list argptr;

	va_start( argptr, str );
	vasprintf( &temp, str, argptr );
	va_end( argptr );
	
	#ifdef __unix__
		ConOut("\033]0;%s\007", temp); // xterm code
	#elif defined(WIN32)
		SetConsoleTitle(temp);
	#endif
	
	free(temp);
}

void constart( void )
{
	setWinTitle("Hypnos %s", strVersion);
	#ifdef WIN32
	if (ServerScp::g_nDeamonMode==0) {
		HANDLE Buff = GetStdHandle(STD_OUTPUT_HANDLE);
		COORD coord; coord.X = 80; coord.Y = (short)ServerScp::g_nLineBuffer;
		WORD arr[80];
		DWORD  w;

		SetConsoleScreenBufferSize(Buff, coord);

		unsigned short color;

		color=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN;


		
		SetConsoleTextAttribute(Buff,color);

		coord.X = coord.Y = 0;
		for (int i = 0; i<80; i++)
			arr[i] = color;
		
		for (coord.Y = 0; coord.Y<1024; coord.Y++) WriteConsoleOutputAttribute(Buff, (CONST WORD *)arr,80,coord,(LPDWORD )&w);       // actual number written

	}
	#endif
}

void initConsole()
{
	if ((ServerScp::g_nRedirectOutput)||(ServerScp::g_nDeamonMode)) {
		if(s_fileStdOut==NULL) s_fileStdOut = fopen(ServerScp::g_szOutput,"wt");
		if(s_fileStdOut==NULL) s_fileStdOut = fopen("nxwout","wt");
		if(s_fileStdOut==NULL) exit(1);
	} else s_fileStdOut = stdout;
}


extern "C" void ConOut(char *txt, ...)
{
	va_list argptr;

	char *temp;
	va_start( argptr, txt );
	vasprintf( &temp, txt, argptr );
	va_end( argptr );

#ifndef _WINDOWS
	fprintf(s_fileStdOut, temp);
	fflush(s_fileStdOut);
#else
	xwprintf("%s", temp);
#endif
	free(temp);
}

extern void setWinTitle(char *str, ...);

static char s_szErrMsg[2048];

const std::string &getDate()
{
	time_t TIME;
	tm* T;
	time(&TIME);
	T = localtime(&TIME);

	char *tmp;
	asprintf(&tmp, "[%02d/%02d/%04d %02d:%02d:%02d]",
	T->tm_mday, T->tm_mon+1, T->tm_year+1900, T->tm_hour, T->tm_min, T->tm_sec);
	
	std::string s(tmp);
	free(tmp);

	return s;
}

/*******************************************************
 ** NEW GEN Console Functions                         **
 *******************************************************/
void AnsiOut(char *txt)
{
	#if defined(__unix__)
		ConOut(txt);
	#endif
}

void ErrOut(char *txt, ...)
{
	va_list argptr;

	va_start( argptr, txt );
	vsnprintf( s_szErrMsg, sizeof(s_szErrMsg)-1, txt, argptr );
	va_end( argptr );

	AnsiOut("\x1B[1;31m");
	ConOut("E %s - %s", getDate().c_str(), s_szErrMsg);
	AnsiOut("\x1B[0m");
}

void WarnOut(char *txt, ...)
{
	va_list argptr;

	va_start( argptr, txt );
	vsnprintf( s_szErrMsg, sizeof(s_szErrMsg)-1, txt, argptr );
	va_end( argptr );

	AnsiOut("\x1B[1;33m");
	ConOut("W %s - %s", getDate().c_str(), s_szErrMsg);
	AnsiOut("\x1B[0m");
}


void InfoOut(char *txt, ...)
{
	va_list argptr;

	va_start( argptr, txt );
	vsnprintf( s_szErrMsg, sizeof(s_szErrMsg)-1, txt, argptr );
	va_end( argptr );
	
	AnsiOut("\x1B[1;34m");
	ConOut("i %s - %s", getDate().c_str(), s_szErrMsg);
	AnsiOut("\x1B[0m");
}

void PanicOut(char *txt, ...)
{
	va_list argptr;

	va_start( argptr, txt );
	vsnprintf( s_szErrMsg, sizeof(s_szErrMsg)-1, txt, argptr );
	va_end( argptr );

	AnsiOut("\x1B[1;31m");
	ConOut("! %s - %s", getDate().c_str(), s_szErrMsg);
	AnsiOut("\x1B[0m");
}


void DmpOut(char *txt, ...)
{
	va_list argptr;

	va_start( argptr, txt );
	vsnprintf( s_szErrMsg, sizeof(s_szErrMsg)-1, txt, argptr );
	va_end( argptr );

	AnsiOut("\x1B[33m");
	ConOut("--> %s", s_szErrMsg);
	AnsiOut("\x1B[0m");
}

