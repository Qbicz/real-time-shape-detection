CXX=g++
CXXFLAGS= `pkg-config --libs opencv`
# --cflags
DEPS = 
OBJ = fps.o

all: welcome clean fps
	
%.o: %.cpp $(DEPS)
	@echo Compiling $< with $(CXX)...
	$(CXX) -c -o $@ $< $(CXXFLAGS)

fps: $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	rm -f *.o *~
	
welcome:
	@echo ---------------------------------------
	@echo -- OpenCV Check FPS of video capture --
	@echo ---------------------------------------
	
