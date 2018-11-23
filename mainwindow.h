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



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void BrowseFile();
    void onConvertButton();

private:
    Ui::MainWindow *ui;
    QGraphicsScene _preview;
    void PreviewImage(QString filePath);
    QImage _previewImage;
};

#endif // MAINWINDOW_H
