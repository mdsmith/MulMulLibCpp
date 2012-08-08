// Author: Martin Smith
// Project: Flexible MULtiple MULtiplicaiton LIBrary in C Plus Plus.
// Created: Early August 2012
// Last Edited: 8/5/2012

#include "muller.h"
#include <iostream>
#include "naiveFunctions.h"
#include "helperFunctions.h"
using namespace std;


NaiveMuller::NaiveMuller()
{
    //cout << "naiveMuller constructed" << endl;
    A.set = false;
    B.set = false;
    C.set = false;
}

const char* NaiveMuller::get_name()
{
    return "Naive";
}


void NaiveMuller::multiply()
{
    //C.data = naive_matrix_multiply(A.data, B.data, A.h, A.w, B.w);
    set_C(naive_matrix_multiply(A.data, B.data, A.h, A.w, B.w), A.h, B.w);
}


float* NaiveMuller::get_C(int offset, int width, int height)
{
    multiply();
    if (C.set)
        return matrix_slice(C, offset, width, height);
    else
    {
        cout << "C is not set!" << endl;
        return NULL;
    }
}


void NaiveMuller::test()
{
    cout << "naiveMuller checking in" << endl;
}


NaiveMuller::~NaiveMuller()
{
    //cout << "naiveMuller destructed" << endl;
}
