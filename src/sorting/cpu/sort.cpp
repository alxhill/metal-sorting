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

void sort_radix_inplace_recur(std::vector<unsigned int> &values, const int start, const int end, unsigned int bitmask) {
    if (bitmask == 0) {
        return;
    }

    int zeroMark = start - 1;
    int oneMark = end + 1;

    while (zeroMark + 1 < oneMark) {
        if ((values[zeroMark + 1] & bitmask) == 0) {
            zeroMark++;
        } else {
            std::swap(values[zeroMark + 1], values[oneMark - 1]);
            oneMark--;
        }
    }

    if (zeroMark - start > 0) {
        sort_radix_inplace_recur(values, start, zeroMark, bitmask >> 1);
    }
    if (end - oneMark > 0) {
        sort_radix_inplace_recur(values, oneMark, end, bitmask >> 1);
    }
}

void sort_radix(std::vector<unsigned int>& values) {
    sort_radix_inplace_recur(values, 0, values.size() - 1, 0b1 << 31);
}

void sort_stdlib(std::vector<unsigned int>& values) {
    std::sort(values.begin(), values.end());
}
