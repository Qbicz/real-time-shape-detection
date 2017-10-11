CXX=g++
CXXFLAGS= `pkg-config --libs opencv`
DEPS = 
OBJ = hu_moments_gather.o

all: welcome clean hu_moments_gather
	
%.o: %.cpp $(DEPS)
	@echo Compiling $< with $(CXX)...
	$(CXX) -c -o $@ $< $(CXXFLAGS)

hu_moments_gather: $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	rm -f *.o *~

less:
	less < <(all 2>&1)
	
welcome:
	@echo ---------------------------------------
	@echo ------- Compute Hu moments ------------
	@echo ---------------------------------------
	
# (useful when debugging)
# redirect compilation output to less:
# less < <(make 2>&1)

