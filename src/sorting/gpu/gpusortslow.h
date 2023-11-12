#pragma once

#include "gpufunc.h"

class GPUSortSlow : public GPUFunc {
    public:
        explicit GPUSortSlow(MTL::Device* device);
        ~GPUSortSlow();

    protected:
        void encode_command(MTL::ComputeCommandEncoder *&encoder) override;
        MTL::Function* get_function(MTL::Library& library) override;
};

