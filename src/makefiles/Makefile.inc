CXXFLAGS	= -I $(PWD)/includes/ -pipe
LIBADD		= -lpthread -lmxmlplus -lwefts 

.cpp.o:
	@echo -e "\tCXX   $@"
	$(CXX) $(CXXBASEFLAGS) $(CXXFLAGS) -c -o $@ $<
