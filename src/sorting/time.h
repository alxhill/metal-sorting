#pragma once

#include <chrono>
#include <functional>
#include <iostream>

void set_log_state(bool new_state);

void time_func(const char* name, std::function<void()> func);

void reset_timer();

void log_with_time(const char* message);