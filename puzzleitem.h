
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
#include <QPixmap>
#include "puzzleboard.h"
#include "util.h"

#define DROPSHADOW_COLOR_DEFAULT QColor(0, 0, 0, 200)
#define DROPSHADOW_COLOR_SELECTED QColor(0, 0, 0, 255)

#define DROPSHADOW_RADIUS_DEFAULT 20
#define DROPSHADOW_RADIUS_SELECTED 25

class PuzzleItem : public QDeclarativeItem
{
private:
    Q_OBJECT
    GENPROPERTY_S(QPoint, _puzzleCoordinates, puzzleCoordinates, setPuzzleCoordinates)
    GENPROPERTY_S(QPointF, _supposedPosition, supposedPosition, setSupposedPosition)
    GENPROPERTY_R(QList<PuzzleItem*>, _neighbours, neighbours)
    GENPROPERTY_S(QPixmap, _pixmap, pixmap, setPixmap)
    GENPROPERTY_S(bool, _canMerge, canMerge, setCanMerge)
    GENPROPERTY_S(qreal, _weight, weight, setWeight)

    QPointF _dragStart;
    QPointF _rotationStartVector;
    bool _dragging;
    bool _isDraggingWithTouch;
    double _previousRotationValue;
    int _previousTouchPointCount;
    QPainterPath _shape;
    QPainterPath _stroke;

public:
    explicit PuzzleItem(const QPixmap &pixmap, QDeclarativeItem *parent = 0);
    inline int tolerance() const;
    inline qreal rotationTolerance() const;
    inline const QSize &unit() const;
    virtual QPainterPath shape() const;
    void setShape(const QPainterPath &shape);
    bool merge(PuzzleItem *item, const QPointF &dragPosition);
    void raise();
    void verifyPosition();
    void addNeighbour(PuzzleItem *piece);
    void removeNeighbour(PuzzleItem *piece);
    bool isNeighbourOf(const PuzzleItem *piece) const;

public slots:
    void enableMerge();
    void disableMerge();

signals:
    void noNeighbours();

protected:
    void startDrag(const QPointF &pos);
    void stopDrag();
    void doDrag(const QPointF &pos);
    void handleRotation(const QPointF &vector);
    void setCompensatedTransformOriginPoint(const QPointF &point);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *ev);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *ev);
    virtual bool sceneEvent(QEvent *event);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

};

inline const QSize &PuzzleItem::unit() const
{
    return ((PuzzleBoard*)parent())->unit();
}

inline int PuzzleItem::tolerance() const
{
    return ((PuzzleBoard*)parent())->tolerance();
}

inline qreal PuzzleItem::rotationTolerance() const
{
    return ((PuzzleBoard*)parent())->rotationTolerance();
}

#endif // PUZZLEPIECE_H
