#pragma once

#include "../../header.hpp"

class GPUSortBitonic {
    public:
        explicit GPUSortBitonic(MTL::Device* device);
        ~GPUSortBitonic();

        void init_with_data(std::vector<unsigned int>& data);

        void execute();

        std::vector<unsigned int> get_data();

    protected:
        void encode_merge(MTL::ComputeCommandEncoder*& encoder, int start, int end, bool ascending);
    private: 
        void init_shaders();

        MTL::Device* m_device;
        MTL::ComputePipelineState* m_swap_asc_kernel;
        MTL::ComputePipelineState* m_swap_dec_kernel;
        MTL::CommandQueue* m_commmand_queue;

        MTL::Buffer* m_data_buffer;

        int input_element_count;

        static MTL::Library* s_library;
};
