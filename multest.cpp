// Author: Martin Smith martin.audacis@gmail.com
// Created: Early August 2012
// Last Edited: 8/5/12

#ifdef OCL
#include "gpuMuller.h"
#elif defined OMP
#include "ompMuller.h"
#elif defined FMA
#include "fmaMuller.h"
#elif defined ARMA
#include "armaMuller.h"
#endif

#include <iostream>
#include "naiveFunctions.h"
#include "muller.h"
#include <cstdlib>
#include <sys/time.h>
using namespace std;

#define DIM1 2000
#define DIM2 200
float A[DIM1*DIM2];
float B[DIM2*DIM1];

int test_muller(Muller* m, float* golden);

int main()
{
    int allpasscode = 0;

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

    cout << "Golden (Naive): " << elapsedTime << " ms.\n";

    gettimeofday(&t1, NULL);

    for (int i = 0; i < 100; i++)
    {

#ifdef OCL
    GPUMuller gm = GPUMuller();
    allpasscode |= test_muller(&gm, goldenC);
#elif defined OMP
    OMPMuller om = OMPMuller();
    allpasscode |= test_muller(&om, goldenC);
#elif defined FMA
    FMAMuller fm = FMAMuller();
    allpasscode |= test_muller(&fm, goldenC);
#elif defined ARMA
    ArmaMuller am = ArmaMuller();
    allpasscode |= test_muller(&am, goldenC);
#else
    NaiveMuller nm = NaiveMuller();
    allpasscode |= test_muller(&nm, goldenC);
#endif

    }
    gettimeofday(&t2, NULL);

    elapsedTime = (t2.tv_sec -t1.tv_sec) * 1000.0;
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;

    cout << "per trial (mean): " << elapsedTime/100 << " ms.\n";

    return allpasscode;
}

int test_muller(Muller* m, float* golden)
{
    //timeval t1, t2;
    //double elapsedTime;

    m->set_A(A, DIM1, DIM2);
    m->set_B(B, DIM2, DIM1);

    //gettimeofday(&t1, NULL);

    float* mulC = m->get_C(0, DIM1, DIM1);

    //gettimeofday(&t2, NULL);

    //elapsedTime = (t2.tv_sec -t1.tv_sec) * 1000.0;
    //elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;

    //cout << m->get_name() << ": " << elapsedTime << " ms.\n";

    bool passed = true;
    for (int i=0; i < DIM1*DIM2; i++)
    {
        //cout << golden[i] << " vs " << mulC[i] << endl;
        if (golden[i] != mulC[i])
            passed = false;
    }
    if (passed)
    {
        //cout << "Test PASSED!" << endl;
        return 0;
    }
    else
    {
        //cout << "Test FAILED!" << endl;
        return 1;
    }
}
