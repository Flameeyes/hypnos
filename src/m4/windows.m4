# Commodity file with check for Windows environment
# This function is called whenever we need to check for Windows
# caching the result.

AC_DEFUN([AC_WINENV], 
[
	AC_CACHE_CHECK([for Windows environment], [is_win], [
		AC_EGREP_CPP(yes,
		[#ifdef WIN32
		yes
		#endif
		], is_win=yes, is_win=no)
	])
])
