#ifndef PUZZLEPIECESHAPE_H
#define PUZZLEPIECESHAPE_H

#include <QString>
#include <QPixmap>
#include <QPainterPath>
#include <QList>

class PuzzleItem;

namespace PuzzlePieceShape
{

enum TabStatus
{
    LeftTab =      1<<0,
    LeftBlank =    1<<1,
    LeftBorder =   1<<2,

    TopTab =       1<<3,
    TopBlank =     1<<4,
    TopBorder =    1<<5,

    RightTab =     1<<6,
    RightBlank =   1<<7,
    RightBorder =  1<<8,

    BottomTab =    1<<9,
    BottomBlank =  1<<10,
    BottomBorder = 1<<11
};

enum MatchMode
{
    NoMatch = 0,
    ExactMatch,
    HorizontalFlipMatch,
    VerticalFlipMatch,
    HorizontalAndVerticalFlipMatch
};

struct Correction
{
    int sxCorrection;
    int syCorrection;
    int xCorrection;
    int yCorrection;
    int widthCorrection;
    int heightCorrection;
};

class CreatorPrivate;

class Creator {
    CreatorPrivate *_p;
public:
    explicit Creator(QSize unit, qreal tabFull, qreal tabSize, qreal tabOffset, qreal tabTolerance, int strokeThickness);
    ~Creator();
    Correction getCorrectionFor(int status);
    QPainterPath getPuzzlePieceShape(int status);
    QPainterPath getPuzzlePieceStrokeShape(int status);
    MatchMode match(int status1, int status2);
};

QPixmap processImage(const QString &url, int width, int height);
void generatePuzzlePieceStatuses(unsigned rows, unsigned cols, int *statuses);
PuzzleItem *findPuzzleItem(QPointF p, const QList<PuzzleItem*> &puzzleItems);

}

#endif // PUZZLEPIECESHAPE_H
