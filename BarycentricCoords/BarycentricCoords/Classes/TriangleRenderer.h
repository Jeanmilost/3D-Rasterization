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

#pragma once

// classes
#include "Triangle.h"
#include "Rect.h"

// windows
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

namespace Rasterizer
{
    /**
    * Triangle renderer
    *@author Jean-Milost Reymond
    */
    class TriangleRenderer
    {
        public:
            /**
            * Constructor
            *@param hDC - canvas device context
            *@param width - canvas width
            *@param height - canvas height
            */
            TriangleRenderer(HDC hDC, int width, int height);

            virtual ~TriangleRenderer();

            /**
            * Gets the render buffer device context
            *@return the render buffer device context
            */
            HDC GetDC() const;

            /**
            * Clears the renderer buffer
            *@param color - fill color
            */
            void Clear(COLORREF color);

            /**
            * Renders the triangle onto the renderer buffer
            *@param triangle - triangle to draw
            */
            void RenderTriangle(const Geometry::Triangle& triangle);

            /**
            * Presents the triangle on the target canvas
            *@param hDC - target canvas device context
            */
            void Present(HDC hDC);

        private:
            HBITMAP m_hBitmap;
            HDC     m_hMemDC;
            DWORD*  m_pPixels;
            int     m_Width;
            int     m_Height;
    };
}
