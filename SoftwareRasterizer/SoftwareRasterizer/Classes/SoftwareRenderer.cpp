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

// classes
#include "Triangle.h"

using namespace Rasterizer;

//---------------------------------------------------------------------------
// Renderer
//---------------------------------------------------------------------------
Renderer::Renderer()
{
    m_Projection = Math::Matrix4x4F::Identity();
    m_View       = Math::Matrix4x4F::Identity();
    m_Model      = Math::Matrix4x4F::Identity();
}
//---------------------------------------------------------------------------
Renderer::~Renderer()
{
    if (m_pTexture)
        delete[] m_pTexture;

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

    ::GetClientRect(hWnd, &m_ScreenRect);

    // calculate the canvas width and height
    m_Width  = (std::size_t)m_ScreenRect.right  - (std::size_t)m_ScreenRect.left;
    m_Height = (std::size_t)m_ScreenRect.bottom - (std::size_t)m_ScreenRect.top;

    if (!m_Width || !m_Height)
        return false;

    // create memory DC
    m_hMemDC = ::CreateCompatibleDC(hDC);

    if (!m_hMemDC)
        return false;

    BITMAPINFO bmi              =  {};
    bmi.bmiHeader.biSize        =  sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       =  (int)m_Width;
    bmi.bmiHeader.biHeight      = -(int)m_Height;
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

    // store near/far values
    m_Near = nearPlane;
    m_Far  = farPlane;

    // perspective projection
    const float f = 1.0f / std::tanf(fov * 0.5f * 3.14159f / 180.0f);

    m_Projection               =  Math::Matrix4x4F::Identity();
    m_Projection.m_Table[0][0] =  f / aspect;
    m_Projection.m_Table[1][1] =  f;
    m_Projection.m_Table[2][2] = (farPlane + nearPlane) / (nearPlane - farPlane);
    m_Projection.m_Table[2][3] = -1.0f;
    m_Projection.m_Table[3][2] = (2.0f * farPlane * nearPlane) / (nearPlane - farPlane);
    m_Projection.m_Table[3][3] =  0.0f;
}
//---------------------------------------------------------------------------
void Renderer::SetView(const Math::Matrix4x4F& view)
{
    m_View = view;
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
void Renderer::LoadTexture(unsigned char* data, std::size_t width, std::size_t height, std::size_t bpp)
{
    if (m_HasTexture)
        delete[] m_pTexture;

    m_TexWidth  = (std::size_t)width;
    m_TexHeight = (std::size_t)height;
    m_TexBPP    = bpp;

    // calculate the texture buffer size
    const std::size_t texSize = m_TexWidth * m_TexHeight * m_TexBPP;

    // create the texture buffer and copy the whole content from source
    m_pTexture = new unsigned char[texSize];
    std::memcpy(m_pTexture, data, texSize);

    m_HasTexture = true;
}
//---------------------------------------------------------------------------
void Renderer::Clear(COLORREF color) const
{
    // clear the canvas
    const DWORD dwColor = ((color & 0xFF) << 16) | (color & 0xFF00) | ((color >> 16) & 0xFF);
    std::fill(m_pPixels, m_pPixels + ((std::size_t)m_Width * (std::size_t)m_Height), dwColor);

    // clear the z buffer
    std::fill(m_pZBuffer, m_pZBuffer + ((std::size_t)m_Width * (std::size_t)m_Height), m_Far);
}
//---------------------------------------------------------------------------
void Renderer::Render(const Model::WaveFront::IMesh& mesh) const
{
    // calculate the render matrix (projection * view * model)
    const Math::Matrix4x4F matrix = m_Model.Multiply(m_View).Multiply(m_Projection);

    // iterate through model faces to draw
    for (const auto& face : mesh.m_Faces)
    {
        // for now only triangles are supported
        if (face.m_VertexIndices.size() != 3)
            continue;

        Geometry::Polygon           polygon;
        std::vector<Math::Vector3F> normal;
        std::vector<Math::Vector2F> st;

        normal.resize(face.m_VertexIndices.size());
        st.resize(face.m_VertexIndices.size());

        // for each face, iterate through its vertices
        for (std::size_t i = 0; i < face.m_VertexIndices.size(); ++i)
        {
            // set texture coordinate if available
            if (!face.m_TexCoordIndices.empty() && face.m_TexCoordIndices[i] < mesh.m_TexCoords.size())
                st[i] = mesh.m_TexCoords[face.m_TexCoordIndices[i]];

            // set normal if available
            if (!face.m_NormalIndices.empty() && face.m_NormalIndices[i] < mesh.m_Normals.size())
                normal[i] = mesh.m_Normals[face.m_NormalIndices[i]];

            // set vertex position
            if (face.m_VertexIndices[i] < mesh.m_Vertices.size())
                polygon.m_Vertex[i] = mesh.m_Vertices[face.m_VertexIndices[i]];
        }

        DrawPolygon(polygon, normal, st, matrix);
    }
}
//---------------------------------------------------------------------------
void Renderer::SwapBuffers() const
{
    if (!m_Initialized)
        return;

    ::BitBlt(m_hDC, 0, 0, (int)m_Width, (int)m_Height, m_hMemDC, 0, 0, SRCCOPY);
}
//---------------------------------------------------------------------------
Math::Vector3F Renderer::TransformVertex(const Math::Vector3F&   vertex,
                                         const Math::Matrix4x4F& matrix) const
{
    // transform to clip space (4D homogeneous coordinates). Need to treat this as a 4D vector with w = 1
    const Math::Vector3F transformed = matrix.Transform(vertex);
          Math::Vector3F ndc;

    // perspective divide, convert from clip space to NDC (Normalized Device Coordinates)
    ndc.m_X = transformed.m_X / transformed.m_Z;
    ndc.m_Y = transformed.m_Y / transformed.m_Z;
    ndc.m_Z = transformed.m_Z / transformed.m_Z;

    // convert from NDC [-1, 1] to screen space [0, width/height]
    Math::Vector3F screen;
    screen.m_X = (ndc.m_X + 1.0f) * 0.5f * (float)m_Width;
    screen.m_Y = (1.0f - ndc.m_Y) * 0.5f * (float)m_Height; // flip Y
    screen.m_Z = ndc.m_Z;                                   // keep depth for z-buffer

    return screen;
}
//---------------------------------------------------------------------------
bool Renderer::DrawPolygon(const Geometry::Polygon&           polygon,
                           const std::vector<Math::Vector3F>& normal,
                           const std::vector<Math::Vector2F>& st,
                           const Math::Matrix4x4F&            matrix) const
{
    // transform vertices to screen space
    Geometry::Polygon rasterPoly(TransformVertex(polygon.m_Vertex[0], matrix),
                                 TransformVertex(polygon.m_Vertex[1], matrix),
                                 TransformVertex(polygon.m_Vertex[2], matrix));

    /*
    // check if the polygon is culled and determine the culling mode to use (0 = CW, 1 = CCW, 2 = both)
    switch (m_CullingType)
    {
        case IECullingType::None:
            // both faces are accepted
            break;

        case IECullingType::Front:
        case IECullingType::Back:
        {
            // calculate the rasterized polygon plane
            const Geometry::PlaneF polygonPlane = rasterPoly.GetPlane();
                  Math::Vector3F   polygonNormal;

            // calculate the rasterized polygon surface normal
            polygonNormal.m_X = polygonPlane.m_A;
            polygonNormal.m_Y = polygonPlane.m_B;
            polygonNormal.m_Z = polygonPlane.m_C;

            Math::Vector3F cullingNormal;

            // get the culling normal
            cullingNormal.m_X =  0.0f;
            cullingNormal.m_Y =  0.0f;
            cullingNormal.m_Z = -1.0f;

            // calculate the dot product between the culling and the polygon normal
            const float cullingDot = polygonNormal.Dot(cullingNormal);

            switch (m_CullingFace)
            {
                case IECullingFace::CW:
                    // is polygon rejected?
                    if (cullingDot <= 0.0f)
                        return true;

                    break;

                case IECullingFace::CCW:
                    // is polygon rejected?
                    if (cullingDot >= 0.0f)
                        return true;

                    break;

                // error
                default:
                    return true;
            }

            break;
        }

        case IECullingType::Both:
        default:
            // both faces are rejected
            return true;
    }
    */
    switch (m_CullingType)
    {
        case IECullingType::None:
            break;

        case IECullingType::Front:
        case IECullingType::Back:
        {
            // use 2D cross product for screen-space culling
            const float edge1X = rasterPoly.m_Vertex[1].m_X - rasterPoly.m_Vertex[0].m_X;
            const float edge1Y = rasterPoly.m_Vertex[1].m_Y - rasterPoly.m_Vertex[0].m_Y;
            const float edge2X = rasterPoly.m_Vertex[2].m_X - rasterPoly.m_Vertex[0].m_X;
            const float edge2Y = rasterPoly.m_Vertex[2].m_Y - rasterPoly.m_Vertex[0].m_Y;
            const float crossZ = edge1X * edge2Y - edge1Y * edge2X;

            switch (m_CullingFace)
            {
                case IECullingFace::CCW:
                    if (crossZ <= 0.0f)
                        return true;

                    break;

                case IECullingFace::CW:
                    if (crossZ >= 0.0f)
                        return true;

                    break;

                default:
                    return true;
            }

            break;
        }

        case IECullingType::Both:
        default:
            return true;
    }

    // invert original depth values for perspective-correct interpolation
    rasterPoly.m_Vertex[0].m_Z = 1.0f / rasterPoly.m_Vertex[0].m_Z;
    rasterPoly.m_Vertex[1].m_Z = 1.0f / rasterPoly.m_Vertex[1].m_Z;
    rasterPoly.m_Vertex[2].m_Z = 1.0f / rasterPoly.m_Vertex[2].m_Z;

    Geometry::Triangle triangle;

    // setup triangle for rasterization
    triangle.m_Vertex[0] = Math::Vector2F(rasterPoly.m_Vertex[0].m_X, rasterPoly.m_Vertex[0].m_Y);
    triangle.m_Vertex[1] = Math::Vector2F(rasterPoly.m_Vertex[1].m_X, rasterPoly.m_Vertex[1].m_Y);
    triangle.m_Vertex[2] = Math::Vector2F(rasterPoly.m_Vertex[2].m_X, rasterPoly.m_Vertex[2].m_Y);

    // calculate bounding box
    const Geometry::Rect bbox = triangle.GetBoundingRect();

    // cull if completely outside screen
    if (bbox.m_Max.m_X < 0.0f || bbox.m_Min.m_X >= (float)m_Width ||
            bbox.m_Max.m_Y < 0.0f || bbox.m_Min.m_Y >= (float)m_Height)
        return true;

    // clamp to screen bounds
    const std::size_t x0 = (std::size_t)std::max(0.0f,              std::floorf(bbox.m_Min.m_X));
    const std::size_t x1 = (std::size_t)std::min((float)(m_Width),  std::floorf(bbox.m_Max.m_X));
    const std::size_t y0 = (std::size_t)std::max(0.0f,              std::floorf(bbox.m_Min.m_Y));
    const std::size_t y1 = (std::size_t)std::min((float)(m_Height), std::floorf(bbox.m_Max.m_Y));

    // rasterize triangle
    for (std::size_t y = y0; y <= y1; ++y)
        for (std::size_t x = x0; x <= x1; ++x)
        {
            const Math::Vector2F         pixelSample(x + 0.5f, y + 0.5f);
            Geometry::Triangle::IWeights weights;

            if (triangle.BarycentricInside(pixelSample, weights))
            {
                // clamp weight values between 0 and 1
                float w0 = std::max(std::min(weights.m_W0, 1.0f), 0.0f);
                float w1 = std::max(std::min(weights.m_W1, 1.0f), 0.0f);
                float w2 = std::max(std::min(weights.m_W2, 1.0f), 0.0f);

                // interpolate 1/z
                const float invZ = rasterPoly.m_Vertex[0].m_Z * w0 +
                                   rasterPoly.m_Vertex[1].m_Z * w1 +
                                   rasterPoly.m_Vertex[2].m_Z * w2;

                // convert back to z for depth testing
                const float z = 1.0f / invZ;

                // calculate the pixel index to draw on the render buffer
                const std::size_t pixelIndex = y * m_Width + x;

                // depth test
                if (z >= m_Near && z <= m_Far && z < m_pZBuffer[pixelIndex])
                {
                    // update depth buffer
                    m_pZBuffer[pixelIndex] = z;

                    if (m_HasTexture)
                    {
                        // calculate the texture coordinate
                        float u = ((st[0].m_X * w0) + (st[1].m_X * w1) + (st[2].m_X * w2)) * z;
                        float v = ((st[0].m_Y * w0) + (st[1].m_Y * w1) + (st[2].m_Y * w2)) * z;

                        // clamp the values between 0 and 1
                        u = std::max(std::min(u, 1.0f), 0.0f);
                        v = std::max(std::min(v, 1.0f), 0.0f);

                        // calculate the x and y coordinate to pick in the texture, and the line length in pixels
                        const std::size_t tx   = (std::size_t)std::floorf(u * m_TexWidth);
                        const std::size_t ty   = (std::size_t)std::floorf(v * m_TexHeight);
                        const std::size_t line = m_TexWidth * m_TexBPP;

                        // calculate the pixel index to get
                        const std::size_t texIndex = (ty * line) + (tx * m_TexBPP);

                        // get the pixel color from texture
                        const BYTE b = m_pTexture[texIndex];
                        const BYTE g = m_pTexture[texIndex + 1];
                        const BYTE r = m_pTexture[texIndex + 2];

                        // write pixel (BGR format for Windows DIB)
                        m_pPixels[pixelIndex] = (b << 16) | (g << 8) | r;
                    }
                    else
                        // draw a white pixel by default
                        m_pPixels[pixelIndex] = 0xFFFFFF;
                }
            }
        }

    return true;
}
//---------------------------------------------------------------------------
