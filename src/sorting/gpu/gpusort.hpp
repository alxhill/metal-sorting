#pragma once

#include "../../header.hpp"
#include <vector>


class GPUSort {
    public:
        explicit GPUSort(MTL::Device* device);
        ~GPUSort();

        void prepare_data(std::vector<unsigned int>& data);
        void compute_sort();
        std::vector<unsigned int> get_data();

    private:
        void init_shaders();
        void encode_command(MTL::ComputeCommandEncoder *&encoder);

        MTL::Device* m_device;
        MTL::ComputePipelineState* m_pso;
        MTL::CommandQueue* m_commmand_queue;
        MTL::Library* m_shader_library;
        MTL::Buffer* m_input_buffer;
        MTL::Buffer* m_output_buffer;

        int input_element_count;
};