#pragma once

#include "gpufunc.hpp"

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