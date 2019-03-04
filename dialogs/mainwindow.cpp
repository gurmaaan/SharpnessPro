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

}

void MainWindow::setupWidgets()
{
    _scene = new QGraphicsScene;
    ui->graphicsView->setScene(_scene);
    ui->zoom_V_lyt->setAlignment(Qt::AlignHCenter);
    setScaleRatio(1.0);
}

void MainWindow::showImg(QImage img)
{
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
    QImage imgWithBorder =_imgService.genImgWithblackBorder(originalImg());
    QImage horizontalSobel = _imgService.applySobelMask(imgWithBorder, Qt::Horizontal);
    QImage verticalSobel = _imgService.applySobelMask(imgWithBorder, Qt::Vertical);
    QImage res;
    if(ui->manhattan_rb->isChecked())
        res = _imgService.manhattan(verticalSobel, horizontalSobel);
    else if (ui->evklid_rb->isChecked())
        res = _imgService.evklid(verticalSobel, horizontalSobel);
    setBaseImg(res);
    setSobelImg(res);
    showImg(res);
}

void MainWindow::on_manhattan_rb_clicked(bool checked)
{
    if(checked && (!baseImg().isNull()))
    {
        QImage verticalSobel = _imgService.applySobelMask(_imgService.genImgWithblackBorder(originalImg()), Qt::Vertical);
        QImage horizontalSobel = _imgService.applySobelMask(_imgService.genImgWithblackBorder(originalImg()), Qt::Horizontal);
        QImage manh = _imgService.manhattan(verticalSobel, horizontalSobel);
        setBaseImg(manh);
        showImg(manh);
        setSobelImg(manh);
    }
}

void MainWindow::on_evklid_rb_clicked(bool checked)
{
    if(checked && (!baseImg().isNull()))
    {
        QImage verticalSobel = _imgService.applySobelMask(_imgService.genImgWithblackBorder(originalImg()), Qt::Vertical);
        QImage horizontalSobel = _imgService.applySobelMask(_imgService.genImgWithblackBorder(originalImg()), Qt::Horizontal);
        QImage evkLid = _imgService.evklid(verticalSobel, horizontalSobel);
        setBaseImg(evkLid);
        showImg(evkLid);
        setSobelImg(evkLid);
    }
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
    //setBaseImg(thresh);
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

void MainWindow::on_test_btn_clicked()
{
    _imgService.labeling(threshImg());
}

QImage MainWindow::threshImg() const
{
    return _threshImg;
}

void MainWindow::setThreshImg(const QImage &threshImg)
{
    _threshImg = threshImg;
}
