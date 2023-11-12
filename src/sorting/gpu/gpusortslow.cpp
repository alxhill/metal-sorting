#include "gpusortslow.h"
#include "Metal/MTLTypes.hpp"

GPUSortSlow::GPUSortSlow(MTL::Device *device, bool even_pass) : GPUFunc(device), m_even_pass(even_pass) {}

GPUSortSlow::~GPUSortSlow() {}

void GPUSortSlow::encode_command(MTL::ComputeCommandEncoder *&encoder) {
    encoder->setComputePipelineState(m_pso);
    encoder->setBuffer(m_data_buffer, 0, 0);

    // odd pass ignores first and last elements
    int grid_width = m_even_pass ? input_element_count / 2 : input_element_count / 2 - 1;
    MTL::Size grid_size = MTL::Size(grid_width, 1, 1);

    int max_threads_per_tg = m_pso->maxTotalThreadsPerThreadgroup();
    int threads_per_group = max_threads_per_tg > grid_size.width ? grid_size.width : max_threads_per_tg;
    MTL::Size thread_group_size = MTL::Size(threads_per_group, 1, 1);

    encoder->dispatchThreads(grid_size, thread_group_size); 
}

MTL::Function *GPUSortSlow::get_function(MTL::Library &library) {
    if (m_even_pass) {
        return library.newFunction(MTLSTR("sort_slow_even"));
    }
    return library.newFunction(MTLSTR("sort_slow_odd"));
}
