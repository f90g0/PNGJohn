#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("PNGJohn v" + QString::number(APP_VERSION_MAJOR) + "." + QString::number(APP_VERSION_MINOR));
    ui->inputFilePath->setReadOnly(true);
    ui->outputFilePath->setReadOnly(true);

    _imgConverter = new ImgConverter();

    ui->statusLabel->setText("Choose Image File");

    connect(ui->imageBrowseButton,  SIGNAL(clicked(bool)), this, SLOT(BrowseFile()));
    connect(ui->convertButton,      SIGNAL(clicked(bool)), this, SLOT(OnConvertStart()));
    connect(_imgConverter,          SIGNAL(ConvertDone()), this, SLOT(OnConvertStart()));
    connect(ui->outputDirBrowseBtn, SIGNAL(clicked(bool)), this, SLOT(BrowseOutputDir()));
    connect(_imgConverter,          SIGNAL(ConvertDone()), this, SLOT(ProgressBar()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::BrowseFile()
{
    QStringList files = QFileDialog::getOpenFileNames(this, "", QDir::homePath(), "Images (*.png *.jpg)");

    if (!files.isEmpty()) {
        for (int i = 0; i < files.size(); i++) {
            QString nativeSeparatorPath = QDir::toNativeSeparators(files.at(i));
            _nativeSeparatorPathList.append(nativeSeparatorPath);
        }
        PreviewImage(_nativeSeparatorPathList.at(0));

        ui->inputFilePath->clear();
        ui->inputFilePath->insert(_nativeSeparatorPathList.at(0));

        QFile outputPath(ui->inputFilePath->text());
        QString absoluteDir = QFileInfo(outputPath).absolutePath();
        ui->outputFilePath->clear();
        ui->outputFilePath->insert(absoluteDir + "/");
        ui->statusLabel->setText("Ready to convert");
    }
}

void MainWindow::BrowseOutputDir()
{
    QString outputDir = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, "", ui->outputFilePath->text()));
    ui->outputFilePath->clear();
    ui->outputFilePath->insert(outputDir + "/");
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

void MainWindow::OnConvertStart()
{    
    ui->progressBar->setValue(0);
    if (_nativeSeparatorPathList.size() == 0) {
        return;
    }

    if (_nativeSeparatorPathList.size() == 1 || _convertCount == 0) {
        if (_convertCount != 1) {
            QFile fileName(_nativeSeparatorPathList.at(0));
            QString outputBaseName = QFileInfo(fileName).baseName();
            _imgConverter->StartConvert(_nativeSeparatorPathList.at(0), ui->outputFilePath->text() + outputBaseName + ui->suffixEdit->text() + ".png", ui->tolerance->value());
            ui->statusLabel->setText("Converting... " + QString::number(_convertCount + 1) + "/" + QString::number(_nativeSeparatorPathList.size()));
            _convertCount = 1;
        }
    } else if (_convertCount != 0 && _convertCount != _nativeSeparatorPathList.size()) {
        if(_nativeSeparatorPathList.size() != _convertCount) {
            QFile fileName(_nativeSeparatorPathList.at(_convertCount));
            QString outputBaseName = QFileInfo(fileName).baseName();
            _imgConverter->StartConvert(_nativeSeparatorPathList.at(_convertCount), ui->outputFilePath->text() + outputBaseName + ui->suffixEdit->text() + ".png", ui->tolerance->value());
            ui->statusLabel->setText("Converting... " + QString::number(_convertCount + 1) + "/" + QString::number(_nativeSeparatorPathList.size()));
            _convertCount+=1;
        }
    }
}

void MainWindow::ProgressBar()
{
    int progressBarMaximum = 100 - (100 % _nativeSeparatorPathList.size());
    ui->progressBar->setMaximum(progressBarMaximum);
    int parCount = 100/_nativeSeparatorPathList.size();
    _progressBarCount += parCount;
    ui->progressBar->setValue(_progressBarCount);

    if (progressBarMaximum == _progressBarCount) {
           ui->statusLabel->setText("Convert done. Choose Image File");
           _progressBarCount = 0;
           _convertCount = 0;
           _nativeSeparatorPathList.clear();
    }
}

