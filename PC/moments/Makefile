CXX=g++
CXXFLAGS=-std=c++11  -Wall -Wno-deprecated -Wno-deprecated-declarations  -DUNIX -g -DDEBUG 

# Check for 32-bit vs 64-bit
PROC_TYPE = $(strip $(shell uname -m | grep 64))

# Check for Linux-AMD
ifdef AMDAPPSDKROOT
   INC_DIRS=. $(AMDAPPSDKROOT)/include
        ifeq ($(PROC_TYPE),)
                LIB_DIRS=$(AMDAPPSDKROOT)/lib/x86
        else
                LIB_DIRS=$(AMDAPPSDKROOT)/lib/x86_64
        endif
else
# Check for Linux-Nvidia
ifdef CUDA
   INC_DIRS=. $(CUDA)/OpenCL/common/inc
endif
endif

OPENCV_LIBS=`pkg-config --cflags --libs opencv`
OPENCL_LIBS=-lOpenCL -lm
DEPS = 
OBJ = moments.o
PROJ = moments

$(info "INC_DIRS:" $(INC_DIRS))
$(info "LIB_DIRS:" $(LIB_DIRS))

all: welcome $(PROJ)

$(PROJ): $(PROJ).cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(INC_DIRS:%=-I%) $(LIB_DIRS:%=-L%) $(OPENCV_LIBS) $(OPENCL_LIBS)

.PHONY: clean

clean:
	rm -f $(PROJ)
#-f to ignore non existing file

welcome:
	@echo ---------------------------------------
	@echo ---- WSW OpenCV Geometric moments -----
	@echo ---------------------------------------

# (useful when debugging)
# redirect compilation output to less:
# less < <(make 2>&1)

