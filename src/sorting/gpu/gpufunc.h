#pragma once

#include "../../header.hpp"
#include <memory>
#include <vector>


class GPUFunc {
    public:
        explicit GPUFunc(MTL::Device* device);
        ~GPUFunc();

        void execute();
        std::vector<unsigned int> get_data();

    protected:
        virtual MTL::Function* get_function(MTL::Library& library) = 0;
        virtual void encode_command(MTL::ComputeCommandEncoder *&encoder) = 0;

        void init_shaders();

        MTL::Device* m_device;
        MTL::ComputePipelineState* m_pso;
        MTL::CommandQueue* m_commmand_queue;
        MTL::Buffer* m_data_buffer;

        int input_element_count;

    private:
        static MTL::Library* s_library;
};
