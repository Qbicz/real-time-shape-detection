CXX=g++
CXXFLAGS= `pkg-config --cflags opencv` -std=c++14
LIBS=`pkg-config --libs opencv`
# --cflags
DEPS = 
OBJ = capture.o

all: welcome clean capture
	
%.o: %.cpp $(DEPS)
	@echo Compiling $< with $(CXX)...
	$(CXX) -c -o $@ $< $(CXXFLAGS)

capture: $(OBJ)
	$(CXX) -o $@ $^ $(LIBS)

clean:
	rm -f *.o *~
	
welcome:
	@echo ---------------------------------------
	@echo ---------- WSW OpenCV PCA -------------
	@echo ---------------------------------------
	
# (useful when debugging)
# redirect compilation output to less:
# less < <(make 2>&1)

