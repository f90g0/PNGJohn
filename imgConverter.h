#ifndef IMGCONVERTER_H
#define IMGCONVERTER_H

#include <QtCore>
#include <QImage>
#include <QtConcurrentRun>
#include <QtConcurrent>


class ImgConverter
{
public:
    ImgConverter(QImage convertImage, QString outputFilePath, double tolerance);

private:
    enum Direction
    {
        Height,
        Width,
    };
    static const qint64 UploadableMaxPixels = 2048 * 2048; //Maximum supported　Pixel size.
    static const qint64 UploadableMaxPngSize = 3000000;

    void ResizeImage(QImage convertImage, QString outputFilePath, double tolerance);
    QImage SetAlphaChannelPixel(QImage nonAlphaImage);
    QImage ScaleImage(QImage image, int scaleSize, Direction scaleDirection);
    double CalcTargetSizeRate(qint64 size);
};

#endif // IMGCONVERTER_H
