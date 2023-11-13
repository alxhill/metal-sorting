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

    GPUSortSlow gpu_sort_slow(device);

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

    gpu_sort_slow.init_with_data(random_ints_3);

    time_func("slow_sort_gpu", [&random_ints_3, &gpu_sort_slow]() {
        reset_timer();
        gpu_sort_slow.execute();
        random_ints_3 = gpu_sort_slow.get_data();
    });

    assert(random_ints == random_ints_3);

    pool->release();

    return 0;
}