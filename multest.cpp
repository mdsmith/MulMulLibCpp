// Author: Martin Smith martin.audacis@gmail.com
// Created: Early August 2012
// Last Edited: 8/5/12

#include <iostream>
#include "naiveFunctions.h"
#include "muller.h"
#include <cstdlib>
#include <sys/time.h>
using namespace std;

#define DIM1 2000
#define DIM2 2000
float A[DIM1*DIM2];
float B[DIM2*DIM1];


int main()
{
    srand((unsigned)time(0));

    for (int i = 0; i < DIM1*DIM2; i++)
    {
        A[i] = (rand()%10)+1;
        B[i] = (rand()%10)+1;
    }


    timeval t1, t2;
    double elapsedTime;
    gettimeofday(&t1, NULL);

    float* goldenC = naive_matrix_multiply(A,B, DIM1, DIM2, DIM1);

    gettimeofday(&t2, NULL);

    elapsedTime = (t2.tv_sec -t1.tv_sec) * 1000.0;
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;

    cout << "Naive: " << elapsedTime << " ms.\n";


    NaiveMuller nM = NaiveMuller();
    GPUMuller gM = GPUMuller();
    Muller* m = &gM;
    m->set_A(A, DIM1, DIM2);
    m->set_B(B, DIM2, DIM1);


    gettimeofday(&t1, NULL);

    float* mulC = m->get_C(0, DIM1, DIM1);

    gettimeofday(&t2, NULL);

    elapsedTime = (t2.tv_sec -t1.tv_sec) * 1000.0;
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;

    cout << "GPU: " << elapsedTime << " ms.\n";


    bool passed = true;
    for (int i=0; i < DIM1*DIM2; i++)
    {
        //cout << goldenC[i] << " vs " << mulC[i] << endl;
        if (goldenC[i] != mulC[i])
            passed = false;
    }
    if (passed)
        cout << "Test PASSED!" << endl;
    else
        cout << "Test FAILED!" << endl;
    return 0;
}
