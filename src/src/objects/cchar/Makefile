SOURCES	:=	$(wildcard *.cpp)
OBJECTS	:=	$(SOURCES:.cpp=.o)

built-in.o: $(OBJECTS)
	touch built-in.o

.cpp.o:
	$(CXX) $(DEPFLAGS) -c $(CXXFLAGS) $(INCPATH) -o $@ $<
