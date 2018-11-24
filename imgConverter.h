#ifndef IMGCONVERTER_H
#define IMGCONVERTER_H

#include <QtCore>
#include <QImage>
#include <QtConcurrentRun>
#include <QtConcurrent>


class ImgConverter
{
public:
    ImgConverter(QImage convertImage, QString outputFilePath);

private:
    void ResizeImage(QImage convertImage, QString outputFilePath);
    QImage SetAlphaChannelPixel(QImage nonAlphaImage);
    double CalcTargetSizeRate(qint64 size);

};

#endif // IMGCONVERTER_H
