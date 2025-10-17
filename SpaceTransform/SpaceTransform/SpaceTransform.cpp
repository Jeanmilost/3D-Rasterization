/****************************************************************************
 * ==> SpaceTransform ------------------------------------------------------*
 ****************************************************************************
 * Description: 3D to 2D transformation in a local space demo               *
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
#include <cmath>

// classes
#include "Point3D.h"

// windows
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// resources
#include "Resource.h"

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

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
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
    wcex.lpszClassName = L"spaceTransformDemo";

    if (!RegisterClassEx(&wcex))
        return 0;

    // create main window
    hWnd = ::CreateWindowEx(0,
                            L"spaceTransformDemo",
                            L"Space Transform Demo",
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

    const std::int32_t  clientWidth  = clientRect.right  - clientRect.left;
    const std::int32_t  clientHeight = clientRect.bottom - clientRect.top;
    const std::int32_t  clientDepth  = 150;
    const float         fieldOfView  = 0.25f;
    const std::size_t   starCount    = 256;
    Rasterizer::Point3D stars[starCount];

    // create the star field
    for (std::int32_t i = 0; i < starCount; ++i)
        stars[i].m_Pos = Math::Vector3F((float)((std::rand() % clientWidth)  - (clientWidth  / 2)) * clientDepth * fieldOfView,
                                        (float)((std::rand() % clientHeight) - (clientHeight / 2)) * clientDepth * fieldOfView,
                                        (float) (std::rand() % clientDepth));

    float  angle    = 0.0f;
    double lastTime = (double)::GetTickCount64();

    // get the window device context
    HDC hDC = ::GetDC(hWnd);

    // create the background brush
    HBRUSH hBrush = ::CreateSolidBrush(RGB(0, 0, 0));

    // fill background
    ::FillRect(hDC, &clientRect, hBrush);

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

            // iterate through start to draw
            for (std::size_t i = 0; i < starCount; ++i)
            {
                // get previous star position on screen
                Math::Vector2F point2D = stars[i].Transform();

                // select background brush
                HGDIOBJ hOldBrush = ::SelectObject(hDC, hBrush);
                RECT    starRect;

                // calculate star bounding rect
                starRect.left   = (std::int32_t)(((float)clientWidth  / 2.0f) + point2D.m_X);
                starRect.top    = (std::int32_t)(((float)clientHeight / 2.0f) + point2D.m_Y);
                starRect.right  = starRect.left + 10 - (std::int32_t)((stars[i].m_Pos.m_Z * 10.0f) / (float)clientDepth);
                starRect.bottom = starRect.top  + 10 - (std::int32_t)((stars[i].m_Pos.m_Z * 10.0f) / (float)clientDepth);

                // erase previous star
                ::Ellipse(hDC, starRect.left, starRect.top, starRect.right, starRect.bottom);

                // move star to next position
                stars[i].m_Pos.m_Z -= 0.1f * (float)elapsedTime;

                // limit star position between 1.0 and client depth. This code should
                // act as a kind of modulo (unfortunately std::fmodf() doesn't behave
                // as expected
                stars[i].m_Pos.m_Z = (float)clientDepth - std::fmodf((float)clientDepth - stars[i].m_Pos.m_Z, (float)clientDepth - 1.0f);

                // get star position on screen
                point2D = stars[i].Transform();

                // calculate star luminance
                const std::size_t luminance = 255 - ((std::size_t)stars[i].m_Pos.m_Z * 255) / clientDepth;

                // create star brush and select it
                HBRUSH hStarBrush = ::CreateSolidBrush(RGB(luminance, luminance, luminance));
                ::SelectObject(hDC, hStarBrush);

                // calculate star bounding rect
                starRect.left   = (std::int32_t)(((float)clientWidth  / 2.0f) + point2D.m_X);
                starRect.top    = (std::int32_t)(((float)clientHeight / 2.0f) + point2D.m_Y);
                starRect.right  = starRect.left + 10 - (std::int32_t)((stars[i].m_Pos.m_Z * 10.0f) / (float)clientDepth);
                starRect.bottom = starRect.top  + 10 - (std::int32_t)((stars[i].m_Pos.m_Z * 10.0f) / (float)clientDepth);

                // draw the star
                ::Ellipse(hDC, starRect.left, starRect.top, starRect.right, starRect.bottom);

                // release star brush
                ::SelectObject(hDC, hOldBrush);
                ::DeleteObject(hStarBrush);
            }

            // slow down the rendering to avoid processor overhead
            ::Sleep(1);
        }
    }

    // release the GDI objects
    ::DeleteObject(hBrush);
    ::ReleaseDC(hWnd, hDC);

    // destroy the window explicitly
    ::DestroyWindow(hWnd);

    return (int)msg.wParam;
}
//------------------------------------------------------------------------------
