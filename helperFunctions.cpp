#include "helperFunctions.h"
#include "matrix.h"
#include <sys/stat.h>
#include <sys/time.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

//#define WOLFRAM
using namespace std;

// definitions
int round_up(int number, int base)
{
    if (number == round_down(number, base))
        return number;
    else
        return round_down(number+base, base);
}

int round_down(int number, int base)
{
    return (int)(number/base)*base;
}

/*
// made obsolete by the movement to Matrix as a class
// Marked for removal on 8/23/12
float* pad(float array[], int row_num, int col_num, int base)
{
    int round_col = round_up(col_num, base);
    int round_row = round_up(row_num, base);
    float *tbr = new float[round_col * round_row];
    for (int i = 0; i < round_row; i++)
    {
        for (int j = 0; j < round_col; j++)
        {
            if (i < row_num && j < col_num)
                tbr[i * round_col + j] = array[i * col_num + j];
            else
                tbr[i * round_col + j] = 0.0f;
        }
    }
    return tbr;
}

float* matrix_slice(Matrix m, int offset, int width, int height)
{
    float* tbr = new float[width*height];
    float* start = m.data + offset;
    for (int r = 0; r < height; r++)
        for (int c = 0; c < width; c++)
            tbr[r*width + c] = start[r*m.num_cols + c];
    return tbr;
    //cout << "width vs numcols: " << width << ", " << m.num_cols << endl;
    //cout << "height vs numrows: " << height << ", " << m.num_rows << endl;
    //return m.data;
}
*/

const char * load_program_source(const char *filename)
{
    struct stat statbuf;
    FILE *fh;
    char *source;

    fh = fopen(filename, "r");
    if (fh == 0)
    {
        cout << "File not found!" << endl;
        return 0;
    }

    stat(filename, &statbuf);
    source = (char *) malloc(statbuf.st_size + 1);
    fread(source, statbuf.st_size, 1, fh);
    source[statbuf.st_size] = '\0';

    return source;
}

// print the subset of m starting at the offset, where m has nr rows and nc
// columns, and you're looking to print h rows and w columns of m.
void print_float_mat(float* m, int row_offset, int col_offset, int h, int w, int nr, int nc)
{
    cout << "{";
    for (int r = row_offset; r < row_offset + h; r++)
    {
        cout << "{";
        for (int c = col_offset; c < col_offset + w; c++)
        {
            if (c > col_offset && c < col_offset + w)
                cout << ",";
            printf("%4.4f", m[r*nc + c]);
        }
        if (r < row_offset + h-1)
#if defined WOLFRAM
            cout << "},";
#else
            cout << "}," << endl;
#endif
        else
            cout << "}";
    }
    cout << "}" << endl;
}
