ROOT_DIR=../..

CXX=g++
CXXFLAGS= -std=c++11 -I$(ROOT_DIR)/modules `pkg-config --cflags opencv` -I$(ROOT_DIR)/PC/svm/includes/
LDLIBS= `pkg-config --libs opencv`
DEPS = 

SRC = $(wildcard ./*.cpp) \
      $(wildcard $(ROOT_DIR)/modules/*.cpp)
OBJ = $(SRC:.cpp=.o)

all: welcome hu_moments_gather
	
%.o: %.cpp $(DEPS)
	@echo Compiling $< with $(CXX)...
	$(CXX) -c -o $@ $< $(CXXFLAGS)

hu_moments_gather: $(OBJ)
	$(CXX) -o $@ $^ $(LDLIBS)

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

