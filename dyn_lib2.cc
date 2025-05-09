#include <iostream>
#include "static_lib.h"

extern "C" {
    void printMessage();
}

void printMessage()
{
    // This function is just a placeholder to demonstrate the use of the dynamic library.
    std::cout << "Hello from the dynamic library 2!" << std::endl;
    std::cout << fakeFunction() << std::endl;
}
