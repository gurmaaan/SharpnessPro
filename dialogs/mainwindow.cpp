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
    //_plot->legend->setVisible(false);
    _plot->xAxis->setLabel("Расстояние от границы");
    _plot->yAxis->setLabel("Коэффициент резкости");
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
    QString fileName = QFileDialog::getOpenFileName(nullptr, DIR_CAPTION, DIR_PATH);
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

void MainWindow::on_ero_sldr_sliderMoved(int position)
{
    if(position % 2 ==1)
    {
        ui->ero_sb->setValue(position);
    }
}

void MainWindow::on_dilat_sldr_sliderMoved(int position)
{
    if(position % 2 ==1)
    {
        ui->dilat_sb->setValue(position);
    }
}

void MainWindow::on_applyMorph_btn_clicked()
{
//    int type = 0;
//    if(ui->morphKernel_rect->isChecked())
//        type = 0;
//    else if(ui->morphKernel_cross->isChecked())
//        type = 1;
//    else if(ui->morphKernel_ellipse->isChecked())
//        type = 2;

    QImage morph = _imgProcessor->dilation(_threshImg, ui->dilat_sb->value(), 2);
    //showImg(morph);
    _mainRect = _imgService.findSkeletRect(morph);
    int sharpMax = _mainRect.width() / 2;
    ui->sharp_sb->setMaximum(sharpMax); ui->sharp_sldr->setMaximum(sharpMax);
    _scene->addEllipse(QRectF(_mainRect), QPen(QColor(Qt::red)));

}

void MainWindow::on_ero_sb_valueChanged(int arg1)
{
    int type = 0;
    if(ui->morphKernel_rect->isChecked())
        type = 0;
    else if(ui->morphKernel_cross->isChecked())
        type = 1;
    else if(ui->morphKernel_ellipse->isChecked())
        type = 2;
    _imgProcessor->erosion(_threshImg, arg1, type);
}

void MainWindow::on_actionSaveImg_triggered()
{
    QPixmap sceneViewPm = ui->graphicsView->grab();
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Сохранить картинку", DIR_PATH);
    sceneViewPm.save(fileName);
}

void MainWindow::on_dilat_sb_valueChanged(int arg1)
{
    int type = 0;
    if(ui->morphKernel_rect->isChecked())
        type = 0;
    else if(ui->morphKernel_cross->isChecked())
        type = 1;
    else if(ui->morphKernel_ellipse->isChecked())
        type = 2;
    _imgProcessor->dilation(_threshImg, arg1, type);
}

void MainWindow::on_sharp_sldr_sliderMoved(int position)
{
    showImg(originalImg());
    _scene->addEllipse(QRectF(_mainRect), QPen(QColor(Qt::red)));

    int tlx, tly, brx, bry;
    tlx = _mainRect.topLeft().x();
    tly = _mainRect.topLeft().y();
    brx = _mainRect.bottomRight().x();
    bry = _mainRect.bottomRight().y();

    _outerRect = QRect(QPoint(tlx - position, tly - position), QPoint(brx + position, bry + position));
    _innerRect = QRect(QPoint(tlx + position, tly + position), QPoint(brx - position, bry - position));
    QPen addPen = QPen(QColor(Qt::green));

    _scene->addEllipse(QRectF(_outerRect), addPen);
    _scene->addEllipse(QRectF(_innerRect), addPen);
}

void MainWindow::on_sharpCalc_btn_clicked()
{
    showImg(_originalImg);
    qDebug() << _imgService.sharpnessK(_originalImg, _mainRect, ui->sharp_sb->value());
}

void MainWindow::on_action_master_triggered()
{
    QString filePath = "C:/Users/Dima/YandexDisk/EDUCATION/__UIR4/TestImages/F0000001.bmp";
    int thrVal = 48;
    int sThresh = 3000;
    int dilatSize = 4;
    int dilatType = 2;
    int ringSize = 8;
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

    QImage morph = _imgProcessor->dilation(thresh, dilatSize, dilatType);
    ui->dilat_cb->setChecked(true);
    ui->dilat_sldr->setValue(dilatSize); ui->dilat_sb->setValue(dilatSize);
    ui->morphKernel_ellipse->setChecked(true);
    _mainRect = _imgService.findSkeletRect(morph);
    int sharpMax = _mainRect.width() / 2;
    ui->sharp_sb->setMaximum(sharpMax); ui->sharp_sldr->setMaximum(sharpMax);
    showImg(img);
    _scene->addEllipse(QRectF(_mainRect), QPen(QColor(Qt::red)));
    on_sharp_sldr_sliderMoved(ringSize);
    ui->sharp_sb->setValue(ringSize); ui->sharp_sldr->setValue(ringSize);
}

void MainWindow::on_plot_btn_clicked()
{
    QVector<double> areaWVector;
    QVector<double> koeffVector;

    for(int i = 1; i < ui->sharp_sb->maximum(); i++)
    {
        areaWVector << static_cast<double>(i);
        koeffVector << _imgService.sharpnessK(_originalImg, _mainRect, i);
    }

    double maxW = static_cast<double>(ui->sharp_sb->maximum());
    double maxk = *std::max_element(koeffVector.begin(), koeffVector.end());

    _plot->xAxis->setRange(0, maxW);
    _plot->yAxis->setRange(0, maxk);
    _plot->addGraph();
    int gn = _plot->graphCount() - 1;
    _plot->graph(gn)->setPen(QColor(Qt::red));
    _plot->graph(gn)->setLineStyle(QCPGraph::lsLine);
    _plot->graph(gn)->setScatterStyle( QCPScatterStyle(QCPScatterStyle::ssDisc, 4) );
    _plot->graph(gn)->setName("График зависимости коэффициента резкости от расстояния от границы объекта");
    _plot->graph(gn)->setData(areaWVector, koeffVector);
    _plot->axisRect()->setupFullAxesBox();
    _plot->replot();

    ui->tabWidget->setCurrentIndex(1);
}
