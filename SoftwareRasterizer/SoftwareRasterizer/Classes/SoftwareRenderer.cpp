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

#include "SoftwareRenderer.h"

using namespace Rasterizer;

//---------------------------------------------------------------------------
// Renderer
//---------------------------------------------------------------------------
Renderer::Renderer()
{
    m_Projection = Math::Matrix4x4F::Identity();
}
//---------------------------------------------------------------------------
Renderer::~Renderer()
{
    if (m_pZBuffer)
        delete[] m_pZBuffer;

    if (m_hCanvas)
        ::DeleteObject(m_hCanvas);

    if (m_hMemDC)
        ::DeleteDC(m_hMemDC);
}
//---------------------------------------------------------------------------
bool Renderer::Initialize(HWND hWnd, HDC hDC)
{
    m_Initialized = false;

    if (!hWnd || !hDC)
        return false;

    m_hWnd = hWnd;
    m_hDC  = hDC;

    RECT rect;
    ::GetClientRect(hWnd, &rect);

    // calculate the canvas width and height
    m_Width  = rect.right  - rect.left;
    m_Height = rect.bottom - rect.top;

    if (!m_Width || !m_Height)
        return false;

    // create memory DC
    m_hMemDC = ::CreateCompatibleDC(hDC);

    if (!m_hMemDC)
        return false;

    BITMAPINFO bmi              =  {};
    bmi.bmiHeader.biSize        =  sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       =  m_Width;
    bmi.bmiHeader.biHeight      = -m_Height;
    bmi.bmiHeader.biPlanes      =  1;
    bmi.bmiHeader.biBitCount    =  32;
    bmi.bmiHeader.biCompression =  BI_RGB;

    // create the canvas
    m_hCanvas = ::CreateDIBSection(m_hMemDC, &bmi, DIB_RGB_COLORS, (void**)&m_pPixels, nullptr, 0);

    if (!m_hCanvas)
        return false;

    // create the z buffer
    m_pZBuffer = new float[(std::size_t)m_Width * (std::size_t)m_Height];

    m_Initialized = true;

    // make this context the current one (need to be called after m_Initialized is set to true)
    MakeCurrent();

    return true;
}
//---------------------------------------------------------------------------
void Renderer::SetProjection()
{
    const float aspect    = (float)m_Width / (float)m_Height;
    const float fov       = 45.0f;
    const float nearPlane = 0.1f;
    const float farPlane  = 1000.0f;

    // perspective projection
    const float f = 1.0f / std::tanf(fov * 0.5f * 3.14159f / 180.0f);

    m_Projection               =  Math::Matrix4x4F::Identity();
    m_Projection.m_Table[0][0] =  f / aspect;
    m_Projection.m_Table[1][1] =  f;
    m_Projection.m_Table[2][2] = (farPlane + nearPlane) / (nearPlane - farPlane);
    m_Projection.m_Table[2][3] = -1.0f;
    m_Projection.m_Table[3][2] = (2.0f * farPlane * nearPlane) / (nearPlane - farPlane);
}
//---------------------------------------------------------------------------
void Renderer::SetModel(const Math::Matrix4x4F& model)
{
    m_Model = model;
}
//---------------------------------------------------------------------------
void Renderer::MakeCurrent() const
{
    if (!m_Initialized)
        return;

    if (m_hCanvas)
        ::SelectObject(m_hMemDC, m_hCanvas);
}
//---------------------------------------------------------------------------
void Renderer::LoadTexture(unsigned char* data, int width, int height)
{
    /*REM
    if (m_HasTexture)
        glDeleteTextures(1, &m_TextureID);

    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    */

    m_HasTexture = true;
}
//---------------------------------------------------------------------------
void Renderer::Clear(COLORREF color) const
{
    // clear the canvas
    const DWORD dwColor = ((color & 0xFF) << 16) | (color & 0xFF00) | ((color >> 16) & 0xFF);
    std::fill(m_pPixels, m_pPixels + ((std::size_t)m_Width * (std::size_t)m_Height), dwColor);

    // clear the z buffer
    std::memset(m_pZBuffer, 0x0, (std::size_t)m_Width * (std::size_t)m_Height * sizeof(float));
}
//---------------------------------------------------------------------------
void Renderer::Render(const Model::WaveFront::IMesh& mesh) const
{
    /*REM
    // enable required features
    glEnable(GL_DEPTH_TEST);

    if (m_HasTexture)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
    }

    // render each face
    for (const auto& face : mesh.m_Faces)
    {
        // determine primitive type based on vertex count
        GLenum mode;

        if (face.m_VertexIndices.size() == 3)
            mode = GL_TRIANGLES;
        else
        if (face.m_VertexIndices.size() == 4)
            mode = GL_QUADS;
        else
            mode = GL_POLYGON;

        glBegin(mode);

        for (std::size_t i = 0; i < face.m_VertexIndices.size(); ++i)
        {
            // set texture coordinate if available
            if (!face.m_TexCoordIndices.empty() && face.m_TexCoordIndices[i] < mesh.m_TexCoords.size())
            {
                const auto& tc = mesh.m_TexCoords[face.m_TexCoordIndices[i]];
                glTexCoord2f(tc.m_X, tc.m_Y);
            }

            // set normal if available
            if (!face.m_NormalIndices.empty() && face.m_NormalIndices[i] < mesh.m_Normals.size())
            {
                const auto& n = mesh.m_Normals[face.m_NormalIndices[i]];
                glNormal3f(n.m_X, n.m_Y, n.m_Z);
            }

            // set vertex position
            if (face.m_VertexIndices[i] < mesh.m_Vertices.size())
            {
                const auto& v = mesh.m_Vertices[face.m_VertexIndices[i]];
                glVertex3f(v.m_X, v.m_Y, v.m_Z);
            }
        }

        glEnd();
    }

    if (m_HasTexture)
        glDisable(GL_TEXTURE_2D);
    */
}
//---------------------------------------------------------------------------
void Renderer::SwapBuffers() const
{
    if (!m_Initialized)
        return;

    ::BitBlt(m_hDC, 0, 0, m_Width, m_Height, m_hMemDC, 0, 0, SRCCOPY);
}
//---------------------------------------------------------------------------
