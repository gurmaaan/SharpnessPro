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
    _imgService.sharpnessK(_sobelImg, _threshImg);
}
