SOURCES	:=	$(wildcard *.cpp)
OBJECTS	:=	$(SOURCES:.cpp=.o)
DEPS	:=	$(SOURCES:.cpp=.d)

objects.o: $(OBJECTS)

objects.d: deps citem/citem.d cchar/cchar.d
	touch objects.d

citem/citem.o:$(wildcard citem/*.cpp)
	cd citem && $(MAKE)

citem/citem.d:$(wildcard citem/*.cpp)
	cd citem && $(MAKE) citem.d

cchar/cchar.o:$(wildcard cchar/*.cpp)
	cd cchar && $(MAKE)

cchar/cchar.d:$(wildcard cchar/*.cpp)
	cd cchar && $(MAKE) cchar.d

deps: $(SOURCES)
	$(CC) $(DEPFLAGS) $(SOURCES) > /dev/null
