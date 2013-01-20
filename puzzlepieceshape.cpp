
#include <QPainter>

#include "puzzleitem.h"
#include "puzzlepieceshape.h"
#include "util.h"

namespace PuzzlePieceShape
{
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

PuzzlePieceShapeDescriptor createPuzzlePieceShape(int i, int j, int rows, int cols, QSize _unit, int *statuses, qreal tabFull, qreal tabSize, qreal tabOffset, qreal tabTolerance)
{
    PuzzlePieceShapeDescriptor descriptor;

    QPainterPath rectClip;
    rectClip.addRect(tabFull - 1, tabFull - 1, _unit.width() + 1, _unit.height() + 1);
    descriptor.shape = rectClip;
    QPainterPath &clip = descriptor.shape;

    int &sxCorrection = descriptor.sxCorrection, &syCorrection = descriptor.syCorrection, &xCorrection = descriptor.xCorrection, &yCorrection = descriptor.yCorrection, &widthCorrection = descriptor.widthCorrection, &heightCorrection = descriptor.heightCorrection;
    sxCorrection = 0, syCorrection = 0, xCorrection = 0, yCorrection = 0, widthCorrection = 0, heightCorrection = 0;

    // Left
    if (i > 0)
    {
        if (statuses[(i - 1) * rows + j] & PuzzlePieceShape::RightTab)
        {
            QPainterPath leftBlank;
            leftBlank.addEllipse(QPointF(tabFull + tabOffset, tabFull + _unit.height() / 2.0), tabSize, tabSize);
            clip = clip.subtracted(leftBlank);
            xCorrection -= tabFull;
        }
        else
        {
            statuses[i * rows + j] |= PuzzlePieceShape::LeftTab;
            QPainterPath leftTab;
            leftTab.addEllipse(QPointF(tabSize + tabTolerance, tabFull + _unit.height() / 2.0), tabSize + tabTolerance, tabSize + tabTolerance);
            clip = clip.united(leftTab);
            sxCorrection -= tabFull;
            widthCorrection += tabFull;
        }
    }
    else
        xCorrection -= tabFull;

    // Top
    if (j > 0)
    {
        if (statuses[i * rows + j - 1] & PuzzlePieceShape::BottomTab)
        {
            QPainterPath topBlank;
            topBlank.addEllipse(QPointF(tabFull + _unit.width() / 2.0, tabFull + tabOffset), tabSize, tabSize);
            clip = clip.subtracted(topBlank);
            yCorrection -= tabFull;
        }
        else
        {
            statuses[i * rows + j] |= PuzzlePieceShape::TopTab;
            QPainterPath topTab;
            topTab.addEllipse(QPointF(tabFull + _unit.width() / 2.0, tabSize + tabTolerance), tabSize + tabTolerance, tabSize + tabTolerance);
            clip = clip.united(topTab);
            syCorrection -= tabFull;
            heightCorrection += tabFull;
        }
    }
    else
        yCorrection -= tabFull;

    // Right
    if (i < cols - 1)
    {
        statuses[i * rows + j] |= randomInt(0, 1) * PuzzlePieceShape::RightTab;

        if (statuses[i * rows + j] & PuzzlePieceShape::RightTab)
        {
            QPainterPath rightTab;
            rightTab.addEllipse(QPointF(tabFull + _unit.width() + tabOffset, tabFull + _unit.height() / 2.0), tabSize + tabTolerance, tabSize + tabTolerance);
            clip = clip.united(rightTab);
            widthCorrection += tabFull;
        }
        else
        {
            QPainterPath rightBlank;
            rightBlank.addEllipse(QPointF(tabFull + _unit.width() - tabOffset, tabFull + _unit.height() / 2.0), tabSize, tabSize);
            clip = clip.subtracted(rightBlank);
        }
    }

    // Bottom
    if (j < rows - 1)
    {
        statuses[i * rows + j] |= randomInt(0, 1) * PuzzlePieceShape::BottomTab;

        if (statuses[i * rows + j] & PuzzlePieceShape::BottomTab)
        {
            QPainterPath bottomTab;
            bottomTab.addEllipse(QPointF(tabFull + _unit.width() / 2.0, tabFull + _unit.height() + tabOffset), tabSize + tabTolerance, tabSize + tabTolerance);
            clip = clip.united(bottomTab);
            heightCorrection += tabFull;
        }
        else
        {
            QPainterPath bottomBlank;
            bottomBlank.addEllipse(QPointF(tabFull + _unit.width() / 2.0, tabFull + _unit.height() - tabOffset), tabSize, tabSize);
            clip = clip.subtracted(bottomBlank);
        }
    }

    clip = clip.translated(xCorrection, yCorrection).simplified();
    rectClip = rectClip.translated(xCorrection, yCorrection);

    return descriptor;
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
