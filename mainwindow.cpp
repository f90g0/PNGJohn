#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imgConverter.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->imageBrowseButton, SIGNAL(clicked(bool)), this, SLOT(BrowseFile()));
    connect(ui->convertButton,     SIGNAL(clicked(bool)), this, SLOT(onConvertButton()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::BrowseFile()
{
    QString filePath = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, "", QDir::currentPath(), "Images (*.png *.jpg)"));

    QFile filePath_QFile(filePath);
    QString absolutePath = QFileInfo(filePath_QFile).absolutePath(),
            baseName     = QFileInfo(filePath_QFile).baseName();

    ui->inputFilePath->insert(filePath);
    ui->outputFilePath->clear();
    ui->outputFilePath->insert(absolutePath + "/" + baseName + "_twi.png");

    PreviewImage(filePath);
}

void MainWindow::PreviewImage(QString filePath)
{
    QFile previewImageData(filePath);

    if (!previewImageData.open(QIODevice::ReadOnly)) {
        return;
    }

    QByteArray previewImageArray = previewImageData.readAll();
    _previewImage = QImage::fromData(previewImageArray);
    QGraphicsPixmapItem *preview_image_item = new QGraphicsPixmapItem(QPixmap::fromImage(_previewImage));
    _preview.addItem(preview_image_item);
    ui->previewImage->scale(ui->previewImage->width() / _preview.width(), ui->previewImage->height() / _preview.height());
    ui->previewImage->setScene(&_preview);
}

void MainWindow::onConvertButton()
{
    ImgConverter(_previewImage, ui->outputFilePath->text(), ui->reductionAmount->value());

    qDebug() << ui->outputFilePath->text();
}
