#pragma once

#include "gpufunc.h"

class GPUFuncDouble : public GPUFunc {
    public:
        explicit GPUFuncDouble(MTL::Device* device, NS::String* function_name, int elements_per_thread);
        ~GPUFuncDouble();

        void prepare_data(std::vector<unsigned int>& data);

    protected:
        void encode_command(MTL::ComputeCommandEncoder *&encoder) override;
        MTL::Function* get_function(MTL::Library& library) override;

    private:
        int m_elements_per_thread;
        NS::String *m_function_name;
};