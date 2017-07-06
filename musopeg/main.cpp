#include <QGraphicsScene>
#include <QApplication>
#include <iostream>
#include "bigjpeg.h"
#include "mainwindow.h"

using namespace std;

int main(int argc, char *argv[])
{

	string jpegName(argv[argc- 1]);

	QApplication a(argc, argv);
	MainWindow w;

	w.setJPEGName(jpegName);

	w.show();


	return a.exec();
}
