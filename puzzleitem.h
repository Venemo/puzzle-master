
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
// Copyright (C) 2010-2013, Timur Kristóf <venemo@fedoraproject.org>

#ifndef PUZZLEPIECE_H
#define PUZZLEPIECE_H

#include <QDeclarativeItem>

#include "util.h"
#include "puzzleboard.h"
#include "helpers/shapeprocessor.h"

class PuzzlePiecePrimitive;

class PuzzleItem : public QDeclarativeItem
{
    Q_OBJECT
    GENPROPERTY_S(QPoint, _puzzleCoordinates, puzzleCoordinates, setPuzzleCoordinates)
    GENPROPERTY_S(QPointF, _supposedPosition, supposedPosition, setSupposedPosition)
    GENPROPERTY_R(QList<PuzzleItem*>, _neighbours, neighbours)
    GENPROPERTY_S(bool, _canMerge, canMerge, setCanMerge)
    GENPROPERTY_S(qreal, _weight, weight, setWeight)
    GENPROPERTY_S(unsigned, _tabStatus, tabStatus, setTabStatus)
    GENPROPERTY_R(QList<int>, _grabbedTouchPointIds, grabbedTouchPointIds)

    QPointF _dragStart;
    bool _dragging, _isDraggingWithTouch, _isRightButtonPressed;
    qreal _rotationStart;
    int _previousTouchPointCount;
    QList<PuzzlePiecePrimitive*> _primitives;

    friend class PuzzleBoard;

public:
    explicit PuzzleItem(PuzzleBoard *parent = 0);
    void mergeIfPossible(PuzzleItem *item);
    void raise();
    void verifyPosition();
    void addNeighbour(PuzzleItem *piece);
    void removeNeighbour(PuzzleItem *piece);
    bool isNeighbourOf(const PuzzleItem *piece) const;
    QPointF centerPoint() const;
    void addPrimitive(PuzzlePiecePrimitive *primitive, const QPointF &correction);
    inline const QList<PuzzlePiecePrimitive*> &primitives() const { return _primitives; }

public slots:
    inline void enableMerge() { _canMerge = true; }
    inline void disableMerge() { _canMerge = false; }

signals:
    void noNeighbours();

protected:
    void startDrag(const QPointF &pos);
    void stopDrag();
    void doDrag(const QPointF &pos);
    void startRotation(const QPointF &vector);
    void handleRotation(const QPointF &vector);
    void setCompensatedTransformOriginPoint(const QPointF &point);
    void checkMergeableSiblings();
    bool checkMergeability(PuzzleItem *item);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
};

#endif // PUZZLEPIECE_H
