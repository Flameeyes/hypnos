CC		=@gcc
CXX		=@g++
LINK		=@g++
STRIP		=@strip
AR		=@ar

CXXFLAGS	:= $(CXXFLAGS) -mno-cygwin
LDFLAGS		:= $(LDFLAGS) -mno-cygwin

TARGET		= hypnos

