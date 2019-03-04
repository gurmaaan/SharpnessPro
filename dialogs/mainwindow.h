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
#include <services/obj.h>

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

    QImage originalImg() const;
    void setOriginalImg(const QImage &originalImg);

    QImage sobelImg() const;
    void setSobelImg(const QImage &sobelImg);

    QImage threshImg() const;
    void setThreshImg(const QImage &threshImg);

    QVector<Obj> objVector() const;
    void setObjVector(const QVector<Obj> &objVector);

private slots:
    void on_actionOpenImg_triggered();
    void on_actionExit_triggered();
    void on_actionZoomIn_triggered();
    void on_actionZoomOut_triggered();
    void on_zoom_sldr_sliderMoved(int position);
    void on_zoom_sb_valueChanged(double arg1);
    void on_calcSobel_btn_clicked();
    void on_manhattan_rb_clicked(bool checked);
    void on_evklid_rb_clicked(bool checked);
    void on_threshold_sb_valueChanged(int arg1);
    void on_s_sldr_sliderMoved(int position);
    void on_s_gb_clicked(bool checked);

private:
    Ui::MainWindow *ui;
    QGraphicsScene *_scene;
    double _scaleRatio;
    ImgService _imgService;
    QImage _baseImg;
    QImage _originalImg;
    QImage _sobelImg;
    QImage _threshImg;
    QVector<Obj> _objVector;
    //--func
    void connectAll();
    void setupWidgets();
    void showImg(QImage img);
};

#endif // MAINWINDOW_H
