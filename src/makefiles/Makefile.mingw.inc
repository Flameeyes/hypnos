CC		=@gcc
CXX		=@g++
LINK		=@g++
STRIP		=@strip

CXXFLAGS	:= $(CXXFLAGS) -mno-cygwin
LDFLAGS		:= $(LDFLAGS) -mno-cygwin

TARGET		= hypnos.exe

