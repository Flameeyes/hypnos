SOURCES	:=	$(wildcard *.cpp)
OBJECTS	:=	$(SOURCES:.cpp=.o)
DEPS	:=	$(SOURCES:.cpp=.d)

src.o: $(OBJECTS)

src.d: deps
	touch src.d

deps: $(SOURCES)
	$(CC) $(DEPFLAGS) $(SOURCES) > /dev/null

abstraction/cchar.o:$(wildcard cchar/*.cpp)
	cd abstraction && $(MAKE)

abstraction/cchar.d:$(wildcard cchar/*.cpp)
	cd abstraction && $(MAKE) cchar.d
		
amx/cchar.o:$(wildcard cchar/*.cpp)
	cd amx && $(MAKE)

amx/cchar.d:$(wildcard cchar/*.cpp)
	cd amx && $(MAKE) cchar.d
		
archs/cchar.o:$(wildcard cchar/*.cpp)
	cd archs && $(MAKE)

archs/cchar.d:$(wildcard cchar/*.cpp)
	cd archs && $(MAKE) cchar.d
		
backend/cchar.o:$(wildcard cchar/*.cpp)
	cd backend && $(MAKE)

backend/cchar.d:$(wildcard cchar/*.cpp)
	cd backend && $(MAKE) cchar.d
		
encryption/cchar.o:$(wildcard cchar/*.cpp)
	cd encryption && $(MAKE)

encryption/cchar.d:$(wildcard cchar/*.cpp)
	cd encryption && $(MAKE) cchar.d
		
extras/cchar.o:$(wildcard cchar/*.cpp)
	cd extras && $(MAKE)

extras/cchar.d:$(wildcard cchar/*.cpp)
	cd extras && $(MAKE) cchar.d
		
objects/cchar.o:$(wildcard cchar/*.cpp)
	cd objects && $(MAKE)

objects/cchar.d:$(wildcard cchar/*.cpp)
	cd objects && $(MAKE) cchar.d
		
python/cchar.o:$(wildcard cchar/*.cpp)
	cd python && $(MAKE)

python/cchar.d:$(wildcard cchar/*.cpp)
	cd python && $(MAKE) cchar.d
		
races/cchar.o:$(wildcard cchar/*.cpp)
	cd races && $(MAKE)

races/cchar.d:$(wildcard cchar/*.cpp)
	cd races && $(MAKE) cchar.d
		
skills/cchar.o:$(wildcard cchar/*.cpp)
	cd skills && $(MAKE)

skills/cchar.d:$(wildcard cchar/*.cpp)
	cd skills && $(MAKE) cchar.d

