#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "jpeglib.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
signals:
	void timeToUpdate();

private:
	std::vector<unsigned char *> lines;
	int byteWidth;
	QString jpegFile;
	FILE *inFile;
	Ui::MainWindow *ui;
	void setByteWidth(const int&);
	bool storeScannedLine(JSAMPROW);
	QGraphicsScene *showUs;
	QImage *musicPic;
private slots:
	void on_pushButton_clicked();
	void updateGraphics();
};

#endif // MAINWINDOW_H
