#include "gpusortslow.h"

GPUSortSlow::GPUSortSlow(MTL::Device *device) : GPUFunc(device) {

}

GPUSortSlow::~GPUSortSlow() {

}

void GPUSortSlow::encode_command(MTL::ComputeCommandEncoder *&encoder) {

}

MTL::Function *GPUSortSlow::get_function(MTL::Library &library) {
    return NULL;
}
