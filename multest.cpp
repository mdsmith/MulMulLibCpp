#include <iostream>
#include "naiveFunctions.h"
#include "muller.h"
#include <cstdlib>
using namespace std;

#define DIM1 20
#define DIM2 20
float A[DIM1*DIM2];
float B[DIM2*DIM1];


int main()
{
    srand((unsigned)time(0));

    for (int i = 0; i < DIM1*DIM2; i++)
    {
        A[i] = (rand()%100)+1;
        B[i] = (rand()%100)+1;
    }

    float* goldenC = naive_matrix_multiply(A,B, DIM1, DIM2, DIM1);

    NaiveMuller nM = NaiveMuller();
    Muller* m = &nM;
    m->test();
    cout << "main checking in" << endl;
    return 0;
}

// XXX pass in the arrays
// XXX get back the arrays
// XXX compare to the output of naiveFunctions.h
// XXX print passed/fail
