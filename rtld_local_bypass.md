# Why macOS Bypasses RTLD_LOCAL for Protobuf Symbols

This document clarifies the technical mechanism behind the symbol collision observed between `pyarrow` and `tink-py` on macOS, even when Python's default `RTLD_LOCAL` loading mode is used.

## 1. The Core Misconception: RTLD_LOCAL == Strict Isolation
While `dlopen(..., RTLD_LOCAL)` is designed to hide a library's symbols from the global namespace, it does **not** provide complete isolation for C++ symbols, especially those related to the **One Definition Rule (ODR)**.

## 2. Weak Symbol Coalescing
On macOS, `dyld` (the dynamic linker) treats certain symbols as "weak" by default. This includes:
-   Functions marked as `inline` in headers.
-   C++ template instantiations.
-   Static members of classes defined in headers.

For these symbols, `dyld`'s primary goal is to ensure that only **one address** exists for that symbol across the entire process. This is to ensure that global registries, singletons, and type comparisons (RTTI) work correctly.

## 3. The "Global Poisoning" Mechanism
The collision occurs through a process we call **Global Poisoning**:

1.  **Global Load (The Poison)**: Some library in the process (e.g., `pyarrow` or one of its dependencies like Gandiva) is loaded with `RTLD_GLOBAL`. This makes its version of the Protobuf symbols (e.g., `google::protobuf::ShutdownData`) part of the process-wide **Global Weak Symbol Table**.
2.  **Local Load (The Victim)**: When you later import `tink-py`, Python uses `RTLD_LOCAL`. 
3.  **The Bypass**: Even though `tink-py` is "local," `dyld` still checks the Global Weak Symbol Table for its weak symbols to satisfy ODR rules. 
4.  **Binding**: If `dyld` finds a matching symbol that's already global (from the poisoned load), it **snaps `tink-py`'s references to that global address**, ignoring the local isolation for that specific symbol.

## 4. Result: Fatal Collision
Once `tink-py`'s code is bound to `pyarrow`'s symbols:
-   `tink-py` (compiled for Protobuf V2) calls a function at an address belonging to `pyarrow` (compiled for Protobuf V1).
-   The V1 function expects a specific data layout (e.g., a larger `Mutex` size).
-   The mismatch causes a **buffer overflow** or **invalid argument** error, leading to the well-known `mutex lock failed` crash.

## 5. Summary
`RTLD_LOCAL` prevents *others* from seeing *your* symbols, but it does **not** protect *you* from being forced to use *their* global symbols if they are marked as weak. On macOS, this behavior is a feature intended to preserve C++ semantics, but it becomes a fatal flaw when conflicting versions of a statically linked library are present in the same process.
