ROOT_DIR=../../..

CXX=g++
CXXFLAGS= `pkg-config --cflags opencv` -std=c++11
LDLIBS= `pkg-config --libs opencv`
DEPS =
INCLUDES = -I../includes -I$(ROOT_DIR)/modules
OBJ = svm_train_and_save.o

SRC = $(wildcard ./*.cpp) \
      $(wildcard $(ROOT_DIR)/modules/*.cpp)
OBJ = $(SRC:.cpp=.o)

all: welcome clean svm_train_and_save

%.o: %.cpp $(DEPS)
	@echo Compiling $< with $(CXX)...
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(INCLUDES)

svm_train_and_save: $(OBJ)
	$(CXX) -o $@ $^ $(LDLIBS)

clean:
	rm -f *.o *~

less:
	less < <(all 2>&1)

welcome:
	@echo ---------------------------------------
	@echo ---- SVM Training + Save to file ------
	@echo ---------------------------------------

# (useful when debugging)
# redirect compilation output to less:
# less < <(make 2>&1)

