#include <dlfcn.h>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  int mode = RTLD_NOW | RTLD_LOCAL;
  std::string mode_str = "RTLD_LOCAL";

  std::string suffix = "";
  if (argc > 1) {
    for (int i = 1; i < argc; ++i) {
      if (std::string(argv[i]) == "--flat")
        suffix = "_flat";
      if (std::string(argv[i]) == "--global") {
        mode = RTLD_NOW | RTLD_GLOBAL;
        mode_str = "RTLD_GLOBAL";
      }
    }
  }

  std::cout << "Loading dynamic libraries in mode: " << mode_str
            << " (Flat: " << suffix << ")\n\n";

#ifdef __apple__
  std::string pathA = "./libA" + suffix + ".dylib";
  std::string pathB = "./libB" + suffix + ".dylib";
#else
  std::string pathA = "./libA" + suffix + ".so";
  std::string pathB = "./libB" + suffix + ".so";
#endif

  int modeA = mode;
  int modeB = mode;
  if (argc > 1) {
    for (int i = 1; i < argc; ++i) {
      if (std::string(argv[i]) == "--poison") {
        modeA = RTLD_NOW | RTLD_GLOBAL;
        modeB = RTLD_NOW | RTLD_LOCAL;
        std::cout << "--- POISONING MODE: LibA is GLOBAL, LibB is LOCAL ---\n";
      }
    }
  }

  void *handleA = dlopen(pathA.c_str(), modeA);
  if (!handleA) {
    std::cerr << "Fail A: " << dlerror() << "\n";
    return 1;
  }
  void (*run_A)() = (void (*)())dlsym(handleA, "run_A");

  void *handleB = dlopen(pathB.c_str(), modeB);
  if (!handleB) {
    std::cerr << "Fail B: " << dlerror() << "\n";
    return 1;
  }
  void (*run_B)() = (void (*)())dlsym(handleB, "run_B");

  // Run A (v1)
  if (run_A) {
    run_A();
  }

  // Find DoShutdown in A
  void *symA =
      dlsym(handleA, "_Z10DoShutdownv"); // Mangled name for DoShutdown()
  std::cout << "[Main] Symbol DoShutdown in libA handle: " << symA << "\n";

  // Run B (v2)
  if (run_B) {
    run_B();
  }

  // Find DoShutdown in B
  void *symB = dlsym(handleB, "_Z10DoShutdownv");
  std::cout << "[Main] Symbol DoShutdown in libB handle: " << symB << "\n";

  if (symA == symB && symA != nullptr) {
    std::cout << "\nWARNING: Symbols coalesced even in " << mode_str
              << " mode!\n";
  }

  return 0;
}
