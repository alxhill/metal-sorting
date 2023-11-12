#include "gpudouble.h"

GPUFuncDouble::GPUFuncDouble(MTL::Device* device, NS::String* function_name, int elements_per_thread) :
    GPUFunc(device), m_elements_per_thread(elements_per_thread), m_function_name(function_name) {}

GPUFuncDouble::~GPUFuncDouble() {
    m_function_name->release();
}

MTL::Function* GPUFuncDouble::get_function(MTL::Library& library) {
    return library.newFunction(m_function_name);
}

void GPUFuncDouble::encode_command(MTL::ComputeCommandEncoder *&encoder) {
    encoder->setComputePipelineState(m_pso);
    encoder->setBuffer(m_data_buffer, 0, 0);

    // the grid represents the total number of work items
    MTL::Size grid_size = MTL::Size(input_element_count/m_elements_per_thread, 1, 1);

    int max_threads_per_tg = m_pso->maxTotalThreadsPerThreadgroup();
    int threads_per_group = max_threads_per_tg > grid_size.width ? grid_size.width : max_threads_per_tg;
    MTL::Size thread_group_size = MTL::Size(threads_per_group, 1, 1);

    encoder->dispatchThreads(grid_size, thread_group_size);
}

void GPUFuncDouble::prepare_data(std::vector<unsigned int> &data) {
    init_shaders();

    auto buffer_size = data.size() * sizeof(unsigned int);
    m_data_buffer =
        m_device->newBuffer(buffer_size, MTL::ResourceStorageModeShared);

    memcpy(m_data_buffer->contents(), data.data(), buffer_size);
    m_data_buffer->didModifyRange(NS::Range(0, buffer_size));

    input_element_count = data.size();
}