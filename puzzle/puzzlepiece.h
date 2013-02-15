
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

#include <QObject>
#include <QSet>

#include "util.h"
#include "puzzleboard.h"
#include "helpers/shapeprocessor.h"

class PuzzlePiecePrimitive;
class PuzzleBoard;

class PuzzlePiece : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation)
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

    GENPROPERTY_S(QPointF, _pos, pos, setPos)
    GENPROPERTY_S(qreal, _rotation, rotation, setRotation)
    GENPROPERTY_S(int, _zValue, zValue, setZValue)
    GENPROPERTY_S(QPointF, _dragStart, dragStart, setDragStart)
    GENPROPERTY_S(QPointF, _transformOriginPoint, transformOriginPoint, setTransformOriginPoint)

    GENPROPERTY_S(QPoint, _puzzleCoordinates, puzzleCoordinates, setPuzzleCoordinates)
    GENPROPERTY_S(QPointF, _supposedPosition, supposedPosition, setSupposedPosition)
    GENPROPERTY_R(QSet<PuzzlePiece*>, _neighbours, neighbours)
    GENPROPERTY_S(unsigned, _tabStatus, tabStatus, setTabStatus)
    GENPROPERTY_R(QSet<int>, _grabbedTouchPointIds, grabbedTouchPointIds)


    GENPROPERTY_R(bool, _dragging, dragging)
    GENPROPERTY_S(bool, _isRightButtonPressed, isRightButtonPressed, setIsRightButtonPressed)
    GENPROPERTY_R(bool, _isDraggingWithTouch, isDraggingWithTouch)
    GENPROPERTY_S(int, _previousTouchPointCount, previousTouchPointCount, setPreviousTouchPointCount)

    qreal _rotationStart;
    QList<PuzzlePiecePrimitive*> _primitives;

public:
    explicit PuzzlePiece(PuzzleBoard *parent = 0);
    void mergeIfPossible(PuzzlePiece *item);
    void raise();
    void verifyPosition();
    void addNeighbour(PuzzlePiece *piece);
    void removeNeighbour(PuzzlePiece *piece);
    QPointF centerPoint() const;
    void addPrimitive(PuzzlePiecePrimitive *primitive, const QPointF &correction);
    inline const QList<PuzzlePiecePrimitive*> &primitives() const { return _primitives; }
    QPointF mapToParent(const QPointF &p) const;
    QPointF mapFromParent(const QPointF &p) const;
    QPointF mapToItem(const PuzzlePiece *item, const QPointF &p) const;

    void startDrag(const QPointF &pos, bool touch = false);
    void stopDrag();
    void doDrag(const QPointF &pos);
    void startRotation(const QPointF &vector);
    void handleRotation(const QPointF &vector);
    void setCompensatedTransformOriginPoint(const QPointF &point);
    void checkMergeableSiblings();

    void grabTouchPoint(int id) { _grabbedTouchPointIds.insert(id); }
    void ungrabTouchPoint(int id) { _grabbedTouchPointIds.remove(id); }

signals:
    void noNeighbours();

protected:
    bool checkMergeability(PuzzlePiece *item);
};

#endif // PUZZLEPIECE_H
