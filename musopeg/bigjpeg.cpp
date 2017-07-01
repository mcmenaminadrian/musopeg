#include <stdio.h>
#include <iostream>
#include <setjmp.h>
#include <string>
#include "jpeglib.h"
#include "bigjpeg.h"

using namespace std;

struct my_error_mgr {
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
};

BigJPEG::BigJPEG(string fileName)
{
	struct jpeg_decompress_struct cinfo;
	FILE* inFile;
	JSAMPARRAY buffer;
	int row_stride;

	if ((inFile = fopen(fileName.c_str(), "rb")) == NULL){
		fprintf(stderr, "cannot open %s\n", fileName.c_str());
		return;
	}

	struct my_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if (setjmp(jerr.setjmp_buffer)) {
		jpeg_destroy_compress(&cinfo);
		fclose(inFile);
		return;
	}

	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, inFile);

	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);



}

