#include "viewdelegate.h"

AlxMTKViewDelegate::~AlxMTKViewDelegate()
{
    delete _pRenderer;
}

void AlxMTKViewDelegate::drawInMTKView( MTK::View* pView )
{
    _pRenderer->draw( pView );
}

