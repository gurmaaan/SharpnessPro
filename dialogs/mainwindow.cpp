 #include "dialogs/mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupWidgets();
    connectAll();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectAll()
{
    connect(_imgProcessor, &ImgProcessor::imgUpdated,
            this, &MainWindow::showImg);
    connect(this, &MainWindow::imgUpdated,
            this, &MainWindow::showImg);
}

void MainWindow::setupWidgets()
{
    _scene = new QGraphicsScene;
    ui->graphicsView->setScene(_scene);
    _imgProcessor = new ImgProcessor();

    _plot = ui->plot_view;
}

void MainWindow::showImg(QImage img)
{
    //setBaseImg(img);
    _scene->clear();
    QGraphicsPixmapItem *pmItem = new QGraphicsPixmapItem(QPixmap::fromImage(img));
    _scene->addItem(pmItem);
    ui->graphicsView->fitInView(_scene->sceneRect(),Qt::KeepAspectRatio);
}

void MainWindow::on_actionOpenImg_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr, DIR_SINGLE_CAPTION, DIR_PATH);
    ui->path_le->setText(fileName);

    QImage baseImg(fileName);
    if(!baseImg.isNull())
    {
        setBaseImg(baseImg);
        setOriginalImg(baseImg);
        showImg(baseImg);
    }
}

void MainWindow::on_actionExit_triggered()
{
    QGuiApplication::exit();
}


QImage MainWindow::baseImg() const
{
    return _baseImg;
}

void MainWindow::setBaseImg(const QImage &baseImg)
{
    _baseImg = baseImg;
}

void MainWindow::on_calcSobel_btn_clicked()
{
    QImage horizontalSobel = _imgService.applySobelMask(originalImg(), Qt::Horizontal);
    QImage verticalSobel = _imgService.applySobelMask(originalImg(), Qt::Vertical);
    QImage res;
    if(ui->manhattan_rb->isChecked())
        res = _imgService.manhattan(verticalSobel, horizontalSobel);
    else if (ui->evklid_rb->isChecked())

        res = _imgService.evklid(verticalSobel, horizontalSobel);
    setBaseImg(res);
    setSobelImg(res);
    showImg(res);
}

QImage MainWindow::originalImg() const
{
    return _originalImg;
}

void MainWindow::setOriginalImg(const QImage &originalImg)
{
    _originalImg = originalImg;
}

void MainWindow::on_threshold_sb_valueChanged(int arg1)
{
    QImage thresh = _imgService.threshold(baseImg(), arg1);
    //BUG::почему-то перестает онлайн обновляться трэшолд если устанавливать базовым изображением трешолдовое
    showImg(thresh);
    setThreshImg(thresh);
}

QImage MainWindow::sobelImg() const
{
    return _sobelImg;
}

void MainWindow::setSobelImg(const QImage &sobelImg)
{
    _sobelImg = sobelImg;
}

QImage MainWindow::threshImg() const
{
    return _threshImg;
}

void MainWindow::setThreshImg(const QImage &threshImg)
{
    _threshImg = threshImg;
}

QVector<Obj> MainWindow::objVector() const
{
    return _objVector;
}

void MainWindow::setObjVector(const QVector<Obj> &objVector)
{
    _objVector = objVector;
}

void MainWindow::on_s_sldr_sliderMoved(int position)
{
    for(auto obj : _objVector)
    {
        QColor clr = (obj.s() > position) ? QColor(Qt::white) : QColor(Qt::black);
        _imgService.fillPixel(&_threshImg, obj, clr);
    }
    showImg(_threshImg);
}

void MainWindow::on_s_gb_clicked(bool checked)
{
    if(checked)
    {
        QVector<Obj> ov = _imgService.labeling(threshImg());
        setObjVector(ov);
        QList<int> sList;
        for(auto o : ov)
        {
            o.calcS();
            sList << o.s();
        }
        int sMin = *std::min_element(sList.begin(), sList.end());
        int sMax = *std::max_element(sList.begin(), sList.end());
        ui->sMax_sb->setMaximum(sMax);
        ui->sMax_sb->setValue(sMax);
        ui->sMin_sb->setValue(sMin);
        ui->s_sldr->setRange(sMin, sMax);
    }
}

void MainWindow::on_actionSaveImg_triggered()
{
    QPixmap sceneViewPm = ui->graphicsView->grab();
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Сохранить картинку", DIR_PATH);
    sceneViewPm.save(fileName);
}

void MainWindow::on_action_master_triggered()
{
    QString filePath = "C:/Users/Dima/YandexDisk/EDUCATION/__UIR4/TestImages/F0000001.bmp";
    int thrVal = 48;
    int sThresh = 3000;
    //-------------------------------------------------------------------------------------
    QImage img(filePath);
    setOriginalImg(img);
    ui->path_le->setText(filePath);

    QImage sobel = _imgService.evklid(_imgService.applySobelMask(img, Qt::Vertical),
                                      _imgService.applySobelMask(img, Qt::Horizontal));

    QImage thresh = _imgService.threshold(sobel, thrVal);
    ui->threshold_gb->setEnabled(true); ui->threshold_gb->setChecked(true);
    ui->threshold_sldr->setValue(thrVal); ui->threshold_sb->setValue(thrVal);

    QVector<Obj> ovFirst = _imgService.labeling(thresh);
    QVector<int> objSVector;
    for (auto obj : ovFirst)
    {
        objSVector << obj.s();
        if(obj.s() <= sThresh)
            _imgService.fillPixel(&thresh, obj, QColor(Qt::black));
    }
    int max_s = *std::max_element(objSVector.begin(), objSVector.end());
    ui->sMax_sb->setMaximum(max_s); ui->sMax_sb->setValue(max_s);
    ui->s_sldr->setMaximum(max_s); ui->s_sldr->setValue(sThresh);
    ui->s_gb->setEnabled(true); ui->s_gb->setChecked(true);
}

void MainWindow::on_sharpness_btn_clicked()
{
    ui->sharpness_sb->setValue(_imgService.sharpnessK(_sobelImg, _threshImg));
}

void MainWindow::on_actionSavePlot_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Сохранить график", DIR_PATH);
    _plot->savePng(fileName);
}

void MainWindow::on_plot_btn_clicked()
{
    QImage sobel = _imgService.evklid(_imgService.applySobelMask(_originalImg, Qt::Vertical),
                                      _imgService.applySobelMask(_originalImg, Qt::Horizontal));

    QVector<double> thrValVector, sharpKVector_noFilter, sharpKVector_Filter;
    for (int thr = ui->thr_min_sb->value(); thr <= ui->thr_max_sb->value(); ++thr)
    {
        thrValVector << static_cast<double>(thr);
        QImage thrImg = _imgService.threshold(sobel, thr);
        sharpKVector_noFilter << _imgService.sharpnessK(sobel, thrImg);

        QVector<Obj> objVector = _imgService.labeling(thrImg);
        QVector<int> objSVector;
        for (auto obj : objVector)
            objSVector << obj.s();
        int max_s = *std::max_element(objSVector.begin(), objSVector.end());
        for(auto obj : objVector)
        {
            if(obj.s() < max_s)
                _imgService.fillPixel(&thrImg, obj, QColor(Qt::black));
        }

        sharpKVector_Filter << _imgService.sharpnessK(sobel, thrImg);
    }

    _plot->clearGraphs();
    _plot->xAxis->setRange(thrValVector.first(), thrValVector.last());
    _plot->yAxis->setRange(*std::min_element(sharpKVector_noFilter.begin(), sharpKVector_noFilter.end()),
                           *std::max_element(sharpKVector_Filter.begin(), sharpKVector_Filter.end()));
    _plot->xAxis->setLabel("Порог бинаризации");
    _plot->yAxis->setLabel("Коэффициент резкости");

    _plot->addGraph();
    int gn = _plot->graphCount() - 1;
    _plot->graph(gn)->setPen(QPen(Qt::red));
    _plot->graph(gn)->setLineStyle(QCPGraph::lsLine);
    _plot->graph(gn)->setScatterStyle( QCPScatterStyle(QCPScatterStyle::ssDisc, 4) );
    _plot->graph(gn)->setData(thrValVector, sharpKVector_noFilter);
    _plot->graph(gn)->setName("Фильтрации по площади не производилось.");

    _plot->addGraph();
    gn++;
    _plot->graph(gn)->setPen(QPen(Qt::blue));
    _plot->graph(gn)->setLineStyle(QCPGraph::lsLine);
    _plot->graph(gn)->setScatterStyle( QCPScatterStyle(QCPScatterStyle::ssDisc, 4) );
    _plot->graph(gn)->setData(thrValVector, sharpKVector_Filter);
    _plot->graph(gn)->setName("Фильтрация по площади.");

    _plot->legend->setVisible(true);
    _plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);
    _plot->replot();

    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::on_thr_min_H_sldr_sliderMoved(int position)
{
    ui->thr_max_H_sldr->setMinimum(position+1);
    if(ui->thr_max_sb->value() <= position)
        ui->thr_max_sb->setValue(position + 1);
}

void MainWindow::on_actionOpenFolder_triggered()
{
    QString folderName = QFileDialog::getExistingDirectory(this,
                                                           DIR_MULTIPLE_CAPTURE,
                                                           DIR_PATH);
    ui->folder_path_le->setText(folderName);
}

void MainWindow::on_calc_series_btn_clicked()
{
    QString folderPath = ui->folder_path_le->text();
    int thr_lim = ui->threshold_sb->value();
    int s_lim = ui->s_sldr->value();
    QVector<double> imgIndexVector, sharpVector;

    if(!folderPath.isEmpty())
    {
       QDir dirImg(folderPath);
       QStringList imgNamesList = dirImg.entryList(QStringList() << "*.tif" << "*.bmp", QDir::Files);
       ui->series_pb->setMaximum(imgNamesList.length());
       for(QString name : imgNamesList)
       {
           QImage img(folderPath + QDir::separator() + name);
           QImage grad = _imgService.evklid(_imgService.applySobelMask(img, Qt::Vertical),
                                            _imgService.applySobelMask(img, Qt::Horizontal));
           QImage thr = _imgService.threshold(grad, thr_lim);
//           QVector<Obj> objVector = _imgService.labeling(thr);
//           for (auto obj : objVector)
//           {
//               if(obj.s() <= s_lim)
//                   _imgService.fillPixel(&thr, obj, QColor(Qt::black));
//           }
           sharpVector << _imgService.sharpnessK(grad, thr);
           qDebug() << name << sharpVector.last();
           imgIndexVector << static_cast<double>(imgNamesList.indexOf(name));
           ui->series_pb->setValue(imgNamesList.indexOf(name) + 1);
       }

       _sharpKVector = sharpVector;
       _plot->clearGraphs();
       _plot->xAxis->setRange(imgIndexVector.first(), imgIndexVector.last());
       _plot->yAxis->setRange(*std::min_element(sharpVector.begin(), sharpVector.end()),
                              *std::max_element(sharpVector.begin(), sharpVector.end()));
       _plot->xAxis->setLabel("Номер изображения в серии");
       _plot->yAxis->setLabel("Коэффициент резкости");
       _plot->addGraph();
       int gn = _plot->graphCount() - 1;
       _plot->graph(gn)->setPen(QPen(Qt::blue));
       _plot->graph(gn)->setLineStyle(QCPGraph::lsLine);
       _plot->graph(gn)->setScatterStyle( QCPScatterStyle(QCPScatterStyle::ssDisc, 4) );
       _plot->graph(gn)->setData(imgIndexVector, sharpVector);
       _plot->replot();
       ui->tabWidget->setCurrentIndex(1);

    }
}


void MainWindow::on_series_pb_valueChanged(int value)
{
    if(value == ui->series_pb->maximum())
        ui->group_series_btn->setEnabled(true);
}

void MainWindow::on_group_series_btn_clicked()
{
   _sharpKVector << 36.0256 << 36.5831 << 36.1080 << 36.0867 << 36.5281 << 36.5228 << 36.9237 << 36.8791 << 37.1100 << 37.5346 << 37.8160 << 37.9415 << 38.1889 << 38.2068 << 38.3668 << 38.5162 << 38.6910 << 39.0216 << 39.1726
                 << 39.1826 << 39.4631 << 39.6099 << 39.8703 << 39.8780 << 40.2311 << 40.4282 << 40.7330 << 40.7891 << 41.1181 << 41.3334 << 41.3741 << 41.4837 << 41.7101 << 41.9523 << 42.1695 << 42.2059 << 42.3043 << 42.5472
                 << 42.7635 << 42.8130 << 42.9223 << 43.1930 << 43.3102 << 43.3796 << 43.6074 << 43.7532 << 43.9444 << 43.9303 << 44.0836 << 44.2297 << 44.1777 << 44.2521 << 44.3068 << 44.3568 << 44.3213 << 44.4077 << 44.3633
                 << 44.3365 << 44.3267 << 44.2567 << 44.1075 << 43.9596 << 43.9949 << 43.9130 << 43.7793 << 43.6666 << 43.6166 << 43.5835 << 43.4280 << 43.4418 << 43.3680 << 43.2429 << 43.1659 << 42.9779 << 42.9156 << 42.8789
                 << 42.7194 << 42.5958 << 42.5326 << 42.4961 << 42.4646 << 42.4552 << 42.4430 << 42.4039 << 42.4038 << 42.5154 << 42.5457 << 42.5852 << 42.7275 << 42.8049 << 42.8185 << 42.9129 << 42.9917 << 43.1701 << 43.2149
                 << 43.2604 << 43.4158 << 43.6109 << 43.5937 << 43.5576 << 43.6856 << 43.7852 << 43.7100 << 43.7898 << 43.8809 << 43.8227 << 43.8496 << 43.9328 << 44.0790 << 44.3502 << 44.4213 << 44.6233 << 45.1227 << 45.1190
                 << 45.3191 << 45.3354 << 45.7221 << 46.4568 << 46.5395 << 46.4458 << 45.7406 << 47.7535 << 47.8077 << 48.2509 << 48.5501 << 48.5794 << 48.4662 << 48.6755 << 48.5839 << 48.4934 << 48.5063 << 48.0262 << 46.8833
                 << 46.8803 << 46.9728 << 46.2995 << 45.2761 << 45.2373 << 45.2792 << 44.1882 << 42.9913 << 42.9806 << 42.9593 << 42.1634 << 42.1228 << 41.6007 << 41.0824 << 39.9332 << 38.9103 << 39.1534 << 39.0171 << 38.8101
                 << 37.3540 << 37.3903 << 36.8712 << 36.1548 << 35.6788 << 35.7855 << 34.7668 << 34.6663 << 34.5114 << 33.6881 << 33.6824 << 33.2342 << 32.7484 << 32.2767 << 32.2834 << 32.1096 << 31.8906 << 32.2227 << 32.1248 << 32.0652 << 31.8938 << 31.7863 << 31.4474;
   double min = *std::min_element(_sharpKVector.begin(), _sharpKVector.end());
   double max = *std::max_element(_sharpKVector.begin(), _sharpKVector.end());
   int n = ui->group_n_series_sb->value();
   double delta = (max-min) / static_cast<double>(n);
   QVector<double> splitIntervals;
   splitIntervals << min;
   for (int i = 0; i < n; ++i)
       splitIntervals << min + delta * (i+1);

   for(int i = 1; i < splitIntervals.length(); i++)
       qDebug() << i << " : " << splitIntervals.at(i-1) <<"__" << splitIntervals.at(i);
}
