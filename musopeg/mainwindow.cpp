#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <QPixmap>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QObject>
#include "jpeglib.h"
#include "bigjpeg.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::setJPEGName(string name)
{
	JPEGName = name;
}

void MainWindow::on_pushButton_clicked()
{
    //load the JPEG and decompress it
		BigJPEG *holder = new BigJPEG(JPEGName);
		QGraphicsScene* showUs = new QGraphicsScene(this);
		ui->graphicsView->setScene(showUs);
		//QPixmap strip = QPixmap::fromImage(*(holder->topImage));
		//QGraphicsPixmapItem* tI = showUs->addPixmap(strip);
		QObject::connect(this, SIGNAL(nextImage()), holder,
			SLOT(displayImages()));
}

void MainWindow::on_pushButton2_clicked()
{
	emit nextImage();
}
