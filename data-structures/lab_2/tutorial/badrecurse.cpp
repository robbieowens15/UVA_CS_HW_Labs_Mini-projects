#include <iostream>
using namespace std;
void recurse(int x) {
  int *y = NULL;
  if ( x == 0 )
    cout << *y << endl;
  recurse(x-1);
}
int main() {
  recurse(5);
  return 0;
}