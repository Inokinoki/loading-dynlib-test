#include <iostream>
#include <dlfcn.h>

#ifdef LOAD_LOCALLY
#define RTLD_CUSTOM_FLAGS (RTLD_LOCAL | RTLD_NOW)
#else
#define RTLD_CUSTOM_FLAGS RTLD_NOW
#endif

int main(int argc, char* argv[]) {
    bool reverse = false;
    if (argc > 1) {
        reverse = (argv[1][0] == 'r');
    }

    // Default open with RTLD_LOCAL
    void *dynlib1_handler = dlopen("./dyn_lib1.so", RTLD_CUSTOM_FLAGS);
    if (!dynlib1_handler) {
        // Handle error
        std::cerr << "Error opening library: " << dlerror() << std::endl;
        return 1;
    }
    // Find the symbol
    void (*printMessage)() = (void (*)())dlsym(dynlib1_handler, "printMessage");
    if (!printMessage) {
        // Handle error
        std::cerr << "Error finding symbol: " << dlerror() << std::endl;
        dlclose(dynlib1_handler);
        return 1;
    }

    // Default open with RTLD_LOCAL
    void *dynlib2_handler = dlopen("./dyn_lib2.so", RTLD_CUSTOM_FLAGS);
    if (!dynlib2_handler) {
        // Handle error
        dlclose(dynlib1_handler);
        return 1;
    }
    // Find the symbol
    void (*printMessage2)() = (void (*)())dlsym(dynlib2_handler, "printMessage");
    if (!printMessage2) {
        // Handle error
        std::cerr << "Error finding symbol: " << dlerror() << std::endl;
        dlclose(dynlib1_handler);
        dlclose(dynlib2_handler);
        return 1;
    }

    if (reverse) {
        // Call the functions in reverse order
        std::cout << "Calling functions in reverse order:" << std::endl;
        printMessage2();
        printMessage();
    } else {
        // Close the libraries in the original order
        std::cout << "Calling functions in original order:" << std::endl;
        printMessage();
        printMessage2();
    }

    std::cout << "End of program." << std::endl;

    // Close the libraries
    dlclose(dynlib1_handler);
    dlclose(dynlib2_handler);

    return 0;
}
