#define BLOCK_SIZE 16
#define SCALAR 10
#define SCAL_THRESH 1e-10
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

__kernel void matMul(
                __global float* A,
                __global int* Ascalings,
                __global float* B,
                __global int* Bscalings,
                __global float* C,
                __global int* Cscalings,
                int a_row_len,
                int a_round_row_len,
                int c_round_row_len,
                int row_bound,  // max real length of a row in C
                int col_bound,  // max real length of a column in C
                int a_row_offset,
                int a_col_offset,
                int b_row_offset,
                int b_col_offset,
                int c_row_offset,
                int c_col_offset,
                bool overwrite)
{
    int a_offset = a_row_offset * a_round_row_len + a_col_offset;
    int b_offset = b_row_offset * c_round_row_len + b_col_offset;
    int c_offset = c_row_offset * c_round_row_len + c_col_offset;

    int wA = a_round_row_len;
    int wB = c_round_row_len;

    // Block index
    int bx = get_group_id(0);
    int by = get_group_id(1);

    // Thread index
    int tx = get_local_id(0);   // tx == col #
    int ty = get_local_id(1);   // ty == row #

    int gx = get_global_id(0);
    int gy = get_global_id(1);
    //C[(get_global_size(0) * get_global_size(1)) - (wB*gy + gx)] = wB*gy +
    //gx; // XXX for testing purposes only

    bool in_frame = false;
    if (gx < row_bound && gy < col_bound)
        in_frame = true;

    // Index of the first sub-matrix of A processed
    // by the block
    int aBegin = wA * BLOCK_SIZE * by;

    // Index of the last sub-matrix of A processed
    // by the block
    int aEnd   = aBegin + wA - 1;

    // Step size used to iterate through the
    // sub-matrices of A
    int aStep  = BLOCK_SIZE;

    // Index of the first sub-matrix of B processed
    // by the block
    int bBegin = BLOCK_SIZE * bx;

    // Step size used to iterate through the
    // sub-matrices of B
    int bStep  = BLOCK_SIZE * wB;

    float Csub_sig = 0.0f;
    int Csub_exp = 0;

    // Loop over all the sub-matrices of A and B
    // required to compute the block sub-matrix
    for (int a = aBegin, b = bBegin;
             a <= aEnd;
             a += aStep, b += bStep)
    {

        // Declaration of the local memory array As
        // used to store the sub-matrix of A
        __local float As[BLOCK_SIZE][BLOCK_SIZE];
        __local int Ascal_cache[BLOCK_SIZE][BLOCK_SIZE];

        // Declaration of the local memory array Bs
        // used to store the sub-matrix of B
        __local float Bs[BLOCK_SIZE][BLOCK_SIZE];
        __local int Bscal_cache[BLOCK_SIZE][BLOCK_SIZE];

        // Load the matrices from global memory
        // to local memory; each thread loads
        // one element of each matrix
        int A_index = a + wA * ty + tx;

        if ((A_index / wA) < col_bound && (A_index % wA) < a_row_len)
        {
            As[ty][tx] = A[a_offset + A_index];
            Ascal_cache[ty][tx] = Ascalings[a_offset + A_index];
        }
        else
        {
            As[ty][tx] = 0.0f;
            Ascal_cache[ty][tx] = 0;
        }

        int B_index = b + wB * ty + tx;
        if ((B_index / wB) < a_row_len && (B_index % wB) < row_bound)
        {
            Bs[ty][tx] = B[b_offset + B_index];
            Bscal_cache[ty][tx] = Bscalings[b_offset + B_index];
        }
        else
        {
            Bs[ty][tx] = 0.0f;
            Bscal_cache[ty][tx] = 0;
        }

        // Synchronize to make sure the matrices
        // are loaded
        barrier(CLK_LOCAL_MEM_FENCE);

        // Multiply the two matrices together;
        // each thread computes one element
        // of the block sub-matrix
        for (int k = 0; k < BLOCK_SIZE; ++k)
        {
            float a_sig = As[ty][k];
            int a_exp = Ascal_cache[ty][k];
            float b_sig = Bs[k][tx];
            int b_exp = Bscal_cache[k][tx];
            float c_sig = a_sig * b_sig;
            int c_exp = a_exp + b_exp;
            int new_exp = MAX(c_exp, Csub_exp);
            Csub_sig    = Csub_sig * pow( 10.0f, (float)new_exp-(float)Csub_exp)
                        + c_sig * pow(10.0f, (float)new_exp - (float)c_exp);
            Csub_exp = new_exp;

            //Csub += As[ty][k] * Bs[k][tx];
        }

        // Synchronize to make sure that the preceding
        // computation is done before loading two new
        // sub-matrices of A and B in the next iteration
        barrier(CLK_LOCAL_MEM_FENCE);

    }
    while (Csub_sig < SCAL_THRESH && Csub_sig > 0)
    {
        Csub_sig *= SCALAR;
        Csub_exp += 1;
    }
    while (Csub_sig > -1*SCAL_THRESH && Csub_sig < 0)
    {
        Csub_sig *= SCALAR;
        Csub_exp += 1;
    }
    while (Csub_sig > 1/SCAL_THRESH)
    {
        Csub_sig /= SCALAR;
        Csub_exp -= 1;
    }
    while (Csub_sig < -1/SCAL_THRESH)
    {
        Csub_sig /= SCALAR;
        Csub_exp -= 1;
    }

    // Write the block sub-matrix to device memory;
    // each thread writes one element
    int c = wB * BLOCK_SIZE * by + BLOCK_SIZE * bx;
    if (in_frame)
    {
        //C[c_offset + wB*gy + gx] = get_global_size(1); // XXX temp
        if (overwrite)
            C[c_offset + wB*gy + gx] = Csub_sig;
        else
            C[c_offset + wB*gy + gx] *= Csub_sig;
        /*
        */
        Cscalings[c_offset + wB * gy + gx] = Csub_exp;
    }
}

__kernel void setVals(
                        __global float* m,
                        __global int* scalings,
                        int row_offset,
                        int col_offset,
                        int row_len_round,
                        int row_bound,
                        int col_bound,
                        float sig_val,
                        int exp_val
                        )
{
    int gx = get_global_id(0);
    int gy = get_global_id(1);

    if (gx + col_offset  < row_bound && gy + row_offset < col_bound)
    {
        m[(gy + row_offset) * row_len_round + (gx + col_offset)] = sig_val;
        scalings[gx + col_offset] = exp_val;
    }
}





