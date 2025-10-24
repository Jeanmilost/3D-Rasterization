/****************************************************************************
 * ==> Texture -------------------------------------------------------------*
 ****************************************************************************
 * Description: Texture loader                                              *
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

// gdi+
#include <objidl.h>
#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")

namespace Texture
{
    /**
    * Texture loader
    *@author Jean-Milost Reymond
    */
    class Loader
    {
        public:
            Loader();
            virtual ~Loader();

            /**
            * Loads texture buffer from JPEG
            *@param fileName - JPEG file to load
            *@param[out] width - image width
            *@param[out] height - image height
            *@return RGBA pixel data buffer, nullptr on failure
            *@note the returned buffer should be deleted by the caller
            */
            unsigned char* FromJPEG(const std::wstring& fileName, int& width, int& height);

            /**
            * Loads texture buffer from JPEG
            *@param fileName - JPEG file to load
            *@param[out] width - image width
            *@param[out] height - image height
            *@return RGBA pixel data buffer, nullptr on failure
            *@note the returned buffer should be deleted by the caller
            */
            unsigned char* FromJPEG(const std::string& fileName, int& width, int& height);

        private:
            ULONG_PTR m_GdiplusToken;
    };
}
