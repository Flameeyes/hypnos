SOURCES	:=	$(wildcard *.cpp)
OBJECTS	:=	$(SOURCES:.cpp=.o)
DEPS	:=	$(SOURCES:.cpp=.d)

amx.o: $(OBJECTS)

amx.d: deps
	touch amx.d

deps: $(SOURCES)
	$(CC) $(DEPFLAGS) $(SOURCES) > /dev/null
