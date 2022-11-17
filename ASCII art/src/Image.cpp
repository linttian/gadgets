#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_resize.h"

#include "Image.h"
#include <cstring>
#include <iostream>
#include <cmath>

const char* Image::characters =
"`^\",:;Il!i~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";

Image::Image(const char* file_path)
	: m_Width(50), m_Height(50)
{
	int n, x, y; 
	unsigned char* predata = stbi_load(file_path, &x, &y, &n, 3);
	unsigned char* data = (unsigned char*)_malloca(m_Width * m_Height * n);
	stbir_resize_uint8(predata, x, y, 0, data, m_Width, m_Height, 0, n);

	m_RGB = new unsigned char*[m_Width * m_Height];
	for (int i = 0; i < m_Width * m_Height; ++i)
	{
		m_RGB[i] = new unsigned char[3];
	}

	m_Brightness = new unsigned char[m_Width * m_Height];

	for (int i = 0; i < m_Height; ++i)
	{
		for (int j = 0; j < m_Width; ++j)
		{
			unsigned char* pixelOffset = data + (m_Width * i + j) * 3;
			m_RGB[i * m_Width + j][0] = pixelOffset[0];
			m_RGB[i * m_Width + j][1] = pixelOffset[1];
			m_RGB[i * m_Width + j][2] = pixelOffset[2];		
		}
	}
	stbi_image_free(predata);
	ConvertToBrightness();
	ConvertToCharacters();
}

void Image::Print(const char* ch) const
{
	for (int i = 0; i < m_Height; ++i)
	{
		for (int j = 0; j < m_Width; ++j)
		{
			std::cout << ch[i * m_Width + j];
			std::cout << ch[i * m_Width + j];
		}
		std::cout << std::endl;
	}
}

Image::~Image()
{
	for (int i = 0; i < m_Width * m_Height; ++i)
	{
		delete[] m_RGB[i];
	}
	delete[] m_RGB;
	delete[] m_Brightness;
	delete[] m_ch;
}

void Image::ConvertToBrightness()
{
	for (int i = 0; i < m_Width * m_Height; ++i)
	{
		auto R = m_RGB[i][0];
		auto G = m_RGB[i][1];
		auto B = m_RGB[i][2];
		m_Brightness[i] = 0.2126 * R + 0.7152 * G + 0.0722 * B;
	}
}

void Image::ConvertToCharacters() 
{
	m_ch = new char[m_Width * m_Height];
	for (int i = 0; i < m_Width * m_Height; ++i)
	{
		int index = round(m_Brightness[i] * (strlen(characters) - 1) / 255);
		m_ch[i] = characters[index];
	}
	Print(m_ch);
}

