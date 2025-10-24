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

#include "Texture.h"

// std
#include <memory>

using namespace Texture;

//---------------------------------------------------------------------------
// Loader
//---------------------------------------------------------------------------
Loader::Loader()
{
    // initialize GDI+
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&m_GdiplusToken, &gdiplusStartupInput, nullptr);
}
//---------------------------------------------------------------------------
Loader::~Loader()
{
    // shutdown GDI+
    Gdiplus::GdiplusShutdown(m_GdiplusToken);
}
//---------------------------------------------------------------------------
unsigned char* Loader::FromJPEG(const std::wstring& fileName, int& width, int& height)
{
    // load image
    std::unique_ptr<Gdiplus::Bitmap> pBitmap = std::make_unique<Gdiplus::Bitmap>(fileName.c_str());

    if (pBitmap->GetLastStatus() != Gdiplus::Ok)
        return nullptr;

    width  = pBitmap->GetWidth();
    height = pBitmap->GetHeight();

    // allocate buffer for RGBA data
    unsigned char* pBuffer = new unsigned char[(std::size_t)width * (std::size_t)height * 4];

    // lock bitmap bits
    Gdiplus::BitmapData bitmapData;
    Gdiplus::Rect       rect(0, 0, width, height);

    pBitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bitmapData);

    // copy pixel data
    unsigned char* src = (unsigned char*)bitmapData.Scan0;
    unsigned char* dst = pBuffer;

    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
        {
            // GDI+ uses BGRA, convert to RGBA. Flip Y axis: read from top, write to bottom
            int srcIndex = y * bitmapData.Stride + x * 4;
            int dstIndex = ((height - 1 - y) * width + x) * 4;

            dst[dstIndex + 0] = src[srcIndex + 2]; // R
            dst[dstIndex + 1] = src[srcIndex + 1]; // G
            dst[dstIndex + 2] = src[srcIndex + 0]; // B
            dst[dstIndex + 3] = src[srcIndex + 3]; // A
        }

    pBitmap->UnlockBits(&bitmapData);

    return pBuffer;
}
//---------------------------------------------------------------------------
unsigned char* Loader::FromJPEG(const std::string& fileName, int& width, int& height)
{
    // convert UTF8 to UTF16
    int      len        = ::MultiByteToWideChar(CP_UTF8, 0, fileName.c_str(), -1, nullptr, 0);
    wchar_t* pwFileName = new wchar_t[len];

    ::MultiByteToWideChar(CP_UTF8, 0, fileName.c_str(), -1, pwFileName, len);

    unsigned char* result = FromJPEG(pwFileName, width, height);

    delete[] pwFileName;

    return result;
}
//---------------------------------------------------------------------------
