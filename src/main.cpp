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

int main(int argc, char* argv[]) {
    NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();

    // AlxAppDelegate del;

    // NS::Application* app = NS::Application::sharedApplication();
    // app->setDelegate(&del);
    // app->run();

    auto random_ints = generate_values<int>(1000000);

    time_func("stdlib sort", [&random_ints]() {
        sort_stdlib(random_ints);
    });

    pool->release();

    return 0;
}
