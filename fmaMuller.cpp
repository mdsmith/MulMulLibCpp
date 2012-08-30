// Author: Martin Smith
// Project: Flexible MULtiple MULtiplicaiton LIBrary in C Plus Plus.
// Created: Early August 2012
// Last Edited: 8/8/2012

#include "fmaMuller.h"
#include <iostream>
#include "naiveFunctions.h"
#include "helperFunctions.h"
using namespace std;


FMAMuller::FMAMuller()
{
    //cout << "naiveMuller constructed" << endl;
    A.set = false;
    B.set = false;
    C.set = false;
}

const char* FMAMuller::get_name()
{
    return "FMA";
}


void FMAMuller::multiply()
{
    // XXX change this to something that can do subsection multiplication
    set_C(fma_matrix_multiply(A.data, B.data, A.h, A.w, B.w), A.h, B.w);
}


float* FMAMuller::get_C(int row_offset, int col_offset, int width, int height)
{
    multiply();
    if (C.set)
        return matrix_slice(C, row_offset, col_offset, width, height);
    else
    {
        cout << "C is not set!" << endl;
        return NULL;
    }
}


void FMAMuller::test()
{
    cout << "FMAMuller checking in" << endl;
}


FMAMuller::~FMAMuller()
{
    //cout << "naiveMuller destructed" << endl;
}
