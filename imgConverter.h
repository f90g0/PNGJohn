#ifndef IMGCONVERTER_H
#define IMGCONVERTER_H

#include <QtCore>
#include <QImage>
#include <QtConcurrentRun>
#include <QtConcurrent>


class ImgConverter : public QObject
{
    Q_OBJECT

public:
    ImgConverter();
    void StartConvert(QString inputFilePath, QString outputFilePath, double tolerance);


signals:
    void ConvertDone();

private:
    enum Direction
    {
        Height,
        Width,
    };
    static const qint64 UploadableMaxPixels = 2048 * 2048; //Maximum supported　Pixel size.
    static const qint64 UploadableMaxPngSize = 3000000;
    static const int    PngQuality = 60;

    void ResizeImage(QString inputFilePath, QString outputFilePath, double tolerance);
    QImage SetAlphaChannelPixel(QImage nonAlphaImage);
    QImage ScaleImage(QImage image, int scaleSize, Direction scaleDirection);
    double CalcTargetSizeRate(qint64 size);
};

#endif // IMGCONVERTER_H
