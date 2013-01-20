#ifndef PUZZLEPIECESHAPE_H
#define PUZZLEPIECESHAPE_H

#include <QString>
#include <QPixmap>
#include <QPainterPath>
#include <QList>

class PuzzleItem;

namespace PuzzlePieceShape
{

enum TabStatus {
    LeftTab =         1,
    TopTab =          2,
    RightTab =        4,
    BottomTab =       8,

    LeftBlank =      16,
    TopBlank =       32,
    RightBlank =     64,
    BottomBlank =   128,

    LeftBorder =    256,
    TopBorder =     512,
    RightBorder =  1024,
    BottomBorder = 2048
};

struct PuzzlePieceShapeDescriptor {
    QPainterPath shape;
    int sxCorrection;
    int syCorrection;
    int xCorrection;
    int yCorrection;
    int widthCorrection;
    int heightCorrection;
};

QPixmap processImage(const QString &url, int width, int height);
void generatePuzzlePieceStatuses(unsigned rows, unsigned cols, int *statuses);
PuzzlePieceShapeDescriptor createPuzzlePieceShape(QSize _unit, int status, qreal tabFull, qreal tabSize, qreal tabOffset, qreal tabTolerance);
PuzzleItem *findPuzzleItem(QPointF p, const QList<PuzzleItem*> &puzzleItems);

}

#endif // PUZZLEPIECESHAPE_H
