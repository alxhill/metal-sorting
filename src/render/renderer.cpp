#include "renderer.h"

// renderer
struct FrameData
{
    [[maybe_unused]] float angle;
};

const int FramesInFlight = 3;

NS::String* str( const char* pStr )
{
    return NS::String::string( pStr, NS::StringEncoding::UTF8StringEncoding );
}

AlxRenderer::AlxRenderer( MTL::Device* pDevice )
: _pDevice(pDevice->retain())
, _angle(1.0f)
, _frame(0)
{
    _pCommandQueue = _pDevice->newCommandQueue();
    buildShaders();
    buildBuffers();
    buildFrameData();

    _semaphore = dispatch_semaphore_create(FramesInFlight);
}

AlxRenderer::~AlxRenderer()
{
    _pShaderLibrary->release();
    _pArgBuffer->release();
    _pPositions->release();
    _pColors->release();

    for ( int i = 0; i <  FramesInFlight; ++i )
    {
        _pFrameData[i]->release();
    }

    _pPSO->release();

    _pCommandQueue->release();
    _pDevice->release();
}

void AlxRenderer::buildShaders()
{
    std::cout << "shaders start" << std::endl;
    NS::Error* pError = nullptr;

    NS::Bundle* pBundle = NS::Bundle::mainBundle();
    NS::URL* libUrl = pBundle->URLForAuxiliaryExecutable(str("shaders.metallib"));
    MTL::Library* pLibrary = _pDevice->newLibrary(libUrl, &pError);
    
    if ( !pLibrary )
    {
        __builtin_printf( "%s", pError->localizedDescription()->utf8String() );
        assert( false );
    }

    MTL::Function* pVertexFunc = pLibrary->newFunction(str("vertexMain"));
    MTL::Function* pFragFunc = pLibrary->newFunction(str("fragmentMain"));

    MTL::RenderPipelineDescriptor* pDesc = MTL::RenderPipelineDescriptor::alloc()->init();
    pDesc->setVertexFunction(pVertexFunc);
    pDesc->setFragmentFunction(pFragFunc);
    pDesc->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);

    _pPSO = _pDevice->newRenderPipelineState(pDesc, &pError);
    if ( !_pPSO )
    {
        __builtin_printf( "%s", pError->localizedDescription()->utf8String() );
        assert( false );
    }

    pVertexFunc->release();
    pFragFunc->release();
    pDesc->release();
    _pShaderLibrary = pLibrary;
    std::cout << "shaders done" << std::endl;
}

void AlxRenderer::buildBuffers()
{
    std::cout << "buildBuffers start" << std::endl;
    const size_t NumVert = 3;

    simd::float3 positions[NumVert] = 
    {
        { -0.8f,  0.8f, 0.0f },
        {  0.0f, -0.8f, 0.0f },
        { +0.8f,  0.8f, 0.0f }
    };

    simd::float3 colors[NumVert] =
    {
        {  1.0, 0.3f, 0.2f },
        {  0.8f, 1.0, 0.0f },
        {  0.8f, 0.0f, 1.0 }
    };

    const size_t posDataSize = NumVert * sizeof(simd::float3);
    const size_t colDataSize = NumVert * sizeof(simd::float3);

    MTL::Buffer* pPosBuffer = _pDevice->newBuffer(posDataSize, MTL::ResourceStorageModeManaged);
    MTL::Buffer* pColBuffer = _pDevice->newBuffer(colDataSize, MTL::ResourceStorageModeManaged);

    _pPositions = pPosBuffer;
    _pColors = pColBuffer;

    memcpy(pPosBuffer->contents(), positions, posDataSize);
    memcpy(pColBuffer->contents(), colors, colDataSize);

    _pPositions->didModifyRange(NS::Range::Make(0, _pPositions->length()));
    _pColors->didModifyRange(NS::Range::Make(0, _pColors->length()));

    assert( _pShaderLibrary );

    MTL::Function* pVertexFunc = _pShaderLibrary->newFunction(str("vertexMain"));
    MTL::ArgumentEncoder* pArgEncoder = pVertexFunc->newArgumentEncoder(0);

    MTL::Buffer* pArgBuffer = _pDevice->newBuffer(pArgEncoder->encodedLength(), MTL::ResourceStorageModeManaged);
    _pArgBuffer = pArgBuffer;

    pArgEncoder->setArgumentBuffer(_pArgBuffer, 0);

    // crashes here :(
    pArgEncoder->setBuffer(_pPositions, 0, 0);
    pArgEncoder->setBuffer(_pColors, 0, 1);

    _pArgBuffer->didModifyRange(NS::Range::Make(0, _pArgBuffer->length()));

    pVertexFunc->release();
    pArgEncoder->release();
}


void AlxRenderer::buildFrameData()
{
    for ( int i = 0; i < FramesInFlight; ++i )
    {
        _pFrameData[i] = _pDevice->newBuffer(sizeof(FrameData), MTL::ResourceStorageModeManaged);
    }
    std::cout << "buildFrameData done" << std::endl;
}

void AlxRenderer::draw(MTK::View* pView)
{
    std::cout << "draw start" << std::endl;
    NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();

    _frame = (_frame + 1) % FramesInFlight;
    MTL::Buffer* pFrameDataBuffer = _pFrameData[_frame];

    MTL::CommandBuffer* pCmd = _pCommandQueue->commandBuffer();
    dispatch_semaphore_wait( _semaphore, DISPATCH_TIME_FOREVER );
    AlxRenderer* pRenderer = this;
    pCmd->addCompletedHandler( ^void( MTL::CommandBuffer* pCmd ){
        dispatch_semaphore_signal( pRenderer->_semaphore );
    });

    reinterpret_cast< FrameData * >( pFrameDataBuffer->contents() )->angle = (_angle += 0.01f);
    pFrameDataBuffer->didModifyRange( NS::Range::Make( 0, sizeof( FrameData ) ) );

    MTL::RenderPassDescriptor* pRpd = pView->currentRenderPassDescriptor();
    MTL::RenderCommandEncoder* pEnc = pCmd->renderCommandEncoder(pRpd);

    std::cout << "draw mid" << std::endl;

    pEnc->setRenderPipelineState(_pPSO);
    pEnc->setVertexBuffer(_pArgBuffer, 0, 0);
    pEnc->useResource(_pPositions, MTL::ResourceUsageRead);
    pEnc->useResource(_pColors, MTL::ResourceUsageRead);
    
    pEnc->setVertexBuffer(pFrameDataBuffer, 0, 1);
    pEnc->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));

    pEnc->endEncoding();
    pCmd->presentDrawable(pView->currentDrawable());
    pCmd->commit();

    pool->release();

    std::cout << "draw end" << std::endl;
}
