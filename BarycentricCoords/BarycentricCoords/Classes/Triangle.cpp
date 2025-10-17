/****************************************************************************
 * ==> Triangle ------------------------------------------------------------*
 ****************************************************************************
 * Description: Geometric triangle                                          *
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

#include "Triangle.h"

using namespace Geometry;

//------------------------------------------------------------------------------
Triangle::Triangle()
{}
//------------------------------------------------------------------------------
Triangle::~Triangle()
{}
//------------------------------------------------------------------------------
Rect Triangle::GetBoundingRect() const
{
    Rect rect;

    // calculate the triangle bounding box
    rect.m_Min.m_X = std::min(m_Vertex[0].m_X, std::min(m_Vertex[1].m_X, m_Vertex[2].m_X));
    rect.m_Min.m_Y = std::min(m_Vertex[0].m_Y, std::min(m_Vertex[1].m_Y, m_Vertex[2].m_Y));
    rect.m_Max.m_X = std::max(m_Vertex[0].m_X, std::max(m_Vertex[1].m_X, m_Vertex[2].m_X));
    rect.m_Max.m_Y = std::max(m_Vertex[0].m_Y, std::max(m_Vertex[1].m_Y, m_Vertex[2].m_Y));

    return rect;
}
//------------------------------------------------------------------------------
bool Triangle::BarycentricInside(const Math::Vector2F& point, IWeights& weights) const
{
    // barycentric coordinates are a way to express any point inside(or outside) a triangle
    // as a weighted combination of the triangle three vertices. For a triangle with vertices
    // A, B, and C, any point P can be expressed as:
    // P = W0 · A + W1 · B + W2 · C
    // Where:
    // - W0,  W1,  W2 are the barycentric coordinates (weights)
    // - W0 + W1 + W2 = 1 (the weights always sum to 1)
    //
    //                                       A
    //                                       ^
    //                                      /|\
    //                                     / | \
    //                                    /  |  \
    //                                   /  /*\  \
    //                                  / /point\ \
    //                               B //_________\\ C

    // whole triangle area
    const float areaABC = CalculateSignedArea(m_Vertex[0], m_Vertex[1], m_Vertex[2]);

    // sub-triangles areas
    const float areaPBC = CalculateSignedArea(point,       m_Vertex[1], m_Vertex[2]);
    const float areaAPC = CalculateSignedArea(m_Vertex[0], point,       m_Vertex[2]);
    const float areaABP = CalculateSignedArea(m_Vertex[0], m_Vertex[1], point);

    // barycentric coordinates
    weights.m_W0 = areaPBC / areaABC;
    weights.m_W1 = areaAPC / areaABC;
    weights.m_W2 = areaABP / areaABC;

    return (weights.m_W0 >= 0 && weights.m_W1 >= 0 && weights.m_W2 >= 0);
}
//------------------------------------------------------------------------------
