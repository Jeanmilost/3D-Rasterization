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

#include "OpenGL.h"

// classes
#include "Matrix4x4.h"

using namespace OpenGL;

//---------------------------------------------------------------------------
// Context
//---------------------------------------------------------------------------
Context::Context() :
    m_hDC(nullptr),
    m_hRC(nullptr),
    m_Initialized(false) {}
//---------------------------------------------------------------------------
Context::~Context()
{
    // name explicitly the namespace in order to avoid pure virtual issues on destruction
    Context::Shutdown();
}
//---------------------------------------------------------------------------
bool Context::Initialize(HDC hDC)
{
    m_hDC = hDC;

    // set up pixel format descriptor
    PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32,                 // color depth
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,                 // depth buffer bits
        8,                  // stencil buffer bits
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

    // choose pixel format
    int pixelFormat = ::ChoosePixelFormat(m_hDC, &pfd);

    if (!pixelFormat)
        return false;

    // set pixel format
    if (!::SetPixelFormat(m_hDC, pixelFormat, &pfd))
        return false;

    // create OpenGL rendering context
    m_hRC = wglCreateContext(m_hDC);

    if (!m_hRC)
        return false;

    // make it current
    if (!wglMakeCurrent(m_hDC, m_hRC))
    {
        wglDeleteContext(m_hRC);
        m_hRC = nullptr;
        return false;
    }

    m_Initialized = true;

    // initialize OpenGL settings
    InitializeOpenGLSettings();

    return true;
}
//---------------------------------------------------------------------------
void Context::Shutdown()
{
    if (m_hRC)
    {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(m_hRC);
        m_hRC = nullptr;
    }

    m_Initialized = false;
}
//---------------------------------------------------------------------------
void Context::MakeCurrent() const
{
    if (!m_Initialized)
        return;

    wglMakeCurrent(m_hDC, m_hRC);
}
//---------------------------------------------------------------------------
void Context::SwapBuffers() const
{
    if (!m_Initialized)
        return;

    ::SwapBuffers(m_hDC);
}
//---------------------------------------------------------------------------
void Context::InitializeOpenGLSettings()
{
    // set clear color (background)
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    // enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // enable back face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // smooth shading
    glShadeModel(GL_SMOOTH);

    // nice perspective calculations
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}
//---------------------------------------------------------------------------
// Renderer
//---------------------------------------------------------------------------
Renderer::Renderer() :
    m_TextureID(0),
    m_HasTexture(false)
{}
//---------------------------------------------------------------------------
Renderer::~Renderer()
{
    if (m_HasTexture)
        glDeleteTextures(1, &m_TextureID);
}
//---------------------------------------------------------------------------
void Renderer::LoadTexture(unsigned char* data, int width, int height)
{
    if (m_HasTexture)
        glDeleteTextures(1, &m_TextureID);

    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    m_HasTexture = true;
}
//---------------------------------------------------------------------------
void Renderer::Render(const Model::WaveFront::IMesh& mesh) const
{
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
}
//---------------------------------------------------------------------------
// Namespace functions
//---------------------------------------------------------------------------
void OpenGL::SetupViewport(HWND hWnd)
{
    RECT rect;
    ::GetClientRect(hWnd, &rect);

    const int width  = rect.right  - rect.left;
    const int height = rect.bottom - rect.top;

    glViewport(0, 0, width, height);

    // set up projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    const float aspect    = (float)width / (float)height;
    const float fov       = 45.0f;
    const float nearPlane = 0.1f;
    const float farPlane  = 1000.0f;

    // perspective projection
    const float f = 1.0f / std::tanf(fov * 0.5f * 3.14159f / 180.0f);

    Math::Matrix4x4F projection =  Math::Matrix4x4F::Identity();
    projection.m_Table[0][0]    =  f / aspect;
    projection.m_Table[1][1]    =  f;
    projection.m_Table[2][2]    = (farPlane + nearPlane) / (nearPlane - farPlane);
    projection.m_Table[2][3]    = -1.0f;
    projection.m_Table[3][2]    = (2.0f * farPlane * nearPlane) / (nearPlane - farPlane);

    glLoadMatrixf(projection.GetPtr());

    // set up model view matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
//---------------------------------------------------------------------------
