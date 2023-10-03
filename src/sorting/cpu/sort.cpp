#include "sort.h"
#include <algorithm>
#include <iostream>

void check_power_of_2(std::vector<unsigned int> values) {
    int i = values.size();
    while (i % 2 == 0) {
        i = i/2;
    }
    if (i != 1) {
        throw std::runtime_error("values must be a power of 2");
    }
}

std::vector<unsigned int> sort_bitonic(std::vector<unsigned int> values) {
    if (values.size() == 1) {
        return values;
    }
    check_power_of_2(values);

    std::vector<unsigned int> left(values.begin(), values.begin() + values.size() / 2);

    return {};
}

void sort_radix_inplace_recur(std::vector<unsigned int>& values, int start, int end, unsigned int bitmask) {
    if (bitmask == (0b1 << 3)) {
        std::cout << "done sorting" << std::endl;
        return;
    }
    int leftBufferPos = start;
    int rightBufferPos = end;

    while (leftBufferPos < rightBufferPos) {
        if ((values[leftBufferPos] & bitmask) == 0) {
            leftBufferPos++;
        } else {
            std::swap(values[leftBufferPos], values[rightBufferPos--]);
        }
    }

    std::cout << "sorted with bitmask:" << std::bitset<4>(bitmask) << std::endl;
    std::cout << "left buffer: " << leftBufferPos << " right buffer: " << rightBufferPos << std::endl;

    std::cout << "ordered values:";
    for (unsigned int i : values) {
        std::cout << std::bitset<4>(i) << " ";
    }
    std::cout << std::endl;

    sort_radix_inplace_recur(values, start, leftBufferPos, bitmask << 1);
    sort_radix_inplace_recur(values, leftBufferPos, end, bitmask << 1);
}

void sort_radix(std::vector<unsigned int>& values) {
    sort_radix_inplace_recur(values, 0, values.size() - 1, 0b1);
}

void sort_stdlib(std::vector<unsigned int>& values) {
    std::sort(values.begin(), values.end());
}
