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
	int wD, hD, sW, sH;
	struct jpeg_decompress_struct cInfo;
	void display(const struct jpeg_decompress_struct& dinfo, int widthD, int heightD,
		 int startW, int startH);

public slots:
	void displayImages();
signals:
	void updateImages();


public:
	QImage* topImage;
	BigJPEG(const std::string& fileName);
	~BigJPEG();

};

#endif // BIGJPEG_H
