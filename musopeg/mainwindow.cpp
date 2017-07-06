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
	ui->goodButton->setEnabled(true);
	ui->badButton->setEnabled(true);
	holder = new BigJPEG(JPEGName);
	showUs = new QGraphicsScene(this);
	ui->graphicsView->setScene(showUs);
	QObject::connect(this, SIGNAL(nextImage()), holder,
		SLOT(displayImages()));
	QObject::connect(this, SIGNAL(badImage()), holder,
		SLOT(badImage()));
	QObject::connect(this, SIGNAL(goodImage()), holder,
		SLOT(goodImage()));
	QObject::connect(holder, SIGNAL(updateImages()), this,
		SLOT(updateGraphics()));
	emit nextImage();
	ui->pushButton->setEnabled(false);
}

void MainWindow::on_badButton_clicked()
{
	emit badImage();
}

void MainWindow::on_goodButton_clicked()
{
	emit goodImage();
}

void MainWindow::updateGraphics()
{
	qDeleteAll(showUs->items());
	QPixmap strip = QPixmap::fromImage(*(holder->topImage));
	QGraphicsPixmapItem* tI = showUs->addPixmap(strip);
}

