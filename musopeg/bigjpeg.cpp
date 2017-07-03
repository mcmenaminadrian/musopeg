#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <setjmp.h>
#include <string>
#include <vector>
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

	for (int i = 0; i < lines.size(); i++) {
		char *innerLine = lines.at(i);
		for (int j = 0; j < row_stride; j= j + cinfo.output_components) {
			cout << "(";
			for (int k = 0; k < cinfo.output_components; k++) {
				if (k > 0) {
					cout << ",";
				}
				unsigned char x = innerLine[j + k];
				cout << (unsigned int)(x);
			}
			cout << ")";
		}
		cout << endl;
	}

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

