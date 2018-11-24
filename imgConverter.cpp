#include "imgConverter.h"

ImgConverter::ImgConverter(QImage convertImage, QString outputFilePath, int reductionAmount)
{
    QFuture<void> imgConverterThread = QtConcurrent::run(this, &ImgConverter::ResizeImage, convertImage, outputFilePath, reductionAmount);

}

void ImgConverter::ResizeImage(QImage convertImage, QString outputFilePath, int reductionAmount)
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
    } else {
        int decrementToPix = reductionAmount;

        QByteArray bufferArray;
        QBuffer imageWriteBuffer(&bufferArray);

        imageWriteBuffer.open(QIODevice::WriteOnly);
        argb32Image.save(&imageWriteBuffer, "PNG");

        //バカっぽい
        bool decrementLine; //0:height, 1:width
        int minLineValue = qMin(argb32Image.height(), argb32Image.width());
        int uploadableMaxPixels = 2048 * 2048; //Maximum supported　Pixel size.

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
            qDebug() << decrementToPix;
            decrementToPix += reductionAmount;
            qDebug() << "buf size" << imageWriteBuffer.size();
            bufferArray.clear();
            scaledImage.save(&imageWriteBuffer, "PNG");
        }
        scaledImage = SetAlphaChannelPixel(scaledImage);
        scaledImage.save(outputFilePath, "PNG");

    }

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
