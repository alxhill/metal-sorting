#pragma once

#include "../../header.hpp"
#include <vector>


class GPUSort {
    public:
        explicit GPUSort(MTL::Device* device);

        std::vector<unsigned int> sort(std::vector<unsigned int>& data);

    private:
        MTL::Device* m_device;
        MTL::CommandQueue* m_commmand_queue;
        MTL::Library* m_shader_library;
        MTL::Buffer* m_input_buffer;
};