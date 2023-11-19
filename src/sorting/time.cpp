#include "time.h"
#include <cassert>
#include <chrono>
#include <cstdio>
#include <ratio>

std::chrono::time_point<std::chrono::high_resolution_clock> global_timer;
std::chrono::time_point<std::chrono::high_resolution_clock> previous_log_time;
bool enable_logging = true;

void set_log_state(bool new_state) {
    enable_logging = new_state;
}

void time_func(const char* name, std::function<void()> func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << name << "() execution time: " << duration << " ms" << std::endl;
}

void reset_timer() {
    global_timer = std::chrono::high_resolution_clock::now();
    previous_log_time = global_timer;
}

void log_with_time(const char* message) {
    if (!enable_logging)
        return;

    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - previous_log_time).count();
    auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - global_timer).count();

    if (global_timer == previous_log_time) {
        std::printf("[%lldms] - %s\n", total_duration, message);
    } else {
        std::printf("[%lldms | Δ%lldms] - %s\n", total_duration, duration, message);
    }
    previous_log_time = now;
}
