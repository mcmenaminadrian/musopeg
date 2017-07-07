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
#include <QString>
#include <QRegularExpression>
#include <fstream>
#include "jpeglib.h"
#include "bigjpeg.h"

using namespace std;

BigJPEG::BigJPEG(QString fileName)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE* inFile;
	JSAMPARRAY buffer;
	int row_stride;

	cout << "Opening " << qPrintable(fileName) << endl;

	if ((inFile = fopen(qPrintable(fileName), "rb")) == NULL){
		fprintf(stderr, "cannot open %s\n", qPrintable(fileName));
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
	fclose(inFile);

	QString outName = fileName.remove(QRegularExpression("\\..*"));
	outName += ".data";
	fileOut.open(qPrintable(outName));
}

BigJPEG::~BigJPEG()
{
	for (unsigned int i = 0; i < lines.size(); i++)
	{
		unsigned char *lineOut = lines.at(i);
		free(lineOut);
	}
	if (topImage) {
		delete topImage;
	}
	fileOut.close();
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
	display(widthDisplayed, heightDisplayed,
		startingWidth, startingHeight);
}

void BigJPEG::goodImage()
{
	_goodImage();
}

void BigJPEG::_goodImage()
{
	//quarters or strips
	if (widthDisplayed > 100) {
		if (heightDisplayed > 100) {
			//show strip
			heightDisplayed = 100;
			return _displayImages();
		} else {
			//block
			widthDisplayed = 100;
			heightDisplayed = 100;
			return _displayImages();
		}
	}
	//have a good block - need to record it and then move on
	fileOut << startingWidth << "," << startingHeight << "," << widthDisplayed;
	fileOut << "," << heightDisplayed << endl;
	bool left = (startingWidth < quarterWidth);
	bool top = (startingHeight < quarterHeight);
	startingWidth += 100;
	if (left && (startingWidth >= quarterWidth)) {
		startingHeight += 100;
		widthDisplayed = quarterWidth;
		if ((top && (startingHeight >= quarterHeight)) || (
			!top && (startingHeight >= (quarterHeight * 2)))) {
			startingWidth = quarterWidth;
			if (top) {
				startingHeight = 0;
			} else {
				startingHeight = quarterHeight;
			}
			heightDisplayed = quarterHeight;
			return _displayImages();
		} else {
			startingWidth = 0;
			heightDisplayed = 100;
			return _displayImages();
		}
	} else if (startingWidth >= (quarterWidth * 2)) {
		startingHeight += 100;
		if (startingHeight >= (quarterHeight * 2)) {
			emit completedRun();
			return;
		}
		widthDisplayed = quarterWidth;
		if (top && startingHeight >= quarterHeight) {
			startingWidth = 0;
			startingHeight = quarterHeight;
			heightDisplayed = quarterHeight;
			return _displayImages();
		} else {
			startingWidth = quarterWidth;
			return _displayImages();
		}
	}
	return _displayImages();
}

void BigJPEG::badImage()
{
	_badImage();
}

void BigJPEG::_badImage()
{
	//quarters or strips
	if (widthDisplayed > 100) {
		if (heightDisplayed > 100) {
			//first quarter and third
			if (startingWidth == 0) {
				startingWidth = quarterWidth;
				return _displayImages();
			} else {
			//second or fourth quarter
				if (startingHeight > 0) {
					emit completedRun();
					return;
				} else {
					startingWidth = 0;
					startingHeight = quarterHeight;
					return _displayImages();
				}
			}
		} else {
			//strip
			bool left = (startingWidth < quarterWidth);
			bool top = (startingHeight < quarterHeight);
			startingHeight += 100;
			if (left && top && (startingHeight >= quarterHeight)) {
				startingHeight = 0;
				startingWidth = quarterWidth;
				widthDisplayed = quarterWidth;
				heightDisplayed = quarterHeight;
				return _displayImages();
			} else if (top && (startingHeight >= quarterHeight)) {
				startingHeight = quarterHeight;
				startingWidth = 0;
				widthDisplayed = quarterWidth;
				heightDisplayed = quarterHeight;
				return _displayImages();
			} else if (left && (startingHeight >= (quarterHeight * 2))) {
				startingHeight = quarterHeight;
				startingWidth = quarterWidth;
				widthDisplayed = quarterWidth;
				heightDisplayed = quarterHeight;
				return _displayImages();
			} else if (startingHeight >= (quarterHeight * 2)) {
				emit completedRun();
				return;
			}
		}
	} else {
		//100 x 100 blocks
		bool left = (startingWidth < quarterWidth);
		bool top = (startingHeight < quarterHeight);
		startingWidth += 100;
		if (left && top && (startingWidth >= quarterWidth)) {
			startingHeight += 100;
			if (startingHeight >= quarterHeight) {
				startingWidth = quarterWidth;
				startingHeight = quarterHeight;
				widthDisplayed = quarterWidth;
				heightDisplayed = quarterHeight;
				return _displayImages();
			} else {
				startingWidth = 0;
				widthDisplayed = quarterWidth;
				return _displayImages();
			}
		} else if (top && (startingWidth >= (quarterWidth * 2))) {
			startingHeight += 100;
			if (startingHeight >= quarterHeight) {
				startingHeight = quarterHeight;
				startingWidth = 0;
				widthDisplayed = quarterWidth;
				heightDisplayed = quarterHeight;
				return _displayImages();
			} else {
				startingWidth = quarterWidth;
				widthDisplayed = quarterWidth;
				return _displayImages();
			}
		} else if (left && (startingWidth >= quarterWidth)) {
			startingHeight += 100;
			if (startingHeight >= (quarterHeight * 2)) {
				startingHeight = quarterHeight;
				startingWidth = quarterWidth;
				widthDisplayed = quarterWidth;
				heightDisplayed = quarterHeight;
				return _displayImages();
			} else {
				startingWidth = quarterWidth;
				widthDisplayed = quarterWidth;
				return _displayImages();
			}
		} else if (startingWidth >= (quarterWidth * 2)) {
			startingHeight += 100;
			if (startingHeight >= (startingHeight * 2)){
				emit completedRun();
				return;
			}
			widthDisplayed = quarterWidth;
			startingWidth = quarterWidth;
			return _displayImages();
		}
	}
	return _displayImages();
}

void BigJPEG::display(int widthD, int heightD, int startW, int startH)
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
