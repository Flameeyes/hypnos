# Configure check for Intel C++ Compiler
# Copyright (c) Diego Pettenò <flameeyes@users.berlios.de>
#
#
# This check is needed to allow warnings activation and deletion.
# After calling that function, you'll have a $is_icc variable which is 'yes'
# if the compiler is the Intel C++ Compiler.

AC_DEFUN(AC_ICC_COMPILER,
[
	AC_MSG_CHECKING([for Intel C++ Compiler])
	AC_EGREP_CPP(yes,
	[#ifdef __ICC
	  yes
	#endif
	], is_icc=yes, is_icc=no)
	
	if test "$is_icc" = "yes"; then
		AC_MSG_RESULT(yes)
	else
		AC_MSG_RESULT(no)
	fi
])
