#ifndef HELPERFUNCTIONS_HY
#define HELPERFUNCTIONS_HY
int round_up(int number, int base);
int round_down(int number, int base);
float* pad(float array[], int row_num, int col_num, int base);
const char* load_program_source(const char* filename);
//float* matrix_slice(struct Matrix m, int row_offset, int col_offset, int width, int height);
void print_float_mat(float* m, int row_offset, int col_offset, int h, int w, int nr, int nc);
void print_double_mat(double* m, int row_offset, int col_offset, int h, int w, int nr, int nc);
#endif
