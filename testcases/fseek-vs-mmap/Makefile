CFLAGS=-O3 -funroll-loops

all: fseek mmap

fseek: fseektest.cpp
	g++ $(CFLAGS) fseektest.cpp -o fseek

mmap: mmaptest.cpp
	g++ $(CFLAGS) mmaptest.cpp -o mmap
