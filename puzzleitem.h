
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

#ifndef PUZZLEPIECE_H
#define PUZZLEPIECE_H

#include <QDeclarativeItem>

#include "util.h"
#include "puzzleboard.h"

class QPixmap;

class PuzzleItem : public QDeclarativeItem
{
    Q_OBJECT
    GENPROPERTY_S(QPoint, _puzzleCoordinates, puzzleCoordinates, setPuzzleCoordinates)
    GENPROPERTY_S(QPointF, _supposedPosition, supposedPosition, setSupposedPosition)
    GENPROPERTY_S(QPoint, _pixmapOffset, pixmapOffset, setPixmapOffset)
    GENPROPERTY_R(QList<PuzzleItem*>, _neighbours, neighbours)
    GENPROPERTY_S(QPixmap, _pixmap, pixmap, setPixmap)
    GENPROPERTY_S(bool, _canMerge, canMerge, setCanMerge)
    GENPROPERTY_S(qreal, _weight, weight, setWeight)
    GENPROPERTY_S(QPainterPath, _stroke, stroke, setStroke)
    GENPROPERTY_S(QPainterPath, _fakeShape, fakeShape, setFakeShape)
    GENPROPERTY_S(unsigned, _tabStatus, tabStatus, setTabStatus)

    QPointF _dragStart, _rotationStartVector;
    bool _dragging, _isDraggingWithTouch, _isRightButtonPressed;
    double _previousRotationValue;
    int _previousTouchPointCount;

public:
    explicit PuzzleItem(const QPixmap &pixmap, PuzzleBoard *parent = 0);
    virtual QPainterPath shape() const;
    void mergeIfPossible(PuzzleItem *item, const QPointF &dragPosition);
    void raise();
    void verifyPosition();
    void addNeighbour(PuzzleItem *piece);
    void removeNeighbour(PuzzleItem *piece);
    bool isNeighbourOf(const PuzzleItem *piece) const;
    QPointF centerPoint() const;

    enum TabStatus {
        LeftTab = 0x01,
        TopTab = 0x02,
        RightTab = 0x04,
        BottomTab = 0x08
    };

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
    void checkMergeableSiblings(const QPointF &position);
    bool checkMergeability(PuzzleItem *item);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *ev);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *ev);
    virtual bool sceneEvent(QEvent *event);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    inline qreal leftTabSize() const { return static_cast<PuzzleBoard*>(parent())->tabSizes() * (_tabStatus & PuzzleItem::LeftTab ? 1 : 0); }
    inline qreal topTabSize() const { return static_cast<PuzzleBoard*>(parent())->tabSizes() * (_tabStatus & PuzzleItem::TopTab ? 1 : 0); }
    inline qreal rightTabSize() const { return static_cast<PuzzleBoard*>(parent())->tabSizes() * (_tabStatus & PuzzleItem::RightTab ? 1 : 0); }
    inline qreal bottomTabSize() const { return static_cast<PuzzleBoard*>(parent())->tabSizes() * (_tabStatus & PuzzleItem::BottomTab ? 1 : 0); }

};

#endif // PUZZLEPIECE_H
