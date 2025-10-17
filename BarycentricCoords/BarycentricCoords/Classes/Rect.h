/****************************************************************************
 * ==> Rect ----------------------------------------------------------------*
 ****************************************************************************
 * Description: Geometric rectangle                                         *
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

namespace Geometry
{
    /**
    * Geometric rectangle
    *@author Jean-Milost Reymond
    */
    class Rect
    {
        public:
            Math::Vector2F m_Min;
            Math::Vector2F m_Max;

            Rect();

            /**
            * Constructor
            *@param min - min corner
            *@param max - max corner
            */
            Rect(const Math::Vector2F& min, const Math::Vector2F& max);

            virtual ~Rect();
    };
}
