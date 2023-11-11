#include "gpusort.hpp"
#include "Foundation/NSString.hpp"
#include "Metal/MTLResource.hpp"
#include <algorithm>

NS::String *FUNC_NAME = MTLSTR("double_value_pair");

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

    std::cout << "Encoding command" << std::endl;
    encode_command(encoder);

    std::cout << "Committing" << std::endl;
    encoder->endEncoding();
    cmd_buffer->commit();

    // this blocks until the GPU is done
    std::cout << "Waiting for completion" << std::endl;
    cmd_buffer->waitUntilCompleted();
    std::cout << "Execution finished" << std::endl;
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

    std::cout << "Encoded buffers" << std::endl;

    // the grid represents the total number of work items
    MTL::Size grid_size = MTL::Size(input_element_count/2, 1, 1);

    std::cout << "created grid with size " << grid_size.width << std::endl;

    int max_threads_per_tg = m_pso->maxTotalThreadsPerThreadgroup();

    int threads_per_group = max_threads_per_tg > grid_size.width ? grid_size.width : max_threads_per_tg;

    MTL::Size thread_group_size = MTL::Size(threads_per_group, 1, 1);
    std::cout << "created threadgroup with size " << threads_per_group << std::endl;

    std::cout << "Dispatching threads" << std::endl;
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

    MTL::Function* sort_func = library->newFunction(FUNC_NAME);

    m_pso = m_device->newComputePipelineState(sort_func, &error);
    
    if (!m_pso) {
        __builtin_printf("%s", error->localizedDescription()->utf8String());
        assert(false);
    }

    m_commmand_queue = m_device->newCommandQueue();
}