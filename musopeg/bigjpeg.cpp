#include <stdio.h>
#include <cstdlib>
#include "stdint.h"
#include <cstring>
#include <iostream>
#include <setjmp.h>
#include <string>
#include <vector>
#include <QPixmap>
#include <QByteArray>
#include <QImage>
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
	jpeg_start_decompress(&cinfo);
	row_stride = cinfo.output_width * cinfo.output_components;
	setByteWidth(row_stride);
	//buffer = (JSAMPARRAY)malloc(row_stride);
	buffer = (*cinfo.mem->alloc_sarray) ((j_common_ptr)
		&cinfo, JPOOL_IMAGE, row_stride, 1);

	while (cinfo.output_scanline < cinfo.output_height) {
		jpeg_read_scanlines(&cinfo, buffer, 1);
		storeScannedLine(buffer);
	}



	topImage = new QPixmap(cinfo.image_width, 20);
	QByteArray rawArray;
	rawArray.append('B');
	rawArray.append('M');
	u_int32_t sizeOfBMP = row_stride * 20 + 14 + 12;
	rawArray.append(sizeOfBMP & 0xFF);
	rawArray.append((sizeOfBMP >> 8) & 0xFF);
	rawArray.append((sizeOfBMP >> 16) & 0xFF);
	rawArray.append((sizeOfBMP >> 24) & 0xFF);
	rawArray.append(0x12);
	rawArray.append('\0');
	rawArray.append('\0');
	rawArray.append('\0');
	uint16_t widthO = cinfo.output_width;
	rawArray.append(widthO & 0xFF);
	rawArray.append((widthO >> 8) & 0xFF);
	uint16_t heightO = cinfo.output_height;
	rawArray.append(heightO & 0xFF);
	rawArray.append((heightO >> 8) & 0xFF);
	rawArray.append(0x01);
	rawArray.append('\0');
	rawArray.append(0x08);
	rawArray.append('\0');

	for (int i = 0; i < 20; i++){

		char* innerLine = lines.at(i);
		for (int j = 0; j < row_stride; j++) {
			rawArray.append(innerLine[j]);
		}
	}


	bool loaded = topImage->loadFromData(rawArray);
	cout << "loaded returns " << loaded << endl;

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);


}

void BigJPEG::setByteWidth(const int &count)
{
	byteWidth = count;
}

bool BigJPEG::storeScannedLine(const JSAMPARRAY sampledLine)
{
	char lineIn[byteWidth];
	strncpy(lineIn, (const char*)sampledLine, byteWidth);
	lines.push_back(lineIn);
	return true;
}

