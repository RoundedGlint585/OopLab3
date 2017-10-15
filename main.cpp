#include <iostream>
#include "WavFile.hpp"

int main() {
    WavFile test("test.wav");
    test.printInfo();
    test.makeMono();
    test.safeAs("res.wav");
    std::cout << "Hello, World!" << std::endl;
    return 0;
}