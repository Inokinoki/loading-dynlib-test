# RTLD_GLOBAL Poisoning: macOS vs Linux

This directory contains a minimal reproduction of the "RTLD_GLOBAL Poisoning" issue, where loading a library with `RTLD_GLOBAL` causes unintended symbol coalescing in subsequently loaded `RTLD_LOCAL` libraries.

## 1. The Experiment
Two libraries, `libA` and `libB`, both define a weak symbol `DoShutdown()`.
- `libA` is loaded with `RTLD_GLOBAL`.
- `libB` is loaded with `RTLD_LOCAL`.

We check if `libB` is "poisoned" by `libA`'s version of `DoShutdown()`.

## 2. Evidence: macOS (Mach-O)
On macOS, weak symbols always participate in a flat global search to satisfy the C++ One Definition Rule (ODR).

### Runtime Addresses
| Library | Flag | `DoShutdown` Address | Result |
| :--- | :--- | :--- | :--- |
| `libA` | `GLOBAL` | `0x1010ec528` | Target |
| `libB` | `LOCAL` | `0x1010ec528` | **POISONED** (Coalesced) |

### Symbol Table (`nm -m`)
Both symbols are exported as `weak external`, which triggers the dynamic linker (`dyld`) to ignore handle-level isolation.
```text
0000000000000528 (__TEXT,__text) weak external __Z10DoShutdownv
```

---

## 3. Evidence: Linux (ELF)
On Linux, we use **Symbol Versioning** (via version scripts) to disambiguate the symbols.

### Runtime Addresses
| Library | Flag | `DoShutdown` Address | Result |
| :--- | :--- | :--- | :--- |
| `libA` | `GLOBAL` | `0x7fffff210170` | Target |
| `libB` | `LOCAL` | `0x7fffff20b160` | **ISOLATED** (Correct) |

### Symbol Table (`nm -D`)
The symbols have version tags (`@@VERSION`). Even though they have the same name, the dynamic linker treats them as distinct entities because their version-scripts do not match.
```text
0000000000001170 W _Z10DoShutdownv@@LIBPROTO_1.0
0000000000001160 W _Z10DoShutdownv@@LIBPROTO_2.0
```

---

## 4. Conclusion
The "Poisoning" issue is a structural weakness of the Mach-O format's handling of weak symbols.
- **macOS**: Weak symbols bypass the Two-Level Namespace by design to support C++. If one version of a weak symbol is global, it wins process-wide.
- **Linux**: Provides **Symbol Versioning**, which allows multiple versions of the "same" symbol (e.g., from different Protobuf versions) to coexist peacefully in the same process.
