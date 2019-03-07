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
    ui->zoom_V_lyt->setAlignment(Qt::AlignHCenter);
    setScaleRatio(1.0);
    _imgProcessor = new ImgProcessor();
}

void MainWindow::showImg(QImage img)
{
    //setBaseImg(img);
    _scene->clear();
    QGraphicsPixmapItem *pmItem = new QGraphicsPixmapItem(QPixmap::fromImage(img));
    _scene->addItem(pmItem);
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
        ui->graphicsView->fitInView(_scene->sceneRect(),Qt::KeepAspectRatio);
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

void MainWindow::on_actionZoomIn_triggered()
{
    double newScaleRatio = scaleRatio() + 0.15;
    setScaleRatio(newScaleRatio);
    int newSliderValue = static_cast<int>(newScaleRatio) * 100;
    ui->zoom_sldr->setValue(newSliderValue);
}

void MainWindow::on_actionZoomOut_triggered()
{
    double newScaleRatio = scaleRatio() - 0.15;
    setScaleRatio(newScaleRatio);
    int newSliderValue = static_cast<int>(newScaleRatio) * 100;
    ui->zoom_sldr->setValue(newSliderValue);
}

void MainWindow::on_zoom_sldr_sliderMoved(int position)
{
    double dPosition = static_cast<double>(position) / 100.0;
    setScaleRatio(dPosition);
}

double MainWindow::scaleRatio() const
{
    return _scaleRatio;
}

void MainWindow::setScaleRatio(double scaleRatio)
{
    _scaleRatio = scaleRatio;
    ui->zoom_sb->setValue(scaleRatio);
}

void MainWindow::on_zoom_sb_valueChanged(double arg1)
{
    if(_scene->items().length() > 0)
    {
        QImage bi = baseImg();
        double newWd = static_cast<double>(bi.width()) * arg1;
        double newHd = static_cast<double>(bi.height()) * arg1;
        int newW = static_cast<int>(newWd);
        int newH = static_cast<int>(newHd);
        bi = bi.scaled(newW, newH);
        showImg(bi);
    }
    else
    {
        QMessageBox::critical(nullptr, "Ошибка", "Ни одного изображения не выбрано, зумирование невозможно. Выберите изображение и повторите попытку.");
    }
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
    QImage timg = threshImg();
    for(auto obj : _objVector)
    {
        obj.calcS();
        int s = obj.s();
        QColor clr = (s > position) ? QColor(Qt::white) : QColor(Qt::black);
        timg = _imgService.fillPixel(timg, obj, clr);
        setThreshImg(timg);
    }
    showImg(timg);
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

void MainWindow::on_pushButton_clicked()
{
    QVector<QPoint> cnt = _imgService.findContour(_threshImg);
    showImg(_threshImg);
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
    int type = 0;
    if(ui->morphKernel_rect->isChecked())
        type = 0;
    else if(ui->morphKernel_cross->isChecked())
        type = 1;
    else if(ui->morphKernel_ellipse->isChecked())
        type = 2;

//    QImage morph;
//    if(ui->dilat_cb->isChecked())
//    {
//        morph = _imgProcessor->dilation(_threshImg, ui->dilat_sb->value(), type);
//    }

//    if(ui->ero_cb->isChecked())
//    {
        //setThreshImg(_imgProcessor->erosion( _imgProcessor->dilation(_threshImg, ui->dilat_sb->value(), type), ui->dilat_sb->value(), type));
    //}

    QImage morph = _imgProcessor->dilation(_threshImg, ui->dilat_sb->value(), type);
    _imgProcessor->erosion(morph, ui->ero_sb->value(), type);

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
