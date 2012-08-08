UNAME := $(shell uname)
CXX = clang++
ifeq ($(UNAME), Linux)
CXX = g++
endif

oclmultest_objs = multest.o naiveFunctions.o naiveMuller.o muller.o helperFunctions.o gpuMuller.o
multest_objs = multest.o naiveFunctions.o naiveMuller.o muller.o helperFunctions.o

ocl: flags += -DOCL

ocl_lib = -framework OpenCL
ifeq ($(UNAME), Linux)
ocl_lib = -lOpenCL
endif

.PHONY: all clean

all: mul

%.o: %.cpp
	$(CXX) -c -o $@ $<

mul: $(multest_objs)
	$(CXX) -c -o multest.o multest.cpp
	$(CXX) $(flags) -o multest $^

ocl: $(oclmultest_objs)
	$(CXX) $(flags) -c -o multest.o multest.cpp
	$(CXX) $(flags) -o oclmultest $^ $(ocl_lib)

clean:
	-rm -f *.o multest oclmultest
