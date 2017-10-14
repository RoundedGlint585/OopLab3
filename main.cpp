#include <iostream>
#include "WavFile.hpp"

int main() {
    WavFile test("tests.wav");
    test.printInfo();
    test.safeAs("Kek.wav");
    std::cout << "Hello, World!" << std::endl;
    return 0;
}