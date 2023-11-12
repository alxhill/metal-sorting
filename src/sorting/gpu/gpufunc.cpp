#include "gpufunc.h"
#include <algorithm>
#include <memory>

MTL::Library *GPUFunc::s_library = nullptr;

GPUFunc::GPUFunc(MTL::Device* device) : m_device(device->retain()) {}

GPUFunc::~GPUFunc() {
    m_pso->release();
    m_commmand_queue->release();
    m_device->release();
}

void GPUFunc::execute() {
    MTL::CommandBuffer *cmd_buffer = m_commmand_queue->commandBuffer();
    MTL::ComputeCommandEncoder *encoder = cmd_buffer->computeCommandEncoder();

    encode_command(encoder);

    encoder->endEncoding();
    cmd_buffer->commit();
    
    // this blocks until the GPU is done
    cmd_buffer->waitUntilCompleted();
}

void GPUFunc::init_shaders() {
    NS::Error* error = nullptr;

    if (s_library == nullptr) {
        NS::Bundle* bundle = NS::Bundle::mainBundle();
        NS::URL* libUrl = bundle->URLForAuxiliaryExecutable(MTLSTR("shaders.metallib"));
        s_library = m_device->newLibrary(libUrl, &error);
    }

    if (!s_library) {
        __builtin_printf("%s", error->localizedDescription()->utf8String());
        assert(false);
    }

    MTL::Function* sort_func = get_function(*s_library);

    m_pso = m_device->newComputePipelineState(sort_func, &error);
    
    if (!m_pso) {
        __builtin_printf("%s", error->localizedDescription()->utf8String());
        assert(false);
    }

    m_commmand_queue = m_device->newCommandQueue();
}
