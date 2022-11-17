#pragma once

const double PI = 3.14159265;

class CRSImage
{
public:
	static int COUNT;
	CRSImage();
	CRSImage(const CRSImage& other) = delete;
	~CRSImage();
	bool OpenFile(const std::string& file_path);
	void Information() const;
	//Basic Statistics
	int Stats_min(const int band) const;
	int Stats_max(const int band) const;
	double Stats_mean(const int band) const;
	double Stats_stdev(const int band) const;
	void ShowBasicStats() const;
	void Histogram(const int band) const;
	void Display();
	void ImageZoom(const double scale);
	void HistogramDisplay(int band);
	void RotateImage(double angle) const;
	void ImageFilter(const int kernel,const int red,const int green,const int blue, const int kind) const;
	void Clean();
	bool SaveImage(unsigned char* data, const int nSamples, const int nLines, const int nBands, const std::string& path) const;
protected:
	unsigned char LaplacianFilter(unsigned char* data, int i, int j) const;
	bool ReadMetaData(const std::string& MetaFilePath);
	void InitBuffer();
	bool ReadImgData(const std::string& file_path);
	std::tuple<int, int, int> SetBandColor() const;
	void SetPixel(DWORD* pImgBuffer, unsigned char* data, int redBand, int greenBand, int blueBand,
		int samples, int lines) const;
	std::map<unsigned char, int> DN_Npts(const int band, const int samples,
		const int lines, unsigned char* data) const;
	int Total(std::map<unsigned char, int>::iterator&& beg,
		std::map<unsigned char, int>::iterator&& end) const;
	unsigned char LinearStretch(unsigned char brtnValue, std::pair<unsigned char, unsigned char>& BV) const;

	std::pair<unsigned char, unsigned char> BVtoBeLinearStretched
	(const int band, const int samples, const int lines, unsigned char* Data) const;
	unsigned char Resample(const int i, const int j, unsigned char* srcData, 
		int nSamples, int nLines, double srcX, double srcY) const;
	void ZoomingAssist(int redBand, int greenBand, int blueBand,
		unsigned char* myData, int samples, int lines, double scale) const;
	double CoordinateZoomTransf(int x, double scale) const;
	unsigned char MeanFilter(unsigned char* data, int i, int j,int kernel) const;
	void RotateHelper(unsigned char* dstiImg, unsigned char* srcImg, 
		double rad, int nSamples, int nLines) const;
	void SetPixelForRotate(DWORD* pImgBuffer, unsigned char* data, int samples, int lines) const;
	
	void WriteMetaFile(int nSamples, int nLines, int nBands,const std::string& nInterleave, std::string& nFilePath) const;
private:
	unsigned char* m_Data;	
	int m_Samples;		
	int m_Lines;		
	int m_Bands;		
	std::string m_Interleave;
	std::string m_MetaFilePath;	
	std::string m_ImgFilePath;
};




namespace MyRectangle 
{
	class Rectangle
	{
	private:
		int m_VertexLeft;
		int m_VertexTop;
		int m_VertexRight;
		int m_VertexBottom;
	public:
		Rectangle(int VertexLeft, int VertexTop, int VertexRight, int VertexBottom)
			: m_VertexLeft(VertexLeft), m_VertexTop(VertexTop),
			m_VertexRight(VertexRight), m_VertexBottom(VertexBottom)
		{
			setfillcolor(CYAN);
			solidrectangle(m_VertexLeft, m_VertexTop, m_VertexRight, m_VertexBottom);
		}
		Rectangle()
			: m_VertexLeft(0),m_VertexTop(0),m_VertexRight(0),m_VertexBottom(0) {}

	};
}


