#include "muller.h"
#include <iostream>
#include "naiveFunctions.h"
using namespace std;


NaiveMuller::NaiveMuller()
{
    //cout << "naiveMuller constructed" << endl;
    A.set = false;
    B.set = false;
    C.set = false;
}


void NaiveMuller::update_A(float* A, int offset, int ah, int ud)
{
    this->A.data = A;
    this->A.offset = offset;
    this->A.h = ah;
    this->A.w = ud;
    this->A.set = true;
}


void NaiveMuller::update_B(float* B, int offset, int ud, int bw)
{
    this->B.data = B;
    this->B.offset = offset;
    this->B.h = ud;
    this->B.w = bw;
    this->B.set = true;
}


void NaiveMuller::multiply()
{
    C.data = naive_matrix_multiply(A.data, B.data, A.h, A.w, B.w);
    C.set = true;
}


float* NaiveMuller::get_C(int offset, int width, int height)
{
    if (C.set)
        return C.data;
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
