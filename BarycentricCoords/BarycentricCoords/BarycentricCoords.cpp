/****************************************************************************
 * ==> BarycentricCoords ---------------------------------------------------*
 ****************************************************************************
 * Description: Using barycentric coordinates demo                          *
 * Developer:   Jean-Milost Reymond                                         *
 ****************************************************************************
 * MIT License                                                              *
 *                                                                          *
 * Permission is hereby granted, free of charge, to any person obtaining a  *
 * copy of this software and associated documentation files (the            *
 * "Software"), to deal in the Software without restriction, including      *
 * without limitation the rights to use, copy, modify, merge, publish,      *
 * distribute, sub-license, and/or sell copies of the Software, and to      *
 * permit persons to whom the Software is furnished to do so, subject to    *
 * the following conditions:                                                *
 *                                                                          *
 * The above copyright notice and this permission notice shall be included  *
 * in all copies or substantial portions of the Software.                   *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  *
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF               *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   *
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY     *
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,     *
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE        *
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                   *
 ****************************************************************************/

// std
#include <string>

// classes
#include "Triangle.h"
#include "TriangleRenderer.h"

// windows
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>

// resources
#include "Resource.h"

//------------------------------------------------------------------------------
Math::Vector2F g_MousePos;
bool           g_IsMouseAboveClientRect = false;
//------------------------------------------------------------------------------
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            ::PostQuitMessage(0);
            break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
            switch (wParam)
            {
                case VK_ESCAPE:
                    ::PostQuitMessage(0);
                    break;
            }

            break;

        case WM_MOUSEMOVE:
            // get mouse position on the client area
            g_MousePos               = Math::Vector2F((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
            g_IsMouseAboveClientRect = true;
            break;

        case WM_NCMOUSEMOVE:
            ::SetCursor(LoadCursor(NULL, IDC_ARROW));
            g_IsMouseAboveClientRect = false;
            break;

        case WM_SETCURSOR:
            // only in client area
            if (LOWORD(lParam) == HTCLIENT)
            {
                ::SetCursor(LoadCursor(NULL, IDC_CROSS));
                return TRUE;
            }

            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}
//------------------------------------------------------------------------------
bool IsMouseOverWindow(HWND hWnd)
{
    POINT pt;
    ::GetCursorPos(&pt);

    RECT rc;
    ::GetWindowRect(hWnd, &rc);

    return ::PtInRect(&rc, pt);
}
//------------------------------------------------------------------------------
void DrawText(HDC hDC, const std::wstring& text, RECT rect, UINT format)
{
    ::DrawText(hDC, text.c_str(), (int)text.length(), &rect, format);
}
//------------------------------------------------------------------------------
int APIENTRY wWinMain(_In_     HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_     LPWSTR    lpCmdLine,
                      _In_     int       nCmdShow)
{
    WNDCLASSEX wcex = {};
    HWND       hWnd = 0;
    MSG        msg = {};
    BOOL       bQuit = FALSE;

    // register window class
    wcex.cbSize        = sizeof(WNDCLASSEX);
    wcex.style         = CS_OWNDC;
    wcex.lpfnWndProc   = WindowProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = hInstance;
    wcex.hIcon         = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON));
    wcex.hIconSm       = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON_SMALL));
    wcex.hCursor       = ::LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName  = nullptr;
    wcex.lpszClassName = L"barycentricCoords";

    if (!RegisterClassEx(&wcex))
        return 0;

    // create main window
    hWnd = ::CreateWindowEx(0,
                            L"barycentricCoords",
                            L"Barycentric Coordinates Demo",
                            WS_DLGFRAME | WS_CAPTION | WS_SYSMENU,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            800,
                            600,
                            nullptr,
                            nullptr,
                            hInstance,
                            nullptr);

    ::ShowWindow(hWnd, nCmdShow);

    // get the window client rect
    RECT clientRect;
    ::GetClientRect(hWnd, &clientRect);

    float  angle    = 0.0f;
    double lastTime = (double)::GetTickCount64();

    // get the window device context
    HDC hDC = ::GetDC(hWnd);

    // create the background brush
    HBRUSH hBrush = ::CreateSolidBrush(RGB(20, 30, 43));

    // fill background
    ::FillRect(hDC, &clientRect, hBrush);

    // create a demo triangle
    Geometry::Triangle triangle;
    triangle.m_Vertex[0] = Math::Vector2F((float) (clientRect.right  - clientRect.left) / 2.0f,         50.0f);
    triangle.m_Vertex[1] = Math::Vector2F((float) (clientRect.right  - clientRect.left) / 3.0f,         400.0f);
    triangle.m_Vertex[2] = Math::Vector2F((float)((clientRect.right  - clientRect.left) * 2.0f) / 3.0f, 400.0f);

    // create the triangle renderer
    Rasterizer::TriangleRenderer renderer(hDC, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);

    // get the renderer buffer device context
    HDC hBufferDC = renderer.GetDC();

    LOGBRUSH lb;
    lb.lbStyle = BS_SOLID;
    lb.lbColor = RGB(128, 130, 126);
    lb.lbHatch = 0;

    // create dash pen
    HPEN hDashPen = ::ExtCreatePen(PS_GEOMETRIC | PS_DASH | PS_ENDCAP_ROUND, 2, &lb, 0, nullptr);

    // program main loop
    while (!bQuit)
    {
        // check for messages
        if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // handle or dispatch messages
            if (msg.message == WM_QUIT)
                bQuit = TRUE;
            else
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }
        }
        else
        {
            // calculate the elapsed time
            double elapsedTime = (double)::GetTickCount64() - lastTime;
                   lastTime    = (double)::GetTickCount64();

            // clear the render buffer
            renderer.Clear(RGB(20, 30, 43));

            // draw the triangle
            renderer.RenderTriangle(triangle);

            // check if mouse position is inside the triangle
            Geometry::Triangle::IWeights weights;
            const bool                   inside = triangle.BarycentricInside(g_MousePos, weights);

            // inside the triangle?
            if (inside)
            {
                HPEN hPen    = ::CreatePen(PS_SOLID, 3, RGB(255, 255, 255));
                HPEN hOldPen = (HPEN)SelectObject(hBufferDC, hPen);

                // outline the triangle
                ::MoveToEx(hBufferDC, (int)triangle.m_Vertex[0].m_X, (int)triangle.m_Vertex[0].m_Y, nullptr);
                ::LineTo  (hBufferDC, (int)triangle.m_Vertex[1].m_X, (int)triangle.m_Vertex[1].m_Y);
                ::LineTo  (hBufferDC, (int)triangle.m_Vertex[2].m_X, (int)triangle.m_Vertex[2].m_Y);
                ::LineTo  (hBufferDC, (int)triangle.m_Vertex[0].m_X, (int)triangle.m_Vertex[0].m_Y);

                ::SelectObject(hBufferDC, hOldPen);
                ::DeleteObject(hPen);
            }

            // check if mouse is above form client area
            if (IsMouseOverWindow(hWnd) && g_IsMouseAboveClientRect)
            {
                HPEN hOldPen = (HPEN)SelectObject(hBufferDC, hDashPen);

                // draw the lines between the current point and each vertex
                ::MoveToEx(hBufferDC, (int)g_MousePos.m_X,           (int)g_MousePos.m_Y, nullptr);
                ::LineTo  (hBufferDC, (int)triangle.m_Vertex[0].m_X, (int)triangle.m_Vertex[0].m_Y);
                ::MoveToEx(hBufferDC, (int)g_MousePos.m_X,           (int)g_MousePos.m_Y, nullptr);
                ::LineTo  (hBufferDC, (int)triangle.m_Vertex[1].m_X, (int)triangle.m_Vertex[1].m_Y);
                ::MoveToEx(hBufferDC, (int)g_MousePos.m_X,           (int)g_MousePos.m_Y, nullptr);
                ::LineTo  (hBufferDC, (int)triangle.m_Vertex[2].m_X, (int)triangle.m_Vertex[2].m_Y);

                ::SelectObject(hBufferDC, hOldPen);
            }

            // configure text rendering
            ::SetBkMode(hBufferDC, TRANSPARENT);
            ::SetBkColor(hBufferDC, 0x000000);
            ::SetTextColor(hBufferDC, 0xffffff);

            RECT colorRect;

            // colored rectangle coordinates
            colorRect.left   = ((clientRect.right - clientRect.left) / 2) - 50;
            colorRect.top    = 450;
            colorRect.right  = ((clientRect.right - clientRect.left) / 2) + 50;
            colorRect.bottom = 480;

            HBRUSH hColorBrush;

            // calculate the color to draw and create a brush
            if (inside)
                hColorBrush = ::CreateSolidBrush(RGB(255 * weights.m_W2, 255 * weights.m_W1, 255 * weights.m_W0));
            else
                hColorBrush = ::CreateSolidBrush(RGB(20, 30, 43));

            // restore previous brush
            HBRUSH hOldBrush = (HBRUSH)::SelectObject(hBufferDC, hColorBrush);

            // draw the colored rectangle
            ::FillRect(hBufferDC, &colorRect, hColorBrush);

            ::SelectObject(hBufferDC, hOldBrush);

            RECT textRect;

            // inside/outside text coordinates
            textRect.left   = 20;
            textRect.top    = 450;
            textRect.right  = textRect.left + 200;
            textRect.bottom = textRect.top  + 20;

            // draw if point is inside or outside the triangle text
            DrawText(hBufferDC, inside ? L"Point status: Inside" : L"Point status: Outside", textRect, DT_SINGLELINE | DT_LEFT | DT_TOP);

            // w0 text coordinates
            textRect.left   = clientRect.right - 150;
            textRect.top    = 450;
            textRect.right  = clientRect.right;
            textRect.bottom = textRect.top + 20;

            // draw w0 text
            DrawText(hBufferDC, L"W0: " + std::to_wstring(weights.m_W0), textRect, DT_SINGLELINE | DT_LEFT | DT_TOP);

            // w1 text coordinates
            textRect.left   = clientRect.right - 150;
            textRect.top    = 480;
            textRect.right  = clientRect.right;
            textRect.bottom = textRect.top + 20;

            // draw w1 text
            DrawText(hBufferDC, L"W1: " + std::to_wstring(weights.m_W1), textRect, DT_SINGLELINE | DT_LEFT | DT_TOP);

            // w2 text coordinates
            textRect.left   = clientRect.right - 150;
            textRect.top    = 510;
            textRect.right  = clientRect.right;
            textRect.bottom = textRect.top + 20;

            // draw w2 text
            DrawText(hBufferDC, L"W2: " + std::to_wstring(weights.m_W2), textRect, DT_SINGLELINE | DT_LEFT | DT_TOP);

            // draw the background, which contains the previously drawn triangle
            renderer.Present(hDC);

            // slow down the rendering to avoid processor overhead
            ::Sleep(1);
        }
    }

    // release the GDI objects
    ::DeleteObject(hDashPen);
    ::DeleteObject(hBrush);
    ::ReleaseDC(hWnd, hDC);

    // destroy the window explicitly
    ::DestroyWindow(hWnd);

    return (int)msg.wParam;
}
//------------------------------------------------------------------------------
