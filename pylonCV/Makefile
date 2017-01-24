# Makefile for Basler pylon sample program
.PHONY: all clean

# The program to build
NAME       := liveHuMomentsPylonCam

# Installation directories for pylon
PYLON_ROOT ?= /opt/pylon5

# Build tools and flags
LD         := $(CXX)
CPPFLAGS   := $(shell $(PYLON_ROOT)/bin/pylon-config --cflags) `pkg-config --cflags --libs opencv`
CXXFLAGS   := -O3 #e.g., CXXFLAGS=-g -O0 for debugging 
LDFLAGS    := $(shell $(PYLON_ROOT)/bin/pylon-config --libs-rpath) 
LDLIBS     := $(shell $(PYLON_ROOT)/bin/pylon-config --libs) `pkg-config --cflags --libs opencv`
#CPPLIBS    :=

# Rules for building
all: $(NAME)

$(NAME): $(NAME).o
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(NAME).o: $(NAME).cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

welcome:
	@echo -------------------------------------------
	@echo ------ Pylon Camera Hu Moments Live -------
	@echo -------------------------------------------
	
clean:
	$(RM) $(NAME).o $(NAME)
