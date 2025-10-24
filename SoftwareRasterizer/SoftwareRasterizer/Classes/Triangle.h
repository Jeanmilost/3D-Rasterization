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

#pragma once

// classes
#include "Vector2.h"
#include "Rect.h"

namespace Geometry
{
    /**
    * Geometric triangle
    *@author Jean-Milost Reymond
    */
    class Triangle
    {
        public:
            /**
            * Barycentric weights
            */
            struct IWeights
            {
                float m_W0 = 0.0f;
                float m_W1 = 0.0f;
                float m_W2 = 0.0f;
            };

            Math::Vector2F m_Vertex[3];

            Triangle();
            virtual ~Triangle();

            /**
            * Get the triangle bounding rectangle
            *@return the triangle bounding rectangle
            */
            Rect GetBoundingRect() const;

            /**
            * Checks if a point is inside the triangle, and get the barycentric weights
            *@param point - point
            *@param[out] weights - barycentric weights
            *@returns true if point is inside the triangle, otherwise false
            */
            bool BarycentricInside(const Math::Vector2F& point, IWeights& weights) const;

        private:
            /**
            * Calculates triangle signed area using cross product
            *@param v1 - first vertex
            *@param v2 - second vertex
            *@param v3 - third vertex
            *@return triangle signed area
            */
            inline float CalculateSignedArea(const Math::Vector2F& v1,
                                             const Math::Vector2F& v2,
                                             const Math::Vector2F& v3) const;
    };

    //------------------------------------------------------------------------------
    // Triangle
    //------------------------------------------------------------------------------
    inline float Triangle::CalculateSignedArea(const Math::Vector2F& v1,
                                               const Math::Vector2F& v2,
                                               const Math::Vector2F& v3) const
    {
        return (v2.m_X - v1.m_X) * (v3.m_Y - v1.m_Y) - (v2.m_Y - v1.m_Y) * (v3.m_X - v1.m_X);
    }
    //------------------------------------------------------------------------------
}
