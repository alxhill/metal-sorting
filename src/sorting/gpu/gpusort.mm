#include "gpusort.hpp"
#include "Metal/MTLRenderPipeline.hpp"
#include <cstddef>
#include <cstring>


NS::String* str( const char* pStr )
{
    return NS::String::string( pStr, NS::StringEncoding::UTF8StringEncoding );
}

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
    // todo
}

void GPUSort::prepare_data(std::vector<unsigned int> &data) {
    auto buffer_size = data.size() * sizeof(unsigned int);
    m_input_buffer =
        m_device->newBuffer(buffer_size, MTL::ResourceStorageModeShared);
    m_output_buffer = m_device->newBuffer(buffer_size, MTL::ResourceStorageModeShared);

    memcpy(m_input_buffer->contents(), data.data(), buffer_size);
    m_input_buffer->didModifyRange(NS::Range(0, buffer_size));
}

void GPUSort::init_shaders() {
    NS::Error* error = nullptr;

    NS::Bundle* bundle = NS::Bundle::mainBundle();
    NS::URL* libUrl = bundle->URLForAuxiliaryExecutable(str("shaders.metallib"));
    MTL::Library* library = m_device->newLibrary(libUrl, &error);

    if (!library) {
        __builtin_printf("%s", error->localizedDescription()->utf8String());
        assert(false);
    }

    MTL::Function* sort_func = library->newFunction(str("double_value_sort"));

    MTL::ComputePipelineState* m_pso = m_device->newComputePipelineState(sort_func, &error);
    
    if (!m_pso) {
        __builtin_printf("%s", error->localizedDescription()->utf8String());
        assert(false);
    }

    m_commmand_queue = m_device->newCommandQueue();
}