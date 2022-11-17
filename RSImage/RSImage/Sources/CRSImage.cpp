#include "pch.h"

bool CRSImage::OpenFile
(const std::string& ImgFilePath)
{
	std::string MetaFilePath;
	if (ImgFilePath.find(".img") != ImgFilePath.npos)
		MetaFilePath = ImgFilePath.substr(0, ImgFilePath.size() - 4) + ".hdr";
	else
		MetaFilePath = ImgFilePath + ".hdr";
	m_MetaFilePath = MetaFilePath;
	m_ImgFilePath = ImgFilePath;
	std::ifstream in(ImgFilePath, std::ios_base::binary);
	if (in && ReadMetaData(MetaFilePath))
	{
		InitBuffer();
		ReadImgData(ImgFilePath);
		++COUNT;
		return true;
	}
	Clean();
	return false;
}

bool CRSImage::ReadMetaData(const std::string& MetaFilePath)
{
	int state = 0;
	std::ifstream in(MetaFilePath);
	if (in)
	{
		std::string str;
		while (!in.eof() && in.good())
		{
			in >> str;
			if (str == "samples" && in>>str>>str)
			{
				std::istringstream ss(str);
				ss >> m_Samples;
				++state;
			}
			if (str == "lines" && in >> str >> str)
			{
				std::istringstream ss(str);
				ss >> m_Lines;
				++state;
			}
			if (str == "bands" && in>>str>>str)
			{
				std::istringstream ss(str);
				ss >> m_Bands;
				++state;
			}
			if (str == "interleave" && in >> str >> str)
			{
				std::istringstream ss(str);
				ss >> m_Interleave;
				++state;
			}
		}
		in.close();
	}
	return (state == 4);
}


void CRSImage::InitBuffer()
{
	m_Data = new unsigned char[m_Lines * m_Samples * m_Bands];
}


bool CRSImage::ReadImgData(const std::string& file_path)
{
	std::ifstream ifs(file_path, std::ios_base::binary);
	if (m_Interleave == "bsq")
	{
		//BSQ
		for (int i = 0; i < m_Bands; ++i)
		{
			for (int j = 0; j < m_Lines; ++j)
			{
				ifs.read((char*)m_Data + j*m_Samples + m_Samples*m_Lines* i 
					, static_cast<std::streamsize>(sizeof(unsigned char)) * m_Samples);
			}
		}
		return true;
	}
	else if (m_Interleave == "bil")
	{
		//BIL
		for (int i = 0; i < m_Lines; ++i)
		{
			for (int j = 0; j < m_Bands; ++j)
			{
				ifs.read((char*)m_Data + m_Samples*i + m_Samples*m_Lines*j,
					static_cast<std::streamsize>(sizeof(unsigned char)) * m_Samples);
			}
		}
		return true;
	}
	else if (m_Interleave == "bip")
	{
		//BIP
		for (int i = 0; i < m_Samples*m_Lines; ++i)
		{
			for (int j = 0; j < m_Bands; ++j)
			{
				ifs.read((char*)m_Data + i + 
					m_Samples * m_Lines * j,sizeof(unsigned char));
			}
		}
		return true;
	}
	return false;
}




int CRSImage::Stats_min(const int band) const
{
	return *std::min_element(m_Data + (band - 1) * m_Samples * m_Lines,
		m_Data + band * m_Samples * m_Lines);
}

int CRSImage::Stats_max(const int band) const
{
	return *std::max_element(m_Data + (band - 1) * m_Samples * m_Lines,
		m_Data + band * m_Samples * m_Lines);
}

double CRSImage::Stats_mean(const int band) const
{
	int sum = 0;
	for (int i = 0; i < m_Samples * m_Lines; ++i)
	{
		sum += m_Data[(band - 1) * m_Samples * m_Lines + i];
	}
	return sum / (m_Samples * m_Lines * 1.0);
}

double CRSImage::Stats_stdev(const int band) const
{
	double mean = Stats_mean(band);
	double sum = 0.0;
	for (int i = 0; i < m_Samples * m_Lines ; ++i)
	{
		sum += pow(m_Data[(band - 1) * m_Samples * m_Lines + i] - mean,2);
	}
	return sqrt(sum / (m_Samples * m_Lines));
}

void CRSImage::ShowBasicStats() const
{
	std::cout << std::right << std::setw(15) << "Basic Stats" << std::setw(15) <<"Min" <<
		std::setw(15) << "Max" << std::setw(15) << "Mean" << std::setw(15) << "Stdev" << std::endl;
	for (int i = 1; i <= m_Bands; ++i)
	{
		std::cout << std::right << std::setw(15) << "Band "  << i << std::setw(15) <<
			Stats_min(i) << std::setw(15) << Stats_max(i) << std::setw(15) 
			<< Stats_mean(i) << std::setw(15) << Stats_stdev(i) << '\n';
	}
}

void CRSImage::Histogram(const int band) const
{
	std::cout << std::right << std::setw(15) << "Histogram" << std::setw(15) << "DN" <<
		std::setw(15) << "Npts" << std::setw(15) << "Total" << std::setw(15) << "Percent" 
		<< std::setw(15) << "Acc Pct" << std::endl;
	std::map<unsigned char, int> container = DN_Npts(band,m_Samples,m_Lines,m_Data);
	int count = 1;
	std::vector<double> percent_accumulate(container.size(),0);
	for (auto& tuple_ : container)
	{
		int total = Total(container.begin(), container.find(tuple_.first));
		double percent = 100 * tuple_.second / (m_Samples * m_Lines * 1.0);
		percent_accumulate[count - 1] = percent;
		if (count == 1)
		{
			std::cout << std::right << std::setw(14) << "Band " << band << std::setw(15) <<
				(int)tuple_.first << std::setw(15) << tuple_.second << std::setw(15)
				<< total << std::setw(15) << std::setiosflags(std::ios::fixed) << 
				std::setprecision(4) << percent << std::setw(15) << percent <<'\n';
		}
		else
		{
			double percent_sum = std::accumulate(percent_accumulate.begin(), percent_accumulate.end(), 0.0);
			std::cout << std::right << std::setw(15) << " " << std::setw(15) <<
				(int)tuple_.first << std::setw(15) << tuple_.second << std::setw(15) << 
				total << std::setw(15) << std::setprecision(4) << percent <<
				std::setw(15) << std::setiosflags(std::ios::fixed) << std::setprecision(6) <<
				percent_sum << '\n';
		}
		++count;
	}
}

std::map<unsigned char, int> CRSImage::DN_Npts(const int band, const int samples, 
	const int lines,unsigned char* data) const
{
	std::map<unsigned char, int> container;
	for (int j = 0; j < samples * lines; ++j)
	{
		auto key = *(data + j + samples * lines * (band-1));
		if (!(container[key]) )
			container[key] = 1;
		else
			++container[key];
	}
	return container;
}

int CRSImage::Total(std::map<unsigned char, int>::iterator&& beg, 
	std::map<unsigned char, int>::iterator&& end) const
{
	int sum = 0;
	while (beg != end)
	{
		sum += beg->second;
		++beg;
	}
	if (beg == end)
		sum+= beg->second;
	return sum;
}

void CRSImage::HistogramDisplay(int band)
{
	initgraph(m_Samples, m_Lines, EW_SHOWCONSOLE);//640,400
	setbkcolor(WHITE);
	cleardevice();

	setcolor(BLACK);
	rectangle(10,0,630,390);
	auto recMap = DN_Npts(band, m_Samples, m_Lines, m_Data);
	auto max_height = std::max_element(recMap.begin(), recMap.end(),
		[](const std::pair<unsigned char, int>& p1, const std::pair<unsigned char, int>& p2) {
			return p1.second < p2.second; })->second;
	int width = 6;
	int count = 0;
	for (auto& rec : recMap)
	{
		int x_left = 10 + count * width;
		int x_right = x_left + width;
		int y_bottom = 390;
		int y_top = y_bottom - rec.second * y_bottom / max_height;
		MyRectangle::Rectangle(x_left,y_top,x_right,y_bottom);
		++count;
	}
}

void CRSImage::Display()
{
	auto colorTuple = SetBandColor();

	initgraph(m_Samples, m_Lines, EW_SHOWCONSOLE);
	setbkcolor(WHITE);
	cleardevice();
	IMAGE img(m_Samples, m_Lines);
	DWORD* pImgBuffer = GetImageBuffer(&img);
	
	
	SetPixel(pImgBuffer,m_Data,std::get<0>(colorTuple), std::get<1>(colorTuple),
		std::get<2>(colorTuple),m_Samples,m_Lines);
	putimage(0, 0, &img);

}

std::tuple<int, int, int> CRSImage::SetBandColor() const
{
	int redBand = 0, greenBand = 0, blueBand = 0;
	std::cout << "Please specify three bands to display:\n";
	std::cin >> redBand >> greenBand >> blueBand;
	return std::make_tuple(redBand,greenBand,blueBand);
}

void CRSImage::SetPixel(DWORD* pImgBuffer,unsigned char* data, int redBand, int greenBand, 
	int blueBand,int samples,int lines) const
{
	BYTE* Red = data + (redBand - 1) * samples * lines;		
	BYTE* Green = data + (greenBand - 1) * samples * lines;
	BYTE * Blue = data + (blueBand - 1) * samples * lines;
	std::pair<unsigned char, unsigned char> BV1 = BVtoBeLinearStretched
	(redBand, samples, lines,data);
	std::pair<unsigned char, unsigned char> BV2 = BVtoBeLinearStretched
	(greenBand, samples, lines,data);
	std::pair<unsigned char, unsigned char> BV3 = BVtoBeLinearStretched
	(blueBand, samples, lines,data);
	for (int i = 0; i < lines; ++i)
	{
		for (int j = 0; j < samples; ++j)
		{
			auto red = LinearStretch(*(Red + j + i * samples),BV1);
			auto green = LinearStretch(*(Green + j + i * samples),BV2);
			auto blue = LinearStretch(*(Blue + j + i * samples),BV3);
			// non-LinearStretch
			/*auto t = Red + j + i * samples;
			auto red = *(Red + j + i * samples);
			auto green = *(Green + j + i * samples);
			auto blue = *(Blue + j + i * samples);*/
			pImgBuffer[i * samples + j] = RGB(red,green, blue);
		}
	}
}

//default stretch = 2%
unsigned char CRSImage::LinearStretch(unsigned char brtnValue, std::pair<unsigned char, unsigned char>& BV) const
{
	unsigned char minBV = BV.first;
	unsigned char maxBV = BV.second;
	if (brtnValue > minBV && brtnValue < maxBV)
	{
		return 255 * (brtnValue - minBV) / (maxBV - minBV);
	}
	else if (brtnValue <= minBV)
		return 0;
	else
		return 255;
}

std::pair<unsigned char, unsigned char> CRSImage::BVtoBeLinearStretched
	(const int band, const int samples, const int lines,unsigned char* Data) const
{
	std::map<unsigned char, int> container = DN_Npts(band, samples, lines, Data);
	int count = 0;
	std::vector<double> percent(container.size(), 0);
	unsigned char min = 0;
	unsigned char max = 0;
	auto iter = container.begin();
	auto end = container.end();
	double amount = 0.0;
	if (container[255] > 0)
		amount = samples * lines * 1.0 - container[255];
	else
		amount = samples * lines * 1.0;
	for (;iter != end;++iter)
	{
		double percent_ = iter->second * 100/ amount ;
		percent[count] = percent_;
		auto percent_sum = std::accumulate(percent.begin(), percent.end(), 0.0);
		if (percent_sum > 2.0)
		{
			min = iter->first;
			++count;
			break;
		}
		++count;
	}
	for (; iter != end; ++iter)
	{
		double percent_ = iter->second * 100 / amount ;
		percent[count] = percent_;
		auto percent_sum = std::accumulate(percent.begin(), percent.end(), 0.0);
		if (percent_sum > 98.0)
		{
			max = iter->first;
			break;
		}
		++count;
	}
	return std::make_pair(min,max);
}

void CRSImage::ImageZoom(const double scale)
{
	auto colorTuple = SetBandColor();
	int nSamples = int(m_Samples * scale);
	int nLines = int(m_Lines * scale);
	initgraph(nSamples,nLines, EW_SHOWCONSOLE);
	setbkcolor(WHITE);
	cleardevice();
	IMAGE img(nSamples, nLines);
	DWORD* pImgBuffer = GetImageBuffer(&img);
	int redBand = std::get<0>(colorTuple);
	int greenBand = std::get<1>(colorTuple);
	int blueBand = std::get<2>(colorTuple);
	unsigned char* Data_backup = new unsigned char[nSamples * nLines * 3]{0};
	ZoomingAssist(redBand, greenBand, blueBand, Data_backup, nSamples, nLines, scale);
	SetPixel(pImgBuffer,Data_backup,1,2,3,nSamples,nLines);
	putimage(0, 0, &img);
	std::cout << "Do you want to save the image?(Y/N):" << '\n';
	char ch = '0';
	std::cin >> ch;
	if (ch == 'Y')
	{
		std::cout << "Please input the path that deposit the new file:" << std::endl;
		std::string path;
		std::cin >> path;
		std::cout << (SaveImage(Data_backup, nSamples, nLines, 3,path) ? "Success!":"Failed.") << std::endl;
		WriteMetaFile(nSamples, nLines, 3, m_Interleave, path);
		delete[] Data_backup;
	}
	else
	{
		closegraph();
		delete[] Data_backup;
	}
}

//Bilinear Resampling
//srcData is the address of the old band
unsigned char CRSImage::Resample(const int i,const int j,unsigned char* srcData,
	int nSamples,int nLines, double srcX,double srcY ) const
{
	if (i==0 || j==0 || i == nLines-1 || j == nSamples -1)
		return *(srcData + int(srcX) * m_Samples + int(srcY));
	int x1 = (int)srcX;
	int x2 = x1 + 1;
	int y1 = (int)srcY;
	int y2 = y1 + 1;
	unsigned char p1 = *(srcData + y1 + x1 * m_Samples) * (x2 - srcX) * (y2 - srcY);
	unsigned char p2 = *(srcData + y1 + x1 * m_Samples) * (srcX - x1) * (y2 - srcY);
	unsigned char p3 = *(srcData + y1 + x1 * m_Samples) * (x2 - srcX) * (srcY - y1);
	unsigned char p4 = *(srcData + y1 + x1 * m_Samples) * (srcX - x1) * (srcY - y1);
	return p1 + p2 + p3 + p4;
}



void CRSImage::ZoomingAssist(int redBand, int greenBand,
	int blueBand,unsigned char* myData,int samples, int lines, double scale) const
{
	BYTE* Red = m_Data + (redBand - 1) * m_Samples * m_Lines;
	BYTE* Green = m_Data + (greenBand - 1) * m_Samples * m_Lines;
	BYTE* Blue = m_Data + (blueBand - 1) * m_Samples * m_Lines;
	for (int i = 0; i < lines; ++i)
	{
		for (int j = 0; j < samples; ++j)
		{
			double srcX = CoordinateZoomTransf(i, scale);
			double srcY = CoordinateZoomTransf(j, scale);
			*(myData + i * samples + j) = Resample(i,j,Red,samples,lines,srcX,srcY);
			*(myData + i * samples + j + samples * lines) = Resample(i, j, Green,  
				samples, lines, srcX, srcY);
			*(myData + i * samples + j + samples * lines * 2) = Resample(i, j, Blue, 
				samples, lines, srcX, srcY);
		}
	}
}

double CRSImage::CoordinateZoomTransf(int x, double scale) const
{
	return (x + 0.5) / scale - 0.5;
}

void CRSImage::ImageFilter(const int kernel,const int red, const int green, const int blue,const int kind) const
{
	unsigned char* data = new unsigned char[m_Samples * m_Lines * 6];
	std::copy(m_Data, m_Data + m_Samples * m_Lines * m_Bands, data);
	BYTE* Red = data + (red - 1) * m_Samples * m_Lines;
	BYTE* Green = data + (green - 1) * m_Samples * m_Lines;
	BYTE* Blue = data + (blue - 1) * m_Samples * m_Lines;
	for (int i = 0; i < m_Lines; ++i)
	{
		for (int j = 0; j < m_Samples; ++j)
		{
			if (kind == 0)
			{
				if (i <= int(kernel / 2) || m_Lines - int(kernel / 2) <= i || j <= int(kernel / 2) || m_Samples - int(kernel / 2) <= j)
				{	}
				else
				{
					*(Red + i  * m_Samples + j) = MeanFilter(Red, i, j,kernel);
					*(Green + i  * m_Samples + j) = MeanFilter(Green, i, j,kernel);
					*(Blue + i  * m_Samples + j) = MeanFilter(Blue, i, j,kernel);
				}
			}
			else
			{
				if (i <= int(kernel / 2) || m_Lines - int(kernel / 2) <= i || j <= int(kernel / 2) || m_Samples - int(kernel / 2) <= j)
				{
				}
				else
				{ 
					*(Red + i * m_Samples + j) = LaplacianFilter(Red, i, j);
					*(Green + i * m_Samples + j) = LaplacianFilter(Green, i, j);
					*(Blue + i * m_Samples + j) = LaplacianFilter(Blue, i, j);
				}
				
			}
		}
	}
	initgraph(m_Samples, m_Lines, EW_SHOWCONSOLE);
	setbkcolor(WHITE);
	cleardevice();
	IMAGE img(m_Samples, m_Lines);
	DWORD* pImgBuffer = GetImageBuffer(&img);
	SetPixel(pImgBuffer, data, red, green, blue, m_Samples, m_Lines);
	putimage(0, 0, &img);
	std::cout << "Do you want to save the image?(Y/N):" << '\n';
	char ch = '0';
	ch = toupper(ch);
	std::cin >> ch;
	if (ch == 'Y')
	{
		std::cout << "Please input the path that deposit the new file:" << std::endl;
		std::string path;
		std::cin >> path;
		std::cout << (SaveImage(data, m_Samples, m_Lines, 3, path) ? "Success!" : "Failed.") << std::endl;
		WriteMetaFile(m_Samples, m_Lines, 3, m_Interleave, path);
		delete[] data;
	}
	else
	{
		closegraph();
		delete[] data;
	}
}

unsigned char CRSImage::MeanFilter(unsigned char* data, int i, int j, int kernel) const
{
	int bound1 = i - (int)(kernel / 2);
	int bound2 = i + (int)(kernel / 2);
	int bound3 = j - (int)(kernel / 2);
	int bound4 = j + (int)(kernel / 2);
	int DN_sum = 0; // do not write like this:unsigned char DN_sum = 0.Because the result might overflow.
	for (int x = bound1; x <= bound2; ++x)
	{
		for (int y = bound3; y <= bound4; ++y)
		{
			DN_sum += ( * (data + x * m_Samples + y));
		}
	}
	return  (DN_sum/(kernel*kernel));
}

unsigned char CRSImage::LaplacianFilter(unsigned char* data, int i, int j) const
{
	int src = *(data + i * m_Samples + j);
	int bound1 = i - 1;
	int bound2 = i + 1;
	int bound3 = j - 1;
	int bound4 = j + 1;
	int DN_sum = 0; // do not write like this:unsigned char DN_sum = 0.Because the result might overflow.
	for (int x = bound1; x <= bound2; ++x)
	{
		for (int y = bound3; y <= bound4; ++y)
		{
			if (x == i && y == j)
			{
				auto t = -8 * (*(data + x * m_Samples + y));
				DN_sum += t;
			}
			else
				DN_sum += (*(data + x * m_Samples + y));
		}
	}
	return  ((src - DN_sum) > 0) ? (src - DN_sum) : src;
}

void CRSImage::Information() const
{
	std::cout << "File path:" << m_ImgFilePath << '\n';
	std::ifstream in(m_MetaFilePath);
	if (in)
	{
		char str[100];
		while (!in.eof() && in.good())
		{
			in.getline(str, 100);
			std::cout << str << '\n';
		}
		std::cout << std::endl;
	}
}


void CRSImage::RotateImage(double angle) const
{
	auto colorTuple = SetBandColor();
	int red = std::get<0>(colorTuple);
	int green = std::get<1>(colorTuple);
	int blue = std::get<2>(colorTuple);
	double radian = angle * PI / 180.0;
	std::pair<int, int> windowSize = { int(m_Samples * cos(radian) + m_Lines * sin(radian)) + 1 ,
		int(m_Samples * sin(radian) + m_Lines * cos(radian)) + 1 };
	unsigned char* data = new unsigned char[windowSize.first * windowSize.second * 3];
	BYTE* Red = data ;
	BYTE* Green = data + windowSize.first* windowSize.second;
	BYTE* Blue = data + 2 * windowSize.first* windowSize.second;
	initgraph(windowSize.first, windowSize.second, EW_SHOWCONSOLE);
	setbkcolor(WHITE);
	cleardevice();
	IMAGE dstiImg(windowSize.first, windowSize.second);
	DWORD* pImgBuffer = GetImageBuffer(&dstiImg);
	RotateHelper(Red, m_Data + (red - 1) * m_Samples * m_Lines, radian, windowSize.first, windowSize.second);
	RotateHelper(Green, m_Data + (green - 1) * m_Samples * m_Lines, radian, windowSize.first, windowSize.second);
	RotateHelper(Blue, m_Data + (blue - 1) * m_Samples * m_Lines, radian, windowSize.first, windowSize.second);
	SetPixelForRotate(pImgBuffer, data, windowSize.first, windowSize.second);
	putimage(0, 0, &dstiImg);
	std::cout << "Do you want to save the image?(Y/N):" << '\n';
	char ch = '0';
	std::cin >> ch;
	if (ch == 'Y')
	{
		std::cout << "Please input the path that deposit the new file:" << std::endl;
		std::string path;
		std::cin >> path;
		std::cout << (SaveImage(data, windowSize.first, windowSize.second, 3, path) ? "Success!" : "Failed.") << std::endl;
		WriteMetaFile(windowSize.first, windowSize.second, 3, m_Interleave, path);
		delete[] data;
	}
	else
	{
		closegraph();
		delete[] data;
	}
}


void CRSImage::SetPixelForRotate(DWORD* pImgBuffer, unsigned char* data,  int samples, int lines) const
{
	BYTE* Red = data ;
	BYTE* Green = data +  samples * lines;
	BYTE* Blue = data + 2 * samples * lines;
	std::pair<unsigned char, unsigned char> BV1 = BVtoBeLinearStretched
	(1, samples, lines, data);
	std::pair<unsigned char, unsigned char> BV2 = BVtoBeLinearStretched
	(2, samples, lines, data);
	std::pair<unsigned char, unsigned char> BV3 = BVtoBeLinearStretched
	(3, samples, lines, data);
	for (int i = 0; i < lines; ++i)
	{
		for (int j = 0; j < samples; ++j)
		{
			auto red = LinearStretch(*(Red + j + i * samples), BV1);
			auto green = LinearStretch(*(Green + j + i * samples), BV2);
			auto blue = LinearStretch(*(Blue + j + i * samples), BV3);
			pImgBuffer[i * samples + j] = RGB(red, green, blue);
		}
	}
}

void CRSImage::RotateHelper(unsigned char* nData, unsigned char* srcData,
	double rad, int nSamples, int nLines) const
{
	for (int i = 0; i < nLines; ++i)
	{
		for (int j = 0; j < nSamples; ++j)
		{
			double srcX = i * cos(rad) + j * sin(rad) - cos(rad)  * sin(rad) * m_Samples;
			double srcY = j * cos(rad) - i * sin(rad) + m_Samples * sin(rad)  * sin(rad) ;
			if ((0 <= srcX && srcX <= m_Lines) && (0 <= srcY && srcY <= m_Samples))
			{
				*(nData + i * nSamples + j) = Resample(i, j, srcData, nSamples, nLines, srcX, srcY);
			}
			else
				*(nData + i * nSamples + j) = 255;
		}
	}

}


bool CRSImage::SaveImage(unsigned char* data,const int nSamples,const int nLines,const int nBands,const std::string& path) const
{
	
	std::ofstream ofs(path,std::ios_base::binary|std::ios_base::out);
	if (ofs.good())
	{
		if (m_Interleave == "bsq")
		{
			//BSQ
			for (int i = 0; i < nBands; ++i)
			{
				for (int j = 0; j < nLines; ++j)
				{
					ofs.write((char*)data + j * nSamples + nSamples * nLines * i
						, static_cast<std::streamsize>(sizeof(unsigned char)) * nSamples);
				}
			}
			return true;
		}
		else if (m_Interleave == "bil")
		{
			//BIL
			for (int i = 0; i < nLines; ++i)
			{
				for (int j = 0; j < nBands; ++j)
				{
					ofs.write(reinterpret_cast<char*>(data + nSamples * i + nSamples * nLines * j),
						static_cast<std::streamsize>(sizeof(unsigned char)) * nSamples);
				}
			}
			return true;
		}
		else if (m_Interleave == "bip")
		{
			//BIP
			for (int i = 0; i < nSamples * nLines; ++i)
			{
				for (int j = 0; j < nBands; ++j)
				{
					ofs.write(reinterpret_cast<char*>(data + i +
						nSamples * nLines * j), sizeof(unsigned char));
				}
			}
			return true;
		}
	}
	else
		return false;
}

void CRSImage::WriteMetaFile(int nSamples, int nLines, int nBands,
	const std::string& nInterleave, std::string& nFilePath) const
{
	std::string metaFile;
	if (nFilePath.find('.') == nFilePath.npos)
		metaFile = nFilePath + ".hdr";
	else
		metaFile = nFilePath.substr(0, nFilePath.find('.')) + ".hdr";
	std::ofstream ofs(metaFile, std::ios_base::binary | std::ios_base::out);
	ofs << "samples = " << nSamples << '\n' << "lines = " << nLines << '\n' << "bands = "
		<< nBands << '\n' << "interleave = " << nInterleave;
}

CRSImage::CRSImage()
	: m_Data(nullptr),m_Samples(0),m_Lines(0),m_Bands(0),
	m_Interleave(" "), m_MetaFilePath(" ") ,m_ImgFilePath(" ")
{}

CRSImage::~CRSImage()
{
	--COUNT;
	delete[] m_Data;
}

void CRSImage::Clean()
{
	delete[] m_Data;
	m_Data = nullptr;
}