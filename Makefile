CXX = clang++
UNAME := $(shell uname)

multest_objs = multest.o naiveFunctions.o

.PHONY: all clean

all: multest

%.o: %.cpp
	$(CXX) -c -o $@ $<

multest: $(multest_objs)
	$(CXX) -o $@ $^ -larmadillo

clean:
	-rm -f *.o multest
