// Author: Martin Smith martin.audacis@gmail.com
// Created: 8/5/12
// Last Edited On: 8/20/12

#include "muller.h"
#include <cstddef>
#include <iostream>
#include <stdio.h>
using namespace std;

const char* Muller::get_name()
{
    return "Abstract?!";
}

void Muller::update_A(  float* A,
                        int row_offset,
                        int col_offset,
                        int ah,
                        int ud)
{
    update_matrix(this->A, A, row_offset, col_offset, ah, ud, ah, ud);
}

void Muller::update_A(  double* A,
                        int row_offset,
                        int col_offset,
                        int ah,
                        int ud)
{
    update_matrix(this->A, A, row_offset, col_offset, ah, ud, ah, ud);
}

void Muller::update_A(  float* A,
                        int row_offset,
                        int col_offset,
                        int ah,
                        int ud,
                        int num_rows,
                        int num_cols
                        )
{
    update_matrix(this->A, A, row_offset, col_offset, ah, ud, num_rows, num_cols);
}
void Muller::update_A(  double* A,
                        int row_offset,
                        int col_offset,
                        int ah,
                        int ud,
                        int num_rows,
                        int num_cols
                        )
{
    update_matrix(this->A, A, row_offset, col_offset, ah, ud, num_rows, num_cols);
}


void Muller::set_A(float* A, int num_rows, int num_cols)
{
    set_matrix(this->A, A, num_rows, num_cols);
}
void Muller::set_A(double* A, int num_rows, int num_cols)
{
    set_matrix(this->A, A, num_rows, num_cols);
}


void Muller::bound_A(int row_offset, int col_offset, int ah, int ud)
{
    bound_matrix(this->A, row_offset, col_offset, ah, ud);
}


void Muller::update_B(  float* B,
                        int row_offset,
                        int col_offset,
                        int ud,
                        int bw
                        )
{
    update_matrix(this->B, B, row_offset, col_offset, ud, bw, ud, bw);
}
void Muller::update_B(  double* B,
                        int row_offset,
                        int col_offset,
                        int ud,
                        int bw
                        )
{
    update_matrix(this->B, B, row_offset, col_offset, ud, bw, ud, bw);
}

void Muller::update_B(  float* B,
                        int row_offset,
                        int col_offset,
                        int ud,
                        int bw,
                        int num_rows,
                        int num_cols
                        )
{
    update_matrix(this->B, B, row_offset, col_offset, ud, bw, num_rows, num_cols);
}
void Muller::update_B(  double* B,
                        int row_offset,
                        int col_offset,
                        int ud,
                        int bw,
                        int num_rows,
                        int num_cols
                        )
{
    update_matrix(this->B, B, row_offset, col_offset, ud, bw, num_rows, num_cols);
}


void Muller::set_B(float* B, int num_rows, int num_cols)
{
    set_matrix(this->B, B, num_rows, num_cols);
}
void Muller::set_B(double* B, int num_rows, int num_cols)
{
    set_matrix(this->B, B, num_rows, num_cols);
}


void Muller::bound_B(int row_offset, int col_offset, int ud, int bw)
{
    bound_matrix(this->B, row_offset, col_offset, ud, bw);
}


void Muller::set_C(float* C, int num_rows, int num_cols)
{
    set_matrix(this->C, C, num_rows, num_cols);
}
void Muller::set_C(double* C, int num_rows, int num_cols)
{
    set_matrix(this->C, C, num_rows, num_cols);
}


// Update a subset of a matrix
void Muller::update_matrix( Matrix &m,
                            float* data,
                            int row_offset,
                            int col_offset,
                            int h,
                            int w,
                            int num_rows,
                            int num_cols
                            )
{
    m.update_data(data, row_offset, col_offset, h, w, num_rows, num_cols);
    /*
    float* start = m.data + offset;
    int sr = offset / m.num_cols;
    int sc = offset % m.num_cols;
    for (int r = sr; r < sr+h; r++)
        for (int c = sc; c < sc+w; c++)
            if (r < m.num_rows && c < m.num_cols)
                start[r*m.num_cols + c] = data[r*num_cols + c];
    */
}
void Muller::update_matrix( Matrix &m,
                            double* data,
                            int row_offset,
                            int col_offset,
                            int h,
                            int w,
                            int num_rows,
                            int num_cols
                            )
{
    m.update_data(data, row_offset, col_offset, h, w, num_rows, num_cols);
}


// Set an entirely new matrix
void Muller::set_matrix(Matrix &m, float* data, int num_rows, int num_cols)
{
    m.set_data(data, 0, 0, num_rows, num_cols, num_rows, num_cols);
    //cout << "done setting data in Muller!" << endl;
    /*
    m.data = data;
    m.offset = 0;
    m.h = num_rows;
    m.w = num_cols;
    m.num_rows = num_rows;
    m.num_cols = num_cols;
    m.scal_thresh = 1e-10;
    m.scalar = 10;
    m.set = true;
    */
}
void Muller::set_matrix(Matrix &m, double* data, int num_rows, int num_cols)
{
    m.set_data(data, 0, 0, num_rows, num_cols, num_rows, num_cols);
}


// Set a new block up for multiplication
void Muller::bound_matrix(Matrix &m, int row_offset, int col_offset, int h, int w)
{
    m.bound_data(row_offset, col_offset, h, w);
/*
    m.offset = offset;
    m.h = h;
    m.w = w;
*/
}

void Muller::print_A()
{
    A.print_mat(A.get_row_offset(), A.get_col_offset(), A.get_bound_rows(), A.get_bound_cols());
}

void Muller::print_A(int row_offset, int col_offset, int num_rows, int num_cols)
{
    A.print_mat(row_offset, col_offset, num_rows, num_cols);
}

void Muller::print_B()
{
    B.print_mat(B.get_row_offset(), B.get_col_offset(), B.get_bound_rows(), B.get_bound_cols());
}

void Muller::print_B(int row_offset, int col_offset, int num_rows, int num_cols)
{
    B.print_mat(row_offset, col_offset, num_rows, num_cols);
}

void Muller::print_C()
{
    C.print_mat(C.get_row_offset(), C.get_col_offset(), C.get_bound_rows(), C.get_bound_cols());
}

void Muller::print_C(int row_offset, int col_offset, int num_rows, int num_cols)
{
    C.print_mat(row_offset, col_offset, num_rows, num_cols);
}


// Print a matrix
void Muller::print_mat( Matrix m,
                        int row_offset,
                        int col_offset,
                        int num_rows,
                        int num_cols
                        )
{
    m.print_mat(row_offset, col_offset, num_rows, num_cols);
    /*
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
    */

}

// Made redundant with Matrix moving to a class
// Marked for removal 8/23/12
/*
void Muller::update_scale_m(struct Matrix &m)
{
    if (m.scalings == NULL)
    {
        m.scalings = new int[m.num_rows * m.num_cols];
        for (int i = 0; i < m.num_rows*m.num_cols; i++)
            m.scalings[i] = 0;
    }
    for (int i = 0; i < m.num_rows*m.num_cols; i++)
    {
        while (m.data[i] < m.scal_thresh)
        {
            m.data[i] *= m.scalar;
            m.scalings[i]++;
        }
    }
}
*/

/*
void Muller::update_scales()
{
    if (A.data != NULL)
        update_scale_m(A);
    if (B.data != NULL)
        update_scale_m(B);
    if (C.data != NULL)
        update_scale_m(C);
}
*/

/*
void Muller::update_scales(struct Matrix &m, float* data, int* scalings)
{
    m.data = data;
    m.scalings = scalings;
}
*/
