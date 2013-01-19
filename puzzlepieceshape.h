#ifndef PUZZLEPIECESHAPE_H
#define PUZZLEPIECESHAPE_H

#include <QString>
#include <QPixmap>
#include <QPainterPath>

namespace PuzzlePieceShape
{

enum TabStatus {
    LeftTab = 0x01,
    TopTab = 0x02,
    RightTab = 0x04,
    BottomTab = 0x08
};

struct PuzzlePieceDescriptor {
    QPainterPath shape;
    int sxCorrection;
    int syCorrection;
    int xCorrection;
    int yCorrection;
    int widthCorrection;
    int heightCorrection;
};

QPixmap processImage(const QString &url, int width, int height);
PuzzlePieceDescriptor createPuzzlePieceShape(int i, int j, int rows, int cols, QSize _unit, int *statuses, qreal tabFull, qreal tabSize, qreal tabOffset, qreal tabTolerance);

}

#endif // PUZZLEPIECESHAPE_H
