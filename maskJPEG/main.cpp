#include <iostream>
#include <fstream>
#include <QGraphicsScene>
#include "jpeglib.h"
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
