dnl Commodity file with check for compiler's flags support
dnl When calling this function the passed variables will be checked
dnl to support for the required compiler, and then the WORKING_C(XX)FLAGS
dnl variables will be used.

AC_DEFUN(AC_TEST_CFLAGS, [
	
	AC_LANG_SAVE
	
	AC_LANG_C
	# Checking for C support
	for FLAG in $1; do
		SAVE_CFLAGS="$CFLAGS"
		CFLAGS="$CFLAGS $FLAG"
		AC_MSG_CHECKING([whether $CC understands $FLAG])
		AC_TRY_COMPILE([], [], has_option=yes, has_option=no)
		CFLAGS="$SAVE_CFLAGS"
		AC_MSG_RESULT($has_option)
		if test $has_option = yes; then
			WORKING_CFLAGS="$WORKING_CFLAGS $FLAG "
		fi
		unset has_option
		unset SAVE_CFLAGS
	done
	
	AC_LANG_RESTORE
])

AC_DEFUN(AC_TEST_CXXFLAGS, [
	
	AC_LANG_SAVE
	
	AC_LANG_CPLUSPLUS
	# Checking for C++ support
	for FLAG in $1; do
		SAVE_CXXFLAGS="$CXXFLAGS"
		CXXFLAGS="$CXXFLAGS $FLAG"
		AC_MSG_CHECKING([whether $CXX understands $FLAG])
		AC_TRY_COMPILE([], [], has_option=yes, has_option=no)
		CXXFLAGS="$SAVE_CXXFLAGS"
		AC_MSG_RESULT($has_option)
		if test $has_option = yes; then
			WORKING_CXXFLAGS="$WORKING_CXXFLAGS $FLAG "
		fi
		unset has_option
		unset SAVE_CXXFLAGS
	done
	
	AC_LANG_RESTORE
])
