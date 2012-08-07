CXX = clang++
UNAME := $(shell uname)

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
	$(CXX) $(ocl_lib) -o $@ $^

clean:
	-rm -f *.o multest
