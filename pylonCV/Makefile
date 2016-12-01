CXX=g++
CXXFLAGS= -I/opt/pylon5/include/ `pkg-config --cflags --libs opencv`
DEPS = 
OBJ = pylon_openCV.o

all: welcome clean pylon_openCV
	
%.o: %.cpp $(DEPS)
	@echo Compiling $< with $(CXX)...
	$(CXX) -c -o $@ $< $(CXXFLAGS)

pylon_openCV: $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	rm -f *.o *~
	
welcome:
	@echo ---------------------------------------
	@echo ------ Pylon Camera with OpenCV -------
	@echo ---------------------------------------
	
# (useful when debugging)
# redirect compilation output to less:
# less < <(make 2>&1)

