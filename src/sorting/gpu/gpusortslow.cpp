#include "gpusortslow.h"
#include "Metal/MTLTypes.hpp"
#include <vector>

GPUSortSlow::GPUSortSlow(MTL::Device* device, bool even_pass) : GPUFunc(device), m_even_pass(even_pass) {}

GPUSortSlow::~GPUSortSlow() {}

void GPUSortSlow::put_buffer(MTL::Buffer* buffer, int element_count) {
    init_shaders();
    if (m_data_buffer != nullptr) {
        std::cout << "GPUSortSlow::put_buffer: m_data_buffer is not null" << std::endl;
        assert(false);
    }
    m_data_buffer = buffer;
    input_element_count = element_count;
}

MTL::Buffer* GPUSortSlow::put_data(std::vector<unsigned int>& data) {
    init_shaders();
    auto buffer_size = data.size() * sizeof(unsigned int);
    m_data_buffer = m_device->newBuffer(buffer_size, MTL::ResourceStorageModeShared);

    memcpy(m_data_buffer->contents(), data.data(), buffer_size);
    m_data_buffer->didModifyRange(NS::Range(0, buffer_size));

    input_element_count = data.size();

    return m_data_buffer;
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

MTL::Function *GPUSortSlow::get_function(MTL::Library &library) {
    if (m_even_pass) {
        return library.newFunction(MTLSTR("slow_sort_even"));
    }
    return library.newFunction(MTLSTR("slow_sort_odd"));
}