#pragma once

using namespace DirectX;

using Microsoft::WRL::ComPtr;

enum class EMouseButton
{
    Left,
    Middle,
    Right,
    X1,
    X2
};

class Engine
{
public:
    Engine(UINT width, UINT height);

    void OnInit();
    void OnResize(HWND hWnd);
    void OnUpdate();
    void OnRender();
    void OnDestroy();
    void OnKeyDown(UINT8 key);
    void OnKeyUp(UINT8 key);
    void OnMouseButtonDown(EMouseButton button);
    void OnMouseButtonUp(EMouseButton button);

    UINT GetWindowX();
    UINT GetWindowY();
    UINT GetWindowCenterX();
    UINT GetWindowCenterY();
    void CenterMouse();
    UINT GetMouseX();
    UINT GetMouseY();
    UINT GetWidth() const { return m_width; }
    UINT GetHeight() const { return m_height; }

    std::wstring GetAssetFullPath(LPCWSTR assetName);

private:
    static const UINT FrameCount = 2;

    UINT m_width;
    UINT m_height;
    float m_aspectRatio;

    std::wstring m_assetsPath;

    struct Vertex
    {
        XMFLOAT3 position;
        XMFLOAT2 texCoord;
        XMFLOAT3 normal;
        XMFLOAT4 color;
    };

    struct SceneConstantBuffer
    {
        XMFLOAT4X4 mWorldViewProj;
        XMFLOAT4X4 mWorld;
        XMFLOAT4 materialColor;
        XMFLOAT3 cameraPos;
        float padding01;

        float padding[24];
    };

    CD3DX12_VIEWPORT m_viewport;
    CD3DX12_RECT m_scissorRect;
    ComPtr<IDXGISwapChain3> m_swapChain;
    ComPtr<ID3D12Device> m_device;
    ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
    ComPtr<ID3D12Resource> m_depthStencil;
    ComPtr<ID3D12CommandAllocator> m_commandAllocators[FrameCount];
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<ID3D12RootSignature> m_rootSignature;
    ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
    ComPtr<ID3D12DescriptorHeap> m_cbvHeap;
    ComPtr<ID3D12PipelineState> m_pipelineState;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;
    UINT m_rtvDescriptorSize;

    ComPtr<ID3D12Resource> m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
    ComPtr<ID3D12Resource> m_indexBuffer;
    D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
    ComPtr<ID3D12Resource> m_constantBuffer;
    SceneConstantBuffer m_constantBufferData;
    UINT8* m_pCbvDataBegin;

    UINT m_frameIndex;
    HANDLE m_fenceEvent;
    ComPtr<ID3D12Fence> m_fence;
    UINT64 m_fenceValues[FrameCount];

    void LoadPipeline();
    void LoadAssets();
    void PopulateCommandList();
    void MoveToNextFrame();
    void WaitForGpu();
    void GetHardwareAdapter(_In_ IDXGIFactory2* pFactory, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter);
};