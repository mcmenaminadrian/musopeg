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
	wD = cinfo.output_width;
	hD = 200;
	sW = 0;
	sH = 0;


//	jpeg_finish_decompress(&cinfo);
//	jpeg_destroy_decompress(&cinfo);


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
	display(cInfo, wD, hD, sW, sH);
}

void BigJPEG::display(const struct jpeg_decompress_struct& cinfo, int widthD, int heightD,
	int startW, int startH)
{
	if (widthD < 100) {
		return;
	}
	if (heightD < 100) {
		heightD = 100;
	}
	topImage = new QImage(widthD, heightD, QImage::Format_Grayscale8);
	for (int i = startH; i < startH + heightD; i++){
		unsigned char* innerLine = lines.at(i);
		for (int j = startW; j < startW + widthD; j++)
		{
			QRgb pixels = qRgb(value, value, value);
			topImage->setPixel(j, i, pixels);
		}
	}

	emit updateImages();
	wD = widthD / 2;
	hD = heightD / 2;
}
