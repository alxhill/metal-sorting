#pragma once

#include "../header.hpp"
#include "drawable.h"

class AlxMTKViewDelegate: public MTK::ViewDelegate
{
    public:
        AlxMTKViewDelegate( MTL::Device* pDevice );
        virtual ~AlxMTKViewDelegate() override;
        virtual void drawInMTKView( MTK::View* pView ) override;

    private:
        Drawable* _pRenderer;
};
