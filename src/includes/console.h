/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Console Handling Functions
*/
#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#ifdef WIN32
#define __CDECL __cdecl
#else
#define __CDECL
#endif


#ifdef __cplusplus

extern "C" void __CDECL ConOut(char *txt, ...) PRINTF_LIKE(1,2)	//!< Unformatted output
extern "C" void __CDECL ErrOut(char *txt, ...) PRINTF_LIKE(1,2)	//!< Formatted as error
extern "C" void __CDECL WarnOut(char *txt, ...) PRINTF_LIKE(1,2)	//!< Formatted as warning
extern "C" void __CDECL InfoOut(char *txt, ...) PRINTF_LIKE(1,2)	//!< Formatted as info
extern "C" void __CDECL PanicOut(char *txt, ...) PRINTF_LIKE(1,2)	//!< Formatted as panic
extern "C" void __CDECL DmpOut(char *txt, ...) PRINTF_LIKE(1,2)	//!< Formatted as internal-development debug info (should not be in releases)
extern "C" void __CDECL SDbgOut(char *txt, ...) PRINTF_LIKE(1,2)	//!< Formatted as amx debug output
extern "C" void __CDECL SDbgIn(char *s, int n);				//!< Formatted as amx debug info
extern "C" void __CDECL STraceOut(char *txt, ...) PRINTF_LIKE(1,2)
void clearscreen( void );
void setWinTitle(char *str, ...) PRINTF_LIKE(1,2)
void constart( void );

#define ALERT_TYPE_OK 0
#define ALERT_TYPE_YESNO 1

#define ALERT_MESSAGE 0
#define ALERT_INFO 1
#define ALERT_WARNING 2
#define ALERT_ERROR 3
#define ALERT_FATAL 4

bool alertBox(char *msg, char *title, int gravity = 0, int type = 0);

#else
void __CDECL ConOut(char *txt, ...) PRINTF_LIKE(1,2)
void __CDECL STraceOut(char *txt, ...) PRINTF_LIKE(1,2)
void __CDECL SDbgOut(char *txt, ...) PRINTF_LIKE(1,2)
void __CDECL SDbgIn(char *s, int n);
#endif

void initConsole();
char* getNoXDate();

#endif //__CONSOLE_H__
