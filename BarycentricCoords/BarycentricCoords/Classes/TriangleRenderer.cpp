/****************************************************************************
 * ==> TriangleRenderer ----------------------------------------------------*
 ****************************************************************************
 * Description: Triangle renderer (using GDI)                               *
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

#include "TriangleRenderer.h"

using namespace Rasterizer;

//------------------------------------------------------------------------------
TriangleRenderer::TriangleRenderer(HDC hDC, int width, int height):
    m_hBitmap(nullptr),
    m_hMemDC(nullptr),
    m_pPixels(nullptr),
    m_Width(width),
    m_Height(height)
{
    // create memory DC
    m_hMemDC = ::CreateCompatibleDC(hDC);

    BITMAPINFO bmi              =  {};
    bmi.bmiHeader.biSize        =  sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       =  width;
    bmi.bmiHeader.biHeight      = -height;
    bmi.bmiHeader.biPlanes      =  1;
    bmi.bmiHeader.biBitCount    =  32;
    bmi.bmiHeader.biCompression =  BI_RGB;

    // create DIB
    m_hBitmap = ::CreateDIBSection(m_hMemDC, &bmi, DIB_RGB_COLORS, (void**)&m_pPixels, nullptr, 0);

    if (m_hBitmap)
        ::SelectObject(m_hMemDC, m_hBitmap);
}
//------------------------------------------------------------------------------
TriangleRenderer::~TriangleRenderer()
{
    if (m_hBitmap)
        ::DeleteObject(m_hBitmap);

    if (m_hMemDC)
        ::DeleteDC(m_hMemDC);
}
//------------------------------------------------------------------------------
HDC TriangleRenderer::GetDC() const
{
    return m_hMemDC;
}
//------------------------------------------------------------------------------
void TriangleRenderer::Clear(COLORREF color)
{
    DWORD dwColor = ((color & 0xFF) << 16) | (color & 0xFF00) | ((color >> 16) & 0xFF);
    std::fill(m_pPixels, m_pPixels + (DWORD)((std::int64_t)m_Width * (std::int64_t)m_Height), dwColor);
}
//------------------------------------------------------------------------------
void TriangleRenderer::RenderTriangle(const Geometry::Triangle& triangle)
{
    // get triangle bounding rectangle
    const Geometry::Rect boundingRect = triangle.GetBoundingRect();
    int                  minX         = std::max(0,        (int)boundingRect.m_Min.m_X);
    int                  minY         = std::max(0,        (int)boundingRect.m_Min.m_Y);
    int                  maxX         = std::min(m_Width,  (int)boundingRect.m_Max.m_X);
    int                  maxY         = std::min(m_Height, (int)boundingRect.m_Max.m_Y);

    // iterate through each pixel covered by the bounding rectangle
    for (int y = minY; y < maxY; ++y)
        for (int x = minX; x < maxX; ++x)
        {
            // get point coordinate
            Math::Vector2F               point((float)x, (float)y);
            Geometry::Triangle::IWeights weights;

            // check if the point is inside the rectangle
            if (triangle.BarycentricInside(point, weights))
            {
                // calculate the pixel color based on weights
                const BYTE r = (BYTE)(255 * weights.m_W0);
                const BYTE g = (BYTE)(255 * weights.m_W1);
                const BYTE b = (BYTE)(255 * weights.m_W2);

                // set the pixel in the renderer buffer
                m_pPixels[y * m_Width + x] = (b << 16) | (g << 8) | r;
            }
        }
}
//------------------------------------------------------------------------------
void TriangleRenderer::Present(HDC hDC)
{
    ::BitBlt(hDC, 0, 0, m_Width, m_Height, m_hMemDC, 0, 0, SRCCOPY);
}
//------------------------------------------------------------------------------
