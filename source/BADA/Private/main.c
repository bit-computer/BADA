//***************************************************************************
// 
// 파일: main.c
// 
// 설명: 메인 함수
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/09/12
// 
//***************************************************************************

#include "precompiled.h"
#include "app.h"

HINSTANCE g_hinstance;
HWND g_hwnd;

HRESULT init_window(const size_t width, const size_t height);
LRESULT CALLBACK wnd_proc(HWND, UINT, WPARAM, LPARAM);

int main(void)
{
    const int window_width = 800;
    const int window_height = 600;
    const int pixel_size = 10;

    if (FAILED(init_window(window_width, window_height)))
    {
        return 0;
    }

    gp_app = (app_t*)malloc(sizeof(app_t));
    if (!init_app(pixel_size))
    {
        ASSERT(false, "Failed to init app");
        SAFE_FREE(gp_app);
        return 0;
    }

    // Main message loop
    MSG msg = { 0 };
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            if (!is_running_app())
            {
                break;
            }

            tick_app();
        }
    }

    shutdown_app();
    SAFE_FREE(gp_app);

    return (int)msg.wParam;
}

HRESULT init_window(const size_t width, const size_t height)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = wnd_proc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = GetModuleHandleW(NULL);
    wcex.hIcon = NULL;
    wcex.hCursor = NULL;
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"safe99";
    wcex.hIconSm = NULL;
    if (!RegisterClassEx(&wcex))
    {
        return E_FAIL;
    }

    // Create window
    g_hinstance = wcex.hInstance;
    RECT rc = { 0, 0, (LONG)width, (LONG)height };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    g_hwnd = CreateWindow(wcex.lpszClassName, L"safe99 - BADA(Bit-computer Art Drawing App)",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, wcex.hInstance,
        NULL);
    if (!g_hwnd)
    {
        return E_FAIL;
    }

    GetClientRect(g_hwnd, &rc);
    ClientToScreen(g_hwnd, (POINT*)&rc.left);
    ClientToScreen(g_hwnd, (POINT*)&rc.right);

    ShowWindow(g_hwnd, SW_SHOWDEFAULT);

    return S_OK;
}

LRESULT CALLBACK wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_LBUTTONDOWN:
    {
        uint32_t x = GET_X_LPARAM(lParam);
        uint32_t y = GET_Y_LPARAM(lParam);
        on_down_left_mouse();
        break;
    }
    case WM_LBUTTONUP:
    {
        uint32_t x = GET_X_LPARAM(lParam);
        uint32_t y = GET_Y_LPARAM(lParam);
        on_up_left_mouse();
        break;
    }
    case WM_RBUTTONDOWN:
    {
        uint32_t x = GET_X_LPARAM(lParam);
        uint32_t y = GET_Y_LPARAM(lParam);
        on_down_right_mouse();
        break;
    }
    case WM_RBUTTONUP:
    {
        uint32_t x = GET_X_LPARAM(lParam);
        uint32_t y = GET_Y_LPARAM(lParam);
        on_up_right_mouse();
        break;
    }
    case WM_MOUSEMOVE:
    {
        uint32_t x = GET_X_LPARAM(lParam);
        uint32_t y = GET_Y_LPARAM(lParam);
        on_move_mouse(x, y);
        break;
    }

    case WM_KEYDOWN:
        key_down((uint8_t)wParam);
        break;
    case WM_KEYUP:
        key_up((uint8_t)wParam);
        break;

    case WM_MOVE:
        if (gp_app != NULL)
        {
            update_window_pos_app();
        }
        break;
    case WM_SIZE:
        if (gp_app != NULL)
        {
            update_window_size_app();
        }
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}