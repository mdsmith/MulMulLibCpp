#ifndef OMPMULLER_H
#define OMPMULLER_H
#include "muller.h"

class OMPMuller: public Muller
{
private:
    void multiply();
public:
    OMPMuller();
    const char* get_name();
    float* get_C(int row_offset, int col_offset, int width, int height);
    void test();
    ~OMPMuller();
};
#endif
