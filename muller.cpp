// Author: Martin Smith martin.audacis@gmail.com
// Created: 8/5/12
// Last Edited On: 8/5/12

#include "muller.h"
#include <cstddef>
#include <iostream>
using namespace std;

void Muller::update_A(float* A, int offset, int ah, int ud)
{
    update_matrix(this->A, A, offset, ah, ud);
}


void Muller::set_A(float* A, int num_rows, int num_cols)
{
    set_matrix(this->A, A, num_rows, num_cols);
}


void Muller::bound_A(int offset, int ah, int ud)
{
    bound_matrix(this->A, offset, ah, ud);
}


void Muller::update_B(float* B, int offset, int ud, int bw)
{
    update_matrix(this->B, B, offset, ud, bw);
}


void Muller::set_B(float* B, int num_rows, int num_cols)
{
    set_matrix(this->B, B, num_rows, num_cols);
}


void Muller::bound_B(int offset, int ud, int bw)
{
    bound_matrix(this->B, offset, ud, bw);
}


void Muller::set_C(float* C, int num_rows, int num_cols)
{
    set_matrix(this->C, C, num_rows, num_cols);
}


// Update a subset of a matrix
void Muller::update_matrix(struct Matrix &m, float* data, int offset, int h, int w)
{
    float* start = m.data + offset;
    int sr = offset / m.num_cols;
    int sc = offset % m.num_cols;
    for (int r = sr; r < sr+h; r++)
        for (int c = sc; c < sc+w; c++)
            if (r < m.num_rows && c < m.num_cols)
                start[r*m.num_cols + c] = data[r*w + h];
}


// Set an entirely new matrix
void Muller::set_matrix(struct Matrix &m, float* data, int num_rows, int num_cols)
{
    // XXX old data here could be a memory leak
    m.data = data;
    m.offset = 0;
    m.h = num_rows;
    m.w = num_cols;
    m.num_rows = num_rows;
    m.num_cols = num_cols;
    m.set = true;
}


// Set a new block up for multiplication
void Muller::bound_matrix(struct Matrix &m, int offset, int h, int w)
{
    m.offset = offset;
    m.h = h;
    m.w = w;
}
