#pragma once

#include "../header.h"
#include "../macos/drawable.h"

class AlxRenderer : public Drawable
{
    public:
        AlxRenderer( MTL::Device* pDevice );
        ~AlxRenderer();
        void buildShaders();
        void buildBuffers();
        void buildFrameData();
        // overrides the abstract method in Drawable
        void draw(MTK::View* pView);

    private:
        MTL::Device* _pDevice;
        MTL::CommandQueue* _pCommandQueue;
        MTL::Library* _pShaderLibrary;
        MTL::RenderPipelineState* _pPSO;
        MTL::Buffer* _pArgBuffer;
        MTL::Buffer* _pPositions;
        MTL::Buffer* _pColors;
        MTL::Buffer* _pFrameData[3];

        float _angle;
        int _frame;
        dispatch_semaphore_t _semaphore;
};