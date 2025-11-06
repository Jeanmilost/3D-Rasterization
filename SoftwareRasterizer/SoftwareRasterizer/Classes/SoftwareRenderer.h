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
#include "Polygon.h"
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
            /**
            * Culling type
            */
            enum class IECullingType
            {
                None,
                Front,
                Back,
                Both
            };

            /**
            * Culling face
            */
            enum class IECullingFace
            {
                CW,
                CCW
            };

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
            * Sets the view matrix
            *@param view - the view matrix to set
            */
            void SetView(const Math::Matrix4x4F& view);

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
            *@param bpp - byte per pixels
            */
            void LoadTexture(unsigned char* data, std::size_t width, std::size_t height, std::size_t bpp);

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
            Math::Matrix4x4F m_View;
            Math::Matrix4x4F m_Model;
            IECullingType    m_CullingType = IECullingType::Back;
            IECullingFace    m_CullingFace = IECullingFace::CW;
            RECT             m_ScreenRect  = { 0 };
            HWND             m_hWnd        = nullptr;
            HDC              m_hDC         = nullptr;
            HDC              m_hMemDC      = nullptr;
            HBITMAP          m_hCanvas     = nullptr;
            unsigned char*   m_pTexture    = nullptr;
            DWORD*           m_pPixels     = nullptr;
            float*           m_pZBuffer    = nullptr;
            float            m_Near        = 0.1f;
            float            m_Far         = 1000.0f;
            std::size_t      m_TexWidth    = 0;
            std::size_t      m_TexHeight   = 0;
            std::size_t      m_TexBPP      = 0;
            std::size_t      m_Width       = 0;
            std::size_t      m_Height      = 0;
            bool             m_HasTexture  = false;
            bool             m_Initialized = false;

            /**
            * Transform a vertex into screen coordinates
            *@param vertex - input vertex
            *@param matrix - matrix
            *@return output vertex
            */
            Math::Vector3F TransformVertex(const Math::Vector3F&   vertex,
                                           const Math::Matrix4x4F& matrix) const;

            /**
            * Draws a polygon
            *@param polygon - polygon
            *@param normal - polygon normal (array of 3 items)
            *@param st - polygon texture coordinates (array of 3 items)
            *@param matrix - matrix
            *@return true on success, otherwise false
            */
            bool DrawPolygon(const Geometry::Polygon&           polygon,
                             const std::vector<Math::Vector3F>& normal,
                             const std::vector<Math::Vector2F>& st,
                             const Math::Matrix4x4F&            matrix) const;
    };
}
