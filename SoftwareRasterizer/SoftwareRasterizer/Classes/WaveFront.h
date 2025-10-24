/****************************************************************************
 * ==> WaveFront -----------------------------------------------------------*
 ****************************************************************************
 * Description: Minimalist Wavefront parser                                 *
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

// std
#include <string>
#include <vector>

// classes
#include "Vector2.h"
#include "Vector3.h"

namespace Model
{
    /**
    * Minimalist Wavefront parser
    *@author Jean-Milost Reymond
    */
    class WaveFront
    {
        public:
            /**
            * Polygon face
            */
            struct IFace
            {
                std::vector<int> m_VertexIndices;
                std::vector<int> m_TexCoordIndices;
                std::vector<int> m_NormalIndices;
            };

            /**
            * Mesh
            */
            struct IMesh
            {
                std::vector<Math::Vector3F> m_Vertices;
                std::vector<Math::Vector2F> m_TexCoords;
                std::vector<Math::Vector3F> m_Normals;
                std::vector<IFace>          m_Faces;
            };

            /**
            * Loads a WaveFront file
            *@param fileName - WaveFront file name to open
            *@returns opened mesh, empty mesh on error
            */
            static IMesh Load(const std::string& fileName);
    };
}
