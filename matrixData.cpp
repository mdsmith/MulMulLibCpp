#include "matrixData.h"
#include "helperFunctions.h"
#include <cstring>
#include <math.h>
#include <iostream>
using namespace std;

// XXX So the problem remains: how do you mediate updates to the data so that
// the appropriate buffers are marked as dirty?

MatrixData::MatrixData()
{
    data = NULL;
    scalings = NULL;
    scal_thresh = 1e-10;
    scalar = 10;
    // XXX this should probably be unsigned, in which case I wont have the
    // option of a sentry value
    orig_id = -1;

}

MatrixData::MatrixData(float scal_thresh, float scalar)
{
    this->scal_thresh = scal_thresh;
    this->scalar = scalar;
    data = NULL;
    scalings = NULL;

    // XXX this should probably be unsigned, in which case I wont have the
    // option of a sentry value
    orig_id = -1;
}

MatrixData::~MatrixData()
{
    delete[] data;
    delete[] scalings;
}


/*
void MatrixData::check_all()
{

}


void MatrixData::check_float()
{

}


void MatrixData::check_double()
{

}


void MatrixData::check_padded()
{

}
*/


void MatrixData::check_scalings()
{
    if (scalings_clean)
    {
        return;
    }
    else if (data == NULL)
    {
        return;
    }
    else
    {
        if (scalings == NULL)
        {
            scalings = new int[num_rows * num_cols];
            for (int i = 0; i < num_rows*num_cols; i++)
                scalings[i] = 0;
        }
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
    }
}


float* MatrixData::get_unscaled_float()
{
    float* tbr = new float[num_rows*num_cols];
    for (int i = 0; i < num_rows*num_cols; i++)
    {
        tbr[i] = data[i] * pow(10, scalings[i]);
    }
    return tbr;
}


double* MatrixData::get_unscaled_double()
{
    double* tbr = new double[num_rows*num_cols];
    for (int i = 0; i < num_rows*num_cols; i++)
    {
        tbr[i] = (double)data[i] * pow(10.0, (double)(scalings[i]));
    }
    return tbr;
}


void MatrixData::set_data(  float* data,
                            int num_rows,
                            int num_cols)
{
    orig_id = (long) data;
    //if (this->data != NULL) delete[] this->data;
    //if (this->scalings != NULL) delete[] this->scalings;
    scalings_clean = false;
    this->data = data;
    this->num_rows = num_rows;
    this->num_cols = num_cols;
    check_scalings();

}


void MatrixData::set_data(  double* data,
                            int num_rows,
                            int num_cols)
{
    orig_id = (long) data;
    //if (this->data != NULL) delete[] this->data;
    //if (this->scalings != NULL) delete[] this->scalings;
    scalings_clean = false;
    //cout << " ...done!" << endl;
    // XXX this needs to change to something that casts the data...
    // XXX and does the scaling beforehand...
    float* temp = new float[num_rows*num_cols];
    for (int i = 0; i < num_rows*num_cols; i++)
        temp[i] = (float)data[i];
    this->data = temp;
    this->num_rows = num_rows;
    this->num_cols = num_cols;
    check_scalings();

}


void MatrixData::print_mat()
{
    print_mat(0, 0, num_rows, num_cols);
}

void MatrixData::print_mat(int row_offset, int col_offset, int num_rows, int num_cols)
{
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


void MatrixData::update_data(   float* new_data,    // new data to be subbed
                                                    // into data
                                int sub_row_offset, // offset in data of
                                                    // substitution
                                int sub_col_offset, // offset in data of
                                                    // substitution
                                int sub_h,          // size of substitution
                                int sub_w,          // size of substitution
                                int newd_num_rows,  // size of new_data
                                int newd_num_cols   // size of new_data
                                )
{
    for (int r = sub_row_offset; r < sub_row_offset + sub_h; r++)
        for (int c = sub_col_offset; c < sub_col_offset + sub_w; c++)
            if (r < num_rows && c < num_cols)
            {
                data[r * num_cols + c] = new_data[r*newd_num_cols + c];
                scalings[r * num_cols + c] = 0;
            }
    check_scalings();
}

void MatrixData::update_data(   double* new_data,    // new data to be subbed
                                                    // into data
                                int sub_row_offset, // offset in data of
                                                    // substitution
                                int sub_col_offset, // offset in data of
                                                    // substitution
                                int sub_h,          // size of substitution
                                int sub_w,          // size of substitution
                                int newd_num_rows,  // size of new_data
                                int newd_num_cols   // size of new_data
                                )
{
    for (int r = sub_row_offset; r < sub_row_offset + sub_h; r++)
        for (int c = sub_col_offset; c < sub_col_offset + sub_w; c++)
            if (r < num_rows && c < num_cols)
            {
                // XXX scaling should be performed before the cast
                data[r * num_cols + c] = (float)(new_data[  r
                                                            * newd_num_cols
                                                            + c]);
                scalings[r * num_cols + c] = 0;
            }
    check_scalings();
}


// Expensive, memory creating and copying
float* MatrixData::get_slice(int row_offset, int col_offset, int width, int height)
{
    float* tbr = new float[width*height];
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


double* MatrixData::get_slice_double(int row_offset, int col_offset, int width, int height)
{
    double* tbr = new double[width*height];
    for (int r = row_offset; r < row_offset + height; r++)
    {
        for (int c = col_offset; c < col_offset + width; c++)
        {
            if (r < num_rows && c < num_cols)
            {
                // XXX HERE scaling messing with numbers here?
                tbr[r*width + c] =  (double)data[r*num_cols + c]
                                    * pow(  10.0,
                                            (double)(scalings[  r*num_cols
                                                                + c])
                                            );
            }
        }
    }
    return tbr;

}


void MatrixData::pad_to(int interval)
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
        // delete[] data;
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
        //delete[] scalings;
        scalings = newScalings;
    }
}



