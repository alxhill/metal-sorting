#include <cassert>
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
#include "sorting/gpu/gpu.h"

// view delegate
AlxMTKViewDelegate::AlxMTKViewDelegate( MTL::Device* pDevice )
: MTK::ViewDelegate()
, _pRenderer( new AlxRenderer( pDevice ) )
{
}

void print_vector(std::vector<unsigned int>& values) {
    for (auto value : values) {
        std::cout << value << ",";
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();

    //  AlxAppDelegate del;
    //  NS::Application* app = NS::Application::sharedApplication();
    //  app->setDelegate(&del);
    //  app->run();

    MTL::Device *device = MTL::CreateSystemDefaultDevice();

    GPUFuncDouble gpu_double_64(device, MTLSTR("double_value_64"), 64);

    GPUSortSlow gpu_sort_even(device, true);
    GPUSortSlow gpu_sort_odd(device, false);

    auto count = (unsigned long) std::pow(2, 5);
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

    auto buffer = gpu_sort_even.put_data(random_ints_3);
    gpu_sort_odd.put_buffer(buffer, random_ints_3.size());

    time_func("slow_sort_gpu", [&random_ints_3, &gpu_sort_even, &gpu_sort_odd, count]() {
        for (int i = 0; i < count; i++) {
            if (i % 2 == 0) {
                gpu_sort_even.execute();
            } else {
                gpu_sort_odd.execute();
            }
        }
        random_ints_3 = gpu_sort_even.get_data();
    });

    assert(random_ints == random_ints_3);

    // std::vector<unsigned int> doubled_ints(random_ints_3.size());

    // time_func("double_cpu", [&random_ints_3, &doubled_ints]() {
    //     std::transform(random_ints_3.begin(), random_ints_3.end(), doubled_ints.begin(), [](unsigned int i) { return i * 2; });
    // });

    // gpu_double_64.prepare_data(random_ints_3);

    // time_func("double_gpu_64", [&gpu_double_64]() {
    //     gpu_double_64.execute();
    // });

    // random_ints_3 = gpu_double_64.get_data();
    // assert(doubled_ints == random_ints_3);

    pool->release();

    return 0;
}