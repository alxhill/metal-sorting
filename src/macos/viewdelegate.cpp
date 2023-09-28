#include "viewdelegate.h"

AlxMTKViewDelegate::~AlxMTKViewDelegate()
{
    delete _pRenderer;
}

void AlxMTKViewDelegate::drawInMTKView( MTK::View* pView )
{
    _pRenderer->draw( pView );
}

NS::String* str( const char* pStr )
{
    return NS::String::string( pStr, NS::StringEncoding::UTF8StringEncoding );
}