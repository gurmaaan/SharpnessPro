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
#include <services/imgprocessor.h>
#include <widgets/qcustomplot.h>

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

    QImage originalImg() const;
    void setOriginalImg(const QImage &originalImg);

    QImage sobelImg() const;
    void setSobelImg(const QImage &sobelImg);

    QImage threshImg() const;
    void setThreshImg(const QImage &threshImg);

    QVector<Obj> objVector() const;
    void setObjVector(const QVector<Obj> &objVector);

signals:
    void imgUpdated(QImage img);

public slots:
    void showImg(QImage img);

private slots:
    void on_actionOpenImg_triggered();
    void on_actionExit_triggered();
    void on_calcSobel_btn_clicked();
    void on_threshold_sb_valueChanged(int arg1);
    void on_s_sldr_sliderMoved(int position);
    void on_s_gb_clicked(bool checked);
    void on_actionSaveImg_triggered();
    void on_action_master_triggered();

    void on_sharpness_btn_clicked();

    void on_actionSavePlot_triggered();

    void on_plot_btn_clicked();

    void on_thr_min_H_sldr_sliderMoved(int position);

    void on_actionOpenFolder_triggered();

    void on_calc_series_btn_clicked();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *_scene;
    ImgService _imgService;
    ImgProcessor *_imgProcessor;
    QImage _baseImg;
    QImage _originalImg;
    QImage _sobelImg;
    QImage _threshImg;

    QCustomPlot *_plot;
    QVector<Obj> _objVector;
    //--func
    void connectAll();
    void setupWidgets();
};

#endif // MAINWINDOW_H
