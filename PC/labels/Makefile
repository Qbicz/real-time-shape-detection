CXX=g++
CXXFLAGS=-Wall -std=c++11 `pkg-config --cflags  opencv`
LIBS=`pkg-config --libs opencv`
DEPS = 
OBJ = segmentation.o

all: welcome clean segmentation
	
%.o: %.cpp $(DEPS)
	@echo Compiling $< with $(CXX)...
	$(CXX) -c -o $@ $< $(CXXFLAGS)

segmentation: $(OBJ)
	$(CXX) -o $@ $^ $(LIBS)

clean:
	rm -f *.o *~
	
welcome:
	@echo ---------------------------------------
	@echo ---------- WSW OpenCV Model -----------
	@echo ---------------------------------------
	
# (useful when debugging)
# redirect compilation output to less:
# less < <(make 2>&1)
