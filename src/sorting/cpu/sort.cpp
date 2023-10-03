#include "sort.h"
#include <algorithm>

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
    int n = values.size();

    std::vector<unsigned int> left(values.begin(), values.begin() + values.size() / 2);

    return {};
}

void sort_radix_inplace_recur(std::vector<unsigned int>& values, unsigned int bitmask) {
    if (bitmask == (0b1 << 31)) {
        return;
    }
    int leftBufferStart = 0;
    int rightBufferStart = values.size() - 1;

    for (int i = 0; i < rightBufferStart; ++i) {
        if ((values[i] & bitmask) == 0) {
            std::swap(values[i], values[leftBufferStart++]);
        } else {
            std::swap(values[i], values[rightBufferStart--]);
        }
    }

    sort_radix_inplace_recur(values, bitmask << 1);
}

void sort_radix(std::vector<unsigned int> values) {
    sort_radix_inplace_recur(values, 0b1);
}

void sort_stdlib(std::vector<unsigned int> values) {
    std::sort(values.begin(), values.end());
}

