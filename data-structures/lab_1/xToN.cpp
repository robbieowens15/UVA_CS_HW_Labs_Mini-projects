/*
Robert Owens (rao7utn)
09/03/2020
xToN.cpp
*/

#include <iostream>
using namespace std;

int xton (int base, int power) {
	if (power == 0){
		return 1;
	}
	else if (power == 1){
		return base;
	}
	else{
		return base*xton(base, power -1);
	}
}

int main () {
    int x, y;
    cin >> x;
    cin >> y;
    cout << xton (x, y) << endl;
    return 0;
}