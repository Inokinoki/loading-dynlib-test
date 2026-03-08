#pragma once
#include <iostream>

struct Mutex {
#ifdef PROTO_V2
    int data[1];
#else
    int data[100];
#endif
    Mutex() {
        // We'll log to see which gets used
        std::cout << "Mutex size: " << sizeof(Mutex) << "\n";
    }
    void lock() {
        std::cout << "Mutex lock, size: " << sizeof(Mutex) << "\n";
        data[0] = 1;
#ifndef PROTO_V2
        std::cout << "Writing at offset 99...\n";
        data[99] = 1; // This will buffer overflow if v2 object is used with v1 method!
#endif
    }
};

struct ShutdownData {
    Mutex mutex;
    void OnShutdownRun() {
        mutex.lock();
    }
};

// Explicitly mark as weak to test dyld coalescing behavior
__attribute__((weak)) void DoShutdown() {
    static ShutdownData* shutdown_data = nullptr;
    if (!shutdown_data) shutdown_data = new ShutdownData();
    
    std::cout << "[Shared] DoShutdown called at address: " << (void*)DoShutdown << "\n";
    std::cout << "[Shared] static shutdown_data address: " << (void*)shutdown_data << "\n";
    shutdown_data->OnShutdownRun();
}
