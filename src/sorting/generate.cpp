#include "generate.h"

std::vector<unsigned int> generate_uints(int count) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::vector<unsigned int> random_numbers;

    std::uniform_int_distribution<unsigned int> distribution(
        std::numeric_limits<unsigned int>::lowest(), std::numeric_limits<unsigned int>::max()
    );
    for (int i = 0; i < count; ++i) {
        random_numbers.push_back(distribution(generator));
    }

    return random_numbers;
}

std::vector<float> generate_floats(int count) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::vector<float> random_numbers;

    std::uniform_real_distribution<float> distribution(
        std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max()
    );
    for (int i = 0; i < count; ++i) {
        random_numbers.push_back(distribution(generator));
    }

    return random_numbers;
}
std::vector<uint16_t> generate_uint_16(int count) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::vector<uint16_t> random_numbers;

    std::uniform_int_distribution<uint16_t> distribution(
        std::numeric_limits<uint16_t>::lowest(), std::numeric_limits<uint16_t>::max()
    );
    for (int i = 0; i < count; ++i) {
        random_numbers.push_back(distribution(generator));
    }

    return random_numbers;
};
std::vector<_Float16> generate_halfs(int count) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::vector<_Float16> random_numbers;

    std::uniform_real_distribution<_Float16> distribution(
        std::numeric_limits<_Float16>::lowest(), std::numeric_limits<_Float16>::max()
    );
    for (int i = 0; i < count; ++i) {
        random_numbers.push_back(distribution(generator));
    }

    return random_numbers;
}
