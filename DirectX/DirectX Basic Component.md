# D3D12 Basic Component (based on D3D12HelloTriangle example)
## Code flow for a simple app
- 초기화 (Initialize)
- 반복 (Repeat)
    - 업데이트 (Update)
    - 렌더링 (Render)
- 파괴 (Destroy)

## 초기화 (Initialize)
### 파이프라인 초기화
```
1. 디버그 계층 사용
#if defined(_DEBUG)
{
    // defined debug controller
    // get debug interface, and succeed to debug controller
    // enable debug layer
}
#enif
```

```
2. 디바이스 생성
// Check WARP device
D3D12CreateDevice
(
    {adapter}, {D3D_FEATURE_LEVEL}, 
    {REFIID: Globally Unique Identifier}, {ppDevice}
)

D3D12CreateDevice
(
    {adapter}, {D3D_FEATURE_LEVEL},
    IID_PPV_ARGS(&{device pointer})
)
```
```
3. 명령 큐 생성
CreateCommandQueue
(
    &{command queue descriptor}}, IID_PPV_ARGS(&{command queue pointer}}
)
```
```
4. 스왑 체인 생성
{
    // Describe and create the swap chain
    // initialize BufferCount, Width, Height, etc
}

CreateSwapChainForHwnd
(
    &{command queue pointer}, {HWND}, &{Swap chain descriptor},
    &{full screen descriptor}, &{restrict to output}, &{swapchain pointer}
)
```
```
5. RTV(Rendering Target View) 설명자 힙 생성   
{
    // Describe and create a render target view descriptor heap
    // initialize Descriptor count, type, flag etc.
}
```
```
6. 프레임 리소스 생성
{
    // Create frame resource
    // Create a RTV for each frame 
    // loop from 0 to frame count. 
}
```
```
7. 명령 할당자 생성
CreateCommandAllocator
(
    {D3D12_COMMAND_LIST_TYPE}, IID_PPV_ARGS(&{command Allocator pointer}
)
```
-----------
### 자산 초기화
