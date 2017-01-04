# Makefile for Basler pylon sample program
.PHONY: all clean

# The program to build
NAME       := pylon_openCV

# Installation directories for pylon
PYLON_ROOT ?= /opt/pylon5

# Build tools and flags
LD         := $(CXX)
CPPFLAGS   := $(shell $(PYLON_ROOT)/bin/pylon-config --cflags) `pkg-config --cflags --libs opencv`
CXXFLAGS   := #e.g., CXXFLAGS=-g -O0 for debugging 
LDFLAGS    := $(shell $(PYLON_ROOT)/bin/pylon-config --libs-rpath) 
LDLIBS     := $(shell $(PYLON_ROOT)/bin/pylon-config --libs) `pkg-config --cflags --libs opencv`

# Rules for building
all: $(NAME)

$(NAME): $(NAME).o
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(NAME).o: $(NAME).cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

welcome:
	@echo ---------------------------------------
	@echo ------ Pylon Camera with OpenCV -------
	@echo ---------------------------------------
	
clean:
	$(RM) $(NAME).o $(NAME)
