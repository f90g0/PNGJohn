#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setAcceptDrops(true);
    setWindowTitle("PNGJohn v" + QString::number(APP_VERSION_MAJOR) + "." + QString::number(APP_VERSION_MINOR));
    ui->inputFilePath->setReadOnly(true);
    ui->outputFilePath->setReadOnly(true);

    _imgConverter = new ImgConverter();

    ui->statusLabel->setText("Choose Image File");

    QStandardItemModel * listItemModel = new QStandardItemModel();
    ui->tableView->setModel(listItemModel);

    connect(ui->imageBrowseButton,  SIGNAL(clicked(bool)), this, SLOT(BrowseFile()));
    connect(ui->convertButton,      SIGNAL(clicked(bool)), this, SLOT(OnConvertStart()));
    connect(_imgConverter,          SIGNAL(ConvertDone()), this, SLOT(OnConvertStart()));
    connect(ui->outputDirBrowseBtn, SIGNAL(clicked(bool)), this, SLOT(BrowseOutputDir()));
    connect(_imgConverter,          SIGNAL(ConvertDone()), this, SLOT(ProgressBar()));
    connect(this,         SIGNAL(OnDropFile(QStringList)), this, SLOT(DropFile(QStringList)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->hasFormat("text/uri-list")) {
        e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *e)
{
    QStringList dropFileList;
    foreach (const QUrl& url, e->mimeData()->urls()) {
        dropFileList.append( url.toLocalFile() );
    }

    emit OnDropFile(dropFileList);
}


void MainWindow::BrowseFile()
{
    QStringList files = QFileDialog::getOpenFileNames(this, "", QDir::homePath(), "Images (*.png *.jpg)");

    if (!files.isEmpty()) {
        JobListControll(files);
    }
}

void MainWindow::DropFile(QStringList dropFileList)
{
    QStringList files = dropFileList;

    if (!files.isEmpty()) {
        JobListControll(files);
    }
}

void MainWindow::JobListControll(QStringList fileList)
{
    QStringList files = fileList;
    for (int i = 0; i < files.size(); i++) {
        QString nativeSeparatorPath = QDir::toNativeSeparators(files.at(i));
        _nativeSeparatorPathList.append(nativeSeparatorPath);
    }

    ui->inputFilePath->clear();
    ui->inputFilePath->insert(_nativeSeparatorPathList.last());
    AddTableViewItem(files);

    QFile outputPath(ui->inputFilePath->text());
    QString absoluteDir = QFileInfo(outputPath).absolutePath();
    ui->outputFilePath->clear();
    ui->outputFilePath->insert(absoluteDir + "/");
    ui->statusLabel->setText("Ready to convert");
}

void MainWindow::AddTableViewItem(QStringList fileList)
{
    QStandardItemModel * model = qobject_cast<QStandardItemModel*>(ui->tableView->model());
    QStandardItem * item = NULL;
    foreach (QString text, fileList) {
        item = new QStandardItem();
        Q_CHECK_PTR(item);
        item->setText(text);
        item->setEditable(false);
        model->appendRow(item);
    }
}

void MainWindow::BrowseOutputDir()
{
    QString outputDir = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, "", ui->outputFilePath->text()));
    ui->outputFilePath->clear();
    ui->outputFilePath->insert(outputDir + "/");
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
            _imgConverter->StartConvert(_nativeSeparatorPathList.at(0), ui->outputFilePath->text()
                                        + outputBaseName + ui->suffixEdit->text() + ".png", ui->tolerance->value());

            ui->statusLabel->setText("Converting... " + QString::number(_convertCount + 1) + "/" + QString::number(_nativeSeparatorPathList.size()));
            _convertCount = 1;
        }
    } else if (_convertCount != 0 && _convertCount != _nativeSeparatorPathList.size()) {
        if(_nativeSeparatorPathList.size() != _convertCount) {
            QFile fileName(_nativeSeparatorPathList.at(_convertCount));
            QString outputBaseName = QFileInfo(fileName).baseName();
            _imgConverter->StartConvert(_nativeSeparatorPathList.at(_convertCount), ui->outputFilePath->text()
                                        + outputBaseName + ui->suffixEdit->text() + ".png", ui->tolerance->value());

            ui->statusLabel->setText("Converting... " + QString::number(_convertCount + 1) + "/" + QString::number(_nativeSeparatorPathList.size()));
            _convertCount += 1;
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

    // Determine the completion of processing by the value of the progress bar. (fuck)
    if (progressBarMaximum == _progressBarCount) {
        ui->tableView->model()->removeRows(0,ui->tableView->model()->rowCount());
        ui->statusLabel->setText("Convert done. Choose Image File");
        _progressBarCount = 0;
        _convertCount = 0;
        _nativeSeparatorPathList.clear();
    }
}

