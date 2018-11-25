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

    if (pngImageBuffer.size() <= UploadableMaxPngSize) {
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
    int lowSize = 0;
    int highSize;
    int middleSize;
    if(argb32Image.width() >= argb32Image.height()) {
        scaleDirection = Direction::Height;
        int limitedPixHeight = static_cast<int>(round(qSqrt(UploadableMaxPixels * static_cast<qreal>(argb32Image.height()) / argb32Image.width())));
        highSize = limitedPixHeight;
        qDebug() << " limited height:" << limitedPixHeight;
    } else {
        scaleDirection = Direction::Width;
        int limitedPixWidth = static_cast<int>(round(qSqrt(UploadableMaxPixels * static_cast<qreal>(argb32Image.width()) / argb32Image.height())));
        highSize = limitedPixWidth;
        qDebug() << "limited width:" << limitedPixWidth;
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
    return static_cast<double>(size) / UploadableMaxPngSize;
}
