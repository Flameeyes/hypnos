SOURCES	:=	$(wildcard *.cpp)
OBJECTS	:=	$(SOURCES:.cpp=.o)
DEPS	:=	$(SOURCES:.cpp=.d)

archs.o: $(OBJECTS)

archs.d: deps
	touch archs.d

deps: $(SOURCES)
	$(CC) $(DEPFLAGS) $(SOURCES) > /dev/null
