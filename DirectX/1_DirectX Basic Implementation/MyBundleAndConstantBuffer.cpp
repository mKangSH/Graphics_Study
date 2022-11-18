#include "MyBundleAndConstantBuffer.h"

D3D12MyBundleAndConstantBuffer::D3D12MyBundleAndConstantBuffer(UINT width, UINT height, std::wstring name) :
    DXSample(width, height, name),
    mFrameIndex(0),
    mViewPort(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)),
    mScissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height)),
    mRTVDescriptorSize(0)
{

}

void D3D12MyBundleAndConstantBuffer::OnInit()
{
    PipelineInit();
    AssetInit();
}

void D3D12MyBundleAndConstantBuffer::PipelineInit()
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
        CBVHeapDesc.NumDescriptors = 1;
        CBVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        CBVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        ThrowIfFailed(mDevice->CreateDescriptorHeap(&CBVHeapDesc, IID_PPV_ARGS(&mCBVHeap)));
    }

    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE RTVHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart());

        for (UINT i = 0; i < frameCount; i++)
        {
            ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mRenderTargets[i])));
            mDevice->CreateRenderTargetView(mRenderTargets[i].Get(), nullptr, RTVHandle);
            RTVHandle.Offset(1, mRTVDescriptorSize);
        }

        ThrowIfFailed(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator)));
        ThrowIfFailed(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE, IID_PPV_ARGS(&mBundleAllocator)));
    }
}

void D3D12MyBundleAndConstantBuffer::AssetInit()
{
    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

        ThrowIfFailed(mDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData)));

        CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
        CD3DX12_ROOT_PARAMETER1 rootParameters[1];

        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

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
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
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

    ThrowIfFailed(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), mPipelineState.Get(), IID_PPV_ARGS(&mCommandList)));
    ThrowIfFailed(mCommandList->Close());

    {
        Vertex triangleVertices[] =
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

        const UINT vertexBufferSize = sizeof(triangleVertices);
        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
        auto desc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);

        ThrowIfFailed(mDevice->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&mVertexBuffer)
        ));

        UINT8* pVertexDataBegin;
        CD3DX12_RANGE readRange(0, 0);
        ThrowIfFailed(mVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
        memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
        mVertexBuffer->Unmap(0, nullptr);

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
        mDevice->CreateConstantBufferView(&CBVDesc, mCBVHeap->GetCPUDescriptorHandleForHeapStart());
        
        CD3DX12_RANGE readRange(0, 0);
        ThrowIfFailed(mConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&mpCBVDataBegin)));
        memcpy(mpCBVDataBegin, &mConstantBufferData, sizeof(mConstantBufferData));
    }

    {
        ThrowIfFailed(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_BUNDLE, mBundleAllocator.Get(), mPipelineState.Get(), IID_PPV_ARGS(&mBundle)));
        mBundle->SetGraphicsRootSignature(mRootSignature.Get());
        mBundle->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        mBundle->IASetVertexBuffers(0, 1, &mVertexBufferView);
        mBundle->DrawInstanced(4, 1, 0, 0);
        ThrowIfFailed(mBundle->Close());
    }

    ThrowIfFailed(mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));
    mFenceValue = 1;
    mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if(mFenceEvent == nullptr)
    {
        ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
    }

    WaitForPreviousFrame();
}

void D3D12MyBundleAndConstantBuffer::OnUpdate()
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

void D3D12MyBundleAndConstantBuffer::OnRender()
{
    {
        ThrowIfFailed(mCommandAllocator->Reset());
        ThrowIfFailed(mCommandList->Reset(mCommandAllocator.Get(), mPipelineState.Get()));

        mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

        ID3D12DescriptorHeap* ppHeaps[] = { mCBVHeap.Get() };
        mCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

        mCommandList->SetGraphicsRootDescriptorTable(0, mCBVHeap->GetGPUDescriptorHandleForHeapStart());
        mCommandList->RSSetViewports(1, &mViewPort);
        mCommandList->RSSetScissorRects(1, &mScissorRect);

        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargets[mFrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        mCommandList->ResourceBarrier(1, &barrier);

        CD3DX12_CPU_DESCRIPTOR_HANDLE RTVHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart(), mFrameIndex, mRTVDescriptorSize);
        mCommandList->OMSetRenderTargets(1, &RTVHandle, FALSE, nullptr);

        const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
        mCommandList->ClearRenderTargetView(RTVHandle, clearColor, 0, nullptr);
        mCommandList->ExecuteBundle(mBundle.Get());

        mCommandList->ResourceBarrier(1, &barrier);

        ThrowIfFailed(mCommandList->Close());
    }

    ID3D12CommandList* ppCommandLists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    ThrowIfFailed(mSwapChain->Present(1, 0));

    WaitForPreviousFrame();
}

void D3D12MyBundleAndConstantBuffer::OnDestroy()
{
    WaitForPreviousFrame();
    CloseHandle(mFenceEvent);
}

void D3D12MyBundleAndConstantBuffer::WaitForPreviousFrame()
{
    const UINT64 fence = mFenceValue;
    ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), fence));
    ++mFenceValue;

    if(mFence->GetCompletedValue() < fence)
    {
        ThrowIfFailed(mFence->SetEventOnCompletion(fence, mFenceEvent));
        WaitForSingleObject(mFenceEvent, INFINITE);
    }

    mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();
}