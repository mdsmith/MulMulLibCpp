// Author: Martin Smith martin.audacis@gmail.com
// Created: Early August 2012
// Last Edited: 8/23/12

//#define WOLFRAM

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
#include "helperFunctions.h"
#include "muller.h"
#include <cstdlib>
#include <sys/time.h>
using namespace std;

// A will here be the nodes

#define ANR 10 // 5 sites * 2 nodes
#define ANC 3 // 5 sites * 2 nodes
#define ARO 5 // 5 sites * 3 codons * 1 node = 1 full node skipped (we're
#define ACO 0 // 5 sites * 3 codons * 1 node = 1 full node skipped (we're
#define AH 5 // 5 Sites * 1 node = 1 full node
#define UD 3 // 3 codons, AKA ANC and BNR
// B will here be the models
#define BNR 3 // 3 codons
#define BNC 6 // 3 codons * 2 nodes
#define BRO 0 // 3 codons * 3 codons * 1 node = 1 full node skipped
#define BCO 3 // 3 codons * 3 codons * 1 node = 1 full node skipped
#define BW 3 // 3 codons * 1 nodes
float* A;
float* B;

int update_submatrix_offset_test(Muller* m, float* golden);
//void print_float_mat(float* m, int offset, int h, int w, int nr, int nc);
int simulate_MLE(Muller* m, float* golden);

int main()
{
    A = new float[ANR*ANC];
    B = new float[BNR*BNC];
    int allpasscode = 0;

    srand(time(NULL));

    for (int i = 0; i < ANR*ANC; i++)
    {
        A[i] = ((rand()%1000)+1)/1000.0;
    }
    for (int i = 0; i < BNR*BNC; i++)
    {
        B[i] = ((rand()%1000)+1)/1000.0;
    }
    /*
    cout << "A at the start: " << endl;
    print_float_mat(A, 0, 0, ANR, ANC, ANR, ANC);
    cout << "A bound: " << endl;
    print_float_mat(A, ARO, ACO, AH, UD, ANR, ANC);
    cout << "B at the start: " << endl;
    print_float_mat(B, 0, 0, BNR, BNC, BNR, BNC);
    cout << "B bound: " << endl;
    print_float_mat(B, BRO, BCO, UD, BW, BNR, BNC);
    */

    timeval t1, t2;
    double elapsedTime;
    gettimeofday(&t1, NULL);

    float* goldenC = naive_matrix_multiply( A,
                                            B,
                                            ARO,
                                            ACO,
                                            BRO,
                                            BCO,
                                            AH,
                                            ANR,
                                            UD,
                                            UD,
                                            BW, //61,
                                            BNC
                                            );
    cout << "Golden C: " << endl;
    print_float_mat(goldenC, 0, 0, AH, BW, AH, BW);
/*
    // multiple multiply!
    goldenC = naive_matrix_multiply(goldenC,A, DIM1, DIM1, DIM2);
    print_mat(goldenC, DIM1, DIM2);
*/

    gettimeofday(&t2, NULL);

    elapsedTime = (t2.tv_sec -t1.tv_sec) * 1000.0;
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;

    cout << "Golden (Naive): " << elapsedTime << " ms.\n";

    gettimeofday(&t1, NULL);

    int trials = 1;
    for (int i = 0; i < trials; i++)
    {

#ifdef OCL
    GPUMuller gm = GPUMuller();
    //allpasscode |= update_submatrix_offset_test(&gm, goldenC);
    allpasscode |= simulate_MLE(&gm, goldenC);
/*
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
*/
#endif

    }
    gettimeofday(&t2, NULL);

    elapsedTime = (t2.tv_sec -t1.tv_sec) * 1000.0;
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;

    cout << "per trial (mean): " << elapsedTime/trials << " ms.\n";

    return allpasscode;
}


int simulate_MLE(Muller* m, float* golden)
{
    //cout << "A pre: " << endl;
    //print_float_mat(A, 0, 0, ANR, ANC, ANR, ANC);
    m->set_A(A, ANR, ANC);
    cout << "A set: " << endl;
    m->print_A(0, 0, ANR, ANC);
    m->set_B(B, BNR, BNC);
    cout << "B set: " << endl;
    m->print_B(0, 0, BNR, BNC);
    m->bound_A(ARO, ACO, AH, UD);
    cout << "A bound: " << endl;
    m->print_A(ARO, ACO, AH, UD);
    m->bound_B(BRO, BCO, UD, BW);
    cout << "B bound: " << endl;
    m->print_B(BRO, BCO, UD, BW);
    m->set_overwrite(true);
    float* mulC = m->get_C(0, 0, AH, BW);
    cout << "C results: " << endl;
    print_float_mat(mulC, 0, 0, AH, BW, AH, BW);
    m->print_C(0, 0, AH, BW);

    bool passed = true;
    for (int i=0; i < AH*BW; i++)
    {
        //cout << golden[i] << " vs " << mulC[i] << endl;
        if (golden[i] != mulC[i])
            passed = false;
    }
    if (passed)
    {
        cout << "Test PASSED!" << endl;
        return 0;
    }
    else
    {
        cout << "Test FAILED!" << endl;
        return 1;
    }
}


/*
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


//  Simple Multiply:
    //float* mulC = m->get_C(0, DIM1, DIM1);
    //cout << "OCL mul1: ";
    //print_mat(mulC, DIM1, DIM1);

//  Multiple multiply:
    //m->set_A(mulC, DIM1, DIM1);
    //m->set_B(A, DIM1, DIM2);
    //mulC = m->get_C(0, DIM1, DIM2);
    //cout << "OCL mul2: ";
    //print_mat(mulC, DIM1, DIM2);

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


void print_mat(float* m, int offset, int h, int w, int nr, int nc)
{
    cout << "{";
    for (int i = 0; i < h; i++)
    {
        cout << "{";
        for (int j = 0; j < w; j++)
        {
            if (j > 0 && j < w)
                cout << ",";
            printf("%4.4f", m[offset + i*nc + j]);
        }
        if (i < h-1)
#if defined WOLFRAM
            cout << "},";
#else
            cout << "}," << endl;
#endif
        else
            cout << "}";
    }
    cout << "}" << endl;
}
*/
