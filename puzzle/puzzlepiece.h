
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

#include "helpers/util.h"
#include "puzzle/creation/shapeprocessor.h"

class PuzzlePiecePrimitive;
class PuzzleGame;

class PuzzlePiece : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation)
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

    GENPROPERTY_S(QPoint, _puzzleCoordinates, puzzleCoordinates, setPuzzleCoordinates)
    GENPROPERTY_S(QPointF, _pos, pos, setPos)
    GENPROPERTY_S(QPointF, _supposedPosition, supposedPosition, setSupposedPosition)
    GENPROPERTY_S(QPointF, _dragStart, dragStart, setDragStart)
    GENPROPERTY_R(QPointF, _transformOriginPoint, transformOriginPoint)
    GENPROPERTY_S(qreal, _rotation, rotation, setRotation)
    GENPROPERTY_F(int, _zValue, zValue, setZValue, zValueChanged)
    GENPROPERTY_S(int, _previousTouchPointCount, previousTouchPointCount, setPreviousTouchPointCount)
    GENPROPERTY_S(unsigned, _tabStatus, tabStatus, setTabStatus)
    GENPROPERTY_R(bool, _dragging, dragging)
    GENPROPERTY_S(bool, _isRightButtonPressed, isRightButtonPressed, setIsRightButtonPressed)
    GENPROPERTY_R(bool, _isDraggingWithTouch, isDraggingWithTouch)
    GENPROPERTY_S(bool, _isEnabled, isEnabled, setIsEnabled)
    GENPROPERTY_R(QSet<PuzzlePiece*>, _neighbours, neighbours)
    GENPROPERTY_R(QSet<PuzzlePiecePrimitive*>, _primitives, primitives)
    GENPROPERTY_R(QList<int>, _grabbedTouchPointIds, grabbedTouchPointIds)

    qreal _rotationStart;
    QPointF _topLeft, _bottomRight;

public:
    explicit PuzzlePiece(PuzzleGame *parent = 0);
    void mergeIfPossible(PuzzlePiece *item);
    void raise();
    void addNeighbour(PuzzlePiece *piece);
    void removeNeighbour(PuzzlePiece *piece);
    QPointF centerPoint() const;
    void addPrimitive(PuzzlePiecePrimitive *primitive, const QPointF &correction);
    QPointF mapToParent(const QPointF &p) const;
    QPointF mapFromParent(const QPointF &p) const;
    QPointF mapToItem(const PuzzlePiece *item, const QPointF &p) const;
    const QPointF &bottomRight() const { return this->_bottomRight; }

    void startDrag(const QPointF &pos, bool touch = false);
    void stopDrag();
    void doDrag(const QPointF &pos);
    void startRotation(const QPointF &vector);
    void handleRotation(const QPointF &vector);
    void setTransformOriginPoint(const QPointF &point);
    void checkMergeableSiblings();

    void grabTouchPoint(int id);
    void ungrabTouchPoint(int id);

    static bool puzzleItemDescLessThan(PuzzlePiece *a, PuzzlePiece *b);
    static bool puzzleItemAscLessThan(PuzzlePiece *a, PuzzlePiece *b);

signals:
    void noNeighbours();
    void zValueChanged();

protected:
    void verifyPosition();
    bool checkMergeability(PuzzlePiece *item);

protected slots:
    void enable() { _isEnabled = true; }
    void disable() { _isEnabled = false; }

};

#endif // PUZZLEPIECE_H
