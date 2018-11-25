#include "imgConverter.h"

ImgConverter::ImgConverter(QImage convertImage, QString outputFilePath, double tolerance)
{
    QFuture<void> imgConverterThread = QtConcurrent::run(this, &ImgConverter::ResizeImage, convertImage, outputFilePath, tolerance);

}

void ImgConverter::ResizeImage(QImage convertImage, QString outputFilePath, double tolerance)
{
    QImage originalImage = convertImage;
    QImage argb32Image = originalImage.convertToFormat(QImage::Format_ARGB32);

    QImage scaledImage; //Raw ARGB32 Image
    QBuffer pngImageBuffer;

    argb32Image.save(&pngImageBuffer, "PNG");

    if (pngImageBuffer.size() <= 3000000) {
        argb32Image = SetAlphaChannelPixel(argb32Image);
        argb32Image.save(outputFilePath, "PNG");
        qDebug() << "orig size";
        return;
    }

    QByteArray bufferArray;
    QBuffer imageWriteBuffer(&bufferArray);
    imageWriteBuffer.open(QIODevice::WriteOnly);
    argb32Image.save(&imageWriteBuffer, "PNG");
    Direction scaleDirection;
    int minLineValue = qMin(argb32Image.height(), argb32Image.width());
    int uploadableMaxPixels = 2048 * 2048; //Maximum supported　Pixel size.
    if(minLineValue == argb32Image.height()) {
        scaleDirection = Direction::Height;
    } else {
        scaleDirection = Direction::Width;
    }
    int lowSize = 0;
    int highSize;
    int middleSize;
    int limitedPixWidth = round(qSqrt((uploadableMaxPixels * (float)argb32Image.width()) / (float)argb32Image.height()));
    int limitedPixHeight = round(qSqrt((uploadableMaxPixels * (float)argb32Image.height()) / (float)argb32Image.width()));
    qDebug() << "limited width:" << limitedPixWidth << " limited height:" << limitedPixHeight << " limited pix num:" << limitedPixHeight * limitedPixWidth;
    if(scaleDirection == Direction::Height) {
        highSize = limitedPixHeight;
    } else {
        highSize = limitedPixWidth;
    }
    scaledImage = ScaleImage(argb32Image, highSize, scaleDirection);

    scaledImage.save(&imageWriteBuffer, "PNG");
    double rate = CalcTargetSizeRate(imageWriteBuffer.size());
    while(rate <= tolerance || 1.00 <= rate){
        middleSize = (lowSize + highSize) / 2;
        scaledImage = ScaleImage(argb32Image, middleSize, scaleDirection);
        qDebug() << "size" << scaledImage.width() << " x " << scaledImage.height();
        imageWriteBuffer.close();
        qDebug() << "buf size" << imageWriteBuffer.size();
        bufferArray.clear();
        scaledImage.save(&imageWriteBuffer, "PNG");
        rate = CalcTargetSizeRate(imageWriteBuffer.size());
        qDebug() << "rate" << rate;
        if(middleSize == highSize){
            break;
        }else if(rate <= tolerance){
            lowSize = middleSize + 1;
        }else if(1.00 <= rate){
            highSize = middleSize + 1;
        }
        qDebug() << "middleSize" << middleSize;
        qDebug() << "lowSize" << lowSize;
        qDebug() << "highSize" << highSize;
    }
    scaledImage = SetAlphaChannelPixel(scaledImage);
    scaledImage.save(outputFilePath, "PNG");
}

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

QImage ImgConverter::ScaleImage(QImage image, int scaleSize, Direction scaleDirection)
{
    QImage scaledImage;

    if(scaleDirection == Direction::Height) {
        scaledImage = image.scaledToHeight(scaleSize, Qt::SmoothTransformation);
    } else {
        scaledImage = image.scaledToWidth(scaleSize, Qt::SmoothTransformation);
    }
    qDebug() << scaledImage;

    return scaledImage;
}

double ImgConverter::CalcTargetSizeRate(qint64 size)
{
    return static_cast<double>(size) / 3000000;
}
