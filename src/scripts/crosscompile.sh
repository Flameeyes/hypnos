#!/bin/sh

export PATH="/opt/xmingw/i386-mingw32msvc/gcc-bin/3.3.1:$PATH"
export ROOTPATH="/opt/xmingw/i386-mingw32msvc/gcc-bin/3.3.1"
export LDPATH="/opt/xmingw/lib/gcc-lib/i386-mingw32msvc/3.3.1/"

make -f Makefile.xmingw32 clean all

