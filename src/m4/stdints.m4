# Configure check for Standardized integer types
# Copyright (c) Diego Pettenò <flameeyes@users.berlios.de>
#
#
# This function checks for standard uintXX_t and intXX_t types in the
# system headers and defines them if they are not found.
#
# TODO: Need to define them if they are not found.

AC_DEFUN(AC_CHECK_STDINTS,
[
	AC_CHECK_HEADERS([stdint.h sys/types.h])
	
	AC_CHECK_TYPES([uint64_t, uint32_t, uint16_t, uint8_t, int64_t, int32_t, int16_t, int8_t], , , [
		#ifdef HAVE_STDINT_H
		#include <stdint.h>
		#elif defined HAVE_SYS_TYPES_H
		#include <sys/types.h>
		#endif
		])
	
])
