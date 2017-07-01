#include "mainwindow.h"
#include <QApplication>

using namespace std;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;

	w.setJPEGName(argv[1]);
	w.show();

	return a.exec();
}
