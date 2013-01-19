#ifndef PUZZLEPIECESHAPE_H
#define PUZZLEPIECESHAPE_H

#include <QString>
#include <QPixmap>

namespace PuzzlePieceShape
{
    QPixmap processImage(const QString &url, int width, int height);
}

#endif // PUZZLEPIECESHAPE_H
