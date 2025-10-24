/****************************************************************************
 * ==> SoftwareRenderer ----------------------------------------------------*
 ****************************************************************************
 * Description: Software renderer                                           *
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
#include "Matrix4x4.h"
#include "WaveFront.h"

// windows
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

namespace Rasterizer
{
    /**
    * Software renderer
    *@author Jean-Milost Reymond
    */
    class Renderer
    {
        public:
            Renderer();
            virtual ~Renderer();

            /**
            * Initializes OpenGL context
            *@param hWnd - Windows handle
            *@param hDC - Windows device context
            *@return true on success, otherwise false
            */
            bool Initialize(HWND hWnd, HDC hDC);

            /**
            * Sets the projection matrix
            */
            void SetProjection();

            /**
            * Sets the model matrix
            *@param model - the model matrix to set
            */
            void SetModel(const Math::Matrix4x4F& model);

            /**
            * Loads texture from bitmap data
            *@param data - raw RGBA bitmap data (unsigned char*)
            *@param width - texture width
            *@param height - texture height
            */
            void LoadTexture(unsigned char* data, int width, int height);

            /**
            * Makes this context current for rendering
            */
            void MakeCurrent() const;

            /**
            * Clears the renderer buffer
            *@param color - fill color
            */
            void Clear(COLORREF color) const;

            /**
            * Renders the mesh
            * @param mesh The mesh to render
            */
            void Render(const Model::WaveFront::IMesh& mesh) const;

            /**
            * Swaps buffers to display rendered frame
            */
            void SwapBuffers() const;

        private:
            Math::Matrix4x4F m_Projection;
            Math::Matrix4x4F m_Model;
            HWND             m_hWnd        = nullptr;
            HDC              m_hDC         = nullptr;
            HDC              m_hMemDC      = nullptr;
            HBITMAP          m_hCanvas     = nullptr;
            DWORD*           m_pPixels     = nullptr;
            float*           m_pZBuffer    = nullptr;
            int              m_Width       = 0;
            int              m_Height      = 0;
            bool             m_HasTexture  = false;
            bool             m_Initialized = false;
    };
}
