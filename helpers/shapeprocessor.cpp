
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
#include "shapeprocessor.h"
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
static QPainterPath createPuzzleShape(QSize unit, int status, qreal tabFull, qreal tabSize, qreal tabOffset, qreal tabTolerance, qreal blankSize, qreal blankOffset)
{
    QPainterPath rectClip;
    rectClip.addRect(tabFull - 1, tabFull - 1, unit.width() + 1, unit.height() + 1);
    QPainterPath clip = rectClip;

    // Left
    if (status & PuzzleHelpers::LeftBlank)
    {
        QPainterPath leftBlank;
        leftBlank.addEllipse(QPointF(tabFull + blankOffset, tabFull + unit.height() / 2.0), blankSize, blankSize);
        clip = clip.subtracted(leftBlank);
    }
    else if (status & PuzzleHelpers::LeftTab)
    {
        QPainterPath leftTab;
        leftTab.addEllipse(QPointF(tabSize + tabTolerance, tabFull + unit.height() / 2.0), tabSize + tabTolerance, tabSize + tabTolerance);
        clip = clip.united(leftTab);
    }

    // Top
    if (status & PuzzleHelpers::TopBlank)
    {
        QPainterPath topBlank;
        topBlank.addEllipse(QPointF(tabFull + unit.width() / 2.0, tabFull + blankOffset), blankSize, blankSize);
        clip = clip.subtracted(topBlank);
    }
    else if (status & PuzzleHelpers::TopTab)
    {
        QPainterPath topTab;
        topTab.addEllipse(QPointF(tabFull + unit.width() / 2.0, tabSize + tabTolerance), tabSize + tabTolerance, tabSize + tabTolerance);
        clip = clip.united(topTab);
    }

    // Right
    if (status & PuzzleHelpers::RightTab)
    {
        QPainterPath rightTab;
        rightTab.addEllipse(QPointF(tabFull + unit.width() + tabOffset, tabFull + unit.height() / 2.0), tabSize + tabTolerance, tabSize + tabTolerance);
        clip = clip.united(rightTab);
    }
    else if (status & PuzzleHelpers::RightBlank)
    {
        QPainterPath rightBlank;
        rightBlank.addEllipse(QPointF(tabFull + unit.width() - blankOffset, tabFull + unit.height() / 2.0), blankSize, blankSize);
        clip = clip.subtracted(rightBlank);
    }

    // Bottom
    if (status & PuzzleHelpers::BottomTab)
    {
        QPainterPath bottomTab;
        bottomTab.addEllipse(QPointF(tabFull + unit.width() / 2.0, tabFull + unit.height() + tabOffset), tabSize + tabTolerance, tabSize + tabTolerance);
        clip = clip.united(bottomTab);
    }
    else if (status & PuzzleHelpers::BottomBlank)
    {
        QPainterPath bottomBlank;
        bottomBlank.addEllipse(QPointF(tabFull + unit.width() / 2.0, tabFull + unit.height() - blankOffset), blankSize, blankSize);
        clip = clip.subtracted(bottomBlank);
    }

    clip = clip.simplified();
    return clip;
}

namespace PuzzleHelpers
{

class ShapeProcessorPrivate
{
    friend class ShapeProcessor;
    QSize unit;
    qreal tabFull, tabSize, tabOffset, tabTolerance;
    int strokeThickness;
    QMap<int, QPainterPath> shapeCache, strokeShapeCache;
};

ShapeProcessor::ShapeProcessor(const GameDescriptor &descriptor)
{
    _p = new ShapeProcessorPrivate();

    _p->unit = descriptor.unitSize;
    _p->tabFull = descriptor.tabFull;
    _p->tabSize = descriptor.tabSize;
    _p->tabOffset = descriptor.tabOffset;
    _p->tabTolerance = descriptor.tabTolerance;
    _p->strokeThickness = descriptor.strokeThickness;
}

ShapeProcessor::~ShapeProcessor()
{
    delete _p;
}

Correction ShapeProcessor::getCorrectionFor(int status)
{
    Correction result = { 0, 0, 0, 0, 0, 0 };

    // Left
    if (status & PuzzleHelpers::LeftBlank)
    {
        result.xCorrection -= _p->tabFull;
    }
    else if (status & PuzzleHelpers::LeftTab)
    {
        result.sxCorrection -= _p->tabFull;
        result.widthCorrection += _p->tabFull;
    }
    else if (status & PuzzleHelpers::LeftBorder)
    {
        result.xCorrection -= _p->tabFull;
    }

    // Top
    if (status & PuzzleHelpers::TopBlank)
    {
        result.yCorrection -= _p->tabFull;
    }
    else if (status & PuzzleHelpers::TopTab)
    {
        result.syCorrection -= _p->tabFull;
        result.heightCorrection += _p->tabFull;
    }
    else if (status & PuzzleHelpers::TopBorder)
    {
        result.yCorrection -= _p->tabFull;
    }

    // Right
    if (status & PuzzleHelpers::RightTab)
    {
        result.widthCorrection += _p->tabFull;
    }

    // Bottom
    if (status & PuzzleHelpers::BottomTab)
    {
        result.heightCorrection += _p->tabFull;
    }

    return result;
}

QPainterPath ShapeProcessor::getPuzzlePieceShape(int status)
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

        return _p->shapeCache[status] =
                createPuzzleShape(
                    _p->unit,
                    status, _p->tabFull, _p->tabSize, _p->tabOffset, _p->tabTolerance, _p->tabSize, _p->tabOffset);
    }

    return _p->shapeCache[status];
}

QPainterPath ShapeProcessor::getPuzzlePieceStrokeShape(int status)
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

        return _p->strokeShapeCache[status] =
                createPuzzleShape(
                    QSize(_p->unit.width() + _p->strokeThickness * 2, _p->unit.height() + _p->strokeThickness * 2),
                    status, _p->tabFull, _p->tabSize + _p->strokeThickness, _p->tabOffset - _p->strokeThickness, _p->tabTolerance, _p->tabSize - _p->strokeThickness, _p->tabOffset + _p->strokeThickness);
    }

    return _p->strokeShapeCache[status];
}

MatchMode ShapeProcessor::match(int status1, int status2)
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

void generatePuzzlePieceStatuses(unsigned rows, unsigned cols, int *statuses)
{
    for (unsigned i = 0; i < cols; i++)
    {
        for (unsigned j = 0; j < rows; j++)
        {
            // Left
            if (i > 0 && statuses[(i - 1) * rows + j] & PuzzleHelpers::RightTab)
                statuses[i * rows + j] |= PuzzleHelpers::LeftBlank;
            else if (i > 0)
                statuses[i * rows + j] |= PuzzleHelpers::LeftTab;
            else
                statuses[i * rows + j] |= PuzzleHelpers::LeftBorder;

            // Top
            if (j > 0 && statuses[i * rows + j - 1] & PuzzleHelpers::BottomTab)
                statuses[i * rows + j] |= PuzzleHelpers::TopBlank;
            else if (j > 0)
                statuses[i * rows + j] |= PuzzleHelpers::TopTab;
            else
                statuses[i * rows + j] |= PuzzleHelpers::TopBorder;

            // Right
            if (i < cols - 1)
                statuses[i * rows + j] |= randomInt(0, 1) ? PuzzleHelpers::RightTab : PuzzleHelpers::RightBlank;
            else
                statuses[i * rows + j] |= PuzzleHelpers::RightBorder;

            // Bottom
            if (j < rows - 1)
                statuses[i * rows + j] |= randomInt(0, 1) ? PuzzleHelpers::BottomTab : PuzzleHelpers::BottomBlank;
            else
                statuses[i * rows + j] |= PuzzleHelpers::BottomBorder;
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
