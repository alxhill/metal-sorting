#pragma once

#include "../../header.hpp"
#include <memory>
#include <vector>


class GPUFunc {
    public:
        explicit GPUFunc(MTL::Device* device);
        ~GPUFunc();

        void prepare_data(std::vector<unsigned int>& data);
        void execute();
        std::vector<unsigned int> get_data();

    protected:
        virtual MTL::Function* get_function(MTL::Library& library) = 0;
        virtual void encode_command(MTL::ComputeCommandEncoder *&encoder) = 0;
        MTL::Device* m_device;
        MTL::ComputePipelineState* m_pso;
        MTL::CommandQueue* m_commmand_queue;
        MTL::Buffer* m_data_buffer;

        int input_element_count;

    private:
        void init_shaders();
        static MTL::Library* s_library;
};


class GPUFuncDouble : public GPUFunc {
    public:
        explicit GPUFuncDouble(MTL::Device* device, NS::String* function_name, int elements_per_thread);
        ~GPUFuncDouble();

    protected:
        void encode_command(MTL::ComputeCommandEncoder *&encoder) override;
        virtual MTL::Function* get_function(MTL::Library& library) override;

    private:
        int m_elements_per_thread;
        NS::String *m_function_name;
};