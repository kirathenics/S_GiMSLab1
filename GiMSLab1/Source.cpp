// bmp_editor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

using namespace std;

#define BMP 1
#define BSO 2

#pragma pack(2)

//��������� ����� BMP 
typedef struct tBITMAPFILEHEADER
{
	WORD bfType;
	DWORD bfSize;
	WORD bfReserved1;
	WORD bfReserved2;
	DWORD bfOffBits;
}sFileHead;

//��������� BitMap's
typedef struct tBITMAPINFOHEADER
{
	DWORD biSize;
	LONG biWidth;
	LONG biHeight;
	WORD biPlanes;
	WORD biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	LONG biXPelsPerMeter;
	LONG biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
}sInfoHead;

//��������� ����� BSO
typedef struct tBSOHEADER
{
	WORD bfType;
	BYTE bfComment[16] = "comment";
	WORD bfOffset;
	DWORD bfSize;
	BYTE bfColorDepth;
	DWORD bfHeight;
	BYTE bfCompressionType;
}sBSOHead;

sFileHead FileHead;
sInfoHead InfoHead;
sBSOHead BSOHead;

//�������
struct Color
{
	BYTE blue;
	BYTE green;
	BYTE red;
};

//������ 1-�� �������
int pixel_size = sizeof(Color);


//1 - BMP, 2 - BSO
int img_type = 0;

//�������� �����������
Color* src_image = 0;
//�������������� �����������
Color* dst_image = 0;

//������ �����������
int width = 0;
int height = 0;

//������� ��������� BMP �����
void ShowBMPHeaders(tBITMAPFILEHEADER fh, tBITMAPINFOHEADER ih)
{
	cout << "Type: " << (CHAR)fh.bfType << endl;
	cout << "Size: " << fh.bfSize << endl;
	cout << "Shift of bits: " << fh.bfOffBits << endl;
	cout << "Width: " << ih.biWidth << endl;
	cout << "Height: " << ih.biHeight << endl;
	cout << "Planes: " << ih.biPlanes << endl;
	cout << "BitCount: " << ih.biBitCount << endl;
	cout << "Compression: " << ih.biCompression << endl;
}

//������� ��������� BKA �����
void ShowBSOHeaders(tBSOHEADER fh)
{
	cout << "Type: " << (CHAR)fh.bfType << endl;
	cout << "Comment: " << fh.bfComment << endl;
	cout << "Offset: " << fh.bfOffset << endl;
	cout << "Size: " << fh.bfSize << endl;
	cout << "Color depth: " << fh.bfColorDepth << endl;
	cout << "Height: " << fh.bfHeight << endl;
	cout << "Width: " << (fh.bfSize - fh.bfOffset) / fh.bfHeight << endl;
	cout << "Compression type: " << fh.bfCompressionType << endl;
}

//������� ���� � �����������
void ReadPath(string& str)
{
	string bmp = ".bmp", bso = ".bso";
	while (true)
	{
		str.clear();
		cout << "Enter path to image" << endl;
		cin >> str;

		if (bmp == str.substr(str.length() - bmp.length()))
		{
			img_type = 1;
			break;
		}
		if (bso == str.substr(str.length() - bso.length()))
		{
			img_type = 2;
			break;
		}
		cout << "Wrong file format!" << endl;
	}
}

//������� ��� �������� �����������
bool OpenImage(string path)
{
	ifstream img_file;
	Color temp;
	char buf[3];

	//������� ���� �� ������
	img_file.open(path.c_str(), ios::in | ios::binary);
	if (!img_file)
	{
		cout << "File isn`t open!" << endl;
		return false;
	}

	switch (img_type)
	{
	case BMP:
		//������� ��������� BMP
		img_file.read((char*)&FileHead, sizeof(FileHead));
		img_file.read((char*)&InfoHead, sizeof(InfoHead));

		ShowBMPHeaders(FileHead, InfoHead);
		//��������� ����� � ������ �����������
		width = InfoHead.biWidth;
		height = InfoHead.biHeight;
		break;
	case BSO:
		//������� ��������� BKA
		img_file.read((char*)&BSOHead, sizeof(BSOHead));

		ShowBSOHeaders(BSOHead);
		//��������� ����� � ������ �����������
		width = (BSOHead.bfSize - BSOHead.bfOffset) / BSOHead.bfHeight;
		height = BSOHead.bfHeight;
		break;
	default:
		break;
	}

	//�������� ����� ��� �����������
	src_image = new Color[width * height];

	int i, j;
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			img_file.read((char*)&temp, pixel_size);
			src_image[i * width + j] = temp;
		}
		//�������� ���� ������������ ��� ������������ �� �������� �����
		img_file.read((char*)buf, j % 4);
	}
	img_file.close();

	return true;
}

//������� ��� ���������� �����������
bool SaveImage(string path)
{
	ofstream img_file;
	char buf[3];

	//������� ���� �� ������
	img_file.open(path.c_str(), ios::out | ios::binary);
	if (!img_file)
	{
		return false;
	}

	switch (img_type)
	{
	case BMP:
		img_file.write((char*)&FileHead, sizeof(FileHead));
		img_file.write((char*)&InfoHead, sizeof(InfoHead));

		//����������� �� ��������� � �������������� �����������
		dst_image = new Color[width * height];
		memcpy(dst_image, src_image, width * height * sizeof(Color));
		break;
	case BSO:
		img_file.write((char*)&BSOHead, sizeof(BSOHead));

		//����������� �� ��������� � �������������� �����������
		dst_image = new Color[width * height];
		memcpy(dst_image, src_image, width * height * sizeof(Color));
		break;
	default:
		break;
	}

	//�������� ����
	int i, j;
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			img_file.write((char*)&dst_image[i * width + j], pixel_size);
		}
		img_file.write((char*)buf, j % 4);
	}
	img_file.close();

	return true;
}

//���������� ������� ����������� � ������� ������ ������������ ������������
void ShowImage(string path)
{
	switch (img_type)
	{
	case BMP:
		ShowBMPHeaders(FileHead, InfoHead);
		system(path.c_str());
		break;
	case BSO:
	{
		ShowBSOHeaders(BSOHead);

		FileHead.bfType = 0x4D42;
		FileHead.bfSize = sizeof(sFileHead) + sizeof(sInfoHead) + height * width * sizeof(Color);
		FileHead.bfReserved1 = 0;
		FileHead.bfReserved2 = 0;
		FileHead.bfOffBits = sizeof(sFileHead) + sizeof(sInfoHead);

		InfoHead.biSize = sizeof(sInfoHead);
		InfoHead.biWidth = width;
		InfoHead.biHeight = height;
		InfoHead.biPlanes = 1;
		InfoHead.biBitCount = sizeof(Color) * 8;
		InfoHead.biCompression = 0;
		InfoHead.biSizeImage = height * width * sizeof(Color);
		InfoHead.biXPelsPerMeter = 0;
		InfoHead.biYPelsPerMeter = 0;
		InfoHead.biClrUsed = 0;
		InfoHead.biClrImportant = 0;

		string bmpName = "gimslab1.bmp";
		ofstream file(bmpName, ios::binary | ios::trunc);

		char buf[3];
		if (file)
		{
			// ���������� ��������� ����� BMP
			file.write((char*)&FileHead, sizeof(sFileHead));
			file.write((char*)&InfoHead, sizeof(sInfoHead));

			// ���������� ������ ��������
			int i, j;
			for (i = 0; i < height; i++)
			{
				for (j = 0; j < width; j++)
				{
					file.write((char*)&src_image[i * width + j], pixel_size);
				}
				file.write((char*)buf, j % 4);
			}

			file.close();
			system(bmpName.c_str());
		}
		else
		{
			std::cout << "Failed to create BMP image: " << bmpName << std::endl;
		}
		break;
	}
	default:
		break;
	}
}

//����������� ���������� ��������������� ����������� � ���������
void CopyDstToSrc()
{
	if (dst_image != 0)
	{
		memcpy(src_image, dst_image, width * height * sizeof(Color));
	}
}

void ClearMemory(void) {
	//���������� ������ ��������� �����������
	if (src_image != 0)
	{
		delete[] src_image;
	}
	//���������� ������ �������������� �����������
	if (dst_image != 0)
	{
		delete[] dst_image;
	}
}

//���������� ����������� � �������� ����� �����������
void AddNoise(double probability)
{
	int size = width * height;
	int count = (int)(size * probability) / 100;
	int x, y;
	long pos;
	for (int i = 0; i < count; i++)
	{
		x = rand() % width;
		y = rand() % height;
		pos = y * width + x;
		src_image[pos].blue = rand() % 255;
		src_image[pos].green = rand() % 255;
		src_image[pos].red = rand() % 255;
	}
	cout << "Point was added: " << count << endl;
}

void MedianFilter()
{
	for (int i = 1; i < height - 1; ++i) 
	{
		for (int j = 1; j < width - 1; ++j)
		{
			vector<Color> windowValues;
			vector<int> weights = { 1, 2, 1, 2, 4, 2, 1, 2, 1 };

			for (int k = -1; k <= 1; ++k) 
			{
				for (int l = -1; l <= 1; ++l) 
				{
					Color value = src_image[(i + k) * width + (j + l)];
					windowValues.push_back(value);
				}
			}

			sort(windowValues.begin(), windowValues.end(), [&](Color a, Color b) 
				{
				int intensityA = a.red + a.green + a.blue;
				int intensityB = b.red + b.green + b.blue;
				return intensityA * weights[4] < intensityB * weights[4];
				});

			// ���������� �������� ������������ ��������
			src_image[i * width + j] = windowValues[4];
		}
	}
}

int main(int argc, char* argv[])
{
	srand((unsigned)time(NULL));

	//���� � �������� �����������
	string path_to_image, temp;

	ReadPath(path_to_image);
	OpenImage(path_to_image);
	ShowImage(path_to_image);
	AddNoise(10);
	MedianFilter();

	BSOHead.bfType = 0x4D42; // 'BM'
	BSOHead.bfOffset = sizeof(sBSOHead);
	BSOHead.bfSize = sizeof(sBSOHead) + width * height;
	BSOHead.bfColorDepth = 8;
	BSOHead.bfHeight = height;
	BSOHead.bfCompressionType = 0;

	ReadPath(temp);
	SaveImage(temp);
	ShowImage(temp);
	ClearMemory();
	cout << "END!" << endl;
	return 0;
}

