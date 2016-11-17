CXX=g++
CXXFLAGS= -std=c++11 `pkg-config --libs opencv`
# --cflags
DEPS = 
OBJ = pca.o

all: welcome clean pca
	
%.o: %.cpp $(DEPS)
	@echo Compiling $< with $(CXX)...
	$(CXX) -c -o $@ $< $(CXXFLAGS)

pca: $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	rm -f *.o *~
	
welcome:
	@echo ---------------------------------------
	@echo ---------- WSW OpenCV PCA -------------
	@echo ---------------------------------------
	
# (useful when debugging)
# redirect compilation output to less:
# less < <(make 2>&1)

