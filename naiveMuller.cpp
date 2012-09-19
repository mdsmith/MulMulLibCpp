// Author: Martin Smith
// Project: Flexible MULtiple MULtiplicaiton LIBrary in C Plus Plus.
// Created: Early August 2012
// Last Edited: 8/5/2012

#include "muller.h"
#include <iostream>
#include "naiveFunctions.h"
#include "helperFunctions.h"
#include "matrix.h"
using namespace std;


NaiveMuller::NaiveMuller()
{
    //cout << "naiveMuller constructed" << endl;
    A.set_set(false);
    B.set_set(false);
    C.set_set(false);
}

const char* NaiveMuller::get_name()
{
    return "Naive";
}


void NaiveMuller::multiply()
{
    //C.data = naive_matrix_multiply(A.data, B.data, A.h, A.w, B.w);
    C.get_data()->update_data(  naive_matrix_multiply(
                                    A.get_data()->get_unscaled_float(),
                                    B.get_data()->get_unscaled_float(),
                                    A.get_row_offset(),
                                    A.get_col_offset(),
                                    B.get_row_offset(),
                                    B.get_col_offset(),
                                    A.get_bound_rows(),
                                    A.get_data()->get_total_rows(),
                                    A.get_bound_cols(),
                                    A.get_data()->get_total_cols(),
                                    B.get_bound_cols(),
                                    B.get_data()->get_total_cols()
                                    ),
                    0,
                    0,
                    A.get_bound_rows(),
                    B.get_bound_cols(),
                    A.get_data()->get_total_rows(),
                    B.get_data()->get_total_cols()
                    );
}


void NaiveMuller::eval_C(int row_offset, int col_offset, int width, int height)
{
    multiply();
}

float* NaiveMuller::get_C(int row_offset, int col_offset, int width, int height)
{
    if (C.is_set())
        return C.get_data()->get_slice(row_offset, col_offset, width, height);
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
