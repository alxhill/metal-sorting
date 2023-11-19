#include "gpubitonic.h"

MTL::Library* GPUSortBitonic::s_library = nullptr;

#define NSASSERT(x, err) { if (!(x)) { __builtin_printf("Assertion failed: %s\n", err->localizedDescription()->utf8String()); assert(false); } }

GPUSortBitonic::GPUSortBitonic(MTL::Device* device) {
    m_device = device->retain();
}

GPUSortBitonic::~GPUSortBitonic() {
    m_kernel->release();
    m_commmand_queue->release();
    m_device->release();
}

void GPUSortBitonic::init_with_data(std::vector<unsigned int>& data) {
    init_shaders();
    auto buffer_size = data.size() * sizeof(unsigned int);
    m_data_buffer = m_device->newBuffer(buffer_size, MTL::ResourceStorageModeShared);
    memcpy(m_data_buffer->contents(), data.data(), buffer_size);
    m_data_buffer->didModifyRange(NS::Range(0, buffer_size));

    input_element_count = data.size();
}

std::vector<unsigned int> GPUSortBitonic::get_data() {
    // create a vector to hold the output data
    std::vector<unsigned int> output_data(input_element_count);

    // copy the data from the GPU to the CPU
    memcpy(output_data.data(), m_data_buffer->contents(),
           input_element_count * sizeof(unsigned int));
    return output_data;
}

void GPUSortBitonic::execute() {

}

void GPUSortBitonic::init_shaders() {
    NS::Error* error = nullptr;

    if (s_library == nullptr) {
        NS::Bundle* bundle = NS::Bundle::mainBundle();
        NS::URL* libUrl = bundle->URLForAuxiliaryExecutable(MTLSTR("shaders.metallib"));
        s_library = m_device->newLibrary(libUrl, &error);
    }

    NSASSERT(s_library, error);

    MTL::Function* single_pass = s_library->newFunction(MTLSTR("slow_sort"));

    m_kernel = m_device->newComputePipelineState(single_pass, &error);

    NSASSERT(m_kernel, error);

    // even_pass_func->release();
    single_pass->release();

    m_commmand_queue = m_device->newCommandQueue();
}
