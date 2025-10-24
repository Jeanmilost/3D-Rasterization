/****************************************************************************
 * ==> OpenGL --------------------------------------------------------------*
 ****************************************************************************
 * Description: OpenGL hardware renderer                                    *
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
#include "WaveFront.h"

// windows
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

// opengl
#include <gl/GL.h>

namespace OpenGL
{
    /**
    * OpenGL context
    *@author Jean-Milost Reymond
    */
    class Context
    {
        public:
            Context();
            virtual ~Context();

            /**
            * Initializes OpenGL context
            *@param hDC - Windows device context
            *@return true on success, otherwise false
            */
            bool Initialize(HDC hDC);

            /**
            * Cleans up OpenGL context
            */
            void Shutdown();

            /**
            * Makes this context current for rendering
            */
            void MakeCurrent() const;

            /**
            * Swaps buffers to display rendered frame
            */
            void SwapBuffers() const;

            /**
            * Checks if context is initialized
            */
            inline bool IsInitialized() const;

        private:
            HDC   m_hDC;
            HGLRC m_hRC;
            bool  m_Initialized;

            /**
            * Sets up initial OpenGL state
            */
            void InitializeOpenGLSettings();
    };

    //---------------------------------------------------------------------------
    // Context
    //---------------------------------------------------------------------------
    inline bool Context::IsInitialized() const
    {
        return m_Initialized;
    }
    //---------------------------------------------------------------------------

    /**
    * OpenGL renderer
    *@author Jean-Milost Reymond
    */
    class Renderer
    {
        public:
            Renderer();
            virtual ~Renderer();

            /**
            * Loads texture from bitmap data
            *@param data - raw RGBA bitmap data (unsigned char*)
            *@param width - texture width
            *@param height - texture height
            */
            void LoadTexture(unsigned char* data, int width, int height);

            /**
            * Renders the mesh
            * @param mesh The mesh to render
            */
            void Render(const Model::WaveFront::IMesh& mesh) const;

        private:
            GLuint m_TextureID;
            bool   m_HasTexture;
    };

    /**
    * Sets up the viewport
    *@param hWnd - Windows handle
    */
    void SetupViewport(HWND hWnd);
}
