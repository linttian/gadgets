/* 
A simple remote sensing image processing program.
This program read an image with .hdr file and manipulate it in several ways.

*/





#include "pch.h"
int CRSImage::COUNT = 0;
void Menu();
//bool state = true;
char cmd = '0';
double scale = 1.0;
int red = 1, green = 1, blue = 1;
double angle = 0;
int band = 1;
std::string path;
int kernel = 3;
int kind = 0;




int main()
{
	Menu();
	CRSImage* rsi = new CRSImage;
	while (std::cin >> cmd)
	{		
		cmd = toupper(cmd);
		switch (cmd)
		{
		case('O'):
			std::cout << "Input the path:";
			if (rsi->COUNT != 0)
				rsi->Clean();
			std::cin >> path;
			std::cout<<(rsi->OpenFile(path)?"Open the file successfully!":"Failed to open the file.") << std::endl;
			break;
		case 'I':
			if (rsi->COUNT == 0)
			{
				std::cout << "Please open a file first." << std::endl;
				break;
			}
			rsi->Information();
			break;
		case 'D':
			rsi->Display();
			break;
		case 'C':
			if ((rsi->COUNT) == 0)
			{
				std::cout << "Please open a file first." << std::endl;
				break;
			}
			closegraph();
			break;
		case 'S':
			if ((rsi->COUNT) == 0)
			{
				std::cout << "Please open a file first." << std::endl;
				break;
			}
			rsi->ShowBasicStats();
			break;
		case 'H':
			if (rsi->COUNT == 0)
			{
				std::cout << "Please open a file first." << std::endl;
				break;
			}
			std::cout << "Please input the band:";
			std::cin >> band;
			rsi->Histogram(band);
			rsi->HistogramDisplay(band);
			break;
		case '?':
			Menu();
			break;
		case 'R':
			if ((rsi->COUNT) == 0)
			{
				std::cout << "Please open a file first." << std::endl;
				break;
			}
			std::cout << "Please input the angle and three bands:";
			std::cin >> angle ;
			rsi->RotateImage(angle);
			break;
		case'Z':
			if ((rsi->COUNT) == 0)
			{
				std::cout << "Please open a file first." << std::endl;
				break;
			}	
			std::cout << "Please input the scale:";
			std::cin >> scale;
			rsi->ImageZoom(scale);
			break;
		case 'F':
			if ((rsi->COUNT) == 0)
			{
				std::cout << "Please open a file first." << std::endl;
				break;
			}
			std::cout << "Smooth filter or sharpen filter?(0/1)\n";
			std::cin >> kind;
			if (kind == 0)
			{
				std::cout << "Please input the kernel and three bands:";
				std::cin >> kernel >> red >> green >> blue;
				rsi->ImageFilter(kernel, red, green, blue , 0);
			}
			else if(kind == 1)
			{
				std::cout << "Please input three bands:";
				std::cin >> red >> green >> blue;
				rsi->ImageFilter(3, red, green, blue,1);
			}
			break;
		case 'X' :
			delete rsi;
			return 1;
		}					
	}
}




void Menu()
{
	std::cout << "############ Remote Sensing Image Process Tools.############\n";
	std::cout << "# X 每 Exit\n";
	std::cout << "# O 每 Open\n" ;
	std::cout << "# I 每 Information\n";
	std::cout << "# D 每 Display the image \n";
	std::cout << "# C 每 Closed the image that currently displayed\n";
	std::cout << "# S 每 Statistics\n";
	std::cout << "# H 每 Histogram\n";
	std::cout << "# ? 每 Help\n" ;
	std::cout << "# R 每 Rotate(anticlockwise,< 90)\n" ;
	std::cout << "# Z 每 Zoom\n"  ;
	std::cout << "# F 每 Filter\n" ;
	std::cout << "#################################################################" << std::endl;
}

