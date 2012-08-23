// Author: Martin Smith martin.audacis@gmail.com
// Created: 8/21/2012
// Last Edited On: 8/21/2012

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
                        int offset,
                        int h,
                        int w,
                        int num_rows,
                        int num_cols
                        )
{
    if (this->data != NULL) delete[] this->data;
    //cout << " ...done!" << endl;
    this->data = data;
    this->offset = 0;
    this->h = num_rows;
    this->w = num_cols;
    this->num_rows = num_rows;
    this->num_cols = num_cols;
    scal_thresh = 1e-10;
    scalar = 10;
    set = true;
    update_scalings();
}

void Matrix::bound_data(int offset, int h, int w)
{
    this->offset = offset;
    this->h = h;
    this->w = w;
}

void Matrix::print_mat(int offset, int num_rows, int num_cols)
{
    int row_offset = offset / this->num_cols;
    int col_offset = offset % this->num_rows;
    cout << "h: " << h;
    cout << " w: " << w;
    cout << " offset: " << this->offset;
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

            printf( "%1.0f",
                    data[i*this->num_cols + j] *
                    pow(10.0, scalings[i*this->num_cols + j])
                    );
        }
        if (i < row_offset + num_rows-1)
            cout << "},";
        else
            cout << "}";
    }
    cout << "}" << endl;
}

void Matrix::update_data(   float* data,
                            int offset,
                            int h,
                            int w,
                            int num_rows,
                            int num_cols
                            )
{
    float* start = this->data + offset;
    int sr = offset / this->num_cols;
    int sc = offset % this->num_cols;
    for (int r = sr; r < sr+h; r++)
        for (int c = sc; c < sc+w; c++)
            if (r < this->num_rows && c < this->num_cols)
            {
                start[r*this->num_cols + c] = data[r*num_cols + c];
                scalings[offset + r*this->num_cols + c] = 0;
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

// XXX This is redundant with get_scaled and get_unscaled
// XXX this "offset" "width" "height" system is unintuitive and complex
float* Matrix::get_slice(int offset, int width, int height)
{
    float* tbr = new float[width*height];
    float* start = data + offset;
    for (int r = 0; r < height; r++)
        for (int c = 0; c < width; c++)
            tbr[r*width + c] = start[r*num_cols + c] * pow(10.0,
            (double)(scalings[offset + r*num_cols + c]));
    return tbr;
}
