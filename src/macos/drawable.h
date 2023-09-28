#pragma once

#include "../header.h"

// simple abstract class for the renderer so I
// don't have to define it before the app delegate
class Drawable {
    public:
        virtual void draw(MTK::View* pView) = 0;
        virtual ~Drawable() = default;
};
