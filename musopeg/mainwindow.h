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
	void setJPEGName(char* arg);

private slots:
	void on_pushButton_clicked();

private:
	Ui::MainWindow *ui;
	std::string JPEGName;
};

#endif // MAINWINDOW_H
