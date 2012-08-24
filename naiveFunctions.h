#ifndef NAIVEFUNCTIONS_H
#define NAIVEFUNCTIONS_H
float* naive_matrix_multiply(float A[], float B[], int ah, int ud, int bw);
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
                                );
float* omp_matrix_multiply(float A[], float B[], int ah, int ud, int bw);
float* fma_matrix_multiply(float A[], float B[], int ah, int ud, int bw);
#endif
