#ifndef FMAMULLER_H
#define FMAMULLER_H
#include "muller.h"

class FMAMuller: public Muller
{
private:
    void multiply();
public:
    FMAMuller();
    const char* get_name();
    float* get_C(int row_offset, int col_offset, int width, int height);
    void test();
    ~FMAMuller();
};
#endif
