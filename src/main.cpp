#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#include "header.h"
#include "macos/drawable.h"
#include "macos/viewdelegate.h"
#include "macos/appdelegate.h"
#include "render/renderer.h"

#include "sorting/generate.h"
#include "sorting/time.h"
#include "sorting/cpu/sort.h"

// view delegate
AlxMTKViewDelegate::AlxMTKViewDelegate( MTL::Device* pDevice )
: MTK::ViewDelegate()
, _pRenderer( new AlxRenderer( pDevice ) )
{
}

template <unsigned long p>
unsigned long constexpr LongPower(const unsigned long x)
{
    if constexpr (p == 0) return 1;
    if constexpr (p == 1) return x;

    int tmp = LongPower<p / 2>(x);
    if constexpr ((p % 2) == 0) { return tmp * tmp; }
    else { return x * tmp * tmp; }
}

int main(int argc, char* argv[]) {
    NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();

    //  AlxAppDelegate del;

    //  NS::Application* app = NS::Application::sharedApplication();
    //  app->setDelegate(&del);
    //  app->run();

    auto count = (unsigned long) std::pow(2, 25);
    std::cout << "Generating " << count << " random integers" << std::endl;
    std::vector<unsigned int> random_ints = generate_uints(count);
    std::cout << "Generated " << random_ints.size() << " random integers" << std::endl;

    std::vector<unsigned int> random_ints_2 = random_ints;

    time_func("std::sort", [&random_ints]() {
        sort_stdlib(random_ints);
    });

    assert(random_ints != random_ints_2);

    time_func("sort_radix", [&random_ints_2]() {
        sort_radix(random_ints_2);
    });

    assert(random_ints == random_ints_2);

    pool->release();

    return 0;
}
