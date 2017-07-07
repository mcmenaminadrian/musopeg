#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "bigjpeg.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	void setJPEGName(std::string arg);
signals:
	void nextImage();
	void goodImage();
	void badImage();


private slots:
	void on_pushButton_clicked();
	void on_goodButton_clicked();
	void on_badButton_clicked();
	void updateGraphics();

private:
	Ui::MainWindow *ui;
	BigJPEG *holder;
	QGraphicsScene *showUs;
};

#endif // MAINWINDOW_H
