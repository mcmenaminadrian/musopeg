#ifndef BIGJPEG_H
#define BIGJPEG_H

#include <QObject>
#include "jpeglib.h"

class BigJPEG : public QObject
{
	Q_OBJECT

private:
	int byteWidth;
	std::vector<unsigned char *> lines;
	void setByteWidth(const int& count);
	bool storeScannedLine(JSAMPROW sampledLine);
	int widthDisplayed, heightDisplayed;
	int startingWidth, startingHeight;
	int widthRag, heightRag, quarterHeight, quarterWidth;
	struct jpeg_decompress_struct cInfo;
	void display(const struct jpeg_decompress_struct& dinfo, int widthD, int heightD,
		 int startW, int startH);
	void _displayImages();
	void _badImage();
	void _goodImage();

public slots:
	void displayImages();
	void goodImage();
	void badImage();

signals:
	void updateImages();


public:
	QImage* topImage;
	BigJPEG(const std::string& fileName);
	~BigJPEG();

};

#endif // BIGJPEG_H
