#pragma once

#include "DXSample.h"

using namespace DirectX;

using Microsoft::WRL::ComPtr;

class D3D12MyTriangle : public DXSample
{
public:
    D3D12MyTriangle(UINT width, UINT height, std::wstring name);

    virtual void OnInit();
    virtual void OnUpdate();
    virtual void OnRender();
    virtual void OnDestroy();

private:
    static const UINT frameCount = 2;
    
    struct Vertex
    {
        XMFLOAT3 position;
        XMFLOAT4 color;
    };

    CD3DX12_VIEWPORT mViewPort;
    CD3DX12_RECT mScissorRect;
    ComPtr<IDXGISwapChain3> mSwapChain;
    ComPtr<ID3D12Device> mDevice;
    ComPtr<ID3D12Resource> mRenderTargets[frameCount];
    ComPtr<ID3D12CommandAllocator> mCommandAllocator;
    ComPtr<ID3D12CommandQueue> mCommandQueue;
    ComPtr<ID3D12RootSignature> mRootSignature;
    ComPtr<ID3D12DescriptorHeap> mRTVHeap;
    ComPtr<ID3D12PipelineState> mPipelineState;
    ComPtr<ID3D12GraphicsCommandList> mCommandList;
    UINT mRTVDescriptorSize;

    ComPtr<ID3D12Resource> mVertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;

    UINT mFrameIndex;
    HANDLE mFenceEvent;
    ComPtr<ID3D12Fence> mFence;
    UINT64 mFenceValue;

    void PipelineInit();
    void AssetInit();

    void WaitForPreviousFrame();
};