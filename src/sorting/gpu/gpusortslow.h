#pragma once

#include "Metal/MTLBuffer.hpp"
#include "gpufunc.h"

class GPUSortSlow : public GPUFunc {
    public:
        explicit GPUSortSlow(MTL::Device* device, bool even_pass);
        ~GPUSortSlow();

        // only one of these should be called
        MTL::Buffer* put_data(std::vector<unsigned int>& data);
        void put_buffer(MTL::Buffer* buffer, int element_count);

        std::vector<unsigned int> get_data() override;

    protected:
        void encode_command(MTL::ComputeCommandEncoder *&encoder) override;
        MTL::Function* get_function(MTL::Library& library) override;
    private: 
        bool m_even_pass;
        MTL::Buffer* m_data_buffer;
};
