#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <cstddef>
#include <QPixmap>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QFileDialog>
#include <QString>
#include <QObject>
#include <QMessageBox>
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
}

void MainWindow::on_pushButton_clicked()
{
    //load the JPEG and decompress it
	QString JPEGName = QFileDialog::getOpenFileName(this,
		tr("Select JPEG"), "", tr("JPEGs (*.jpeg *.jpg *.jfif)"));

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
	QObject::connect(holder, SIGNAL(completedRun()), this,
		SLOT(finishProcess()));
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
	showUs->addPixmap(strip);
}

void MainWindow::finishProcess()
{
	QMessageBox messageBox;
	messageBox.setText("Processing now complete.");
	messageBox.exec();
	delete holder;
	QApplication::quit();
}

