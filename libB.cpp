#define PROTO_V2
#include "proto.h"

extern "C" void run_B() {
    std::cout << "\n--- Running B (v2 expected) ---\n";
    DoShutdown();
    std::cout << "--- run_B completed successfully ---\n";
}
