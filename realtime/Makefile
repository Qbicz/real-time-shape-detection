CXX=g++
CXXFLAGS= `pkg-config --cflags opencv` -std=c++14 -I./interface -I../modules
LIBS=`pkg-config --libs opencv` -lpthread
# --cflags
DEPS = 
OBJ = implementation/realtime.o implementation/FSM.o implementation/Voter.o implementation/FrameProcessor.o

all: welcome realtime
	
realtime: $(OBJ)
	$(CXX) -o $@ $^ $(LIBS)

clean:
	rm -f implementation/*.o realtime
	
welcome:
	@echo ---------- Acorn Orientation -------------
	
# (useful when debugging)
# redirect compilation output to less:
# less < <(make 2>&1)

