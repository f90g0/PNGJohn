#ifndef IMGCONVERTER_H
#define IMGCONVERTER_H

#include <QtCore>
#include <QImage>


class ImgConverter
{
public:
    ImgConverter(QImage convertImage, QString outputFilePath);
    QImage SetAlphaChannelPixel(QImage nonAlphaImage);
};

#endif // IMGCONVERTER_H
