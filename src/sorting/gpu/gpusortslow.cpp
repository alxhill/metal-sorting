#include "gpusortslow.h"
#include "../time.h"
#include <MacTypes.h>
#include <vector>

#define NSASSERT(x, err) { if (!(x)) { __builtin_printf("Assertion failed: %s\n", err->localizedDescription()->utf8String()); assert(false); } }

MTL::Library* GPUSortSlow::s_library = nullptr;

GPUSortSlow::GPUSortSlow(MTL::Device* device) {
    m_device = device->retain();
}

GPUSortSlow::~GPUSortSlow() {
    m_kernel->release();
    m_commmand_queue->release();
    m_device->release();
}

void GPUSortSlow::init_with_data(std::vector<unsigned int>& data) {
    init_shaders();
    auto buffer_size = data.size() * sizeof(unsigned int);
    m_data_buffer = m_device->newBuffer(buffer_size, MTL::ResourceStorageModeShared);

    memcpy(m_data_buffer->contents(), data.data(), buffer_size);
    m_data_buffer->didModifyRange(NS::Range(0, buffer_size));

    m_internal_buffer = m_device->newBuffer(buffer_size, MTL::ResourceStorageModePrivate);

    input_element_count = data.size();
}

std::vector<unsigned int> GPUSortSlow::get_data() {
    // create a vector to hold the output data
    std::vector<unsigned int> output_data(input_element_count);

    // copy the data from the GPU to the CPU
    memcpy(output_data.data(), m_data_buffer->contents(),
           input_element_count * sizeof(unsigned int));
    return output_data;
}

void GPUSortSlow::execute() {
    MTL::CommandBuffer *cmd_buffer = m_commmand_queue->commandBuffer();
    
    MTL::ComputeCommandEncoder *encoder = cmd_buffer->computeCommandEncoder();

    log_with_time("Starting encoding pass");
    // each pass = two iterations, one even, one odd
    for (int i = 0; i < input_element_count / 2; i++) {
        encode_pass(encoder, i);
    }
    log_with_time("Finished encoding");

    encoder->endEncoding();
    cmd_buffer->commit();

    log_with_time("Committed command buffer");
    // this blocks until the GPU is done
    cmd_buffer->waitUntilCompleted();
    log_with_time("Execution completed");
}

MTL::Size tg_size(MTL::ComputePipelineState* kernel, int input_element_count) {
    int max_threads_per_tg = kernel->maxTotalThreadsPerThreadgroup();
    int threads_per_group = max_threads_per_tg > input_element_count ? input_element_count : max_threads_per_tg;
    return MTL::Size(threads_per_group, 1, 1);
}

void GPUSortSlow::encode_pass(MTL::ComputeCommandEncoder *&encoder, int pass_idx) {
    int grid_width = input_element_count / 2;
    auto kernel = m_kernel;
    // even pass
    if (pass_idx == 0) {
        kernel = m_copy_kernel;
        encoder->setComputePipelineState(m_copy_kernel);
        // first copy to the internal buffer
        encoder->setBuffer(m_data_buffer, 0, 0);
        encoder->setBuffer(m_internal_buffer, 0, 1);
    } else {
        // otherwise just mutate the internal buffer
        encoder->setComputePipelineState(m_kernel);
        encoder->setBuffer(m_internal_buffer, 0, 0);
    }

    MTL::Size even_grid_size = MTL::Size(grid_width, 1, 1);
    MTL::Size even_tgs = tg_size(kernel, even_grid_size.width);

    encoder->dispatchThreads(even_grid_size, even_tgs); 

    // odd pass
    if (pass_idx == input_element_count / 2 - 1) {
        kernel = m_copy_kernel;
        encoder->setComputePipelineState(m_copy_kernel);
        // copy back to the data buffer on the last pass
        encoder->setBuffer(m_internal_buffer, 0, 0);
        encoder->setBuffer(m_data_buffer, sizeof(unsigned int), 1);
    } else {
        // otherwise mutate the internal buffer
        encoder->setComputePipelineState(m_kernel);
        encoder->setBuffer(m_internal_buffer, sizeof(unsigned int), 0);
    }
    encoder->setComputePipelineState(m_kernel);
    // skip the first element
    encoder->setBuffer(m_data_buffer, sizeof(unsigned int), 0);

    // and ignore the last element
    MTL::Size odd_grid_size = MTL::Size(grid_width - 1, 1, 1);
    MTL::Size odd_tgs = tg_size(kernel, odd_grid_size.width);

    encoder->dispatchThreads(odd_grid_size, odd_tgs);
}

void GPUSortSlow::init_shaders() {
    NS::Error* error = nullptr;

    if (s_library == nullptr) {
        NS::Bundle* bundle = NS::Bundle::mainBundle();
        NS::URL* libUrl = bundle->URLForAuxiliaryExecutable(MTLSTR("shaders.metallib"));
        s_library = m_device->newLibrary(libUrl, &error);
    }

    NSASSERT(s_library, error);

    MTL::Function* single_pass = s_library->newFunction(MTLSTR("slow_sort"));
    MTL::Function* copy_pass = s_library->newFunction(MTLSTR("slow_sort_copy"));

    m_kernel = m_device->newComputePipelineState(single_pass, &error);
    m_copy_kernel = m_device->newComputePipelineState(copy_pass, &error);

    NSASSERT(m_kernel, error);
    NSASSERT(m_copy_kernel, error);

    single_pass->release();
    copy_pass->release();

    m_commmand_queue = m_device->newCommandQueue();
}
