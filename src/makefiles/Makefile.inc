CXXFLAGS	= -I $(PWD)/includes/ -I $(PWD)/libhypnos/ -pipe
LIBADD		= -lpthread -lmxmlplus -lwefts 

.cpp.o:
	@echo -e "\tCXX   $@"
	$(CXX) $(CXXBASEFLAGS) $(CXXFLAGS) -c -o $@ $<
