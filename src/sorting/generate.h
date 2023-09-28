#pragma once
#include <iostream>
#include <random>
#include <vector>
#include <cstdint>

std::vector<unsigned int> generate_uints(unsigned long count);
std::vector<float> generate_floats(int count);
std::vector<uint16_t> generate_uint_16(int count);
std::vector<_Float16> generate_halfs(int count);
