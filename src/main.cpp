#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#include "header.hpp"
#include "macos/drawable.h"
#include "macos/viewdelegate.h"
#include "macos/appdelegate.h"
#include "render/renderer.h"

#include "sorting/generate.h"
#include "sorting/time.h"
#include "sorting/cpu/sort.h"
#include "sorting/gpu/gpu.hpp"

// view delegate
AlxMTKViewDelegate::AlxMTKViewDelegate( MTL::Device* pDevice )
: MTK::ViewDelegate()
, _pRenderer( new AlxRenderer( pDevice ) )
{
}

int main(int argc, char* argv[]) {
    NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();

    //  AlxAppDelegate del;
    //  NS::Application* app = NS::Application::sharedApplication();
    //  app->setDelegate(&del);
    //  app->run();

    MTL::Device *device = MTL::CreateSystemDefaultDevice();

    GPUFuncDouble gpu_double(device, MTLSTR("double_value"), 1);
    GPUFuncDouble gpu_double_2(device, MTLSTR("double_value_pair"), 2);
    GPUFuncDouble gpu_double_64(device, MTLSTR("double_value_64"), 64);
    GPUFuncDouble gpu_double_128(device, MTLSTR("double_value_128"), 128);

    auto count = (unsigned long) std::pow(2, 20);
    std::cout << "Generating " << count << " random integers" << std::endl;
    std::vector<unsigned int> random_ints = generate_uints(count);
    std::cout << "Generated " << random_ints.size() << " random integers" << std::endl;

    std::vector<unsigned int> random_ints_2 = random_ints;
    std::vector<unsigned int> random_ints_3 = random_ints;

    time_func("std::sort", [&random_ints]() {
        random_ints = sort_stdlib(random_ints);
    });

    assert(random_ints != random_ints_2);

    time_func("sort_radix", [&random_ints_2]() {
        random_ints_2 = sort_radix(random_ints_2);
    });

    assert(random_ints == random_ints_2);

    std::vector<unsigned int> doubled_ints(random_ints_3.size());

    time_func("double_cpu", [&random_ints_3, &doubled_ints]() {
        std::transform(random_ints_3.begin(), random_ints_3.end(), doubled_ints.begin(), [](unsigned int i) { return i * 2; });
    });

    gpu_double.prepare_data(random_ints_3);
    gpu_double_2.prepare_data(random_ints_3);
    gpu_double_64.prepare_data(random_ints_3);
    gpu_double_128.prepare_data(random_ints_3);

    time_func("double_gpu_1", [&gpu_double]() {
        gpu_double.execute();
    });
    time_func("double_gpu_2", [&gpu_double_2]() {
        gpu_double_2.execute();
    });
    time_func("double_gpu_64", [&gpu_double_64]() {
        gpu_double_64.execute();
    });
    time_func("double_gpu_128", [&gpu_double_128]() {
        gpu_double_128.execute();
    });

    random_ints_3 = gpu_double.get_data();
    assert(doubled_ints == random_ints_3);
    random_ints_3 = gpu_double_2.get_data();
    assert(doubled_ints == random_ints_3);
    random_ints_3 = gpu_double_64.get_data();
    assert(doubled_ints == random_ints_3);
    random_ints_3 = gpu_double_128.get_data();
    assert(doubled_ints == random_ints_3);

    pool->release();

    return 0;
}