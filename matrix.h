#ifndef MATRIX_H
#define MATRIX_H
struct Matrix
{
    bool set;
    float* data;
    int w;
    int h;
    int offset;
    int num_rows;
    int num_cols;
};
#endif
