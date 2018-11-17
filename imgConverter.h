#ifndef IMGCONVERTER_H
#define IMGCONVERTER_H

#include <QtCore>
#include <QImage>


class ImgConverter
{
public:
    ImgConverter();
    QImage SetAlphaChannelPixel(QImage nonAlphaImage);
    //getColor();
};

#endif // IMGCONVERTER_H
