#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <string>
#include <cstring>
#include <vector>
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
}
