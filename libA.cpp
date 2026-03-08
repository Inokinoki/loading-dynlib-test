#define PROTO_V1
#include "proto.h"

extern "C" void run_A() {
    std::cout << "\n--- Running A (v1 expected) ---\n";
    DoShutdown();
    std::cout << "--- run_A completed successfully ---\n";
}
