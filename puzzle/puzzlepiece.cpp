
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

#include <QPropertyAnimation>
#include <cmath>

#include "puzzlepiece.h"
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

PuzzlePiece::PuzzlePiece(PuzzleBoard *parent)
    : QObject(parent)
    , _rotation(0)
    , _zValue(0)
    , _previousTouchPointCount(0)
    , _dragging(false)
    , _isRightButtonPressed(false)
    , _isDraggingWithTouch(false)
    , _isEnabled(true)
{
}

QPointF PuzzlePiece::centerPoint() const
{
    return (_topLeft + _bottomRight) / 2;
}

void PuzzlePiece::addNeighbour(PuzzlePiece *piece)
{
    if (piece == this)
        return;

    this->_neighbours.insert(piece);
    piece->_neighbours.insert(this);
}

void PuzzlePiece::removeNeighbour(PuzzlePiece *piece)
{
    this->_neighbours.remove(piece);
    piece->_neighbours.remove(this);
}

void PuzzlePiece::mergeIfPossible(PuzzlePiece *item)
{
    // Add the neighbours of the other item to this item
    foreach (PuzzlePiece *n, item->neighbours())
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
        this->_grabbedTouchPointIds.insert(id);

    // See if the puzzle is solved
    if (neighbours().count() == 0)
    {
        _dragging = _isDraggingWithTouch = false;
        qDebug() << "puzzle solved! :)";
        emit noNeighbours();
    }
}

void PuzzlePiece::startDrag(const QPointF &p, bool touch)
{
    raise();
    _dragging = true;
    _dragStart = mapToParent(p) - pos();
    _isDraggingWithTouch = touch;
}

void PuzzlePiece::stopDrag()
{
    _dragging = _isDraggingWithTouch = false;
    verifyPosition();
}

void PuzzlePiece::doDrag(const QPointF &position)
{
    if (!_isEnabled)
        return;

    if (_dragging)
        setPos(mapToParent(position) - _dragStart);
}

void PuzzlePiece::startRotation(const QPointF &vector)
{
    _rotationStart = angle(vector) * 180 / M_PI - rotation();
}

void PuzzlePiece::handleRotation(const QPointF &v)
{
    if (!_isEnabled)
        return;

    qreal a = angle(v) * 180 / M_PI - _rotationStart;
    setRotation(simplifyAngle(a));
}

void PuzzlePiece::checkMergeableSiblings()
{
    foreach (PuzzlePiece *p, neighbours())
        if (checkMergeability(p) || p->checkMergeability(this))
            mergeIfPossible(p);
}

bool PuzzlePiece::checkMergeability(PuzzlePiece *p)
{
    PuzzleBoard *board = static_cast<PuzzleBoard*>(parent());
    qreal rotationDiff = abs(simplifyAngle(p->rotation() - rotation()));

    // Check rotation difference
    if (rotationDiff > board->rotationTolerance())
        return false;

    QPointF diff = (this->supposedPosition() - p->supposedPosition()) - this->mapToItem(p, QPointF(0, 0));
    qreal distance = sqrt(diff.x() * diff.x() + diff.y() * diff.y());
    return distance < board->tolerance();
}

void PuzzlePiece::setTransformOriginPoint(const QPointF &point)
{
    if (transformOriginPoint() != point)
    {
        QPointF compensation = mapToParent(QPointF(0, 0));
        _transformOriginPoint = point;
        compensation -= mapToParent(QPointF(0, 0));
        setPos(pos() + compensation);
        _dragStart -= compensation;
    }
}

void PuzzlePiece::verifyPosition()
{
    // If the current PuzzlePiece doesn't contain any primitives, return

    if (!_primitives.count())
        return;

    // Find out the coordinates in the parent's coordinate system

    PuzzleBoard *board = static_cast<PuzzleBoard*>(parent());
    QPointF p1 = mapToParent(QPointF(_topLeft.x(),     _topLeft.y())), // top left point (in piece coordinates)
            p2 = mapToParent(QPointF(_bottomRight.x(), _topLeft.y())), // top right point (in piece coordinates)
            p3 = mapToParent(QPointF(_topLeft.x(),     _bottomRight.y())), // bottom left point (in piece coordinates)
            p4 = mapToParent(QPointF(_bottomRight.x(), _bottomRight.y())), // bottom right point (in piece coordinates)
            // top left of "bounding rect" (in parent coordinates)
            p(myMin<qreal>(myMin<qreal>(p1.x(), p2.x()), myMin<qreal>(p3.x(), p4.x())), myMin<qreal>(myMin<qreal>(p1.y(), p2.y()), myMin<qreal>(p3.y(), p4.y()))),
            // bottom right of "bounding rect" (in parent coordinates)
            q(myMax<qreal>(myMax<qreal>(p1.x(), p2.x()), myMax<qreal>(p3.x(), p4.x())), myMax<qreal>(myMax<qreal>(p1.y(), p2.y()), myMax<qreal>(p3.y(), p4.y())));

    qreal   w = q.x() - p.x(),
            h = q.y() - p.y(),
            maxX = board->width() - w / 2,
            minX = - w / 2,
            maxY = board->height() - h / 2,
            minY = - h / 2;

    if (p.x() > maxX || p.x() < minX || p.y() > maxY || p.y() < minY)
    {
        QPointF newPos = QPointF(CLAMP(p.x(), minX + 40, maxX - 40), CLAMP(p.y(), minY + 40, maxY - 40)) + pos() - p;
        QPropertyAnimation *anim = new QPropertyAnimation(this, "pos", this);

        anim->setEndValue(newPos);
        anim->setDuration(150);
        anim->setEasingCurve(QEasingCurve(QEasingCurve::OutBounce));

        board->enableAutoRepaint();
        this->disable();
        connect(anim, SIGNAL(finished()), board, SLOT(disableAutoRepaint()));
        connect(anim, SIGNAL(finished()), this, SLOT(enable()));
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void PuzzlePiece::raise()
{
    PuzzlePiece *maxItem = this;
    foreach (PuzzlePiece *item, static_cast<PuzzleBoard*>(parent())->puzzleItems())
        if (item->zValue() > maxItem->zValue())
            maxItem = item;

    if (maxItem == this)
        return;

    qreal max = maxItem->zValue();
    foreach (PuzzlePiece *item, static_cast<PuzzleBoard*>(parent())->puzzleItems())
        if (item->zValue() > this->zValue())
            item->setZValue(item->zValue() - 1);

    setZValue(max);
}

void PuzzlePiece::addPrimitive(PuzzlePiecePrimitive *p, const QPointF &corr)
{
    if (_primitives.contains(p))
        return;

    p->setParent(this);
    p->setPixmapOffset(p->pixmapOffset() + corr);
    p->setStrokeOffset(p->strokeOffset() + corr);
    _primitives.insert(p);

    // Find the topleft and bottomright points of this PuzzlePiece
    // according to the PuzzlePiecePrimitive instances it contains

    PuzzlePiecePrimitive *ppp = *(_primitives.begin());
    qreal x1, y1, x2, y2;
    x1 = ppp->pixmapOffset().x();
    y1 = ppp->pixmapOffset().y();
    x2 = ppp->pixmapOffset().x() + ppp->pixmap().width();
    y2 = ppp->pixmapOffset().y() + ppp->pixmap().height();

    foreach (const PuzzlePiecePrimitive* pp, _primitives)
    {
        // Find the top-left point of this puzzle piece
        if (pp->pixmapOffset().x() < x1)
            x1 = pp->pixmapOffset().x();
        if (pp->pixmapOffset().y() < y1)
            y1 = pp->pixmapOffset().y();
        // Find the bottom-right point of this puzzle piece
        if (pp->pixmapOffset().x() + pp->pixmap().width() > x2)
            x2 = pp->pixmapOffset().x() + pp->pixmap().width();
        if (pp->pixmapOffset().x() + pp->pixmap().height() > y2)
            y2 = pp->pixmapOffset().y() + pp->pixmap().height();
    }

    _topLeft = QPointF(x1, y1);
    _bottomRight = QPointF(x2, y2);
}

QPointF PuzzlePiece::mapToParent(const QPointF &p0) const
{
    qreal a = _rotation * M_PI / 180;
    QPointF p = p0 - _transformOriginPoint;
    QPointF r(p.x() * cos(-a) + p.y() * sin(-a), - p.x() * sin(-a) + p.y() * cos(-a));
    return _pos + r + _transformOriginPoint;
}

QPointF PuzzlePiece::mapFromParent(const QPointF &p0) const
{
    qreal a = _rotation * M_PI / 180;
    QPointF p = p0 - mapToParent(_transformOriginPoint);
    QPointF r(p.x() * cos(a) + p.y() * sin(a), - p.x() * sin(a) + p.y() * cos(a));
    return r + _transformOriginPoint;
}

QPointF PuzzlePiece::mapToItem(const PuzzlePiece *item, const QPointF &p) const
{
    return item->mapFromParent(this->mapToParent(p));
}
