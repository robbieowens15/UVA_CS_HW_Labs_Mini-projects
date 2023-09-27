
#include <iostream>
#include <cstdlib>
#include "timer.h"

using namespace std;

extern "C" unsigned int threexplusone (int);

int  main () {

    unsigned int x, n, steps;

    cout << "Enter a number:  ";
    cin >> x;
    cout << "Enter iterations of subroutine:  ";
    cin >> n;

    steps = threexplusone(x);
    cout << "Steps: " << steps << endl;

    timer t;
    t.start();

    for(int i = 0; i < n; i++){
        threexplusone(x);
    }

    t.stop();
    //cout << t.getTime() << " s" << endl;

    return 0;
}