#include "gpusort.hpp"
#include "Foundation/NSString.hpp"
#include "Metal/MTLLibrary.hpp"
#include "Metal/MTLResource.hpp"
#include <algorithm>
#include <memory>

MTL::Library *GPUFunc::s_library = nullptr;

GPUFunc::GPUFunc(MTL::Device* device) : m_device(device->retain()) {}

GPUFunc::~GPUFunc() {
    m_data_buffer->release();
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

std::vector<unsigned int> GPUFunc::get_data() {
    // create a vector to hold the output data
    std::vector<unsigned int> output_data(input_element_count);

    // copy the data from the GPU to the CPU
    memcpy(output_data.data(), m_data_buffer->contents(),
           input_element_count * sizeof(unsigned int));
    return output_data;
}


void GPUFunc::prepare_data(std::vector<unsigned int> &data) {
    init_shaders();

    auto buffer_size = data.size() * sizeof(unsigned int);
    m_data_buffer =
        m_device->newBuffer(buffer_size, MTL::ResourceStorageModeShared);
    // m_output_buffer = m_device->newBuffer(buffer_size, MTL::ResourceStorageModeShared);

    memcpy(m_data_buffer->contents(), data.data(), buffer_size);
    m_data_buffer->didModifyRange(NS::Range(0, buffer_size));

    input_element_count = data.size();
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

// value doubler impl
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
    // encoder->setBuffer(m_output_buffer, 0, 1);

    // std::cout << "Encoded buffers" << std::endl;

    // the grid represents the total number of work items
    MTL::Size grid_size = MTL::Size(input_element_count/m_elements_per_thread, 1, 1);

    // std::cout << "created grid with size " << grid_size.width << std::endl;

    int max_threads_per_tg = m_pso->maxTotalThreadsPerThreadgroup();

    int threads_per_group = max_threads_per_tg > grid_size.width ? grid_size.width : max_threads_per_tg;

    MTL::Size thread_group_size = MTL::Size(threads_per_group, 1, 1);
    // std::cout << "created threadgroup with size " << threads_per_group << std::endl;

    // std::cout << "Dispatching threads" << std::endl;
    encoder->dispatchThreads(grid_size, thread_group_size);
}