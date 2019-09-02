#include <iostream>
#include <iomanip>

using namespace std;
int main() {
    int a = 4;
    int b = 5; 
    int c = 6;
    int inputLength = 10;
    char d [inputLength];
    cout << a + b  + c << std::endl; 
    cin >> std::setw(inputLength) >> d;
    cout << d << std::endl; 
    cout << "Hello Easy C++ project!" << std::endl;
}