#include "gpusort.hpp"


GPUSort::GPUSort(MTL::Device* device) : m_device(device->retain()) {
    init_shaders();
}

GPUSort::~GPUSort() {
    m_shader_library->release();
    m_input_buffer->release();
    m_output_buffer->release();
    m_pso->release();
    m_commmand_queue->release();
    m_device->release();
}

void GPUSort::compute_sort() {
    MTL::CommandBuffer *cmd_buffer = m_commmand_queue->commandBuffer();
    MTL::ComputeCommandEncoder *encoder = cmd_buffer->computeCommandEncoder();

    encode_command(encoder);

    encoder->endEncoding();
    cmd_buffer->commit();

    // this blocks until the GPU is done
    cmd_buffer->waitUntilCompleted();
}

std::vector<unsigned int> GPUSort::get_data() {
    // create a vector to hold the output data
    std::vector<unsigned int> output_data(input_element_count);

    // copy the data from the GPU to the CPU
    memcpy(output_data.data(), m_output_buffer->contents(),
           input_element_count * sizeof(unsigned int));
    return output_data;
}

void GPUSort::encode_command(MTL::ComputeCommandEncoder *&encoder) {
    encoder->setComputePipelineState(m_pso);
    encoder->setBuffer(m_input_buffer, 0, 0);
    encoder->setBuffer(m_output_buffer, 0, 1);

    MTL::Size grid_size = MTL::Size(input_element_count, 1, 1);

    int thread_group_count = m_pso->maxTotalThreadsPerThreadgroup();
    if (thread_group_count > input_element_count) {
      thread_group_count = input_element_count;
    }
    MTL::Size thread_group_size = MTL::Size(thread_group_count, 1, 1);

    encoder->dispatchThreads(grid_size, thread_group_size);
}

void GPUSort::prepare_data(std::vector<unsigned int> &data) {
    auto buffer_size = data.size() * sizeof(unsigned int);
    m_input_buffer =
        m_device->newBuffer(buffer_size, MTL::ResourceStorageModeShared);
    m_output_buffer = m_device->newBuffer(buffer_size, MTL::ResourceStorageModeShared);

    memcpy(m_input_buffer->contents(), data.data(), buffer_size);
    m_input_buffer->didModifyRange(NS::Range(0, buffer_size));

    input_element_count = data.size();
}

void GPUSort::init_shaders() {
    NS::Error* error = nullptr;

    NS::Bundle* bundle = NS::Bundle::mainBundle();
    NS::URL* libUrl = bundle->URLForAuxiliaryExecutable(MTLSTR("shaders.metallib"));
    MTL::Library* library = m_device->newLibrary(libUrl, &error);

    if (!library) {
        __builtin_printf("%s", error->localizedDescription()->utf8String());
        assert(false);
    }

    MTL::Function* sort_func = library->newFunction(MTLSTR("double_value"));

    MTL::ComputePipelineState* m_pso = m_device->newComputePipelineState(sort_func, &error);
    
    if (!m_pso) {
        __builtin_printf("%s", error->localizedDescription()->utf8String());
        assert(false);
    }

    m_commmand_queue = m_device->newCommandQueue();
}