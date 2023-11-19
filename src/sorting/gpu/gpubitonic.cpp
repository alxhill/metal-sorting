#include "gpubitonic.h"
#include "../time.h"
#include "Metal/MTLComputeCommandEncoder.hpp"

#define NSASSERT(x, err) { if (!(x)) { __builtin_printf("Assertion failed: %s\n", err->localizedDescription()->utf8String()); assert(false); } }

GPUSortBitonic::GPUSortBitonic(MTL::Device* device) {
    m_device = device->retain();
}

GPUSortBitonic::~GPUSortBitonic() {
    m_swap_asc_kernel->release();
    m_swap_dec_kernel->release();
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
    MTL::CommandBuffer* cmd_buffer = m_commmand_queue->commandBuffer();
    MTL::ComputeCommandEncoder* encoder = cmd_buffer->computeCommandEncoder();

    log_with_time("Starting bitonic encoding");

    for (int base_size = 2; base_size < input_element_count; base_size *= 2) {
        for (int i = 0; i < input_element_count; i += base_size*2) {
            encode_merge(encoder, i, i + base_size, true);
            encode_merge(encoder, i + base_size, i + base_size * 2, false);
        }
    }
    encode_merge(encoder, 0, input_element_count, true);
    log_with_time("Finished bitonic encoding");

    encoder->endEncoding();
    cmd_buffer->commit();

    cmd_buffer->waitUntilCompleted();

    log_with_time("Bitonic execution completed");
}

void GPUSortBitonic::encode_merge(MTL::ComputeCommandEncoder*& encoder, int start, int end, bool ascending) {

}

void GPUSortBitonic::init_shaders() {
    NS::Error* error = nullptr;

    NS::Bundle* bundle = NS::Bundle::mainBundle();
    NS::URL* libUrl = bundle->URLForAuxiliaryExecutable(MTLSTR("shaders.metallib"));
    MTL::Library* library = m_device->newLibrary(libUrl, &error);

    NSASSERT(library, error);

    MTL::Function* swap_asc = library->newFunction(MTLSTR("bitonic_swap_asc"));
    MTL::Function* swap_dec = library->newFunction(MTLSTR("bitonic_swap_dec"));

    m_swap_asc_kernel = m_device->newComputePipelineState(swap_asc, &error);
    m_swap_dec_kernel = m_device->newComputePipelineState(swap_dec, &error);

    NSASSERT(m_swap_asc_kernel, error);
    NSASSERT(m_swap_dec_kernel, error);

    swap_asc->release();
    swap_dec->release();

    m_commmand_queue = m_device->newCommandQueue();
}
