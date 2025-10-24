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

#include "WaveFront.h"

// std
#include <iostream>
#include <fstream>
#include <sstream>

using namespace Model;

//------------------------------------------------------------------------------
// WaveFront
//------------------------------------------------------------------------------
WaveFront::IMesh WaveFront::Load(const std::string& fileName)
{
    IMesh         mesh;
    std::ifstream file(fileName);

    if (!file.is_open())
        return mesh;

    std::string line;

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string        prefix;
        iss >> prefix;

        if (prefix == "v")
        {
            // vertex position
            Math::Vector3F v;
            iss >> v.m_X >> v.m_Y >> v.m_Z;
            mesh.m_Vertices.push_back(v);
        }
        else
        if (prefix == "vt")
        {
            // texture coordinate
            Math::Vector2F vt;
            iss >> vt.m_X >> vt.m_Y;
            mesh.m_TexCoords.push_back(vt);
        }
        else
        if (prefix == "vn")
        {
            // vertex normal
            Math::Vector3F vn;
            iss >> vn.m_X >> vn.m_Y >> vn.m_Z;
            mesh.m_Normals.push_back(vn);
        }
        else
        if (prefix == "f")
        {
            // face
            IFace       face;
            std::string vertex;

            while (iss >> vertex)
            {
                std::replace(vertex.begin(), vertex.end(), '/', ' ');
                std::istringstream viss(vertex);

                int vIdx, vtIdx, vnIdx;
                viss >> vIdx;

                // OBJ indices start at 1
                face.m_VertexIndices.push_back(vIdx - 1);

                if (viss >> vtIdx)
                {
                    face.m_TexCoordIndices.push_back(vtIdx - 1);

                    if (viss >> vnIdx)
                        face.m_NormalIndices.push_back(vnIdx - 1);
                }
            }

            mesh.m_Faces.push_back(face);
        }
    }

    file.close();

    return mesh;
}
//------------------------------------------------------------------------------
