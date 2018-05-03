CXX=g++
CXXFLAGS= -std=c++11 `pkg-config --cflags --libs opencv`
DEPS = 
OBJ = OpenCV-CLIntro.o

all: welcome clean OpenCV-CLIntro
	
%.o: %.cpp $(DEPS)
	@echo Compiling $< with $(CXX)...
	$(CXX) -c -o $@ $< $(CXXFLAGS)

OpenCV-CLIntro: $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	rm -f *.o *~
	
welcome:
	@echo ---------------------------------------
	@echo ---- WSW OpenCV Geometric moments -----
	@echo ---------------------------------------
	
# (useful when debugging)
# redirect compilation output to less:
# less < <(make 2>&1)

