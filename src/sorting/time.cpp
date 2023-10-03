#include "time.h"

void time_func(const char* name, std::function<void()> func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << name << "() execution time: " << duration << " microseconds" << std::endl;
}