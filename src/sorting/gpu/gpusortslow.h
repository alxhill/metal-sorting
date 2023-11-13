#pragma once

#include "Metal/MTLBuffer.hpp"
#include "gpufunc.h"

class GPUSortSlow {
    public:
        explicit GPUSortSlow(MTL::Device* device);
        ~GPUSortSlow();

        void init_with_data(std::vector<unsigned int>& data);

        void execute();

        std::vector<unsigned int> get_data();

    protected:
        void encode_pass(MTL::ComputeCommandEncoder *&encoder, int pass_idx);
    private: 
        void init_shaders();

        MTL::Device* m_device;
        MTL::ComputePipelineState* m_kernel;
        MTL::ComputePipelineState* m_copy_kernel;
        MTL::CommandQueue* m_commmand_queue;

        MTL::Fence* m_fence;

        MTL::Buffer* m_data_buffer;
        MTL::Buffer* m_internal_buffer;

        int input_element_count;

        static MTL::Library* s_library;
};
