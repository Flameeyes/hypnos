SOURCES	:=	$(wildcard *.cpp)
OBJECTS	:=	$(SOURCES:.cpp=.o)
DEPS	:=	$(SOURCES:.cpp=.d)

cchar.o: $(OBJECTS)

cchar.d: deps
	touch cchar.d

deps: $(SOURCES)
	$(CC) $(DEPFLAGS) $(SOURCES) > /dev/null
