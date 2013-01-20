
#include <QPainter>

#include "puzzleitem.h"
#include "puzzlepieceshape.h"
#include "util.h"

namespace PuzzlePieceShape
{

class CreatorPrivate
{
    friend class Creator;
    QSize unit;
    qreal tabFull, tabSize, tabOffset, tabTolerance;
};

Creator::Creator(QSize unit, qreal tabFull, qreal tabSize, qreal tabOffset, qreal tabTolerance)
{
    _p = new CreatorPrivate();
    _p->unit = unit;
    _p->tabFull = tabFull;
    _p->tabSize = tabSize;
    _p->tabOffset = tabOffset;
    _p->tabTolerance = tabTolerance;
}

Creator::~Creator()
{
    delete _p;
}

Correction Creator::getCorrectionFor(int status)
{
    Correction result = { 0, 0, 0, 0, 0, 0 };

    // Left
    if (status & PuzzlePieceShape::LeftBlank)
    {
        result.xCorrection -= _p->tabFull;
    }
    else if (status & PuzzlePieceShape::LeftTab)
    {
        result.sxCorrection -= _p->tabFull;
        result.widthCorrection += _p->tabFull;
    }
    else if (status & PuzzlePieceShape::LeftBorder)
    {
        result.xCorrection -= _p->tabFull;
    }

    // Top
    if (status & PuzzlePieceShape::TopBlank)
    {
        result.yCorrection -= _p->tabFull;
    }
    else if (status & PuzzlePieceShape::TopTab)
    {
        result.syCorrection -= _p->tabFull;
        result.heightCorrection += _p->tabFull;
    }
    else if (status & PuzzlePieceShape::TopBorder)
    {
        result.yCorrection -= _p->tabFull;
    }

    // Right
    if (status & PuzzlePieceShape::RightTab)
    {
        result.widthCorrection += _p->tabFull;
    }

    // Bottom
    if (status & PuzzlePieceShape::BottomTab)
    {
        result.heightCorrection += _p->tabFull;
    }

    return result;
}

QPixmap processImage(const QString &url, int width, int height)
{
    QString url2(url);
    if (url.contains(QRegExp("/[A-Za-z]:/")))
        url2.remove("file:///");
    else
        url2.remove("file://");
    QPixmap pix(url2);

    if (pix.isNull())
        return pix;

    // If the image is better displayed in "portrait mode", rotate it.
    if ((pix.width() < pix.height() && width >= height) || (pix.width() >= pix.height() && width < height))
    {
        pix = pix.scaledToHeight(width);
        QPixmap pix2(pix.height(), pix.width());
        QPainter p;
        p.begin(&pix2);
        p.rotate(-90);
        p.translate(- pix2.height(), 0);
        p.drawPixmap(0, 0, pix);
        p.end();
        pix = pix2;
    }

    // Scale it to our width
    if (pix.width() - 1 > width || pix.width() + 1 < width)
        pix = pix.scaledToWidth(width);

    // If still not good enough, just crop it
    if (pix.height() > height)
    {
        QPixmap pix2(width, height);
        QPainter p;
        p.begin(&pix2);
        p.drawPixmap(0, 0, pix, 0, (pix.height() - height) / 2, pix.width(), pix.height());
        p.end();
        pix = pix2;
    }

    return pix;
}

void generatePuzzlePieceStatuses(unsigned rows, unsigned cols, int *statuses)
{
    for (unsigned i = 0; i < cols; i++)
    {
        for (unsigned j = 0; j < rows; j++)
        {
            // Left
            if (i > 0 && statuses[(i - 1) * rows + j] & PuzzlePieceShape::RightTab)
                statuses[i * rows + j] |= PuzzlePieceShape::LeftBlank;
            else if (i > 0)
                statuses[i * rows + j] |= PuzzlePieceShape::LeftTab;
            else
                statuses[i * rows + j] |= PuzzlePieceShape::LeftBorder;

            // Top
            if (j > 0 && statuses[i * rows + j - 1] & PuzzlePieceShape::BottomTab)
                statuses[i * rows + j] |= PuzzlePieceShape::TopBlank;
            else if (j > 0)
                statuses[i * rows + j] |= PuzzlePieceShape::TopTab;
            else
                statuses[i * rows + j] |= PuzzlePieceShape::TopBorder;

            // Right
            if (i < cols - 1)
                statuses[i * rows + j] |= randomInt(0, 1) ? PuzzlePieceShape::RightTab : PuzzlePieceShape::RightBlank;
            else
                statuses[i * rows + j] |= PuzzlePieceShape::RightBorder;

            // Bottom
            if (j < rows - 1)
                statuses[i * rows + j] |= randomInt(0, 1) ? PuzzlePieceShape::BottomTab : PuzzlePieceShape::BottomBlank;
            else
                statuses[i * rows + j] |= PuzzlePieceShape::BottomBorder;
        }
    }
}

QPainterPath createPuzzlePieceShape(QSize _unit, int status, qreal tabFull, qreal tabSize, qreal tabOffset, qreal tabTolerance)
{
    QPainterPath rectClip;
    rectClip.addRect(tabFull - 1, tabFull - 1, _unit.width() + 1, _unit.height() + 1);
    QPainterPath clip = rectClip;

    // Left
    if (status & PuzzlePieceShape::LeftBlank)
    {
        QPainterPath leftBlank;
        leftBlank.addEllipse(QPointF(tabFull + tabOffset, tabFull + _unit.height() / 2.0), tabSize, tabSize);
        clip = clip.subtracted(leftBlank);
    }
    else if (status & PuzzlePieceShape::LeftTab)
    {
        QPainterPath leftTab;
        leftTab.addEllipse(QPointF(tabSize + tabTolerance, tabFull + _unit.height() / 2.0), tabSize + tabTolerance, tabSize + tabTolerance);
        clip = clip.united(leftTab);
    }

    // Top
    if (status & PuzzlePieceShape::TopBlank)
    {
        QPainterPath topBlank;
        topBlank.addEllipse(QPointF(tabFull + _unit.width() / 2.0, tabFull + tabOffset), tabSize, tabSize);
        clip = clip.subtracted(topBlank);
    }
    else if (status & PuzzlePieceShape::TopTab)
    {
        QPainterPath topTab;
        topTab.addEllipse(QPointF(tabFull + _unit.width() / 2.0, tabSize + tabTolerance), tabSize + tabTolerance, tabSize + tabTolerance);
        clip = clip.united(topTab);
    }

    // Right
    if (status & PuzzlePieceShape::RightTab)
    {
        QPainterPath rightTab;
        rightTab.addEllipse(QPointF(tabFull + _unit.width() + tabOffset, tabFull + _unit.height() / 2.0), tabSize + tabTolerance, tabSize + tabTolerance);
        clip = clip.united(rightTab);
    }
    else if (status & PuzzlePieceShape::RightBlank)
    {
        QPainterPath rightBlank;
        rightBlank.addEllipse(QPointF(tabFull + _unit.width() - tabOffset, tabFull + _unit.height() / 2.0), tabSize, tabSize);
        clip = clip.subtracted(rightBlank);
    }

    // Bottom
    if (status & PuzzlePieceShape::BottomTab)
    {
        QPainterPath bottomTab;
        bottomTab.addEllipse(QPointF(tabFull + _unit.width() / 2.0, tabFull + _unit.height() + tabOffset), tabSize + tabTolerance, tabSize + tabTolerance);
        clip = clip.united(bottomTab);
    }
    else if (status & PuzzlePieceShape::BottomBlank)
    {
        QPainterPath bottomBlank;
        bottomBlank.addEllipse(QPointF(tabFull + _unit.width() / 2.0, tabFull + _unit.height() - tabOffset), tabSize, tabSize);
        clip = clip.subtracted(bottomBlank);
    }

    clip = clip.simplified();
    return clip;
}

PuzzleItem *findPuzzleItem(QPointF p, const QList<PuzzleItem*> &puzzleItems)
{
    foreach (PuzzleItem *item, puzzleItems)
    {
        QPointF tr = item->mapFromParent(p);

        if (item->shape().contains(tr))
            return item;
    }

    return 0;
}
}
