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

SOURCES	:=	$(foreach dir, $(SUBDIRS), $(wildcard $(dir)/*.cpp))
OBJECTS	:=	$(SOURCES:.cpp=.o)

TARGET=hypnos

ifndef COMSPEC
	default=linux
else
	default=mingw
endif

ifdef DEBUG
	actdefault =$(default)-debug
else
	actdefault =$(default)
endif

default: $(actdefault)

nodebug: $(default)

debug: $(default)-debug

linux: #$(TARGET)
	@make -f makefiles/Makefile.linux all CXXFLAGS="-O3 -fshort-enums"

linux-debug: #$(TARGET)
	@make -f makefiles/Makefile.linux all CXXFLAGS="-O0 -g -Wall"

mingw: #$(TARGET).exe
	@make -f makefiles/Makefile.mingw all CXXFLAGS="-O3 -fshort-enums"

mingw-debug: #$(TARGET).exe
	@make -f makefiles/Makefile.mingw all CXXFLAGS="-O0 -g -Wall"

xmingw: #$(TARGET).exe
	@make -f makefiles/Makefile.xmingw32 all CXXFLAGS="-O3 -fshort-enums"

xmingw-debug: #$(TARGET).exe
	@make -f makefiles/Makefile.xmingw32 all CXXFLAGS="-O0 -g -Wall"

clean:
	@echo -e "\tCLEAN"
	@rm -f $(OBJECTS) $(TARGET) $(TARGET).exe
