# Wrapper to binreloc function.
# This function is a wrapper to the binreloc.m4 file which defines
# the official function to check for binreloc.
# This function, other than calling AM_BINRELOC() function, also
# cheks for dir passed to the configure script when binreloc isn't
# applicable (and we are in an unix environment).

AC_DEFUN([AC_HYPNOSRELOC],
[
	AC_WINENV()

	if test "$is_win" == "no"; then
		AM_BINRELOC()
		
		if test "x$br_cv_binreloc" != "xyes"; then
			DEFDIRS="-DDATADIR=\"$datadir\" "
		fi
		
		AM_CONDITIONAL(BINRELOC, test "x$br_cv_binreloc" = "xyes")
	fi
])
