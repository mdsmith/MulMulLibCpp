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
    float* get_C(int offset, int width, int height);
    void test();
    ~ArmadilloMuller();
};
#endif
