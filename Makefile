UNAME := $(shell uname)
CXX = clang++
ifeq ($(UNAME), Linux)
CXX = g++
endif

.PHONY: all clean
all: mul ocl

core_objs = muller.o helperFunctions.o
naive_objs = naiveFunctions.o naiveMuller.o


###### Core Test ######

mul_objs = $(naive_objs) $(core_objs)

multest.o: multest.cpp
	$(CXX) -c -o multest.o multest.cpp

mul: $(mul_objs) multest.o
	$(CXX) -o multest $^


###### OCL Test ######

ocl_objs = gpuMuller.o $(naive_objs) $(core_objs)
ocl: flags = -DOCL

ocl: ocl_lib = -framework OpenCL
ifeq ($(UNAME), Linux)
ocl: ocl_lib = -lOpenCL
endif

oclmultest.o: multest.cpp
	$(CXX) $(flags) -c -o $@ $<

ocl: $(ocl_objs) oclmultest.o
	$(CXX) $(flags) -o oclmultest $^ $(ocl_lib)


###### OMP Test ######

omp_objs = ompMuller.o naiveMuller.o ompNF.o $(core_objs)
omp: omp_lib = -fopenmp
omp: flags = -DOMP
omp: CXX = g++

ompNF.o: naiveFunctions.cpp
	$(CXX)  -c -o $@ $< $(omp_lib)

ompMuller.o: ompMuller.cpp
	$(CXX)  -c -o $@ $< $(omp_lib)

ompmultest.o: multest.cpp
	$(CXX) $(flags) -c -o $@ $<

omp: $(omp_objs) ompmultest.o
	$(CXX) $(flags) -o ompmultest $^ $(omp_lib)


###### FMA Test ######

fma_objs = fmaMuller.o $(naive_objs) $(core_objs)
fma: fma_lib = -mfma4
fma: flags = -DFMA

fmamultest.o: multest.cpp
	$(CXX) $(flags) -c -o $@ $<

fma: $(fma_objs) fmamultest.o
	$(CXX) $(flags) -o fmamultest $^ $(fma_lib)


###### ARMA Test ######

arma_objs = armaMuller.o $(naive_objs) $(core_objs)
arma: arma_lib = -larmadillo
arma: flags = -DARMA

armamultest.o: multest.cpp
	$(CXX) $(flags) -c -o $@ $<

arma: $(arma_objs) armamultest.o
	$(CXX) $(flags) -o armamultest $^ $(arma_lib)


###### ETC ######

%.o: %.cpp
	$(CXX) -c -o $@ $<

clean:
	-rm -f *.o multest oclmultest fmamultest ompmultest armamultest
