#include "../time.h"
#include "sort.h"
#include <algorithm>
#include <iostream>

void binary_radix_sort(std::vector<unsigned int> &values, const int start, const int end, unsigned int bitmask);
void check_power_of_2(std::vector<unsigned int> values);
void bitonic_merge(std::vector<unsigned int> &values, const int start, const int end, bool ascending);
void bitonic_split(std::vector<unsigned int> &bitonic_seq, const int start, const int end, bool ascending);

std::vector<unsigned int> sort_bitonic(const std::vector<unsigned int>& values) {
    if (values.size() == 1) {
        return values;
    }
    check_power_of_2(values);

    std::vector<unsigned int> result(values);
    for (int base_size = 2; base_size < values.size(); base_size *= 2) {
        for (int i = 0; i < values.size(); i += base_size*2) {
            bitonic_merge(result, i, i + base_size, true);
            bitonic_merge(result, i + base_size, i + base_size*2, false);
        }
    }

    bitonic_merge(result, 0, result.size(), true);
    return result;
}

void bitonic_merge(std::vector<unsigned int> &bitonic_seq, const int start, const int end, bool ascending) {
    bitonic_split(bitonic_seq, start, end, ascending);
    if (end-start > 2) {
        int mid = start + (end - start) / 2;
        bitonic_merge(bitonic_seq, start, mid, ascending);
        bitonic_merge(bitonic_seq, mid, end, ascending);
    }
}

void bitonic_split(std::vector<unsigned int> &bitonic_seq, const int start, const int end, bool ascending) {
    int diff = (end - start) / 2;
    for (int i = start; i < start + diff; i++) {
        int j = i + diff;
        unsigned int left = bitonic_seq[i];
        unsigned int right = bitonic_seq[j];
        if (ascending) {
            bitonic_seq[i] = std::min(left, right);
            bitonic_seq[j] = std::max(left, right);
        } else {    
            bitonic_seq[i] = std::max(left, right);
            bitonic_seq[j] = std::min(left, right);
        }
    }
}

std::vector<unsigned int> sort_radix(const std::vector<unsigned int>& values) {
    std::vector<unsigned int> copy(values);
    binary_radix_sort(copy, 0, copy.size() - 1, 0b1 << 31);
    return copy;
}

std::vector<unsigned int> sort_stdlib(const std::vector<unsigned int>& values) {
    std::vector<unsigned int> copy(values);
    std::sort(copy.begin(), copy.end());
    return copy;
}

void binary_radix_sort(std::vector<unsigned int> &values, const int start, const int end, unsigned int bitmask) {
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
        binary_radix_sort(values, start, zeroMark, bitmask >> 1);
    }
    if (end - oneMark > 0) {
        binary_radix_sort(values, oneMark, end, bitmask >> 1);
    }
}

void check_power_of_2(std::vector<unsigned int> values) {
    int i = values.size();
    while (i % 2 == 0) {
        i = i/2;
    }
    if (i != 1) {
        throw std::runtime_error("values must be a power of 2");
    }
}