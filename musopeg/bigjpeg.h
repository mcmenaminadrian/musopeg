#ifndef BIGJPEG_H
#define BIGJPEG_H

#include <QObject>
#include <QString>
#include <fstream>
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
	void display(int widthD, int heightD, int startW, int startH);
	void _displayImages();
	void _badImage();
	void _goodImage();
	std::ofstream fileOut;

public slots:
	void displayImages();
	void goodImage();
	void badImage();

signals:
	void updateImages();
	void completedRun();


public:
	QImage* topImage;
	BigJPEG(QString fileName);
	~BigJPEG();

};

#endif // BIGJPEG_H
