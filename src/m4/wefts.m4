# Configure paths for Wefts
# Copyright (c) Eric Crahen <zthread@code-foo.com>
# Copyright (c) Diego Pettenò <flameeyes@users.berlios.de>

dnl Detect the library and include paths for Wefts++, perform some test
dnl compilations.
dnl
dnl Should be used in AC_PROG_CC mode before the swtich to C++ if any is made
dnl (eg before AC_LANG_CPLUSPLUS)
dnl
dnl --with-wefts-prefix : Skip detection, use this general path
dnl --with-wefts-exec-prefix : Skip detecting the wefts-conf tool
dnl 
dnl Sets the following variables.
dnl
dnl WEFTS_CXXFLAGS
dnl WEFTS_LIBS
dnl 
AC_DEFUN(AM_PATH_WEFTS,
[

AC_ARG_WITH(wefts-prefix,[  --with-wefts-prefix=PFX   Prefix where Wefts++ is installed (optional)],
            wefts_prefix="$withval", wefts_prefix="")
AC_ARG_WITH(wefts-exec-prefix,[  --with-wefts-exec-prefix=PFX Exec prefix where Wefts++ is installed (optional)],
            wefts_exec_prefix="$withval", wefts_exec_prefix="")
AC_ARG_ENABLE(weftstest, [  --disable-weftstest       Do not try to compile and run a test Wefts++ program],
		    , enable_weftstest=yes)

  if test x$wefts_exec_prefix != x ; then
     wefts_args="$wefts_args --exec-prefix=$wefts_exec_prefix"
     if test x${WEFTS_CONFIG+set} != xset ; then
        WEFTS_CONFIG=$wefts_exec_prefix/bin/wefts-conf
     fi
  fi
  if test x$wefts_prefix != x ; then
     wefts_args="$wefts_args --prefix=$wefts_prefix"
     if test x${WEFTS_CONFIG+set} != xset ; then
        WEFTS_CONFIG=$wefts_prefix/bin/wefts-conf
     fi
  fi

  AC_PATH_PROG(WEFTS_CONFIG, wefts-conf, no)
  AC_MSG_CHECKING(for Wefts++)
  no_wefts=""
  if test "$WEFTS_CONFIG" = "no" ; then
    no_wefts=yes
  else
    WEFTS_CXXFLAGS=`$WEFTS_CONFIG $weftsconf_args --cflags`
    WEFTS_LIBS=`$WEFTS_CONFIG $weftsconf_args --libs`

    if test "x$enable_weftstest" = "xyes" ; then
      ac_save_CXXFLAGS="$CXXFLAGS"
      ac_save_LIBS="$LIBS"
      CXXFLAGS="$CXXFLAGS $WEFTS_CXXFLAGS"
      LIBS="$LIBS $WEFTS_LIBS"

    CXXFLAGS="$ac_save_CXXFLAGS"
    LIBS="$ac_save_LIBS"
    fi
  fi

  if test "x$no_wefts" = x ; then

     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     

  else

     AC_MSG_RESULT(no)

     if test "$WEFTS_CONFIG" = "no" ; then

       echo "*** The wefts-conf script installed by Wefts++ could not be found"
       echo "*** If Wefts++ was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the WEFTS_CONFIG environment variable to the"
       echo "*** full path to wefts-conf."

     else
       if test -f conf.weftstest ; then
        :
       else

          echo "*** Could not run Wefts++ test program, checking why..."
          CXXFLAGS="$CXXFLAGS $WEFTS_CXXFLAGS"
          LIBS="$LIBS $WEFTS_LIBS"

          echo $LIBS;

          AC_TRY_LINK([#include <wefts.h>], 
                      [ return 0; ], [
          echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding Wefts++ or finding the wrong"
          echo "*** version of Wefts++. If it is not finding Wefts++, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	        echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means Wefts++ was incorrectly installed"
          echo "*** or that you have moved Wefts++ since it was installed. In the latter case, you"
          echo "*** may want to edit the wefts-conf script: $WEFTS_CONFIG" ])
          CXXFLAGS="$ac_save_CXXFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi

     WEFTS_CXXFLAGS=""
     WEFTS_LIBS=""
     ifelse([$3], , :, [$3])

  fi

  AC_SUBST(WEFTS_CXXFLAGS)
  AC_SUBST(WEFTS_LIBS)

  rm -f conf.weftstest

])
