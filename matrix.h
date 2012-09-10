#ifndef MATRIX_H
#define MATRIX_H
class Matrix
{
private:
    bool set;
    float* data;
    int* scalings;
    float scal_thresh;
    float scalar;
    int w;
    int h;
    int col_offset;
    int row_offset;
    int num_rows;
    int num_cols;
public:
    Matrix();
    ~Matrix();
    void update_scalings();
    float* get_unscaled();
    double* get_unscaled_double();
    float* get_scaled() {return data;};
    int* get_scalings() {return scalings;};
    float* get_data() {return get_unscaled();};
    void set_data_and_scalings( float* data,
                                int* scalings,
                                int num_rows,
                                int num_cols)
    {   this->data = data;
        this->scalings = scalings;
        this->num_rows = num_rows;
        this->num_cols = num_cols;};
    int get_bound_cols() {return w;};
    int get_bound_rows() {return h;};
    int get_total_cols() {return num_cols;};
    int get_total_rows() {return num_rows;};
    int get_row_offset() {return row_offset;};
    int get_col_offset() {return col_offset;};
    void set_data(  float* data,
                    int row_offset,
                    int col_offset,
                    int h,
                    int w,
                    int num_rows,
                    int num_cols);
    void set_data(  double* data,
                    int row_offset,
                    int col_offset,
                    int h,
                    int w,
                    int num_rows,
                    int num_cols);
    void bound_data(int row_offset, int col_offset, int h, int w);
    void print_mat(int row_offset, int col_offset, int num_rows, int num_cols);
    void print_total();
    void print_bound();
    void update_data(   float* data,
                        int row_offset,
                        int col_offset,
                        int h,
                        int w,
                        int num_rows,
                        int num_cols
                        );
    void update_data(   double* data,
                        int row_offset,
                        int col_offset,
                        int h,
                        int w,
                        int num_rows,
                        int num_cols
                        );
    void pad_to(int interval);
    bool is_set() {return set;};
    void set_set(bool setting) {set = setting;};
    float* get_slice(int row_offset, int col_offset, int width, int height);
    double* get_slice_double(int row_offset, int col_offset, int width, int height);

    // if w*h is < this->num_rows*this->num_cols it will trim to this square
    // w/upper left @offset.
    // if w*h > this->num_rows*this->num_cols it will expand the borders and put
    // current upper left @offset
    // void resize(int offset, int w, int h); // XXX
};
#endif
