#include "gpusortslow.h"
#include <vector>

#define NSASSERT(x, err) { if (!(x)) { __builtin_printf("Assertion failed: %s\n", err->localizedDescription()->utf8String()); assert(false); } }

GPUSortSlow::GPUSortSlow(MTL::Device* device) {
    m_device = device->retain();
}

GPUSortSlow::~GPUSortSlow() {}


void GPUSortSlow::init_with_data(std::vector<unsigned int>& data) {
    init_shaders();
    auto buffer_size = data.size() * sizeof(unsigned int);
    m_data_buffer = m_device->newBuffer(buffer_size, MTL::ResourceStorageModeShared);

    memcpy(m_data_buffer->contents(), data.data(), buffer_size);
    m_data_buffer->didModifyRange(NS::Range(0, buffer_size));

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

void GPUSortSlow::encode_command(MTL::ComputeCommandEncoder *&encoder) {
    encoder->setComputePipelineState(m_pso);
    encoder->setBuffer(m_data_buffer, 0, 0);

    // odd pass ignores first and last element pair
    int grid_width = m_even_pass ? input_element_count / 2 : input_element_count / 2 - 1;
    MTL::Size grid_size = MTL::Size(grid_width, 1, 1);

    int max_threads_per_tg = m_pso->maxTotalThreadsPerThreadgroup();
    int threads_per_group = max_threads_per_tg > grid_size.width ? grid_size.width : max_threads_per_tg;
    MTL::Size thread_group_size = MTL::Size(threads_per_group, 1, 1);

    encoder->dispatchThreads(grid_size, thread_group_size); 
}

void GPUSortSlow::init_shaders() {
    NS::Error* error = nullptr;

    if (s_library == nullptr) {
        NS::Bundle* bundle = NS::Bundle::mainBundle();
        NS::URL* libUrl = bundle->URLForAuxiliaryExecutable(MTLSTR("shaders.metallib"));
        s_library = m_device->newLibrary(libUrl, &error);
    }

    NSASSERT(s_library, error);

    MTL::Function* even_pass_func = s_library->newFunction(MTLSTR("slow_sort_even"));
    MTL::Function* odd_pass_func = s_library->newFunction(MTLSTR("slow_sort_odd"));

    m_even_kernel = m_device->newComputePipelineState(even_pass_func, &error);
    m_odd_kernel = m_device->newComputePipelineState(odd_pass_func, &error);

    NSASSERT(m_even_kernel, error);
    NSASSERT(m_odd_kernel, error);

    m_commmand_queue = m_device->newCommandQueue();
}
