#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QString>
#include <QPixmap>
#include "helpers/imageprocessor.h"
#include "helpers/shapeprocessor.h"

namespace PuzzleHelpers
{

class ImageProcessorPrivate;

class ImageProcessor
{
    ImageProcessorPrivate *_p;

public:
    explicit ImageProcessor(const QString &url, int width, int height, int rows, int cols);
    ~ImageProcessor();

    bool isValid();
    QSize unit();
    QSize pixmapSize();
    QPixmap drawPiece(int i, int j, int tabFull, const QSize &unit, const QPainterPath &shape, const PuzzleHelpers::Correction &corr);
    QPixmap drawStroke();

};
}

#endif // IMAGEPROCESSOR_H
