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
    int offset;
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
    int get_offset() {return offset;};
    void set_data(  float* data,
                    int offset,
                    int h,
                    int w,
                    int num_rows,
                    int num_cols);
    void bound_data(int offset, int h, int w);
    void print_mat(int offset, int num_rows, int num_cols);
    void update_data(   float* data
                        , int offset
                        , int h
                        , int w
                        , int num_rows
                        , int num_cols
                        );
    void pad_to(int interval);
    bool is_set() {return set;};
    void set_set(bool setting) {set = setting;};
    float* get_slice(int offset, int width, int height);

    //void print_bound(); // XXX

    // if w*h is < this->num_rows*this->num_cols it will trim to this square
    // w/upper left @offset.
    // if w*h > this->num_rows*this->num_cols it will expand the borders and put
    // current upper left @offset
    // void resize(int offset, int w, int h); // XXX
};
#endif
