CXXBASEFLAGS	=-Wall -I $(PWD)/includes/ -pipe 
LIBADD		=-lpthread -lmxmlplus -lwefts 

TARGET		= hypnos

SUBDIRS		= src src/abstraction src/archs src/backend src/encryption src/extras src/objects src/objects/cchar src/objects/citem src/objects/cnpc src/races src/skills

SOURCES	:=	$(foreach dir, $(SUBDIRS), $(wildcard $(dir)/*.cpp))
OBJECTS	:=	$(SOURCES:.cpp=.o)

.SUFFIXES: .cpp

all: ${TARGET}

.cpp.o:
	@echo -e "\tCXX   $@"
	$(CXX) $(CXXBASEFLAGS) $(CXXFLAGS) -c -o $@ $<

clean:
	-rm -f $(OBJECTS) $(TARGET)
