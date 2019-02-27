#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGuiApplication>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsItem>
//
#include <static.h>
#include <services/imgservice.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QImage baseImg() const;
    void setBaseImg(const QImage &baseImg);

private slots:
    void on_actionOpenImg_triggered();
    void on_actionExit_triggered();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *_scene;
    //
    QImage _baseImg;

    //--func
    void connectAll();
    void setupWidgets();
};

#endif // MAINWINDOW_H
