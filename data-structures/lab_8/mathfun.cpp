// main.cpp

#include <iostream>
#include <time.h>
#include <cstdlib>

using namespace std;

extern "C" int product (int, int);
extern "C" int power (int, int);

int  main () {

    int  first, second, productResult, powerResult;

    cout << "Enter integer 1:  ";
    cin >> first;
    cout << "Enter integer 2:  ";
    cin >> second;

    productResult = product(first, second);
    cout << "Product: " << productResult << endl;

    powerResult = power(first, second);
    cout << "Power: " << powerResult << endl;

    return 0;
}