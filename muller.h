#ifndef MULLER_H
#define MULLER_H

class Muller
{
public:
    virtual void test() =0;
};

class NaiveMuller: public Muller
{
public:
    NaiveMuller();
    void test();
    ~NaiveMuller();
};

# endif
