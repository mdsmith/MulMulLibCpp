#ifndef MULLER_H
#define MULLER_H
#include "matrix.h"

class Muller
{
protected:
    struct Matrix A;
    struct Matrix B;
    struct Matrix C;
public:
    virtual void update_A(float* A, int offset, int ah, int ud) =0;
    virtual void update_B(float* B, int offset, int ud, int bw) =0;
    virtual void multiply() =0;
    virtual float* get_C(int offset, int width, int height) =0;
    virtual void test() =0;
};

class NaiveMuller: public Muller
{
public:
    NaiveMuller();
    void update_A(float* A, int offset, int ah, int ud);
    void update_B(float* B, int offset, int ud, int bw);
    void multiply();
    float* get_C(int offset, int width, int height);
    void test();
    ~NaiveMuller();
};

# endif
