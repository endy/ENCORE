#include "TextureMaker.h"

#include <algorithm>

TextureMaker::TextureMaker(int pixelCols, int pixelRows, int pixelFormat /* = 4 */):
M_PIXEL_FORMAT(pixelFormat)
{
    m_PixelCols = pixelCols;
    m_PixelRows = pixelRows;
    m_Width = m_PixelCols * M_PIXEL_FORMAT;
    m_Height = pixelRows;

    // alloc data
    m_Data = new GLfloat[m_Width * m_Height];
    Clear();
}

TextureMaker::~TextureMaker()
{
    delete[] m_Data;
    m_Data = NULL;
}

void TextureMaker::Clear()
{
    GLfloat *end = m_Data + (m_Width * m_Height);
    std::fill(m_Data, end, 0.0f);
}

void TextureMaker::SetPixel(int row, int col, GLfloat *data)
{
    int pixelIndex = m_Width * row + col * M_PIXEL_FORMAT;

    if(m_Data[pixelIndex] > 1.0f)
    {
        int i = 0;
        i += 1;
    }

    m_Data[pixelIndex + 0] = data[0];
    m_Data[pixelIndex + 1] = data[1];
    m_Data[pixelIndex + 2] = data[2];
    m_Data[pixelIndex + 3] = data[3];
}

void TextureMaker::SetRow(int row, GLfloat data[])
{
    assert(sizeof(data) == (sizeof(GLfloat) * m_Width)); 

    for(int index = 0; index < m_PixelCols; index++)
    {
        int pixelIndex = m_Width * row;

        int dataIndex = index * M_PIXEL_FORMAT;

        m_Data[pixelIndex + 0] = data[dataIndex + 0];
        m_Data[pixelIndex + 1] = data[dataIndex + 1];
        m_Data[pixelIndex + 2] = data[dataIndex + 2];
        m_Data[pixelIndex + 3] = data[dataIndex + 3];
    }
}

GLfloat* TextureMaker::GetPixel(int row, int col)
{
    int pixelIndex = m_Width * row + col * M_PIXEL_FORMAT;

    return m_Data + pixelIndex;
}

GLfloat* TextureMaker::GetRow(int row)
{
    assert(row < m_Height);

    int offset = m_Width * row;
    return m_Data + offset;
}

void TextureMaker::FillRect(int x, int y, int width, int height, float *color, POS start /* = TOP_LEFT */)
{
    if(start == TOP_LEFT)
    {
        for(int xi = x; xi < x + width; xi++)
        {
            for(int yi = y; yi < y + height; yi++)
            {
                SetPixel(yi, xi, color);
            }
        }
    }
    else if(start == BOTTOM_LEFT)
    {
        for(int xi = x; xi < x + width; xi++)
        {
            for(int yi = y + height - 1; yi >= y; yi--)
            {
                SetPixel(yi, xi, color);
            }
        }
    }
}

void TextureMaker::FillRectAvg(unsigned int avgCount, int x, int y, int width, int height, GLfloat* color, POS start /* = TOP_LEFT */)
{
    if(start == TOP_LEFT)
    {
        for(int xi = x; xi < x + width; xi++)
        {
            for(int yi = y; yi < y + height; yi++)
            {
                GLfloat *pColor = GetPixel(yi, xi);
                pColor[0] = ((pColor[0] * avgCount) + color[0]) / (GLfloat)(avgCount + 1);
                pColor[1] = ((pColor[1] * avgCount) + color[1]) / (GLfloat)(avgCount + 1);
                pColor[2] = ((pColor[2] * avgCount) + color[2]) / (GLfloat)(avgCount + 1);
                pColor[3] = ((pColor[3] * avgCount) + color[3]) / (GLfloat)(avgCount + 1);
                SetPixel(yi, xi, pColor);
            }
        }
    }
    else if(start == BOTTOM_LEFT)
    {
        for(int xi = x; xi < x + width; xi++)
        {
            for(int yi = y + height - 1; yi >= y; yi--)
            {
                GLfloat *pColor = GetPixel(yi, xi);
                pColor[0] = ((pColor[0] * avgCount) + color[0]) / (GLfloat)(avgCount + 1);
                pColor[1] = ((pColor[1] * avgCount) + color[1]) / (GLfloat)(avgCount + 1);
                pColor[2] = ((pColor[2] * avgCount) + color[2]) / (GLfloat)(avgCount + 1);
                pColor[3] = ((pColor[3] * avgCount) + color[3]) / (GLfloat)(avgCount + 1);
                SetPixel(yi, xi, pColor);
            }
        }
    }
}

TextureGLf* TextureMaker::MakeTexture()
{
    TextureGLf *tex = new TextureGLf(GL_FLOAT, GL_TEXTURE_2D, m_PixelCols, m_PixelRows, m_Data);
    tex->Create();

    return tex;
}