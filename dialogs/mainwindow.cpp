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
    QImage imgWithBorder =_imgService.genImgWithblackBorder(baseImg());


    QImage horizontalSobel = _imgService.applySobelMask(imgWithBorder, Qt::Horizontal);
}

void MainWindow::on_manhattan_rb_clicked(bool checked)
{
    if(checked)
    {
        QImage verticalSobel = _imgService.applySobelMask(_imgService.genImgWithblackBorder(baseImg()), Qt::Vertical);
        showImg(verticalSobel);
    }
}

void MainWindow::on_evklid_rb_clicked(bool checked)
{
    if(checked)
    {
        QImage horizontalSobel = _imgService.applySobelMask(_imgService.genImgWithblackBorder(baseImg()), Qt::Horizontal);
        showImg(horizontalSobel);
    }
}
