# Configure check for Precompiled Headers
# Copyright (c) Diego Pettenò <flameeyes@users.berlios.de>
#
#
# This simple function checks for gcc 3.4 precompiled headers support
# and then defines HAVE_PCH if the PCH can be used.

AC_DEFUN(AC_PCH,
[
	AC_ARG_ENABLE(pch,
		AC_HELP_STRING([--enable-pch],
			[enables precompiled headers support]),
		[ use_pch=$enableval ], [ use_pch=no ] )
	
	if test "$use_pch" = "yes"; then
		AC_MSG_CHECKING(whether $CXX supports precompiling c++ header files)
		echo >conftest.h
		if $CXX conftest.h >/dev/null 2>/dev/null; then
			have_pch=yes
		else
			have_pch=no
		fi
		rm -f conftest.h conftest.h.gch
		AC_MSG_RESULT($have_pch)
	else
		have_pch=no
	fi
	
	AM_CONDITIONAL(HAVE_PCH, test "$have_pch" = "yes")
])
