#pragma once
#include <vector>

template <typename T>
std::vector<T> sort_bitonic(std::vector<T> values);

template <typename T>
std::vector<T> sort_merge(std::vector<T> values);

std::vector<unsigned int> sort_stdlib(std::vector<unsigned int> values);
