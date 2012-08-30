#ifndef ARMADILLOMULLER_H
#define ARMADILLOMULLER_H
#include "muller.h"

class ArmadilloMuller: public Muller
{
private:
    void multiply();
public:
    ArmadilloMuller();
    const char* get_name();
    float* get_C(int row_offset, int col_offset, int width, int height);
    float* get_C(int row_offset, int col_offset, int width, int height);
    void test();
    ~ArmadilloMuller();
};
#endif
