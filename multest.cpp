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
    m->update_A(A, 0, DIM1, DIM2);
    m->update_B(B, 0, DIM2, DIM1);
    m->multiply();
    float* mulC = m->get_C(0, DIM1, DIM1);

    bool passed = true;
    for (int i=0; i < DIM1*DIM2; i++)
    {
        if (goldenC[i] != mulC[i])
            passed = false;
    }
    if (passed)
        cout << "Test PASSED!" << endl;
    else
        cout << "Test FAILED!" << endl;
    return 0;
}
