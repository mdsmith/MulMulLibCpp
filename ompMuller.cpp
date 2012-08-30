// Author: Martin Smith
// Project: Flexible MULtiple MULtiplicaiton LIBrary in C Plus Plus.
// Created: Early August 2012
// Last Edited: 8/8/2012

#include "ompMuller.h"
#include <iostream>
#include "naiveFunctions.h"
#include "helperFunctions.h"
using namespace std;


OMPMuller::OMPMuller()
{
    //cout << "naiveMuller constructed" << endl;
    A.set = false;
    B.set = false;
    C.set = false;
}

const char* OMPMuller::get_name()
{
    return "OMP";
}


void OMPMuller::multiply()
{
    // XXX change this to something that can do subsection multiplication
    set_C(omp_matrix_multiply(A.data, B.data, A.h, A.w, B.w), A.h, B.w);
}


float* OMPMuller::get_C(int row_offset, int col_offset, int width, int height)
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


void OMPMuller::test()
{
    cout << "OMPMuller checking in" << endl;
}


OMPMuller::~OMPMuller()
{
    //cout << "naiveMuller destructed" << endl;
}
