#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGuiApplication>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QMessageBox>
#include <QDebug>
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

    double scaleRatio() const;
    void setScaleRatio(double scaleRatio);

private slots:
    void on_actionOpenImg_triggered();
    void on_actionExit_triggered();
    void on_actionZoomIn_triggered();
    void on_actionZoomOut_triggered();
    void on_zoom_sldr_sliderMoved(int position);
    void on_zoom_sb_valueChanged(double arg1);

    void on_calcSobel_btn_clicked();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *_scene;
    double _scaleRatio;
    QImage _baseImg;
    ImgService _imgService;
    //--func
    void connectAll();
    void setupWidgets();
    void showImg(QImage img);
};

#endif // MAINWINDOW_H
