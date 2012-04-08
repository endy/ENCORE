/****************************************************************************
Brandon Light
05/21/2006

TextureMaker.h

Class that stores color data that can be used to generate a texture.

IMPORTANT:  This supports GLfloat textures ONLY.  

****************************************************************************/
#pragma once

#include <cassert>
#include "Texture.h"

enum POS
{
    TOP_LEFT = 0,
    BOTTOM_LEFT,
};

class TextureMaker
{
public:
	TextureMaker(int pixelCols, int pixelRows, int pixelFormat = 4);
	virtual ~TextureMaker();

	void Clear();

	int Width(){ return m_Width; }
	int Height(){ return m_Height; }

    int GetImageWidth(){ return m_Width / 4; }
    int GetImageHeight(){ return m_Height; }

	void SetPixel(int row, int col, GLfloat pixel[]);
	void SetRow(int row, GLfloat data[]);

	GLfloat* GetPixel(int row, int col);
	GLfloat* GetRow(int row);

    // provides direct access to the texture data
    GLfloat& GetDataValue(int index){ return m_Data[index]; }

	void FillRect(int x, int y, int width, int height, GLfloat* color, POS start = TOP_LEFT);
    void FillRectAvg(unsigned int avgCount, int x, int y, int width, int height, GLfloat* color, POS start = TOP_LEFT);

	TextureGL<GLfloat> *MakeTexture();

private:
	// the data array
	GLfloat *m_Data;
	// number of columns of pixels
	int m_PixelCols;
	// number of rows of pixels
	int m_PixelRows;
	// width of the framebuffer
	int m_Width;
	// height of the framebuffer
	int m_Height;
	// number of color components per pixel
	const int M_PIXEL_FORMAT;
};

