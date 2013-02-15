
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

#include <QPixmap>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QTouchEvent>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QElapsedTimer>
#include <cmath>

#include "puzzleitem.h"
#include "puzzle/puzzlepieceprimitive.h"

using namespace std;

inline static qreal angle(const QPointF &v)
{
    if (v.x() == 0 && v.y() > 0)
        return M_PI / 2;
    if (v.x() == 0)
        return - M_PI / 2;
    if (v.x() > 0)
        return atan(v.y() / v.x());

    return atan(v.y() / v.x()) - M_PI;
}

inline static qreal simplifyAngle(qreal a)
{
    while (a >= 360)
        a -= 360;
    while (a < 0)
        a += 360;

    return a;
}

PuzzleItem::PuzzleItem(PuzzleBoard *parent)
    : QDeclarativeItem(parent),
      _canMerge(false),
      _weight(randomInt(100, 950) / 1000.0),
      _dragging(false),
      _isDraggingWithTouch(false),
      _isRightButtonPressed(false),
      _previousTouchPointCount(0)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    setFlag(QGraphicsItem::ItemStacksBehindParent, false);
    setFlag(QGraphicsItem::ItemNegativeZStacksBehindParent, false);
    setAcceptTouchEvents(false);
    setAcceptedMouseButtons(Qt::NoButton);
}

QPointF PuzzleItem::centerPoint() const
{
    return QPointF(width() / 2.0, height() / 2.0);
}

void PuzzleItem::addNeighbour(PuzzleItem *piece)
{
    if (piece == this)
        return;

    this->_neighbours.insert(piece);
    piece->_neighbours.insert(this);
}

void PuzzleItem::removeNeighbour(PuzzleItem *piece)
{
    this->_neighbours.remove(piece);
    piece->_neighbours.remove(this);
}

bool PuzzleItem::isNeighbourOf(const PuzzleItem *piece) const
{
    if (piece->neighbours().contains((PuzzleItem*)this) && this->neighbours().contains((PuzzleItem*)piece))
        return true;
    return false;
}

void PuzzleItem::mergeIfPossible(PuzzleItem *item)
{
    item->_canMerge = _canMerge = false;

    // Add the neighbours of the other item to this item
    foreach (PuzzleItem *n, item->neighbours())
    {
        item->removeNeighbour(n);
        this->addNeighbour(n);
    }

    // Add the other items' primitives to this item
    foreach (PuzzlePiecePrimitive *pr, item->_primitives)
        this->addPrimitive(pr, item->supposedPosition() - this->supposedPosition());
    item->_primitives.clear();

    static_cast<PuzzleBoard*>(parent())->removePuzzleItem(item);

    // Grab the touch points of the other item
    foreach (int id, item->_grabbedTouchPointIds)
        this->_grabbedTouchPointIds.append(id);

    // See if the puzzle is solved
    if (neighbours().count() == 0)
    {
        _dragging = _isDraggingWithTouch = _canMerge = false;
        qDebug() << "puzzle solved! :)";
        emit noNeighbours();
    }
    else
        _canMerge = true;
}

void PuzzleItem::startDrag(const QPointF &p)
{
    if (!_canMerge)
    {
        stopDrag();
        return;
    }

    raise();
    _dragging = true;
    _dragStart = mapToParent(p) - pos();
}

void PuzzleItem::stopDrag()
{
    _dragging = false;
    _isDraggingWithTouch = false;
    verifyPosition();
}

void PuzzleItem::doDrag(const QPointF &position)
{
    if (_dragging)
        setPos(mapToParent(position) - _dragStart);
}

void PuzzleItem::startRotation(const QPointF &vector)
{
    _rotationStart = angle(vector) * 180 / M_PI - rotation();
}

void PuzzleItem::handleRotation(const QPointF &v)
{
    qreal a = angle(v) * 180 / M_PI - _rotationStart;
    setRotation(simplifyAngle(a));
}

void PuzzleItem::checkMergeableSiblings()
{
    if (_canMerge)
        foreach (PuzzleItem *p, neighbours())
            if (checkMergeability(p) || p->checkMergeability(this))
                mergeIfPossible(p);
}

bool PuzzleItem::checkMergeability(PuzzleItem *p)
{
    PuzzleBoard *board = static_cast<PuzzleBoard*>(parent());
    qreal rotationDiff = abs(simplifyAngle(p->rotation() - rotation()));

    // Check rotation difference
    if (rotationDiff > board->rotationTolerance())
        return false;

    QPointF diff = (this->supposedPosition() - p->supposedPosition()) - this->QGraphicsItem::mapToItem(p, QPointF(0, 0));
    qreal distance = sqrt(diff.x() * diff.x() + diff.y() * diff.y());
    return distance < board->tolerance();
}

void PuzzleItem::setCompensatedTransformOriginPoint(const QPointF &point)
{
    if (transformOriginPoint() != point)
    {
        QPointF compensation = mapToParent(0, 0);
        setTransformOriginPoint(point);
        compensation -= mapToParent(0, 0);
        setPos(pos() + compensation);
        _dragStart -= compensation;
    }
}

void PuzzleItem::verifyPosition()
{
    PuzzleBoard *board = static_cast<PuzzleBoard*>(parent());
    qreal a = rotation();
    QPointF p1 = mapToScene(QPointF(0, 0)),
            p2 = mapToScene(QPointF(width(), 0)),
            p3 = mapToScene(QPointF(0, height())),
            p4 = mapToScene(QPointF(width(), height())),
            p(myMin<qreal>(myMin<qreal>(p1.x(), p2.x()), myMin<qreal>(p3.x(), p4.x())), myMin<qreal>(myMin<qreal>(p1.y(), p2.y()), myMin<qreal>(p3.y(), p4.y())));

    if (a >= 0 && a < 90)
        a = 90 - a;
    else if (a >= 90 && a < 180)
        a = a - 90;
    else if (a >= 180 && a < 270)
        a = a - 180;
    else
        a = a - 270;

    a = a * M_PI / 180.0;

    qreal   w = boundingRect().height() * cos(a) + boundingRect().width() * sin(a),
            h = boundingRect().width() * cos(a) + boundingRect().height() * sin(a),
            maxX = board->width() - w / 2,
            minX = - w / 2,
            maxY = board->height() - h / 2,
            minY = - h / 2;

    if (p.x() > maxX || p.x() < minX || p.y() > maxY || p.y() < minY)
    {
        QPointF newPos = QPointF(CLAMP(p.x(), minX + 40, maxX - 40), CLAMP(p.y(), minY + 40, maxY - 40)) + pos() - p;

        _dragging = false;
        _isDraggingWithTouch = false;
        _canMerge = false;

        QPropertyAnimation *anim = new QPropertyAnimation(this, "pos", this);
        connect(anim, SIGNAL(finished()), this, SLOT(enableMerge()));

        anim->setEndValue(newPos);
        anim->setDuration(150);
        anim->setEasingCurve(QEasingCurve(QEasingCurve::OutBounce));
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void PuzzleItem::raise()
{
    PuzzleItem *maxItem = this;
    foreach (PuzzleItem *item, static_cast<PuzzleBoard*>(parent())->puzzleItems())
        if (item->zValue() > maxItem->zValue())
            maxItem = item;

    if (maxItem == this)
        return;

    qreal max = maxItem->zValue();
    foreach (PuzzleItem *item, static_cast<PuzzleBoard*>(parent())->puzzleItems())
        if (item->zValue() > this->zValue())
            item->setZValue(item->zValue() - 1);

    setZValue(max);
}

void PuzzleItem::addPrimitive(PuzzlePiecePrimitive *p, const QPointF &corr)
{
    if (_primitives.contains(p))
        return;

    p->setParent(this);
    p->setPixmapOffset(p->pixmapOffset() + corr);
    p->setStrokeOffset(p->strokeOffset() + corr);
    _primitives.append(p);
}

void PuzzleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // Uncomment for or various debugging purposes

    //painter->drawRect(boundingRect());
    //painter->drawEllipse(mapFromScene(pos()), 10, 10);
    //painter->fillPath(_fakeShape, QBrush(QColor(0, 0, 255, 130)));
    //painter->fillPath(_realShape, QBrush(QColor(0, 255, 0, 130)));
    //painter->fillRect(QRectF(0, 0, this->width(), this->height()), QBrush(QColor(255, 0, 0, 130)));

    // Draw the strokes first
    foreach (PuzzlePiecePrimitive *p, _primitives)
        painter->drawPixmap(p->strokeOffset(), p->stroke());

    // Draw the actual pixmaps
    foreach (PuzzlePiecePrimitive *p, _primitives)
        painter->drawPixmap(p->pixmapOffset(), p->pixmap());
}
