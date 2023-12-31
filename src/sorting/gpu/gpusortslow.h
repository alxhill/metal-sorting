#pragma once

#include "../../header.hpp"

class GPUSortSlow {
    public:
        explicit GPUSortSlow(MTL::Device* device);
        ~GPUSortSlow();

        void init_with_data(std::vector<unsigned int>& data);

        void execute();

        std::vector<unsigned int> get_data();

    protected:
        void encode_pass(MTL::ComputeCommandEncoder *&encoder);
    private: 
        void init_shaders();

        MTL::Device* m_device;
        MTL::ComputePipelineState* m_kernel;
        MTL::CommandQueue* m_commmand_queue;

        MTL::Buffer* m_data_buffer;

        int input_element_count;

        static MTL::Library* s_library;
};
