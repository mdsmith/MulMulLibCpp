#ifndef GPUMULLER_H
#define GPUMULLER_H
#include "muller.h"
typedef short scaltype;
class GPUMuller: public Muller
{
private:
    void multiply();
    void setup_context();
    void update_buffers();
public:
    GPUMuller();
    const char* get_name();
    void set_A(float* A, int num_rows, int num_cols);
    void set_B(float* B, int num_rows, int num_cols);
    void set_C(float* C, int num_rows, int num_cols);
    void update_A(float* A, int row_offset, int col_offset, int ah, int ud, int num_rows, int num_cols);
    void update_B(float* B, int row_offset, int col_offset, int ud, int bw, int num_rows, int num_cols);
    void bound_A(int row_offset, int col_offset, int ah, int ud);
    void bound_B(int row_offset, int col_offset, int ud, int bw);
    void check_buffers();
    float* get_C(int row_offset, int col_offset, int height, int width);
    void test();
    ~GPUMuller();
};
#endif
