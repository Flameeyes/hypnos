dnl
dnl Check for dlopen symbol and set DYNAMIC_LD_LIBS.
dnl
dnl AM_DL()
dnl

AC_DEFUN([AM_DL], [

case "$host" in
  *-*-mingw* | *-*-cygwin*)
    dnl check if we are using the cygwin, mingw or cygwin with mno-cygwin mode
    dnl in which case we are actually dealing with a mingw32 compiler
    case "$host" in
      *-*-mingw32*)
        SYS=mingw32
        ;;
      *-*-cygwin*)
        AC_EGREP_CPP(pattern, 
                     [#ifdef WIN32
                      yes
                      #endif],
                     SYS=mingw32, SYS=cygwin)
        ;;
    esac

    if test "$SYS" = "mingw32"; then
      have_dl=yes
      AC_MSG_RESULT(using WIN32 dinamic libraries)
    fi
  ;;
  
  *)
    AC_CHECK_LIB(c, dlopen,
     [DYNAMIC_LD_LIBS=""
      have_dl=yes])

    if test x$have_dl != "xyes"; then
      AC_CHECK_LIB(dl, dlopen,
       [DYNAMIC_LD_LIBS="-ldl"
        have_dl=yes])
    fi

    if test x$have_dl != "xyes"; then
      AC_MSG_ERROR(dynamic linker needed)
    fi

    AC_SUBST(DYNAMIC_LD_LIBS)
  ;;
esac

])
