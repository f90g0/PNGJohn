#include "imgConverter.h"

QImage ImgConverter::SetAlphaChannelPixel(QImage image)
{
    QImage alphaSetImage = image;
    QColor PixColor = QColor::fromRgba(alphaSetImage.pixel(0,0));

    qDebug() << "Original Color : " << QColor::fromRgba(alphaSetImage.pixel(0,0));

    alphaSetImage.setPixel(0,0,QColor(PixColor.red(),PixColor.green(),PixColor.blue(),254).rgba());

    qDebug() << "Edited Color   : " << QColor::fromRgba(alphaSetImage.pixel(0,0));
    qDebug() << "Output Size    : " << alphaSetImage.width() << " x " << alphaSetImage.height();

    return alphaSetImage;
}

ImgConverter::ImgConverter()
{
    QFile originalImageData("test.png");

    if (!originalImageData.open(QIODevice::ReadOnly)) {
        return;
    }

    QByteArray originalImageArray = originalImageData.readAll();

    QImage originalImage = QImage::fromData(originalImageArray,"PNG");
    qDebug() << originalImage;

    QImage argb32Image = originalImage.convertToFormat(QImage::Format_ARGB32);

    QImage scaledImage; //Raw ARGB32 Image
    QBuffer pngImageBuffer;

    argb32Image.save(&pngImageBuffer, "PNG");

    if (pngImageBuffer.size() <= 3000000) {
        argb32Image = SetAlphaChannelPixel(argb32Image);
        argb32Image.save("output.png", "PNG");
        qDebug() << "orig size";
    } else {
        int decrementToPix = 5;

        QByteArray bufferArray;
        QBuffer imageWriteBuffer(&bufferArray);

        imageWriteBuffer.open(QIODevice::WriteOnly);
        argb32Image.save(&imageWriteBuffer, "PNG");

        //バカっぽい
        bool decrementLine; //0:height, 1:width
        int minLineValue = qMin(argb32Image.height(), argb32Image.width());
        int uploadableMaxPixels = 2048 * 2048; //

        if(minLineValue == argb32Image.height()) {
            decrementLine = 0;
        } else {
            decrementLine = 1;
        }

        int limitedPixWidth = round(qSqrt((uploadableMaxPixels * (float)argb32Image.width()) / (float)argb32Image.height()));
        int limitedPixHeight = round(qSqrt((uploadableMaxPixels * (float)argb32Image.height()) / (float)argb32Image.width()));
        qDebug() << "limited width:" << limitedPixWidth << " limited height:" << limitedPixHeight << " limited pix num:" << limitedPixHeight * limitedPixWidth;

        if(decrementLine == 0) {
            scaledImage = argb32Image.scaledToHeight((limitedPixHeight),Qt::SmoothTransformation);
        } else {
            scaledImage = argb32Image.scaledToWidth((limitedPixWidth),Qt::SmoothTransformation);
            qDebug() << scaledImage;
        }
        scaledImage.save(&imageWriteBuffer, "PNG");



        while(imageWriteBuffer.size() > 3000000){
            if(decrementLine == 0) {
                scaledImage = scaledImage.scaledToHeight((scaledImage.height() - decrementToPix),Qt::SmoothTransformation);
            } else {
                scaledImage = scaledImage.scaledToWidth((scaledImage.width() - decrementToPix),Qt::SmoothTransformation);
            }
            qDebug() << "size" << scaledImage.width() << " x " << scaledImage.height();

            imageWriteBuffer.close();
            decrementToPix += 5;
            qDebug() << decrementToPix;
            qDebug() << "buf size" << imageWriteBuffer.size();
            bufferArray.clear();
            scaledImage.save(&imageWriteBuffer, "PNG");
        }
        scaledImage = SetAlphaChannelPixel(scaledImage);
        scaledImage.save("output.png", "PNG");

    }
}
