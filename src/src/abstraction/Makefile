SOURCES	:=	$(wildcard *.cpp)
OBJECTS	:=	$(SOURCES:.cpp=.o)
DEPS	:=	$(SOURCES:.cpp=.d)

abstraction.o: $(OBJECTS)

abstraction.d: deps
	touch abstraction.d

deps: $(SOURCES)
	$(CC) $(DEPFLAGS) $(SOURCES) > /dev/null
