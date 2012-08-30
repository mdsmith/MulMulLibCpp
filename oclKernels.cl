#define BLOCK_SIZE 16
#define SCALAR 10
#define SCAL_THRESH 1e-10
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

// XXX this is multiplying the wrong parts together, thus all the zeros
// XXX it doesn't look like data is being copied to all the right places.
// Check post-padding

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
                int c_col_offset)
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
        // We're loading the data into the scratch areas with the offset, so
        // the scratch multiplication is full-matrix-like.
        // This requires the saving of C to be with an offset as well XXX
        // As well as careful analysis of appropriate bounds (tx = 0 ty = 0
        // will be working on i= 15 u = 19), perhaps... XXX

        if ((A_index / wA) < col_bound && (A_index % wA) < a_row_len)
        {
            As[ty][tx] = A[a_offset + A_index]; // XXX real
            // setting this equal to tx results in all zeros
            //As[ty][tx] = A_index;
            //As[ty][tx] = 0.1f;
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
            Bs[ty][tx] = B[b_offset + B_index]; // XXX real
            //Bs[ty][tx] = 1.0f;
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
        for (int k = 0; k < BLOCK_SIZE; ++k) // XXX real
        //for (int k = 0; k < 1; ++k)
        {
            float a_sig = As[ty][k];
            int a_exp = Ascal_cache[ty][k];
            float b_sig = Bs[k][tx];
            int b_exp = Bscal_cache[k][tx];
            float c_sig = a_sig * b_sig;
            int c_exp = a_exp + b_exp;
            int new_exp = MAX(c_exp, Csub_exp);
            /* XXX this is temporarily commented out
            // XXX XXX XXX XXX XXX XXX XXX XXX XXX This is where the problem
            // is. The real version below is broken. This small part of the
            // scaling mechanism is screwing everything up
            int c_sub_sig_local = Csub_sig * pow(   10.0f,
                                                    (float)new_exp -
                                                    (float)Csub_exp);
            c_sig = c_sig * pow(10.0f,
                                (float)new_exp -
                                (float)c_exp);
            */
            int c_sub_sig_local = Csub_sig; // XXX temp
            //c_sig = a_sig; // XXX temp
            //Csub_sig = c_sub_sig_local + c_sig; // XXX real
            //int c_temp = Csub_sig;
            Csub_sig    = Csub_sig * pow( 10.0f, (float)new_exp-(float)Csub_exp)
                        + c_sig * pow(10.0f, (float)new_exp - (float)c_exp); // XXX real
            //Csub_sig += c_sig;
            Csub_exp = new_exp;

            //Csub_sig += c_sig;
            //Csub += c_sig * pow(10, c_exp - Cexp);
            // problem: Cexp will never change
            // problem: What should Cexp start as?
            // when should it move?
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
        //C[c_offset + c + wB * ty + tx] = Csub_sig;
        //C[c_offset + wB * ty + tx] = Csub_sig;
        //C[c_offset + wB * ty + tx] = 5.4f;
        //C[c_offset + c + wB * ty + tx] = c;
        //C[c_offset + wB*gy + gx] = (float)(c_offset + wB*gy + gx);
        //C[c_offset + wB*gy + gx] = 5.4f;
        C[c_offset + wB*gy + gx] = Csub_sig; // XXX the correct one
        //C[c_offset + wB*gy + gx] = A[a_offset + gy*a_round_row_len + gx];
        //C[0] = 5.4f;
        //C[c_offset + wB * ty + tx] = 5.4;
        //Cscalings[c_offset + c + wB * ty + tx] = Csub_exp;
        Cscalings[c_offset + wB * gy + gx] = Csub_exp;
    }
}
