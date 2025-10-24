/****************************************************************************
 * ==> Point3D -------------------------------------------------------------*
 ****************************************************************************
 * Description: 3D point, which may be transformed to a 2D point            *
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

#include "Point3D.h"

using namespace Rasterizer;

//------------------------------------------------------------------------------
// Point3D
//------------------------------------------------------------------------------
Point3D::Point3D()
{}
//------------------------------------------------------------------------------
Point3D::~Point3D()
{}
//------------------------------------------------------------------------------
Math::Vector2F Point3D::Transform() const
{
    Math::Vector2F point2D;

    // transform the 3D point a 2D point. This may be performed by dividing the
    // x/y values by the z value
    point2D.m_X = m_Pos.m_X / m_Pos.m_Z;
    point2D.m_Y = m_Pos.m_Y / m_Pos.m_Z;

    return point2D;
}
//------------------------------------------------------------------------------
