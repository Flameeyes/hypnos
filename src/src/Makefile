SOURCES	:=	$(wildcard *.cpp)
OBJECTS	:=	$(SOURCES:.cpp=.o)

.cpp.o:
	@echo "CXX $@"
	$(CXX) $(INCPATH) -c $(CXXFLAGS) -o $@ $<

built-in.o: $(OBJECTS)
	touch built-in.o
