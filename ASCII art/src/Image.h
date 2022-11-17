#pragma once

/*
1. choose an image
2. load an image and its height and width to a 2d array
3. convert the rgb data to integers
4. convert integers to ascii characters
5. print 
*/

class Image
{
private:
	int m_Height;
	int m_Width;
	unsigned char** m_RGB; // n*3 array
	unsigned char* m_Brightness;
	char* m_ch;
	static const char* characters;
public:
	Image(const char* file_path);
	~Image();
protected:
	void ConvertToBrightness();
	void ConvertToCharacters();
	void Print(const char* ch) const;
};



