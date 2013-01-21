
// This file is part of Puzzle Master, a fun and addictive jigsaw puzzle game.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Copyright (C) 2010-2011, Timur Kristóf <venemo@fedoraproject.org>

#include <QPainter>
#include <QMap>
#include <QPainterPathStroker>

#include "puzzleitem.h"
#include "puzzlepieceshape.h"
#include "util.h"

// This function generates puzzle piece shapes.
// ----------
// unit - the size of the rectangular base of the puzzle piece
// status - flags of TabStatus values that describe the shape of this puzzle piece
// tabFull - the full size of tabs including stroke, offset, tolerance
// tabSize - size of tabs
// tabOffset - offset of tabs
// tabTolerance - extra size to the tabs (so that merging works flawlessly)
// blankSize - size of blanks
// blankOffset - offset of blanks
// ----------
static QPainterPath createPuzzlePieceShape(QSize unit, int status, qreal tabFull, qreal tabSize, qreal tabOffset, qreal tabTolerance, qreal blankSize, qreal blankOffset)
{
    QPainterPath rectClip;
    rectClip.addRect(tabFull - 1, tabFull - 1, unit.width() + 1, unit.height() + 1);
    QPainterPath clip = rectClip;

    // Left
    if (status & PuzzlePieceShape::LeftBlank)
    {
        QPainterPath leftBlank;
        leftBlank.addEllipse(QPointF(tabFull + blankOffset, tabFull + unit.height() / 2.0), blankSize, blankSize);
        clip = clip.subtracted(leftBlank);
    }
    else if (status & PuzzlePieceShape::LeftTab)
    {
        QPainterPath leftTab;
        leftTab.addEllipse(QPointF(tabSize + tabTolerance, tabFull + unit.height() / 2.0), tabSize + tabTolerance, tabSize + tabTolerance);
        clip = clip.united(leftTab);
    }

    // Top
    if (status & PuzzlePieceShape::TopBlank)
    {
        QPainterPath topBlank;
        topBlank.addEllipse(QPointF(tabFull + unit.width() / 2.0, tabFull + blankOffset), blankSize, blankSize);
        clip = clip.subtracted(topBlank);
    }
    else if (status & PuzzlePieceShape::TopTab)
    {
        QPainterPath topTab;
        topTab.addEllipse(QPointF(tabFull + unit.width() / 2.0, tabSize + tabTolerance), tabSize + tabTolerance, tabSize + tabTolerance);
        clip = clip.united(topTab);
    }

    // Right
    if (status & PuzzlePieceShape::RightTab)
    {
        QPainterPath rightTab;
        rightTab.addEllipse(QPointF(tabFull + unit.width() + tabOffset, tabFull + unit.height() / 2.0), tabSize + tabTolerance, tabSize + tabTolerance);
        clip = clip.united(rightTab);
    }
    else if (status & PuzzlePieceShape::RightBlank)
    {
        QPainterPath rightBlank;
        rightBlank.addEllipse(QPointF(tabFull + unit.width() - blankOffset, tabFull + unit.height() / 2.0), blankSize, blankSize);
        clip = clip.subtracted(rightBlank);
    }

    // Bottom
    if (status & PuzzlePieceShape::BottomTab)
    {
        QPainterPath bottomTab;
        bottomTab.addEllipse(QPointF(tabFull + unit.width() / 2.0, tabFull + unit.height() + tabOffset), tabSize + tabTolerance, tabSize + tabTolerance);
        clip = clip.united(bottomTab);
    }
    else if (status & PuzzlePieceShape::BottomBlank)
    {
        QPainterPath bottomBlank;
        bottomBlank.addEllipse(QPointF(tabFull + unit.width() / 2.0, tabFull + unit.height() - blankOffset), blankSize, blankSize);
        clip = clip.subtracted(bottomBlank);
    }

    clip = clip.simplified();
    return clip;
}

namespace PuzzlePieceShape
{

class CreatorPrivate
{
    friend class Creator;
    QSize unit;
    qreal tabFull, tabSize, tabOffset, tabTolerance;
    int strokeThickness;
    QMap<int, QPainterPath> shapeCache, strokeShapeCache;
};

Creator::Creator(QSize unit, qreal tabFull, qreal tabSize, qreal tabOffset, qreal tabTolerance, int strokeThickness)
{
    _p = new CreatorPrivate();

    _p->unit = unit;
    _p->tabFull = tabFull;
    _p->tabSize = tabSize;
    _p->tabOffset = tabOffset;
    _p->tabTolerance = tabTolerance;
    _p->strokeThickness = strokeThickness;
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

QPainterPath Creator::getPuzzlePieceShape(int status)
{
    if (!_p->shapeCache.contains(status))
    {
        foreach (int s, _p->shapeCache.keys())
        {
            MatchMode m = match(s, status);

            if (m == NoMatch)
                continue;

            QTransform tr;

            if (m == HorizontalFlipMatch)
                tr = tr.scale(-1.0, 1.0).translate(-_p->unit.width() - _p->tabFull * 2, 0);
            else if (m == VerticalFlipMatch)
                tr = tr.scale(1.0, -1.0).translate(0, - _p->unit.height() - _p->tabFull * 2);
            else if (m == HorizontalAndVerticalFlipMatch)
                tr = tr.scale(-1.0, -1.0).translate(-_p->unit.width() - _p->tabFull * 2, - _p->unit.height() - _p->tabFull * 2);

            return _p->shapeCache[status] = tr.map(_p->shapeCache[s]);
        }

        return _p->shapeCache[status] = createPuzzlePieceShape(_p->unit, status, _p->tabFull, _p->tabSize, _p->tabOffset, _p->tabTolerance, _p->tabSize, _p->tabOffset);
    }

    return _p->shapeCache[status];
}

QPainterPath Creator::getPuzzlePieceStrokeShape(int status)
{
    if (!_p->strokeShapeCache.contains(status))
    {
        foreach (int s, _p->strokeShapeCache.keys())
        {
            MatchMode m = match(s, status);

            if (m == NoMatch)
                continue;

            QTransform tr;

            if (m == HorizontalFlipMatch)
                tr = tr.scale(-1.0, 1.0).translate(-_p->unit.width() - _p->tabFull * 2 - _p->strokeThickness * 2, 0);
            else if (m == VerticalFlipMatch)
                tr = tr.scale(1.0, -1.0).translate(0, - _p->unit.height() - _p->tabFull * 2 - _p->strokeThickness * 2);
            else if (m == HorizontalAndVerticalFlipMatch)
                tr = tr.scale(-1.0, -1.0).translate(-_p->unit.width() - _p->tabFull * 2 - _p->strokeThickness * 2, - _p->unit.height() - _p->tabFull * 2 - _p->strokeThickness * 2);

            return _p->strokeShapeCache[status] = tr.map(_p->strokeShapeCache[s]);
        }

        return _p->strokeShapeCache[status] = createPuzzlePieceShape(
                    QSize(_p->unit.width() + _p->strokeThickness * 2, _p->unit.height() + _p->strokeThickness * 2),
                    status, _p->tabFull, _p->tabSize + _p->strokeThickness, _p->tabOffset - _p->strokeThickness, _p->tabTolerance, _p->tabSize - _p->strokeThickness, _p->tabOffset + _p->strokeThickness);
    }

    return _p->strokeShapeCache[status];
}

MatchMode Creator::match(int status1, int status2)
{
    if (status1 == status2)
        return ExactMatch;

    int left1 = status1 << 29 >> 29;
    int left2 = status2 << 29 >> 29;
    int top1 = status1 << 26 >> 29;
    int top2 = status2 << 26 >> 29;
    int right1 = status1 << 23 >> 29;
    int right2 = status2 << 23 >> 29;
    int bottom1 = status1 << 20 >> 29;
    int bottom2 = status2 << 20 >> 29;

    if (left1 == right2 && left2 == right1 && top1 == top2 && bottom1 == bottom2)
        return HorizontalFlipMatch;
    if (left1 == left2 && right1 == right2 && top1 == bottom2 && bottom1 == top2)
        return VerticalFlipMatch;
    if (left1 == right2 && left2 == right1 && top1 == bottom2 && bottom1 == top2)
        return HorizontalAndVerticalFlipMatch;

    return NoMatch;
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

PuzzleItem *findPuzzleItem(QPointF p, const QList<PuzzleItem*> &puzzleItems)
{
    foreach (PuzzleItem *item, puzzleItems)
    {
        QPointF tr = item->mapFromParent(p);
        bool enableUsabilityImprovement = item->grabbedTouchPointIds().count();

        if (!enableUsabilityImprovement && item->realShape().contains(tr))
            return item;
        else if (enableUsabilityImprovement && item->fakeShape().contains(tr))
            return item;
    }

    return 0;
}
}
