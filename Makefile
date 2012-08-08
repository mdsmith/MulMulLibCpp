UNAME := $(shell uname)
CXX = clang++
ifeq ($(UNAME), Linux)
CXX = g++
endif

multest_objs = multest.o naiveFunctions.o naiveMuller.o muller.o helperFunctions.o gpuMuller.o

ocl_lib = -framework OpenCL
ifeq ($(UNAME), Linux)
ocl_lib = -lOpenCL
endif

.PHONY: all clean

all: multest

%.o: %.cpp
	$(CXX) -c -o $@ $<

multest: $(multest_objs)
	$(CXX) -o $@ $^ $(ocl_lib)

clean:
	-rm -f *.o multest
