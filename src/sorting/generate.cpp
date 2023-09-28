#include "generate.h"

template<typename T>
std::vector<T> generate_values(int count) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::vector<T> random_numbers;

    // Check if T is floating point and generate numbers accordingly
    if (std::is_floating_point<T>::value) {
        std::uniform_real_distribution<T> distribution(
            std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max()
        );
        for (int i = 0; i < count; ++i) {
            random_numbers.push_back(distribution(generator));
        }
    }

    // For integer types
    else if (std::is_integral<T>::value) {
        std::uniform_int_distribution<T> distribution(
            std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max()
        );
        for (int i = 0; i < count; ++i) {
            random_numbers.push_back(distribution(generator));
        }
    }

    return random_numbers;
}

