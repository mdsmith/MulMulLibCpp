// Author: Martin Smith martin.audacis@gmail.com
// Created: Early August, 2012
// Last Edited: 8/5/12

#ifndef MULLER_H
#define MULLER_H
#include "matrix.h"

class Muller
{
protected:
    struct Matrix A;
    struct Matrix B;
    struct Matrix C;
    void update_matrix(struct Matrix &m, float* data, int offset, int h, int w);
    void set_matrix(struct Matrix &m, float* data, int num_rows, int num_cols);
    void bound_matrix(struct Matrix &m, int offset, int h, int w);
    virtual void multiply() =0;
public:
    void update_A(float* A, int offset, int ah, int ud);
    void set_A(float* A, int num_rows, int num_cols);
    void bound_A(int offset, int ah, int ud);
    void update_B(float* B, int offset, int ud, int bw);
    void set_B(float* B, int num_rows, int num_cols);
    void bound_B(int offset, int ud, int bw);
    void set_C(float* C, int num_rows, int num_cols);
    virtual float* get_C(int offset, int width, int height) =0;
    virtual void test() =0;
};

class NaiveMuller: public Muller
{
private:
    void multiply();
public:
    NaiveMuller();
    float* get_C(int offset, int width, int height);
    void test();
    ~NaiveMuller();
};

class GPUMuller: public Muller
{
private:
    void multiply();
    void setup_context();
    void update_buffers();
public:
    GPUMuller();
    float* get_C(int offset, int width, int height);
    void test();
    ~GPUMuller();
};

# endif
