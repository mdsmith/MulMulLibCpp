// Author: Martin Smith martin.audacis@gmail.com
// Created: Early August, 2012
// Last Edited: 8/21/12

#ifndef MULLER_H
#define MULLER_H
#include "matrix.h"

class Muller
{
protected:
    Matrix A;
    Matrix B;
    Matrix C;
    //void update_scales(struct Matrix &m, float* data, int* scalings);
    //void update_scale_m(struct Matrix &m);
    void update_matrix(Matrix &m, float* data, int offset, int h, int w, int num_rows, int num_cols);
    void set_matrix(Matrix &m, float* data, int num_rows, int num_cols);
    void bound_matrix(Matrix &m, int offset, int h, int w);
    void print_mat(Matrix m, int offset, int num_rows, int num_cols);
    virtual void multiply() =0;
public:
    virtual const char* get_name();
    virtual void set_A(float* A, int num_rows, int num_cols);
    virtual void set_B(float* B, int num_rows, int num_cols);
    virtual void set_C(float* C, int num_rows, int num_cols);
    virtual void update_A(float* A, int offset, int ah, int ud);
    virtual void update_A(float* A, int offset, int ah, int ud, int num_rows, int num_cols);
    virtual void update_B(float* B, int offset, int ud, int bw);
    virtual void update_B(float* B, int offset, int ud, int bw, int num_rows, int num_cols);
    //void update_scales();
    virtual void bound_A(int offset, int ah, int ud);
    virtual void bound_B(int offset, int ud, int bw);
    void print_A();
    void print_A(int offset, int num_rows, int num_cols);
    void print_B();
    void print_B(int offset, int num_rows, int num_cols);
    void print_C();
    void print_C(int offset, int num_rows, int num_cols);
    virtual float* get_C(int offset, int width, int height) =0;
    virtual void test() =0;
};

class NaiveMuller: public Muller
{
private:
    void multiply();
public:
    NaiveMuller();
    const char* get_name();
    float* get_C(int offset, int width, int height);
    void test();
    ~NaiveMuller();
};

#endif
