#pragma once

#include "DXSample.h"

using namespace DirectX;

using Microsoft::WRL::ComPtr;

class D3D12MyBundleAndConstantBuffer : public DXSample
{
public:
    D3D12MyBundleAndConstantBuffer(UINT width, UINT height, std::wstring name);

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

    // To align 256 bytes, padding is defined.
    struct SceneConstantBuffer
    {
        XMFLOAT4 offset;
        float padding[60];
    };
    static_assert((sizeof(SceneConstantBuffer) % 256) == 0, "Constant Buffer size must be 256-byte aligned");

    CD3DX12_VIEWPORT mViewPort;
    CD3DX12_RECT mScissorRect;
    ComPtr<IDXGISwapChain3> mSwapChain;
    ComPtr<ID3D12Device> mDevice;
    ComPtr<ID3D12Resource> mRenderTargets[frameCount];
    ComPtr<ID3D12CommandAllocator> mCommandAllocator;
    ComPtr<ID3D12CommandAllocator> mBundleAllocator;
    ComPtr<ID3D12CommandQueue> mCommandQueue;
    ComPtr<ID3D12RootSignature> mRootSignature;
    ComPtr<ID3D12DescriptorHeap> mRTVHeap;
    ComPtr<ID3D12DescriptorHeap> mCBVHeap;
    ComPtr<ID3D12PipelineState> mPipelineState;
    ComPtr<ID3D12GraphicsCommandList> mCommandList;
    ComPtr<ID3D12GraphicsCommandList> mBundle;
    UINT mRTVDescriptorSize;

    ComPtr<ID3D12Resource> mVertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;
    ComPtr<ID3D12Resource> mConstantBuffer;
    SceneConstantBuffer mConstantBufferData;
    UINT8* mpCBVDataBegin;

    UINT mFrameIndex;
    HANDLE mFenceEvent;
    ComPtr<ID3D12Fence> mFence;
    UINT64 mFenceValue;

    void PipelineInit();
    void AssetInit();

    void WaitForPreviousFrame();
};