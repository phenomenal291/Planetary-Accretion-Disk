#include "../include/Vector.h"
#include <iostream>
using namespace std;
int main(){
    // test library;

    Vector<float> a(1.2f, 2.8f, 3.7f);
    a = a / 2.0f;
    a.Print();
}
