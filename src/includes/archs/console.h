/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Console management
*/
#ifndef __ARCHS_CONSOLE_H__
#define __ARCHS_CONSOLE_H__

void ConOut(char *txt, ...) PRINTF_LIKE(1,2)	//!< Unformatted output
void ErrOut(char *txt, ...) PRINTF_LIKE(1,2)	//!< Formatted as error
void WarnOut(char *txt, ...) PRINTF_LIKE(1,2)	//!< Formatted as warning
void InfoOut(char *txt, ...) PRINTF_LIKE(1,2)	//!< Formatted as info
void PanicOut(char *txt, ...) PRINTF_LIKE(1,2)	//!< Formatted as panic
void SDbgOut(char *txt, ...) PRINTF_LIKE(1,2)	//!< Formatted as amx debug output
void setWinTitle(char *str, ...) PRINTF_LIKE(1,2)

#endif
