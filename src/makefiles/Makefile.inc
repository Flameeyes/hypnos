.cpp.o:
	@echo -e "\tCXX   $@"
	$(CXX) $(CXXBASEFLAGS) $(CXXFLAGS) -c -o $@ $<

