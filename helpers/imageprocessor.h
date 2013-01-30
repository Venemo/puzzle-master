#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QString>
#include <QPixmap>
#include "helpers/helpertypes.h"

namespace PuzzleHelpers
{

class ImageProcessorPrivate;

class ImageProcessor
{
    ImageProcessorPrivate *_p;

public:
    explicit ImageProcessor(const QString &url, const QSize &viewportSize, int rows, int cols);
    ~ImageProcessor();

    bool isValid();
    const GameDescriptor &descriptor();
    QPixmap drawPiece(int i, int j, const QPainterPath &shape, const PuzzleHelpers::Correction &corr);
    QPixmap drawStroke();

};
}

#endif // IMAGEPROCESSOR_H
