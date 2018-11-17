#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imgConverter.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ImgConverter imgConverter = ImgConverter();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//int MainWindow::calcProcessTime()
//{
//    QString gcode = ui->gcodeTextBox->toPlainText();
//    ProcessTime processTime = ProcessTime(gcode);
//    processTime.setSeekSpeed(5500);
//    return processTime.calc();
//    //return 1;
//}

//void MainWindow::loadImage(QString filePath)
//{

//}

/*
void MainWindow::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls() == false) {
        return;
    }

    QUrl    fileUrl  = event->mimeData()->urls()[0];
    QString filePath = QDir::toNativeSeparators(fileUrl.toLocalFile());

    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly)) {
        //Debug::failed("@drop event: file open error.");
        return;
    }

    QFileInfo fileInfo(file);
    if (fileInfo.completeSuffix() != "gcode") {
        //Debug::failed("@drop event: file is not 'gcode' suffix.");
        return;
    }

    QTextStream textData(&file);
    QString text = textData.readAll();
    file.close();

    loadImage(text);
}
*/

