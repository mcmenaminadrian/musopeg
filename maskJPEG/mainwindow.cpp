#include <QFileDialog>
#include <QString>
#include <QGraphicsScene>
#include <vector>
#include <QMessageBox>
#include <fstream>
#include <sstream>
#include <iostream>
#include "jpeglib.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"


using namespace std;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	jpegFile = QFileDialog::getOpenFileName(this, tr("Pick JPEG"), "");
	if ((inFile = fopen(qPrintable(jpegFile), "rb")) == NULL){
			fprintf(stderr, "cannot open %s\n", qPrintable(jpegFile));
			return;
	}

}

void MainWindow::on_pushButton_clicked()
{

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPARRAY buffer;
	int row_stride;


	cinfo.err = jpeg_std_error(&jerr);

	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, inFile);

	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);
	row_stride = cinfo.output_width * cinfo.output_components;
	setByteWidth(row_stride);
	buffer = (*cinfo.mem->alloc_sarray) ((j_common_ptr)
		&cinfo, JPOOL_IMAGE, row_stride, 1);

	while (cinfo.output_scanline < cinfo.output_height) {
		jpeg_read_scanlines(&cinfo, buffer, 1);
		storeScannedLine(buffer[0]);
	}

	//show untainted File
	musicPic = new QImage(cinfo.output_width, cinfo.output_height,
			QImage::Format_RGB888);
	for (unsigned int i = 0; i < lines.size(); i++){
		unsigned char* innerLine = lines.at(i);
		for (int j = 0; j < row_stride; j =  j + cinfo.output_components)
		{
			QRgb pixels = qRgb(*(innerLine + j), *(innerLine + j + 1),
				*(innerLine + j + 2));
			musicPic->setPixel(j/3, i, pixels);
		}
	}

	//get data file
	QString dataFile = jpegFile.remove(QRegularExpression("\\..*"));
	dataFile += ".data";

	showUs = new QGraphicsScene(this);

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(inFile);
	QObject::connect(this, SIGNAL(timeToUpdate()), this,
		SLOT(updateGraphics()));
	ui->graphicsView->setScene(showUs);
	//now process the data file
	ifstream dataStream(qPrintable(dataFile));
	string line;
	while (getline(dataStream, line))
	{
		istringstream iss(line);
		unsigned int xCoord, yCoord, xSpan, ySpan;
		char c;
		if (!(iss >> xCoord >> c >> yCoord >> c >> xSpan >> c >> ySpan))
		{
			cerr << "FAIL" << endl;
			break;
		}
		for (unsigned int i = yCoord; i < yCoord + ySpan; i++) {
			unsigned char *line = lines.at(i);
			for (unsigned int j = xCoord; j < xCoord + xSpan; j++) {

				QRgb pixel = qRgb(0xFF, *(line + (j + 1) * 3),
					*(line + (j + 2) * 3));
				musicPic->setPixel(j, i, pixel);
			}
		}
	}
	emit timeToUpdate();

}

void MainWindow::updateGraphics()
{
	qDeleteAll(showUs->items());
	QPixmap musicPix = QPixmap::fromImage(*musicPic);
	showUs->addPixmap(musicPix);

}

void MainWindow::setByteWidth(const int &count)
{
	byteWidth = count;
}

bool MainWindow::storeScannedLine(JSAMPROW sampledLine)
{
	unsigned char* lineIn = (unsigned char *)malloc(byteWidth);
	memcpy(lineIn, sampledLine, byteWidth);
	lines.push_back(lineIn);
	return true;
}


MainWindow::~MainWindow()
{
	delete showUs;
	delete musicPic;
	delete ui;
}
