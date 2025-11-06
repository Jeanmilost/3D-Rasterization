/****************************************************************************
 * ==> Software rasterizer -------------------------------------------------*
 ****************************************************************************
 * Description: Minimal software rasterizer demo                            *
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
#define _USE_MATH_DEFINES
#include <math.h>

// classes
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Texture.h"
#include "WaveFront.h"
#include "OpenGL.h"
#include "SoftwareRenderer.h"

// windows
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

// resources
#include "Resource.h"

//------------------------------------------------------------------------------
HDC     g_TextBufferDC     = nullptr;
HBITMAP g_TextBufferBitmap = nullptr;
RECT    g_TextRect         = { 10, 10, 400, 150 };
bool    g_UseOpenGL        = false;
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
                case '1':
                    g_UseOpenGL = false;
                    break;

                case '2':
                    g_UseOpenGL = true;
                    break;

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
void InitializeTextBuffer(HDC hdc)
{
    int width  = g_TextRect.right  - g_TextRect.left;
    int height = g_TextRect.bottom - g_TextRect.top;

    g_TextBufferDC = ::CreateCompatibleDC(hdc);

    // create a 32-bit bitmap for alpha channel support
    BITMAPINFO bmi              =  {};
    bmi.bmiHeader.biSize        =  sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       =  width;
    bmi.bmiHeader.biHeight      = -height;
    bmi.bmiHeader.biPlanes      =  1;
    bmi.bmiHeader.biBitCount    =  32;
    bmi.bmiHeader.biCompression =  BI_RGB;

    void* pBits;
    g_TextBufferBitmap = ::CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);

    if (!g_TextBufferBitmap)
        return;

    ::SelectObject(g_TextBufferDC, g_TextBufferBitmap);

    // clear to transparent (all zeros = fully transparent)
    std::memset(pBits, 0, (std::size_t)width * (std::size_t)height * 4);

    // create the text font
    HFONT hFont = ::CreateFont(14,
                                0,
                                0,
                                0,
                                FW_BOLD,
                                FALSE,
                                FALSE,
                                FALSE,
                                DEFAULT_CHARSET,
                                OUT_DEFAULT_PRECIS,
                                CLIP_DEFAULT_PRECIS,
                                CLEARTYPE_QUALITY,
                                DEFAULT_PITCH | FF_DONTCARE,
                                L"Arial");

    HFONT hOldFont = (HFONT)::SelectObject(g_TextBufferDC, hFont);

    // set up text rendering, use yellow text for visibility
    ::SetBkMode   (g_TextBufferDC, TRANSPARENT);
    ::SetTextColor(g_TextBufferDC, RGB(255, 255, 0));

    RECT textRect = { 0, 0, width, height };

    // draw text
    ::DrawText(g_TextBufferDC,
               L"Press 1: Software Renderer\r\nPress 2: OpenGL Renderer\r\nPress ESC: Exit",
              -1,
               &textRect,
               DT_LEFT | DT_TOP);

    BYTE* pixels = (BYTE*)pBits;

    // manually set alpha channel for text pixels
    for (int i = 0; i < width * height; ++i)
    {
        BYTE b = pixels[i * 4 + 0];
        BYTE g = pixels[i * 4 + 1];
        BYTE r = pixels[i * 4 + 2];

        // if pixel has color (not black), set full opacity
        if (r > 0 || g > 0 || b > 0)
        {
            // full alpha
            pixels[i * 4 + 3] = 255;

            // pre-multiply alpha
            pixels[i * 4 + 0] = b;
            pixels[i * 4 + 1] = g;
            pixels[i * 4 + 2] = r;
        }
    }

    ::SelectObject(g_TextBufferDC, hOldFont);
    ::DeleteObject(hFont);
}
//------------------------------------------------------------------------------
void BlitOverlayText(HDC hDC)
{
    int width  = g_TextRect.right  - g_TextRect.left;
    int height = g_TextRect.bottom - g_TextRect.top;

    BLENDFUNCTION blend       = {};
    blend.BlendOp             = AC_SRC_OVER;
    blend.BlendFlags          = 0;
    blend.SourceConstantAlpha = 255;          // full opacity
    blend.AlphaFormat         = AC_SRC_ALPHA; // use per-pixel alpha

    ::AlphaBlend(hDC,
                 g_TextRect.left, g_TextRect.top,
                 width, height,
                 g_TextBufferDC,
                 0, 0,
                 width, height,
                 blend);
}
//------------------------------------------------------------------------------
void CleanupTextBuffer()
{
    if (g_TextBufferBitmap)
    {
        ::DeleteObject(g_TextBufferBitmap);
        g_TextBufferBitmap = nullptr;
    }

    if (g_TextBufferDC)
    {
        ::DeleteDC(g_TextBufferDC);
        g_TextBufferDC = nullptr;
    }
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
    wcex.lpszClassName = L"softwareRasterizer";

    if (!RegisterClassEx(&wcex))
        return 0;

    // create main window
    hWnd = ::CreateWindowEx(0,
                            L"softwareRasterizer",
                            L"Software Rasterizer",
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

    // get window device context
    HDC hDC = ::GetDC(hWnd);

    InitializeTextBuffer(hDC);

    OpenGL::Context context;

    // initialize the opengl renderer
    if (!context.Initialize(hDC))
    {
        ::MessageBox(hWnd, L"Failed to initialize OpenGL", L"Error", MB_OK);
        return 1;
    }

    // set up viewport and projection
    OpenGL::SetupViewport(hWnd);

    // load the WaveFront model
    Model::WaveFront::IMesh mesh = Model::WaveFront::Load("..\\..\\Assets\\Models\\Cat\\model.obj");

    OpenGL::Renderer     openGLRenderer;
    Rasterizer::Renderer softwareRenderer;

    // initialize the software renderer
    softwareRenderer.Initialize(hWnd, hDC);
    softwareRenderer.SetProjection();

    Texture::Loader loader;
    int             width, height;
    unsigned char*  pixels = loader.FromJPEG(L"..\\..\\Assets\\Models\\Cat\\texture.jpg", width, height);

    if (pixels)
    {
        // use the pixel data
        openGLRenderer.LoadTexture(pixels, width, height);
        softwareRenderer.LoadTexture(pixels, width, height, 4);

        // clean up when done
        delete[] pixels;
    }

    float  angle    = 0.0f;
    double lastTime = (double)::GetTickCount64();

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

            if (g_UseOpenGL)
            {
                context.MakeCurrent();

                // clear buffers
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // reset model view matrix
                glMatrixMode(GL_MODELVIEW);

                // calculate model position and rotation
                Math::Matrix4x4F model =  Math::Matrix4x4F::Identity();
                model.m_Table[3][2]    = -250.0f;
                model.Rotate(angle, Math::Vector3F(0.0f, 1.0f, 0.0f));

                // calculate next rotation
                angle = std::fmodf(angle + 0.05f, (float)M_PI * 2.0f);

                // set model matrix
                glLoadMatrixf(model.GetPtr());

                // set up camera (looking at the model)
                glTranslatef(0.0f, 0.0f, 0.0f);

                // render the mesh
                openGLRenderer.Render(mesh);

                // swap buffers to display
                context.SwapBuffers();
            }
            else
            {
                // clear buffers
                softwareRenderer.Clear(0xFF333333);

                // calculate model position and rotation
                Math::Matrix4x4F model =  Math::Matrix4x4F::Identity();
                model.m_Table[3][2]    = -250.0f;
                model.Rotate(angle, Math::Vector3F(0.0f, 1.0f, 0.0f));

                // calculate next rotation
                angle = std::fmodf(angle + 0.05f, (float)M_PI * 2.0f);

                // set model matrix
                softwareRenderer.SetModel(model);

                // render the mesh
                softwareRenderer.Render(mesh);

                // swap buffers to display
                softwareRenderer.SwapBuffers();
            }

            BlitOverlayText(hDC);

            // slow down the rendering to avoid processor overhead
            ::Sleep(1);
        }
    }

    context.Shutdown();
    CleanupTextBuffer();

    // destroy the window explicitly
    ::DestroyWindow(hWnd);

    return (int)msg.wParam;
}
//------------------------------------------------------------------------------
