// Author: Martin Smith martin.audacis@gmail.com
// Created: 8/5/12
// Last Edited On: 8/5/12

#include "muller.h"
#include <cstddef>
#include <iostream>
using namespace std;

const char* Muller::get_name()
{
    return "Abstract?!";
}

void Muller::update_A(float* A, int offset, int ah, int ud)
{
    update_matrix(this->A, A, offset, ah, ud, ah, ud);
}

void Muller::update_A(  float* A,
                        int offset,
                        int ah,
                        int ud,
                        int num_rows,
                        int num_cols
                        )
{
    update_matrix(this->A, A, offset, ah, ud, num_rows, num_cols);
}


void Muller::set_A(float* A, int num_rows, int num_cols)
{
    set_matrix(this->A, A, num_rows, num_cols);
}


void Muller::bound_A(int offset, int ah, int ud)
{
    bound_matrix(this->A, offset, ah, ud);
}


void Muller::update_B(  float* B,
                        int offset,
                        int ud,
                        int bw
                        )
{
    update_matrix(this->B, B, offset, ud, bw, ud, bw);
}

void Muller::update_B(  float* B,
                        int offset,
                        int ud,
                        int bw,
                        int num_rows,
                        int num_cols
                        )
{
    update_matrix(this->B, B, offset, ud, bw, num_rows, num_cols);
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
void Muller::update_matrix( struct Matrix &m,
                            float* data,
                            int offset,
                            int h,
                            int w,
                            int num_rows,
                            int num_cols
                            )
{
    float* start = m.data + offset;
    int sr = offset / m.num_cols;
    int sc = offset % m.num_cols;
    for (int r = sr; r < sr+h; r++)
        for (int c = sc; c < sc+w; c++)
            if (r < m.num_rows && c < m.num_cols)
                start[r*m.num_cols + c] = data[r*num_cols + c];
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

void Muller::print_A()
{
    print_mat(A, A.offset, A.h, A.w);
}

void Muller::print_A(int offset, int num_rows, int num_cols)
{
    print_mat(A, offset, num_rows, num_cols);
}

void Muller::print_B()
{
    print_mat(B, B.offset, B.h, B.w);
}

void Muller::print_B(int offset, int num_rows, int num_cols)
{
    print_mat(B, offset, num_rows, num_cols);
}

void Muller::print_C()
{
    print_mat(C, C.offset, C.h, C.w);
}

void Muller::print_C(int offset, int num_rows, int num_cols)
{
    print_mat(C, offset, num_rows, num_cols);
}

// Print a matrix
void Muller::print_mat( struct Matrix m,
                        int offset,
                        int num_rows,
                        int num_cols
                        )
{
    cout << "m.h: " << m.h;
    cout << " m.w: " << m.w;
    cout << " m.offset: " << m.offset;
    cout << " m.data: " << endl;
    int row_offset = offset / num_cols;
    int col_offset = offset % num_rows;
    cout << "{";
    for (int i = row_offset; i < row_offset + num_rows; i++)
    {
        cout << "{";
        for (int j = col_offset; j < col_offset + num_cols; j++)
        {
            if (j > col_offset && j < col_offset + num_cols)
                cout << ",";
            printf("%1.0f", m.data[i*m.num_cols + j]);
        }
        if (i < row_offset + num_rows-1)
            cout << "},";
        else
            cout << "}";
    }
    cout << "}" << endl;

}


