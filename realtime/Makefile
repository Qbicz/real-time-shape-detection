CXX=g++
CXXFLAGS= `pkg-config --cflags opencv` -std=c++14
LIBS=`pkg-config --libs opencv` -lpthread
# --cflags
DEPS = 
OBJ = realtime.o FSM.o Voter.o

all: welcome realtime
	
%.o: %.cpp $(DEPS)
	@echo Compiling $< with $(CXX)...
	$(CXX) -c -o $@ $< $(CXXFLAGS)

realtime: $(OBJ)
	$(CXX) -o $@ $^ $(LIBS)

clean:
	rm -f *.o *~
	
welcome:
	@echo ---------- Acorn Orientation -------------
	
# (useful when debugging)
# redirect compilation output to less:
# less < <(make 2>&1)

