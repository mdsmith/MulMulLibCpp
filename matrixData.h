#ifndef MATRIX_DATA_H
#define MATRIX_DATA_H

class MatrixData
{
private:
    // Is it really necessary to store three different versions? If you're
    // not storing the matrix bounds, it doesn't matter if the storage array
    // is padded or not padded. As for double, it is impossible to say it it
    // has been tampered with so you might as well generate it fresh, plus
    // you don't really want to mess with whatever array has been passed in.
    // Plus the array that is being passed in isn't neccessarily the same one
    // or the same size that would be an appropriate output.
    float* data;
    //double* data_double;
    //float* data_padded;
    int* scalings;
    int num_rows;
    int num_cols;
    float scal_thresh;
    float scalar;
    long orig_id;

    //bool float_clean;
    //bool double_clean;
    //bool padded_clean;
    bool scalings_clean;
    //int current_clean;

    // check and possibly update the appropriate arrays relative to the
    // current_clean. At least one array should be clean at all times.
    //void check_all();
    //void check_float();
    //void check_double();
    //void check_padded();
public:
    void check_scalings();
    MatrixData();
    MatrixData(float scal_thresh, float scalar);
    ~MatrixData();
    long get_id(){return orig_id;};

    // returns an unpadded float array appropriately scaled in conjunction
    // with "scalings"
    float* get_scaled_float(){return data;};

    // returns an unpadded unscaled float array. Overflow or underflow may
    // result in rounding
    float* get_unscaled_float();

    // returns an unpadded double array appropriately scaled in conjunction
    // with "scalings"
    // double* get_scaled_double(){return data_double;}; // not really needed
    // and requires separate scaling arrays and thresholds
    // Therefore internal representation is float ONLY

    // returns an unpadded unscaled double array. Overflow or underflow may
    // result in rounding, but it shouldn't be as much of an issue as it is
    // with get_unscaled_float()
    double* get_unscaled_double();

    // returns a padded scaled float array. This is the kind of thing you'd
    // want to put on a GPU. Scaling is done in conjunction with "scalings"
    //float* get_padded(){return data_padded;
    int* get_scalings(){return scalings;};

    void set_data_and_scalings( float* data,
                                int* scalings,
                                int num_rows,
                                int num_cols)
    {   this->data = data;
        this->scalings = scalings;
        this->num_rows = num_rows;
        this->num_cols = num_cols;
    };

    int get_total_cols() const {return num_cols;};
    int get_total_rows() const {return num_rows;};

    void set_data(  float* data,
                    int num_rows,
                    int num_cols);
    void set_data(  double* data,
                    int num_rows,
                    int num_cols);

    void print_mat();
    void print_mat(int row_offset, int col_offset, int num_rows, int num_cols);

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
    float* get_slice(int row_offset, int col_offset, int width, int height);
    double* get_slice_double(int row_offset, int col_offset, int width, int height);
    void pad_to(int interval);
};

#endif
