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

std::vector<unsigned int> sort_merge(std::vector<unsigned int> values) {

}

std::vector<unsigned int> sort_stdlib(std::vector<unsigned int> values) {
    std::sort(values.begin(), values.end());
    return values;
}

