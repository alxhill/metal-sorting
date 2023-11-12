#pragma once

#include "gpufunc.h"

class GPUSortSlow : public GPUFunc {
    public:
        explicit GPUSortSlow(MTL::Device* device, bool even_pass);
        ~GPUSortSlow();

        static MTL::Buffer* create_data_buffer(MTL::Device* device, std::vector<unsigned int>& data);

    protected:
        void encode_command(MTL::ComputeCommandEncoder *&encoder) override;
        MTL::Function* get_function(MTL::Library& library) override;
    private: 
        bool m_even_pass;
};

