#include "naiveFunctions.h"
#include <iostream>
using namespace std;

float* naive_matrix_multiply(   float A[],
                                float B[],
                                int A_offset,
                                int B_offset,
                                int ah,
                                int a_num_rows,
                                int ud,
                                int ud_total,
                                int bw,
                                int b_num_cols
                                )
{
    float *C = new float[ah * bw];
    for (int i = 0; i < ah; i++)
    {
        for (int j = 0; j < bw; j++)
        {
            float sum = 0.0f;
            for (int l = 0; l < ud; l++)
            {
                sum += A[A_offset + i*ud_total + l]
                     * B[B_offset + l*b_num_cols + j];
            }
            C[i*bw+j] = sum;
        }
    }
    return C;
}

float* naive_matrix_multiply(float A[], float B[], int ah, int ud, int bw)
{
    float *C = new float[ah * bw];
    for (int i = 0; i < ah; i++)
    {
        for (int j = 0; j < bw; j++)
        {
            float sum = 0.0f;
            for (int l = 0; l < ud; l++)
            {
                sum += A[i*ud + l] * B[l*bw + j];
            }
            C[i*bw+j] = sum;
        }
    }
    return C;
}

float* omp_matrix_multiply(float A[], float B[], int ah, int ud, int bw)
{
    float *C = new float[ah * bw];
    float sum = 0.0f;
    #pragma omp parallel for private(sum)
    for (int i = 0; i < ah; i++)
    {
        for (int j = 0; j < bw; j++)
        {
            sum = 0.0f;
            for (int l = 0; l < ud; l++)
            {
                sum += A[i*ud + l] * B[l*bw + j];
            }
            C[i*bw+j] = sum;
        }
    }
    return C;
}

float* fma_matrix_multiply(float A[], float B[], int ah, int ud, int bw)
{
    float *C = new float[ah * bw];
    float sum = 0.0f;
    #pragma omp parallel for private(sum)
    for (int i = 0; i < ah; i++)
    {
        for (int j = 0; j < bw; j++)
        {
            sum = 0.0f;
            for (int l = 0; l < ud; l++)
            {
                sum += A[i*ud + l] * B[l*bw + j];
            }
            C[i*bw+j] = sum;
        }
    }
    return C;
}
