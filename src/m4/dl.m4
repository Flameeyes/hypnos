dnl
dnl Check for dlopen symbol and set DYNAMIC_LD_LIBS.
dnl
dnl AM_DL()
dnl

AC_DEFUN([AM_DL], [

	AC_WINENV()
	if test "$is_win" == "yes"; then
		have_dl=yes
		AC_MSG_RESULT(using WIN32 dinamic libraries)
	else
		AC_CHECK_LIB(c, dlopen, [DYNAMIC_LD_LIBS=""; have_dl=yes])
	
		if test x$have_dl != "xyes"; then
			AC_CHECK_LIB(dl, dlopen, [DYNAMIC_LD_LIBS="-ldl"; have_dl=yes])
		fi
	
		if test x$have_dl != "xyes"; then
			AC_MSG_ERROR(dynamic linker needed)
		fi
	
		AC_SUBST(DYNAMIC_LD_LIBS)
	fi

])
