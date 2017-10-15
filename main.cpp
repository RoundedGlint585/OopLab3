#include <iostream>
#include "WavFile.hpp"

int main() {
    WavFile test("test.wav");
    test.printInfo();
    test.makeMono();
    test.makeReverb( 0.500, 0.6f );
    test.safeAs("res.wav");
    std::cout << "Hello, World!" << std::endl;
    return 0;
}