/****************************************************************************
 * ==> Polygon -------------------------------------------------------------*
 ****************************************************************************
 * Description: Geometric polygon                                           *
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

#include "Polygon.h"

using namespace Geometry;

//---------------------------------------------------------------------------
// Polygon
//---------------------------------------------------------------------------
Polygon::Polygon()
{}
//---------------------------------------------------------------------------
Polygon::Polygon(const Math::Vector3F& v1,
                 const Math::Vector3F& v2,
                 const Math::Vector3F& v3)
{
    m_Vertex[0] = v1;
    m_Vertex[1] = v2;
    m_Vertex[2] = v3;
}
//---------------------------------------------------------------------------
Polygon::~Polygon()
{}
//---------------------------------------------------------------------------
Polygon* Polygon::GetClone() const
{
    // copies the polygon, then returns the copy
    return new Polygon(m_Vertex[0], m_Vertex[1], m_Vertex[2]);
}
//---------------------------------------------------------------------------
Geometry::PlaneF Polygon::GetPlane() const
{
    // calculates the plane from the values of the 3 vertices of the polygon
    return Geometry::PlaneF::FromPoints(m_Vertex[0], m_Vertex[1], m_Vertex[2]);
}
//---------------------------------------------------------------------------
Math::Vector3F Polygon::GetCenter() const
{
    // calculates then returns the value of the midpoint of the polygon
    return Math::Vector3F(((m_Vertex[0].m_X + m_Vertex[1].m_X + m_Vertex[2].m_X) / 3.0f),
                          ((m_Vertex[0].m_Y + m_Vertex[1].m_Y + m_Vertex[2].m_Y) / 3.0f),
                          ((m_Vertex[0].m_Z + m_Vertex[1].m_Z + m_Vertex[2].m_Z) / 3.0f));
}
//---------------------------------------------------------------------------
bool Polygon::Inside(float x, float y, float z) const
{
    return Inside(Math::Vector3F(x, y, z));
}
//---------------------------------------------------------------------------
bool Polygon::Inside(const Math::Vector3F& point) const
{
    /*
    * check if the point p is inside the polygon in the following manner:
    *
    *                  V1                         V1
    *                  /\                         /\
    *                 /  \                       /  \
    *                / *p \                  *P /    \
    *               /      \                   /      \
    *            V2 -------- V3             V2 -------- V3
    *
    * calculate the vectors between the point p and each polygon vertex, then
    * calculate the angle formed by each of these vectors. If the sum of the
    * angles is equal to a complete circle, i.e. 2 * pi in radians, then the
    * point p is inside the polygon limits, otherwise the point is outside. It
    * is assumed that the point to check belongs to the polygon's plane
    */
    const Math::Vector3F nPToV1 = (m_Vertex[0] - point).Normalize();
    const Math::Vector3F nPToV2 = (m_Vertex[1] - point).Normalize();
    const Math::Vector3F nPToV3 = (m_Vertex[2] - point).Normalize();

    // calculate the angles using the dot product of each vectors. Limit range
    // to values between -1.0f and 1.0f
    const float a1 = std::min(std::max(nPToV1.Dot(nPToV2), -1.0f), 1.0f);
    const float a2 = std::min(std::max(nPToV2.Dot(nPToV3), -1.0f), 1.0f);
    const float a3 = std::min(std::max(nPToV3.Dot(nPToV1), -1.0f), 1.0f);

    // calculate the sum of all angles
    const float angleResult = std::acos(a1) + std::acos(a2) + std::acos(a3);

    // if sum is equal to 2 PI radians then point p is inside polygon. NOTE can
    // be higher due to precision errors in calculations
    return (angleResult >= (float)(M_PI * 2.0));
}
//---------------------------------------------------------------------------
bool Polygon::Inside(const Math::Vector3F& point, float& w0, float& w1, float& w2) const
{
    const Math::Vector3F v0v1 = m_Vertex[1] - m_Vertex[0];
    const Math::Vector3F v0v2 = m_Vertex[2] - m_Vertex[0];
    const Math::Vector3F v0p  = point       - m_Vertex[0];

    // calculate dot products
    const float d00 = v0v1.Dot(v0v1);
    const float d01 = v0v1.Dot(v0v2);
    const float d11 = v0v2.Dot(v0v2);
    const float d20 =  v0p.Dot(v0v1);
    const float d21 =  v0p.Dot(v0v2);

    // calculate determinant
    const float denom = d00 * d11 - d01 * d01;

    // check for degenerate triangle (i.e. triangles without surface, where all
    // the vertices are aligned on a line)
    if (std::abs(denom) < 1e-6f)
        return false;

    // calculate barycentric coordinates, which are a way to express any point
    // inside (or outside) a triangle as a weighted combination of the triangle
    // three vertices. For a triangle with vertices A, B, and C, any point P
    // can be expressed as:
    // P = w0 * A + w1 * B + w2 * C
    // Where:
    // - w0,  w1,  w2 are the barycentric coordinates (weights)
    // - w0 + w1 + w2 = 1 (the weights always sum to 1)
    w1 = (d11 * d20 - d01 * d21) / denom;
    w2 = (d00 * d21 - d01 * d20) / denom;
    w0 = 1.0f - w1 - w2;

    const float epsilon = -1e-6f;

    // inside test
    return (w0 >= epsilon && w1 >= epsilon && w2 >= epsilon);
}
//---------------------------------------------------------------------------
Polygon* Polygon::ApplyMatrix(const Math::Matrix4x4F& matrix) const
{
    // build a new polygon transforming all vertices of the polygon using
    // given matrix, and return new built polygon
    return new Polygon(matrix.Transform(m_Vertex[0]),
                       matrix.Transform(m_Vertex[1]),
                       matrix.Transform(m_Vertex[2]));
}
//---------------------------------------------------------------------------
