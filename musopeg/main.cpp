#include <QGraphicsScene>
#include <QApplication>
#include <iostream>
#include <fstream>
#include "bigjpeg.h"
#include "mainwindow.h"

using namespace std;

int main(int argc, char *argv[])
{

	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
