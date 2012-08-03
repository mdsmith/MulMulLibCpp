#include <iostream>
#include "naiveFunctions.h"
#include "muller.h"
using namespace std;

int main()
{
    NaiveMuller nM = NaiveMuller();
    nM.test();
    Muller* m = &nM;
    m->test();
    cout << "main checking in" << endl;
    return 0;
}

// XXX make the test arrays (A, B)


// XXX make the muller object
// XXX pass in the arrays
// XXX get back the arrays
// XXX compare to the output of naiveFunctions.h
// XXX print passed/fail
