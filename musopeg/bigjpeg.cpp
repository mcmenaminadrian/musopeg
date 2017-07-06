#include <stdio.h>
#include <cstdlib>
#include "stdint.h"
#include <cstring>
#include <iostream>
#include <setjmp.h>
#include <string>
#include <vector>
#include <QObject>
#include <QByteArray>
#include <QImage>
#include <QThread>
#include "jpeglib.h"
#include "bigjpeg.h"

using namespace std;

BigJPEG::BigJPEG(const string& fileName)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE* inFile;
	JSAMPARRAY buffer;
	int row_stride;

	cerr << "Opening " << fileName << endl;

	if ((inFile = fopen(fileName.c_str(), "rb")) == NULL){
		fprintf(stderr, "cannot open %s\n", fileName.c_str());
		return;
	}

	cinfo.err = jpeg_std_error(&jerr);

	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, inFile);

	jpeg_read_header(&cinfo, TRUE);
	cinfo.out_color_space = JCS_GRAYSCALE;
	jpeg_start_decompress(&cinfo);
	row_stride = cinfo.output_width * cinfo.output_components;
	setByteWidth(row_stride);
	buffer = (*cinfo.mem->alloc_sarray) ((j_common_ptr)
		&cinfo, JPOOL_IMAGE, row_stride, 1);

	while (cinfo.output_scanline < cinfo.output_height) {
		jpeg_read_scanlines(&cinfo, buffer, 1);
		storeScannedLine(buffer[0]);
	}
	cInfo = cinfo;
	widthDisplayed = (cinfo.output_width/100) * 50;
	heightDisplayed = (cinfo.output_height/100) * 50;
	widthRag = cinfo.output_width%100;
	heightRag = cinfo.output_height%100;
	quarterHeight = heightDisplayed;
	quarterWidth = widthDisplayed;
	startingWidth = 0;
	startingHeight = 0;
	topImage = NULL;

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

}

BigJPEG::~BigJPEG()
{
	for (int i = 0; i < lines.size(); i++)
	{
		unsigned char *lineOut = lines.at(i);
		free(lineOut);
	}
	if (topImage) {
		delete topImage;
	}
}

void BigJPEG::setByteWidth(const int &count)
{
	byteWidth = count;
}

bool BigJPEG::storeScannedLine(JSAMPROW sampledLine)
{
	unsigned char* lineIn = (unsigned char *)malloc(byteWidth);
	memcpy(lineIn, sampledLine, byteWidth);
	lines.push_back(lineIn);
	return true;
}

void BigJPEG::displayImages()
{
	_displayImages();
}

void BigJPEG::_displayImages()
{
	display(cInfo, widthDisplayed, heightDisplayed,
		startingWidth, startingHeight);
}

void BigJPEG::goodImage()
{

}

void BigJPEG::badImage()
{

	//quarters or strips
	if (widthDisplayed > 100) {
		if (heightDisplayed > 100) {
			//first quarter and third
			if (startingWidth == 0) {
				startingWidth += widthDisplayed;
				return _displayImages();
			} else {
			//second or fourth quarter
				if (startingHeight > 0) {
					return;
				} else {
					startingWidth = 0;
					startingHeight += heightDisplayed;
					return _displayImages();
				}
			}
		} else {
			//strip
			bool left = (startingWidth < quarterWidth);
			bool top = (startingHeight < quarterHeight);
			startingHeight += 100;
			if (left && top && startingHeight >= quarterHeight) {
				startingHeight = 0;
				startingWidth = quarterWidth;
				widthDisplayed = quarterWidth;
				heightDisplayed = quarterHeight;
				_displayImages();
			} else if (top && startingHeight >= startingHeight) {
				startingHeight = quarterHeight;
				startingWidth = 0;
				widthDisplayed = quarterWidth;
				heightDisplayed = quarterHeight;
				_displayImages();

			} else if (left && startingHeight >= (quarterHeight * 2)) {
				startingHeight = quarterHeight;
				startingWidth = quarterWidth;
				widthDisplayed = quarterWidth;
				heightDisplayed = quarterHeight;
				_displayImages();
			} else if (startingHeight >= (quarterHeight * 2)) {
				return;
			}
		}
	}

}

void BigJPEG::display(const struct jpeg_decompress_struct& cinfo, int widthD, int heightD,
	int startW, int startH)
{
	delete topImage;
	topImage = NULL;

	if (widthD < 100) {
		widthD = 100;
	}
	if (heightD < 100) {
		heightD = 100;
	}
	topImage = new QImage(widthD, heightD, QImage::Format_Grayscale8);
	for (int i = startH; i < startH + heightD; i++){
		unsigned char* innerLine = lines.at(i);
		for (int j = startW; j < startW + widthD; j++)
		{
			uint8_t value = *(innerLine + j);
			QRgb pixels = qRgb(value, value, value);
			topImage->setPixel(j - startW, i - startH, pixels);
		}
	}

	emit updateImages();
}
