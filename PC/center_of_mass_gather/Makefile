ROOT_DIR=../..

CXX=g++
CXXFLAGS= -std=c++11 -I$(ROOT_DIR)/modules `pkg-config --cflags opencv` -I$(ROOT_DIR)/PC/svm/includes/
LDLIBS= `pkg-config --libs opencv` -lstdc++
DEPS =

SRC = $(wildcard ./*.cpp) \
      $(wildcard $(ROOT_DIR)/modules/*.cpp)
OBJ = $(SRC:.cpp=.o)

all: welcome center_of_mass_gather

%.o: %.cpp $(DEPS)
	@echo Compiling $< with $(CXX)...
	$(CXX) -c -o $@ $< $(CXXFLAGS)

center_of_mass_gather: $(OBJ)
	$(CXX) -o $@ $^ $(LDLIBS)

clean:
	rm -f *.o *~

less:
	less < <(all 2>&1)

welcome:
	@echo ---------------------------------------
	@echo --- Compute Center of Mass Position ---
	@echo ---------------------------------------

# (useful when debugging)
# redirect compilation output to less:
# less < <(make 2>&1)

