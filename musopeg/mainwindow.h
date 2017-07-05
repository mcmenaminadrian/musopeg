#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

private slots:
	void on_pushButton_clicked();
	void on_pushButton2_clicked();
	void updateGraphics();

private:
	Ui::MainWindow *ui;
	std::string JPEGName;
	QGraphicsScene *showUs;
};

#endif // MAINWINDOW_H
