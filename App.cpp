#include "stdafx.h"
#include "resource.h"
#include "App.h"

HWND App::m_hwnd = nullptr;

int App::Run(Engine* pEngine, HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASSEX windowClass = { 0 };
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.lpszClassName = L"Test";
    windowClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    RegisterClassEx(&windowClass);

    RECT windowRect = { 0, 0, static_cast<LONG>(pEngine->GetWidth()), static_cast<LONG>(pEngine->GetHeight()) };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    m_hwnd = CreateWindow(
        windowClass.lpszClassName,
        L"Test",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,
        nullptr,
        hInstance,
        pEngine);

    pEngine->OnInit();

    ShowWindow(m_hwnd, nCmdShow);

    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    pEngine->OnDestroy();

    return static_cast<char>(msg.wParam);
}

LRESULT CALLBACK App::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    Engine* pEngine = reinterpret_cast<Engine*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (message)
    {
    case WM_CREATE:
        {
            LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
        }
        return 0;

    case WM_KEYDOWN:
        if (pEngine)
        {
            pEngine->OnKeyDown(static_cast<UINT8>(wParam));
        }
        return 0;

    case WM_KEYUP:
        if (pEngine)
        {
            pEngine->OnKeyUp(static_cast<UINT8>(wParam));
        }
        return 0;

    case WM_LBUTTONDOWN:
        if (pEngine)
        {
            pEngine->OnMouseButtonDown(EMouseButton::Left);
        }
        return 0;

    case WM_LBUTTONUP:
        if (pEngine)
        {
            pEngine->OnMouseButtonUp(EMouseButton::Left);
        }
        return 0;

    case WM_RBUTTONDOWN:
        if (pEngine)
        {
            pEngine->OnMouseButtonDown(EMouseButton::Right);
        }
        return 0;

    case WM_RBUTTONUP:
        if (pEngine)
        {
            pEngine->OnMouseButtonUp(EMouseButton::Right);
        }
        return 0;

    case WM_MBUTTONDOWN:
        if (pEngine)
        {
            pEngine->OnMouseButtonDown(EMouseButton::Middle);
        }
        return 0;

    case WM_MBUTTONUP:
        if (pEngine)
        {
            pEngine->OnMouseButtonUp(EMouseButton::Middle);
        }
        return 0;

    case WM_SIZE:
        if (pEngine)
        {
            pEngine->OnResize(hWnd);
        }
        return 0;

    case WM_PAINT:
        if (pEngine)
        {
            pEngine->OnUpdate();
            pEngine->OnRender();
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}
