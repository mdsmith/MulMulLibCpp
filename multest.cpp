// Author: Martin Smith martin.audacis@gmail.com
// Created: Early August 2012
// Last Edited: 8/22/12

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
#include <stdio.h>
#include "naiveFunctions.h"
#include "muller.h"
#include <cstdlib>
#include <sys/time.h>
using namespace std;

#define DIM1 4
#define DIM2 4
//float A[DIM1*DIM2];
//float B[DIM2*DIM1];
float* A;
float* B;

int update_submatrix_offset_test(Muller* m, float* golden);
void print_mat(float* m, int w, int h);

int main()
{
    A = new float[DIM1*DIM2];
    B = new float[DIM1*DIM2];
    int allpasscode = 0;

    srand((unsigned)time(0));

    for (int i = 0; i < DIM1*DIM2; i++)
    {
        //A[i] = (rand()%10)+1;
        //B[i] = (rand()%10)+1;
        A[i] = i;
        B[i] = DIM1*DIM2 - i;
    }
    cout << "A at the start: ";
    print_mat(A, DIM1, DIM2);
    cout << "B at the start: ";
    print_mat(B, DIM2, DIM1);

/*
    timeval t1, t2;
    double elapsedTime;
    gettimeofday(&t1, NULL);
*/

    float* goldenC = naive_matrix_multiply(A,B, DIM1, DIM2, DIM1);
    cout << "Golden C: ";
    print_mat(goldenC, DIM1, DIM1);
    /*
    // multiple multiply!
    goldenC = naive_matrix_multiply(goldenC,A, DIM1, DIM1, DIM2);
    print_mat(goldenC, DIM1, DIM2);

    gettimeofday(&t2, NULL);

    elapsedTime = (t2.tv_sec -t1.tv_sec) * 1000.0;
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;

    cout << "Golden (Naive): " << elapsedTime << " ms.\n";

    gettimeofday(&t1, NULL);
*/

    //for (int i = 0; i < 1000; i++)
    //{

#ifdef OCL
    GPUMuller gm = GPUMuller();
    allpasscode |= update_submatrix_offset_test(&gm, goldenC);
#elif defined OMP
    OMPMuller om = OMPMuller();
    allpasscode |= update_submatrix_offset_test(&om, goldenC);
#elif defined FMA
    FMAMuller fm = FMAMuller();
    allpasscode |= update_submatrix_offset_test(&fm, goldenC);
#elif defined ARMA
    ArmaMuller am = ArmaMuller();
    allpasscode |= update_submatrix_offset_test(&am, goldenC);
#else
    NaiveMuller nm = NaiveMuller();
    allpasscode |= update_submatrix_offset_test(&nm, goldenC);
#endif

    //}
/*
    gettimeofday(&t2, NULL);

    elapsedTime = (t2.tv_sec -t1.tv_sec) * 1000.0;
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;

    cout << "per trial (mean): " << elapsedTime/1000 << " ms.\n";
*/

    return allpasscode;
}

int large_test(Muller* m, float* golden)
{
    cout << "setting A..." << endl;
    m->set_A(A, DIM1, DIM2);
    cout << "setting B..." << endl;
    m->set_B(A, DIM1, DIM2);
    cout << "setting C..." << endl;
    m->set_C(A, DIM1, DIM2);

    float* mulC = m->get_C(1, 2, 2);
    cout << "OCL mul1: ";
    print_mat(mulC, 2, 2);

    cout << "A before update: ";
    m->print_A(0, DIM1, DIM2);

    float* D = new float[DIM1*DIM2];
    for (int i = 0; i < DIM1*DIM2; i++)
        D[i] = i;
    m->update_A(D, 0, 2, 2, DIM1, DIM2);

    cout << "A after update: ";
    m->print_A(0, DIM1, DIM2);

    mulC = m->get_C(1, 2, 2);
    cout << "OCL mul2: ";
    print_mat(mulC, 2, 2);

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
int update_submatrix_offset_test(Muller* m, float* golden)
{
    //timeval t1, t2;
    //double elapsedTime;

    cout << "setting A..." << endl;
    m->set_A(A, DIM1, DIM2);
    //m->set_B(B, DIM2, DIM1);
    cout << "setting B..." << endl;
    m->set_B(A, DIM1, DIM2);
    cout << "setting C..." << endl;
    m->set_C(A, DIM1, DIM2);

    //gettimeofday(&t1, NULL);
    cout << "A set in muller: ";
    m->print_A();
    cout << "B set in muller: ";
    m->print_B();

    m->bound_A(0, 2, 2);
    m->bound_B(1, 2, 2);

    cout << "A bound in muller: ";
    m->print_A();
    cout << "B bound in muller: ";
    m->print_B();

    float* mulC = m->get_C(1, 2, 2);
    cout << "OCL mul1: ";
    print_mat(mulC, 2, 2);

    cout << "A before update: ";
    m->print_A(0, DIM1, DIM2);

    float* D = new float[DIM1*DIM2];
    for (int i = 0; i < DIM1*DIM2; i++)
        D[i] = i;
    m->update_A(D, 0, 2, 2, DIM1, DIM2);

    cout << "A after update: ";
    m->print_A(0, DIM1, DIM2);

    mulC = m->get_C(1, 2, 2);
    cout << "OCL mul2: ";
    print_mat(mulC, 2, 2);


/*
//  Simple Multiply:
    float* mulC = m->get_C(0, DIM1, DIM1);
    cout << "OCL mul1: ";
    print_mat(mulC, DIM1, DIM1);
*/

/*
//  Multiple multiply:
    m->set_A(mulC, DIM1, DIM1);
    m->set_B(A, DIM1, DIM2);
    mulC = m->get_C(0, DIM1, DIM2);
    cout << "OCL mul2: ";
    print_mat(mulC, DIM1, DIM2);
*/

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


void print_mat(float* m, int w, int h)
{
    cout << "{";
    for (int i = 0; i < h; i++)
    {
        cout << "{";
        for (int j = 0; j < w; j++)
        {
            if (j > 0 && j < w)
                cout << ",";
            printf("%1.0f", m[i*w + j]);
        }
        if (i < h-1)
            cout << "},";
        else
            cout << "}";
    }
    cout << "}" << endl;
/*
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
            printf("%4.4f ", m[i*w + j]);
        cout << endl;
    }
    cout << endl;
*/
}
