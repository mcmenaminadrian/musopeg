#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <cstddef>
#include <QPixmap>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QObject>
#include "jpeglib.h"
#include "bigjpeg.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	holder = NULL;
}

MainWindow::~MainWindow()
{
	delete ui;
	delete holder;
}

void MainWindow::setJPEGName(string name)
{
	JPEGName = name;
}

void MainWindow::on_pushButton_clicked()
{
    //load the JPEG and decompress it
	holder = new BigJPEG(JPEGName);
	showUs = new QGraphicsScene(this);
	ui->graphicsView->setScene(showUs);
	QObject::connect(this, SIGNAL(nextImage()), holder,
		SLOT(displayImages()));
	QObject::connect(holder, SIGNAL(updateImages()), this,
		SLOT(updateGraphics()));
	emit nextImage();
}

void MainWindow::on_nextButton_clicked()
{		
	emit nextImage();
}

void MainWindow::updateGraphics()
{
	qDeleteAll(showUs->items());
	QPixmap strip = QPixmap::fromImage(*(holder->topImage));
	QGraphicsPixmapItem* tI = showUs->addPixmap(strip);
}

