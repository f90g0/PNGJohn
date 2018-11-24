#ifndef IMGCONVERTER_H
#define IMGCONVERTER_H

#include <QtCore>
#include <QImage>
#include <QtConcurrentRun>
#include <QtConcurrent>


class ImgConverter
{
public:
    ImgConverter(QImage convertImage, QString outputFilePath, int reductionAmount);

private:
    void ResizeImage(QImage convertImage, QString outputFilePath, int reductionAmount);
    QImage SetAlphaChannelPixel(QImage nonAlphaImage);

};

#endif // IMGCONVERTER_H
