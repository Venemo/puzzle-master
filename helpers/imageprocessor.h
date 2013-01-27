#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QString>
#include <QPixmap>
#include "helpers/imageprocessor.h"
#include "helpers/shapeprocessor.h"

namespace PuzzleHelpers
{

struct GameDescriptor
{
    QSize viewportSize, pixmapSize, unitSize;
    int rows, cols, tabSize, tabOffset, tabFull, tabTolerance, strokeThickness, usabilityThickness;
};

class ImageProcessorPrivate;

class ImageProcessor
{
    ImageProcessorPrivate *_p;

public:
    explicit ImageProcessor(const QString &url, const QSize &viewportSize, int rows, int cols);
    ~ImageProcessor();

    bool isValid();
    const GameDescriptor &descriptor();
    QPixmap drawPiece(int i, int j, int tabFull, const QSize &unit, const QPainterPath &shape, const PuzzleHelpers::Correction &corr);
    QPixmap drawStroke();

};
}

#endif // IMAGEPROCESSOR_H
