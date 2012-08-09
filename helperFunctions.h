#ifndef HELPERFUNCTIONS_HY
#define HELPERFUNCTIONS_HY
int round_up(int number, int base);
int round_down(int number, int base);
float* pad(float array[], int row_num, int col_num, int base);
const char* load_program_source(const char* filename);
float* matrix_slice(struct Matrix m, int offset, int width, int height);
#endif