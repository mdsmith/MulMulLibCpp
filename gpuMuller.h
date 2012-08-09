#ifndef GPUMULLER_H
#define GPUMULLER_H
#include "muller.h"
class GPUMuller: public Muller
{
private:
    void multiply();
    void setup_context();
    void update_buffers();
public:
    GPUMuller();
    const char* get_name();
    float* get_C(int offset, int width, int height);
    void test();
    ~GPUMuller();
};
#endif
