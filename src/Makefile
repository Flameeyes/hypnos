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

export SUBDIRS  = src src/abstraction src/archs src/backend src/encryption src/extras src/objects src/objects/cchar src/objects/citem src/objects/cnpc src/races src/skills

export SOURCES := $(foreach dir, $(SUBDIRS), $(PWD)/$(wildcard $(dir)/*.cpp))
export OBJECTS := $(SOURCES:.cpp=.o)

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
	CXXBASEFLAGS = -O3 -fshort-enums
endif

.SUFFIXES: .cpp

default: $(actdefault)

nodebug: $(default)

debug: $(default)-debug

linux: #$(TARGET)
	@make -f makefiles/Makefile.linux $(TARGET) CXXFLAGS="$(CXXFLAGS) -O3 -fshort-enums"

linux-debug: #$(TARGET)
	@make -f makefiles/Makefile.linux $(TARGET) CXXFLAGS="$(CXXFLAGS) -O0 -g -Wall"

mingw: #$(TARGET).exe
	@make -f makefiles/Makefile.mingw $(TARGET).exe CXXFLAGS="$(CXXFLAGS) -O3 -fshort-enums"

mingw-debug: #$(TARGET).exe
	@make -f makefiles/Makefile.mingw $(TARGET).exe CXXFLAGS="$(CXXFLAGS) -O0 -g -Wall"

xmingw: #$(TARGET).exe
	@make -f makefiles/Makefile.xmingw32 $(TARGET).exe CXXFLAGS="$(CXXFLAGS) -O3 -fshort-enums"

xmingw-debug: #$(TARGET).exe
	@make -f makefiles/Makefile.xmingw32 $(TARGET).exe CXXFLAGS="$(CXXFLAGS) -O0 -g -Wall"

clean:
	@echo -e "\tCLEAN"
	@rm -f $(OBJECTS) $(TARGET) $(TARGET).exe

include makefiles/Makefile.inc
