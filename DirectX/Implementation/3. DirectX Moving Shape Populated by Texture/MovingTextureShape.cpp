#include "MovingTextureShape.h"

MovingTextureShape::MovingTextureShape(UINT width, UINT height, std::wstring name) :
    DXSample(width, height, name),
    mFrameIndex(0),
    mViewPort(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)),
    mScissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height)),
    mRTVDescriptorSize(0)
{

}

void MovingTextureShape::OnInit()
{
    PipelineInit();
    AssetInit();
}

void MovingTextureShape::PipelineInit()
{
    UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    {
        ComPtr<ID3D12Debug> debugController;
        if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }   
#endif    

    ComPtr<IDXGIFactory4> factory;
    ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&factory)));

    if (m_useWarpDevice)
    {
        ComPtr<IDXGIAdapter> warpAdapter;
        ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

        ThrowIfFailed(D3D12CreateDevice(
            warpAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&mDevice)
        ));
    }
    else
    {
        ComPtr<IDXGIAdapter1> hardwareAdapter;
        GetHardwareAdapter(factory.Get(), &hardwareAdapter);

        ThrowIfFailed(D3D12CreateDevice(
            hardwareAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&mDevice)
        ));
    }

    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    ThrowIfFailed(mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = frameCount;
    swapChainDesc.BufferDesc.Width = m_width;
    swapChainDesc.BufferDesc.Height = m_height;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.OutputWindow = Win32Application::GetHwnd();
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Windowed = TRUE;

    ComPtr<IDXGISwapChain> swapChain;

    ThrowIfFailed(factory->CreateSwapChain(
        mCommandQueue.Get(),   
        &swapChainDesc,
        &swapChain
    ));

    ThrowIfFailed(swapChain.As(&mSwapChain));

    ThrowIfFailed(factory->MakeWindowAssociation(Win32Application::GetHwnd(), DXGI_MWA_NO_ALT_ENTER));

    mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();

    {
        D3D12_DESCRIPTOR_HEAP_DESC RTVHeapDesc = {};
        RTVHeapDesc.NumDescriptors = frameCount;
        RTVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        RTVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        ThrowIfFailed(mDevice->CreateDescriptorHeap(&RTVHeapDesc, IID_PPV_ARGS(&mRTVHeap)));

        mRTVDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        D3D12_DESCRIPTOR_HEAP_DESC CBVHeapDesc = {};
        CBVHeapDesc.NumDescriptors = 2;
        CBVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        CBVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        ThrowIfFailed(mDevice->CreateDescriptorHeap(&CBVHeapDesc, IID_PPV_ARGS(&mCBVSRVHeap)));
        
        mCBVSRVDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE RTVHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart());

        for (UINT i = 0; i < frameCount; i++)
        {
            ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mRenderTargets[i])));
            mDevice->CreateRenderTargetView(mRenderTargets[i].Get(), nullptr, RTVHandle);
            RTVHandle.Offset(1, mRTVDescriptorSize);
            ThrowIfFailed(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocators[i])));
        }
        
        ThrowIfFailed(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE, IID_PPV_ARGS(&mBundleAllocator)));
    }
}

void MovingTextureShape::AssetInit()
{
    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
        ThrowIfFailed(mDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData)));

        CD3DX12_DESCRIPTOR_RANGE1 ranges[2];
        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

        CD3DX12_ROOT_PARAMETER1 rootParameters[2];
        rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);
        rootParameters[1].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);

        D3D12_STATIC_SAMPLER_DESC sampler = {};
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.MipLODBias = 0;
        sampler.MaxAnisotropy = 0;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        sampler.MinLOD = 0.0f;
        sampler.MaxLOD = D3D12_FLOAT32_MAX;
        sampler.ShaderRegister = 0;
        sampler.RegisterSpace = 0;
        sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, rootSignatureFlags);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
        ThrowIfFailed(mDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&mRootSignature)));
    }

    {
        ComPtr<ID3DBlob> vertexShader;
        ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
        UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        UINT compileFlags = 0;
#endif

        {
            std::wstring shaderFilePath = L"./shaders.hlsl";
            ThrowIfFailed(D3DCompileFromFile(shaderFilePath.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
            ThrowIfFailed(D3DCompileFromFile(shaderFilePath.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));
        }

        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
        };

        D3D12_GRAPHICS_PIPELINE_STATE_DESC PSODesc = {};
        PSODesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        PSODesc.pRootSignature = mRootSignature.Get();
        PSODesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
        PSODesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
        PSODesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        PSODesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        PSODesc.DepthStencilState.DepthEnable = FALSE;
        PSODesc.DepthStencilState.StencilEnable = FALSE;
        PSODesc.SampleMask = UINT_MAX;
        PSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        PSODesc.NumRenderTargets = 1;
        PSODesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        PSODesc.SampleDesc.Count = 1;

        ThrowIfFailed(mDevice->CreateGraphicsPipelineState(&PSODesc, IID_PPV_ARGS(&mPipelineState)));
    }

    ThrowIfFailed(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocators[mFrameIndex].Get(), mPipelineState.Get(), IID_PPV_ARGS(&mCommandList)));

    ComPtr<ID3D12Resource> vertexBufferUpload;
    {
        Vertex shapeVertices[] =
        {
            { {-0.2f, -0.2f * m_aspectRatio, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f} },
            { {-0.2f, 0.2f * m_aspectRatio, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f} },
            { {0.2f, -0.2f * m_aspectRatio, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f} },
            { {0.2f, 0.2f * m_aspectRatio, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
            //{ {0.2f, 0.2f * m_aspectRatio, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f} },
            //{ {0.2f, -0.2f * m_aspectRatio, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f} },
            //{ {-0.2f, 0.2f * m_aspectRatio, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f} },
            //{ {-0.2f, -0.2f * m_aspectRatio, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f} }
        };

        const UINT vertexBufferSize = sizeof(shapeVertices);
        CD3DX12_HEAP_PROPERTIES heapProps1(D3D12_HEAP_TYPE_DEFAULT);
        auto desc1 = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);

        ThrowIfFailed(mDevice->CreateCommittedResource(
            &heapProps1,
            D3D12_HEAP_FLAG_NONE,
            &desc1,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&mVertexBuffer)
        ));

        CD3DX12_HEAP_PROPERTIES heapProps2(D3D12_HEAP_TYPE_UPLOAD);
        auto desc2 = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
        ThrowIfFailed(mDevice->CreateCommittedResource(
            &heapProps2,
            D3D12_HEAP_FLAG_NONE,
            &desc2,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&vertexBufferUpload)
        ));

        D3D12_SUBRESOURCE_DATA vertexData = {};
        vertexData.pData = reinterpret_cast<UINT8*>(shapeVertices);
        vertexData.RowPitch = vertexBufferSize;
        vertexData.SlicePitch = vertexData.RowPitch;

        UpdateSubresources<1>(mCommandList.Get(), mVertexBuffer.Get(), vertexBufferUpload.Get(), 0, 0, 1, &vertexData);

        auto verBarrier = CD3DX12_RESOURCE_BARRIER::Transition(mVertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        mCommandList->ResourceBarrier(1, &verBarrier);

        mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
        mVertexBufferView.StrideInBytes = sizeof(Vertex);
        mVertexBufferView.SizeInBytes = vertexBufferSize;
    }

    {
        const UINT constantBufferSize = sizeof(SceneConstantBuffer);
        CD3DX12_HEAP_PROPERTIES CBVHeapProps(D3D12_HEAP_TYPE_UPLOAD);
        auto CBVDescBuffer = CD3DX12_RESOURCE_DESC::Buffer(constantBufferSize);

        ThrowIfFailed(mDevice->CreateCommittedResource(
            &CBVHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &CBVDescBuffer,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&mConstantBuffer)
        ));

        D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc = {};
        CBVDesc.BufferLocation = mConstantBuffer->GetGPUVirtualAddress();
        CBVDesc.SizeInBytes = constantBufferSize;
        mDevice->CreateConstantBufferView(&CBVDesc, mCBVSRVHeap->GetCPUDescriptorHandleForHeapStart());
        
        CD3DX12_RANGE readRange(0, 0);
        ThrowIfFailed(mConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&mpCBVDataBegin)));
        memcpy(mpCBVDataBegin, &mConstantBufferData, sizeof(mConstantBufferData));
    }

    ComPtr<ID3D12Resource> textureUploadHeap;
    {
        // Describe and create a Texture2D.
        D3D12_RESOURCE_DESC textureDesc = {};
        textureDesc.MipLevels = 1;
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        textureDesc.Width = textureWidth;
        textureDesc.Height = textureHeight;
        textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
        textureDesc.DepthOrArraySize = 1;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

        auto SRVProps1 = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        ThrowIfFailed(mDevice->CreateCommittedResource(
            &SRVProps1,
            D3D12_HEAP_FLAG_NONE,
            &textureDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&mTexture)
        ));

        const UINT64 uploadBufferSize = GetRequiredIntermediateSize(mTexture.Get(), 0, 1);

        auto SRVProps2 = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto SRVBuffer = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
        ThrowIfFailed(mDevice->CreateCommittedResource(
            &SRVProps2,
            D3D12_HEAP_FLAG_NONE,
            &SRVBuffer,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&textureUploadHeap)
        ));

        std::vector<UINT8> texture = GenerateTextureData();

        D3D12_SUBRESOURCE_DATA textureData = {};
        textureData.pData = &texture[0];
        textureData.RowPitch = textureWidth * texturePixelSize;
        textureData.SlicePitch = textureData.RowPitch * textureHeight;

        UpdateSubresources(mCommandList.Get(), mTexture.Get(), textureUploadHeap.Get(), 0, 0, 1, &textureData);

        // Describe and create a SRV for the texture.
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = textureDesc.Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        mDevice->CreateShaderResourceView(mTexture.Get(), &srvDesc, mCBVSRVHeap->GetCPUDescriptorHandleForHeapStart());

        
        auto SRVBarrier = CD3DX12_RESOURCE_BARRIER::Transition(mTexture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        mCommandList->ResourceBarrier(1, &SRVBarrier);
    }

    {
        ThrowIfFailed(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_BUNDLE, mBundleAllocator.Get(), mPipelineState.Get(), IID_PPV_ARGS(&mBundle)));
        mBundle->SetGraphicsRootSignature(mRootSignature.Get());
        mBundle->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        mBundle->IASetVertexBuffers(0, 1, &mVertexBufferView);
        mBundle->DrawInstanced(4, 1, 0, 0);
        ThrowIfFailed(mBundle->Close());
    }

    ThrowIfFailed(mCommandList->Close());
    ID3D12CommandList* CommandLists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(CommandLists), CommandLists);

    {
        ThrowIfFailed(mDevice->CreateFence(mFenceValues[mFrameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));
        mFenceValues[mFrameIndex]++;

        mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if(mFenceEvent == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }

        WaitForGpu();
    }
}

void MovingTextureShape::OnUpdate()
{
    const float translationSpeed = 0.005f;
    const float offsetBounds = 1.25;

    mConstantBufferData.offset.x += translationSpeed;
    if (mConstantBufferData.offset.x > offsetBounds)
    {
        mConstantBufferData.offset.x = -offsetBounds;
    }
    memcpy(mpCBVDataBegin, &mConstantBufferData, sizeof(mConstantBufferData));
}

void MovingTextureShape::OnRender()
{
    {
        ThrowIfFailed(mCommandAllocators[mFrameIndex]->Reset());
        ThrowIfFailed(mCommandList->Reset(mCommandAllocators[mFrameIndex].Get(), mPipelineState.Get()));

        mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

        ID3D12DescriptorHeap* descriptorHeaps[] = { mCBVSRVHeap.Get() };
        mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

        mCommandList->SetGraphicsRootDescriptorTable(0, mCBVSRVHeap->GetGPUDescriptorHandleForHeapStart());
        mCommandList->SetGraphicsRootDescriptorTable(1, mCBVSRVHeap->GetGPUDescriptorHandleForHeapStart());
        mCommandList->RSSetViewports(1, &mViewPort);
        mCommandList->RSSetScissorRects(1, &mScissorRect);

        auto rtvBarrier = CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargets[mFrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        mCommandList->ResourceBarrier(1, &rtvBarrier);

        CD3DX12_CPU_DESCRIPTOR_HANDLE RTVHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart(), mFrameIndex, mRTVDescriptorSize);
        mCommandList->OMSetRenderTargets(1, &RTVHandle, FALSE, nullptr);

        const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
        mCommandList->ClearRenderTargetView(RTVHandle, clearColor, 0, nullptr);
        mCommandList->ExecuteBundle(mBundle.Get());

        auto rtvBarrier2 = CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargets[mFrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        mCommandList->ResourceBarrier(1, &rtvBarrier2);

        ThrowIfFailed(mCommandList->Close());
    }

    ID3D12CommandList* ppCommandLists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    ThrowIfFailed(mSwapChain->Present(1, 0));

    MoveToNextFrame();
}

void MovingTextureShape::OnDestroy()
{
    WaitForGpu();
    CloseHandle(mFenceEvent);
}

std::vector<UINT8> MovingTextureShape::GenerateTextureData()
{
    const UINT rowPitch = textureWidth * texturePixelSize;
    const UINT cellPitch = rowPitch >> 3;        // The width of a cell in the checkboard texture.
    const UINT cellHeight = textureWidth >> 3;    // The height of a cell in the checkerboard texture.
    const UINT textureSize = rowPitch * textureHeight;

    std::vector<UINT8> data(textureSize);
    UINT8* pData = &data[0];

    for (UINT n = 0; n < textureSize; n += texturePixelSize)
    {
        UINT x = n % rowPitch;
        UINT y = n / rowPitch;
        UINT i = x / cellPitch;
        UINT j = y / cellHeight;

        if (i % 2 == j % 2)
        {
            pData[n] = 0x00;        // R
            pData[n + 1] = 0x00;    // G
            pData[n + 2] = 0x00;    // B
            pData[n + 3] = 0xff;    // A
        }
        else
        {
            pData[n] = 0xff;        // R
            pData[n + 1] = 0xff;    // G
            pData[n + 2] = 0xff;    // B
            pData[n + 3] = 0xff;    // A
        }
    }

    return data;
}

void MovingTextureShape::WaitForGpu()
{
    // Schedule a Signal command in the queue.
    ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), mFenceValues[mFrameIndex]));

    // Wait until the fence has been processed.
    ThrowIfFailed(mFence->SetEventOnCompletion(mFenceValues[mFrameIndex], mFenceEvent));
    WaitForSingleObjectEx(mFenceEvent, INFINITE, FALSE);

    // Increment the fence value for the current frame.
    mFenceValues[mFrameIndex]++;
}

// Prepare to render the next frame.
void MovingTextureShape::MoveToNextFrame()
{
    // Schedule a Signal command in the queue.
    const UINT64 currentFenceValue = mFenceValues[mFrameIndex];
    ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), currentFenceValue));

    // Update the frame index.
    mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();

    
    if (mFence->GetCompletedValue() < mFenceValues[mFrameIndex])
    {
        ThrowIfFailed(mFence->SetEventOnCompletion(mFenceValues[mFrameIndex], mFenceEvent));
        WaitForSingleObjectEx(mFenceEvent, INFINITE, FALSE);
    }

    // Set the fence value for the next frame.
    mFenceValues[mFrameIndex] = currentFenceValue + 1;
}