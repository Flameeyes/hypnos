###############################################################################
#
# This is a special makefile, instead of use different amkefiles for different
# platforms, you can use this makefile and tell it the platform where you are
# going to build Hypnos UO Server Emulator.
#
# Please note that this file can be used only by GNU's make, so if you have a
# different default make than that, you should launch it using 'gmake' name.
#
###############################################################################

SUBDIRSTMP  = src src/abstraction src/archs src/backend src/encryption src/extras src/objects src/objects/cchar src/objects/citem src/objects/cnpc src/races src/skills
LIBSUBDIRSTMP = libhypnos libhypnos/muls

SUBDIRS = $(foreach dir, $(SUBDIRSTMP), $(PWD)/$(dir))
LIBSUBDIRS = $(foreach dir, $(LIBSUBDIRSTMP), $(PWD)/$(dir))

export SOURCES := $(foreach dir, $(SUBDIRS), $(wildcard $(dir)/*.cpp))
export OBJECTS := $(SOURCES:.cpp=.o)

export LIBSRCS := $(foreach dir, $(LIBSUBDIRS), $(wildcard $(dir)/*.cpp))
export LIBOBJS := $(LIBSRCS:.cpp=.o)

ifndef COMSPEC
	default=linux
	include makefiles/Makefile.linux.inc
else
	default=mingw
	include makefiles/Makefile.mingw.inc
endif

ifdef DEBUG
	actdefault =$(default)-debug
	CXXBASEFLAGS = -O0 -g -Wall
else
	actdefault =$(default)
	CXXBASEFLAGS = -O3 -fshort-enums -DNDEBUG
endif

.SUFFIXES: .cpp

default: $(actdefault)

nodebug: $(default)

debug: $(default)-debug

linux:
	@make -f makefiles/Makefile.linux libhypnos/libhypnos.a CXXFLAGS="$(CXXFLAGS) -O3 -fshort-enums -DNDEBUG"
	@make -f makefiles/Makefile.linux $(TARGET) CXXFLAGS="$(CXXFLAGS) -O3 -fshort-enums -DNDEBUG"

linux-debug:
	@make -f makefiles/Makefile.linux libhypnos/libhypnos.a CXXFLAGS="$(CXXFLAGS) -O0 -g -Wall"
	@make -f makefiles/Makefile.linux $(TARGET) CXXFLAGS="$(CXXFLAGS) -O0 -g -Wall"

mingw:
	@make -f makefiles/Makefile.linux libhypnos/libhypnos.a CXXFLAGS="$(CXXFLAGS) -O3 -fshort-enums -DNDEBUG"
	@make -f makefiles/Makefile.mingw $(TARGET).exe CXXFLAGS="$(CXXFLAGS) -O3 -fshort-enums -DNDEBUG"

mingw-debug:
	@make -f makefiles/Makefile.linux libhypnos/libhypnos.a CXXFLAGS="$(CXXFLAGS) -O0 -g -Wall"
	@make -f makefiles/Makefile.mingw $(TARGET).exe CXXFLAGS="$(CXXFLAGS) -O0 -g -Wall"

xmingw:
	@make -f makefiles/Makefile.linux libhypnos/libhypnos.a CXXFLAGS="$(CXXFLAGS) -O3 -fshort-enums -DNDEBUG"
	@make -f makefiles/Makefile.xmingw32 $(TARGET).exe CXXFLAGS="$(CXXFLAGS) -O3 -fshort-enums -DNDEBUG"

xmingw-debug:
	@make -f makefiles/Makefile.linux libhypnos/libhypnos.a CXXFLAGS="$(CXXFLAGS) -O0 -g -Wall"
	@make -f makefiles/Makefile.xmingw32 $(TARGET).exe CXXFLAGS="$(CXXFLAGS) -O0 -g -Wall"

clean:
	@echo -e "\tCLEAN"
	@rm -f $(OBJECTS) $(TARGET) $(TARGET).exe libhypnos/libhypnos.a

include makefiles/Makefile.inc
