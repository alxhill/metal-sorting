#pragma once

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>

#include <simd/simd.h>

#include <iostream>
#include <cassert>

NS::String* str( const char* pStr )
{
    return NS::String::string( pStr, NS::StringEncoding::UTF8StringEncoding );
}
