CXX=g++
CXXFLAGS= `pkg-config --libs opencv` -std=c++11
# --cflags
DEPS = 
INCLUDES = -I../includes
OBJ = svm_train_and_save.o


all: welcome clean svm_train_and_save
	
%.o: %.cpp $(DEPS)
	@echo Compiling $< with $(CXX)...
	$(CXX) -c $(INCLUDES) -o $@ $< $(CXXFLAGS)

svm_train_and_save: $(OBJ)
	$(CXX) $(INCLUDES) -o $@ $^ $(CXXFLAGS)

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

