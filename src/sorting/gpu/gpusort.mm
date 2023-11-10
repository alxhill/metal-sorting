#include "gpusort.hpp"

GPUSort::GPUSort(MTL::Device* device) : m_device(device) {
    
}

std::vector<unsigned int> GPUSort::sort(std::vector<unsigned int> &data) {
    return {1, 2, 3, 4, 5,};
}