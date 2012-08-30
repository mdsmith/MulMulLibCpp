UNAME := $(shell uname)
CXX = clang++
ifeq ($(UNAME), Linux)
CXX = g++
endif

all_flags =

.PHONY: all clean
all: mul ocl

core_objs = muller.o helperFunctions.o matrix.o
naive_objs = naiveFunctions.o naiveMuller.o


###### Core Test ######

mul_objs = $(naive_objs) $(core_objs)
mul: flags += -O3

multest.o: multest.cpp
	$(CXX) $(flags) -c -o multest.o multest.cpp

mul: $(mul_objs) multest.o
	$(CXX) $(flags) -o multest $^

###### Opt Test ######

opt_objs = optMuller.o optNF.o naiveMuller.o $(core_objs)
opt_lib = -Ofast -ffast-math -fopenmp
opt: flags = -DOMP
opt: CXX = g++-4.7

optNF.o: naiveFunctions.cpp
	$(CXX) $(flags) -c -o $@ $< $(opt_lib)

optMuller.o: ompMuller.cpp
	$(CXX) $(flags) -c -o $@ $< $(opt_lib)

opttest.o: multest.cpp
	$(CXX) $(flags) -c -o $@ $<

opt: $(opt_objs) opttest.o
	$(CXX) $(flags) -o optmultest $^ $(opt_lib)


###### OCL Test ######

ocl_objs = gpuMuller.o $(naive_objs) $(core_objs)
ocl: flags += -DOCL
#ocl: all_flags += -DWOLFRAM

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
omp: flags += -DOMP -O3
omp: CXX = g++

ompNF.o: naiveFunctions.cpp
	$(CXX)  $(flags) -c -o $@ $< $(omp_lib)

ompMuller.o: ompMuller.cpp
	$(CXX)  -c -o $@ $< $(omp_lib)

ompmultest.o: multest.cpp
	$(CXX) $(flags) -c -o $@ $<

omp: $(omp_objs) ompmultest.o
	$(CXX) $(flags) -o ompmultest $^ $(omp_lib)


###### FMA Test ######

fma_objs = fmaMuller.o naiveMuller.o fmaNF.o $(core_objs)
fma: fma_lib = -Ofast -ffast-math -mfma4 -march=bdver1 -fopenmp
fma: flags = -DFMA
fma: CXX = g++-4.7

fmaNF.o: naiveFunctions.cpp
	$(CXX)  $(flags) -c -o $@ $< $(fma_lib)

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
	$(CXX) $(all_flags) -c -o $@ $<

clean:
	-rm -f *.o multest oclmultest fmamultest ompmultest armamultest optmultest
