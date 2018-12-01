#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QTextEdit>
#include <QScrollBar>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include "imgConverter.h"



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void ConvertOrder();

public slots:
    void BrowseFile();
    void BrowseOutputDir();
    void OnConvertStart();

private:
    Ui::MainWindow *ui;
    QGraphicsScene _preview;
    void PreviewImage(QString filePath);
    QImage _previewImage;
    QStringList _nativeSeparatorPathList;
    ImgConverter* _imgConverter;
    int _convertCount = 0;
};

#endif // MAINWINDOW_H
