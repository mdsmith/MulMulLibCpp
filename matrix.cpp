// Author: Martin Smith martin.audacis@gmail.com
// Created: 8/21/2012
// Last Edited On: 8/21/2012

//#define WOLFRAM

#include "matrix.h"
#include "helperFunctions.h"
#include <math.h>
#include <stdio.h>
#include <iostream>
using namespace std;

Matrix::Matrix()
{
    set = false;
    data = NULL;
    scalings = NULL;
}


// XXX this is currently unstable, as multiple matrices can house pointers to
// the same memory. Freeing it twice is undefined, I'm not sure how to
// prevent it. Letting the memory fall off the end of the program isn't good
// either...
Matrix::~Matrix()
{
/*
    cout << "destructing...";
    if (data != NULL)
    {
        delete[] data;
        data = NULL;
    }
    if (scalings != NULL)
    {
        delete[] scalings;
        scalings = NULL;
    }
    cout << "...complete" << endl;
*/
}


void Matrix::update_scalings()
{
    //cout << "updating scalings..." << endl;
    if (data == NULL)
        return;
    if (scalings == NULL)
    {
        scalings = new int[num_rows * num_cols];
        for (int i = 0; i < num_rows*num_cols; i++)
            scalings[i] = 0;
    }
    //cout << "computing exponents...";
    for (int i = 0; i < num_rows*num_cols; i++)
    {
        float value = data[i];
        while (value < scal_thresh && value > 0)
        {
            //cout << "value: " << value << endl;
            value *= scalar;
            scalings[i]++;
        }
        while (value > -1*scal_thresh && value < 0)
        {
            //cout << "value: " << value << endl;
            value *= scalar;
            scalings[i]++;
        }
        while (value > 1/scal_thresh)
        {
            //cout << "value: " << value << endl;
            value /= scalar;
            scalings[i]--;
        }
        while (value < -1/scal_thresh)
        {
            //cout << "value: " << value << endl;
            value /= scalar;
            scalings[i]--;
        }
        data[i] = value;
    }
    //cout << " ...done!" << endl;
}


float* Matrix::get_unscaled()
{
    float* tbr = new float[num_rows*num_cols];
    for (int i = 0; i < num_rows*num_cols; i++)
    {
        tbr[i] = data[i] * pow(10, scalings[i]);
    }
    return tbr;
}


double* Matrix::get_unscaled_double()
{
    double* tbr = new double[num_rows*num_cols];
    for (int i = 0; i < num_rows*num_cols; i++)
    {
        tbr[i] = (double)data[i] * pow(10.0, (double)(scalings[i]));
    }
    return tbr;
}

void Matrix::set_data(  float* data,
                        int row_offset,
                        int col_offset,
                        int h,
                        int w,
                        int num_rows,
                        int num_cols
                        )
{
    if (this->data != NULL) delete[] this->data;
    if (this->scalings != NULL) delete[] this->scalings;
    //cout << " ...done!" << endl;
    this->data = data;
    this->row_offset = row_offset;
    this->col_offset = col_offset;
    this->h = num_rows;
    this->w = num_cols;
    this->num_rows = num_rows;
    this->num_cols = num_cols;
    scal_thresh = 1e-10;
    scalar = 10;
    set = true;
    update_scalings();
}
void Matrix::set_data(  double* data,
                        int row_offset,
                        int col_offset,
                        int h,
                        int w,
                        int num_rows,
                        int num_cols
                        )
{
    if (this->data != NULL) delete[] this->data;
    if (this->scalings != NULL) delete[] this->scalings;
    //cout << " ...done!" << endl;
    // XXX this needs to change to something that casts the data...
    // XXX and does the scaling beforehand...
    float* temp = new float[num_rows*num_cols];
    for (int i = 0; i < num_rows*num_cols; i++)
        temp[i] = (float)data[i];
    this->data = temp;
    this->row_offset = row_offset;
    this->col_offset = col_offset;
    this->h = num_rows;
    this->w = num_cols;
    this->num_rows = num_rows;
    this->num_cols = num_cols;
    scal_thresh = 1e-10;
    scalar = 10;
    set = true;
    update_scalings();
}

void Matrix::bound_data(int row_offset, int col_offset, int h, int w)
{
    this->row_offset = row_offset;
    this->col_offset = col_offset;
    this->h = h;
    this->w = w;
}

void Matrix::print_mat(int row_offset, int col_offset, int num_rows, int num_cols)
{
    cout << "h: " << h;
    cout << " w: " << w;
    cout << " row_offset: " << row_offset;
    cout << " col_offset: " << col_offset;
    cout << " num_rows: " << num_rows;
    cout << " num_cols: " << num_cols;
    cout << " data: " << endl;
    cout << "{";
    for (int i = row_offset; i < row_offset + num_rows; i++)
    {
        cout << "{";
        for (int j = col_offset; j < col_offset + num_cols; j++)
        {
            if (j > col_offset && j < col_offset + num_cols)
                cout << ",";

            printf( "%4.4f",
                    data[i*this->num_cols + j] *
                    pow(10.0, scalings[i*this->num_cols + j])
                    );
        }
        if (i < row_offset + num_rows-1)
        #if defined WOLFRAM
            cout << "},";
        #else
            cout << "}," << endl;
        #endif
        else
            cout << "}";
    }
    cout << "}" << endl;
}

void Matrix::print_total()
{
    print_mat(0, 0, num_rows, num_cols);
}

void Matrix::print_bound()
{
    print_mat(row_offset, col_offset, h, w);
}

// replace the portion of memory specified by the row and column offsets with
// data from float* data, where the size of the substitution is given by h
// and w and the size of data is given by num_rows and num_cols
void Matrix::update_data(   float* new_data,
                            int row_offset,
                            int col_offset,
                            int sub_h,
                            int sub_w,
                            int newd_num_rows,
                            int newd_num_cols
                            )
{
    for (int r = row_offset; r < sub_h; r++)
        for (int c = col_offset; c < sub_w; c++)
            if (r < num_rows && c < num_cols)
            {
                data[r * num_cols + c] = new_data[r*newd_num_cols + c];
                scalings[r * num_cols + c] = 0;
            }
    update_scalings();
}

void Matrix::update_data(   double* new_data,
                            int row_offset,
                            int col_offset,
                            int sub_h,
                            int sub_w,
                            int newd_num_rows,
                            int newd_num_cols
                            )
{
    for (int r = row_offset; r < sub_h; r++)
        for (int c = col_offset; c < sub_w; c++)
            if (r < num_rows && c < num_cols)
            {
                // XXX scalings should be updated before the cast...
                data[r * num_cols + c] = (float)(new_data[r*newd_num_cols +
                c]);
                scalings[r * num_cols + c] = 0;
            }
    update_scalings();
}

void Matrix::pad_to(int interval)
{
    int old_num_rows = num_rows;
    int old_num_cols = num_cols;
    if (data != NULL)
    {
        // create new num_rows and num_cols
        num_rows = round_up(num_rows, interval);
        num_cols = round_up(num_cols, interval);
        // create new and fill
        float* newData = new float[num_rows * num_cols];
        for (int r = 0; r < num_rows; r++)
        {
            for (int c = 0; c < num_cols; c++)
            {
                if (r < old_num_rows && c < old_num_cols)
                    newData[r * num_cols + c] = data[r * old_num_cols + c];
                else
                    newData[r * num_cols + c] = 0.0f;
            }
        }
        /*
        for (int i = 0; i < old_num_rows; i++)
        {
            memcpy( newData + i * num_cols,
                    data + i * old_num_cols,
                    old_num_cols);
        }
        */
        delete[] data;
        data = newData;
    }
    if (scalings != NULL)
    {
        // create new and fill
        int* newScalings = new int[num_rows * num_cols];
        for (int r = 0; r < num_rows; r++)
        {
            for (int c = 0; c < num_cols; c++)
            {
                if (r < old_num_rows && c < old_num_cols)
                    newScalings[r * num_cols + c] = scalings[r * old_num_cols + c];
                else
                    newScalings[r * num_cols + c] = 0;
            }
        }
        /*
        for (int i = 0; i < old_num_rows; i++)
        {
            memcpy( newScalings + i * num_cols,
                    scalings + i * old_num_cols,
                    old_num_cols);
        }
        */
        delete[] scalings;
        scalings = newScalings;
    }
}

float* Matrix::get_slice(int row_offset, int col_offset, int width, int height)
{
    float* tbr = new float[width*height];
/*
    float* start = data + offset;
    for (int r = 0; r < height; r++)
        for (int c = 0; c < width; c++)
            tbr[r*width + c] = start[r*num_cols + c] * pow(10.0,
            (double)(scalings[offset + r*num_cols + c]));
*/
    for (int r = row_offset; r < row_offset + height; r++)
        for (int c = col_offset; c < col_offset + width; c++)
            if (r < num_rows && c < num_cols)
                tbr[r*width + c] =  data[r*num_cols + c]
                                    * pow(  10.0,
                                            (double)(scalings[  r*num_cols
                                                                + c])
                                            );
    return tbr;
}

double* Matrix::get_slice_double(int row_offset, int col_offset, int width, int height)
{
    double* tbr = new double[width*height];
/*
    float* start = data + offset;
    for (int r = 0; r < height; r++)
        for (int c = 0; c < width; c++)
            tbr[r*width + c] = start[r*num_cols + c] * pow(10.0,
            (double)(scalings[offset + r*num_cols + c]));
*/
    cout << "in slice" << endl;
    for (int r = row_offset; r < row_offset + height; r++)
    {
        for (int c = col_offset; c < col_offset + width; c++)
        {
            if (r < num_rows && c < num_cols)
            {
                cout << (double)data[r*num_cols + c] << " ";
                // XXX HERE scaling messing with numbers here?
                tbr[r*width + c] =  (double)data[r*num_cols + c]
                                    * pow(  10.0,
                                            (double)(scalings[  r*num_cols
                                                                + c])
                                            );
            }
        }
        cout << endl;
    }
    cout << "end slice" << endl;
    return tbr;
}
