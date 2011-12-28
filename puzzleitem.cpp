
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

#include <QPixmap>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QTouchEvent>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QElapsedTimer>
#include <cmath>

#include "puzzleitem.h"

inline static qreal angle(const QPointF &v)
{
    if (v.x() >= 0)
        return atan(v.y() / v.x());

    return atan(v.y() / v.x()) - M_PI;
}

inline static qreal angle(const QPointF &v1, const QPointF &v2)
{
    return angle(v2) - angle(v1);
}

inline static qreal simplifyAngle(qreal a)
{
    while (a >= 360)
        a -= 360;
    while (a < 0)
        a += 360;

    return a;
}

inline static QPointF findMidpoint(QTouchEvent *touchEvent)
{
    QPointF midpoint;

    // Finding the midpoint
    foreach (const QTouchEvent::TouchPoint &touchPoint, touchEvent->touchPoints())
        midpoint += touchPoint.pos();

    midpoint /= touchEvent->touchPoints().count();
    return midpoint;
}

PuzzleItem::PuzzleItem(const QPixmap &pixmap, PuzzleBoard *parent)
    : QDeclarativeItem(parent),
      _canMerge(false),
      _weight(randomInt(100, 950) / 1000.0),
      _dragging(false),
      _isDraggingWithTouch(false),
      _isRightButtonPressed(false),
      _previousRotationValue(0),
      _previousTouchPointCount(0)
{
    setPixmap(pixmap);
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    setFlag(QGraphicsItem::ItemStacksBehindParent, false);
    setFlag(QGraphicsItem::ItemNegativeZStacksBehindParent, false);
    setAcceptTouchEvents(true);
#if !defined(MEEGO_EDITION_HARMATTAN) && !defined(Q_OS_SYMBIAN)
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
#endif
}

QPointF PuzzleItem::centerPoint() const
{
    return QPointF(width() / 2.0, height() / 2.0);
}

void PuzzleItem::addNeighbour(PuzzleItem *piece)
{
    if (piece != this)
    {
        if (!_neighbours.contains(piece))
            _neighbours.append(piece);
        if (!piece->_neighbours.contains(this))
            piece->_neighbours.append(this);
    }
}

void PuzzleItem::removeNeighbour(PuzzleItem *piece)
{
    if (piece != this)
    {
        if (_neighbours.contains(piece))
            _neighbours.removeAll(piece);
        if (piece->_neighbours.contains(this))
            piece->_neighbours.removeAll(this);
    }
}

bool PuzzleItem::isNeighbourOf(const PuzzleItem *piece) const
{
    if (piece->neighbours().contains((PuzzleItem*)this) && this->neighbours().contains((PuzzleItem*)piece))
        return true;
    return false;
}

void PuzzleItem::mergeIfPossible(PuzzleItem *item, const QPointF &dragPosition)
{
    if (isNeighbourOf(item) && _canMerge && item->_canMerge)
    {
        item->_canMerge = _canMerge = false;

        foreach (PuzzleItem *n, item->neighbours())
        {
            item->removeNeighbour(n);
            this->addNeighbour(n);
        }

        QPointF positionVector = item->supposedPosition() - supposedPosition();
        QPointF old00 = mapToParent(0, 0);

        int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
        if (positionVector.x() >= 0)
            x2 = positionVector.x();
        else
            x1 = - positionVector.x();
        if (positionVector.y() >= 0)
            y2 = positionVector.y();
        else
            y1 = - positionVector.y();

        QPixmap pix(max<int>(x1 + pixmap().width(), x2 + item->pixmap().width()),
                    max<int>(y1 + pixmap().height(), y2 + item->pixmap().height()));
        pix.fill(Qt::transparent);

        QPainter p;
        p.begin(&pix);
        p.drawPixmap(x1, y1, pixmap());
        p.drawPixmap(x2, y2, item->pixmap());
        p.end();

        int newStatus = 0;
        if ((item->_supposedPosition.x() <= _supposedPosition.x() && (item->_tabStatus & PuzzleItem::LeftTab))
                || (_supposedPosition.x() <= item->_supposedPosition.x() && (_tabStatus & PuzzleItem::LeftTab)))
            newStatus |= PuzzleItem::LeftTab;
        if ((item->_supposedPosition.y() <= _supposedPosition.y() && (item->_tabStatus & PuzzleItem::TopTab))
                || (_supposedPosition.y() <= item->_supposedPosition.y() && (_tabStatus & PuzzleItem::TopTab)))
            newStatus |= PuzzleItem::TopTab;
        if ((item->_supposedPosition.x() + item->_pixmap.width() >= _supposedPosition.x() + _pixmap.width() && (item->_tabStatus & PuzzleItem::RightTab))
                || (_supposedPosition.x() + _pixmap.width() >= item->_supposedPosition.x() + item->_pixmap.width() && (_tabStatus & PuzzleItem::RightTab)))
            newStatus |= PuzzleItem::RightTab;
        if ((item->_supposedPosition.y() + item->_pixmap.height() >= _supposedPosition.y() + _pixmap.height() && (item->_tabStatus & PuzzleItem::BottomTab))
                || (_supposedPosition.y() + _pixmap.height() >= item->_supposedPosition.y() + item->_pixmap.height() && (_tabStatus & PuzzleItem::BottomTab)))
            newStatus |= PuzzleItem::BottomTab;

        setTabStatus(newStatus);
        setPuzzleCoordinates(QPoint(min<int>(item->puzzleCoordinates().x(), puzzleCoordinates().x()), min<int>(item->puzzleCoordinates().y(), puzzleCoordinates().y())));
        setSupposedPosition(QPointF(min<qreal>(item->supposedPosition().x(), supposedPosition().x()), min<qreal>(item->supposedPosition().y(), supposedPosition().y())));
        setStroke(_stroke.translated(x1, y1).united(item->_stroke.translated(x2, y2)).simplified());
        setFakeShape(_fakeShape.translated(x1, y1).united(item->_fakeShape.translated(x2, y2)).simplified());
        setPixmap(pix);
        setWidth(_pixmap.width());
        setHeight(_pixmap.height());
        setPos(pos() + old00 - mapToParent(x1, y1));
        _dragStart = mapToParent(dragPosition + QPointF(x1, y1)) - pos();
        static_cast<PuzzleBoard*>(parent())->removePuzzleItem(item);

        if (neighbours().count() == 0)
        {
            _dragging = _isDraggingWithTouch = _canMerge = false;
            emit noNeighbours();
        }
        else
            _canMerge = true;
    }
}

void PuzzleItem::startDrag(const QPointF &p)
{
    if (_canMerge)
    {
        _dragging = true;
        _dragStart = mapToParent(p) - pos();
        raise();
    }
    else
    {
        stopDrag();
    }
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
    _previousRotationValue = rotation();
    _rotationStartVector = vector;
}

void PuzzleItem::handleRotation(const QPointF &v)
{
    qreal a = angle(_rotationStartVector, v) * 180 / M_PI;

    if (!isnan(a))
        setRotation(simplifyAngle(a + _previousRotationValue));
}

void PuzzleItem::checkMergeableSiblings(const QPointF &position)
{
    if (_canMerge)
    {
        foreach (PuzzleItem *p, neighbours())
        {
            if (checkMergeability(p) || p->checkMergeability(this))
            {
                mergeIfPossible(p, position);
            }
        }
    }
}

bool PuzzleItem::checkMergeability(PuzzleItem *p)
{
    PuzzleBoard *board = static_cast<PuzzleBoard*>(parent());
    qreal rotationDiff = abs(simplifyAngle(p->rotation() - rotation())), px = 0, py = 0;

    // Horizontal
    if (p->_puzzleCoordinates.x() > _puzzleCoordinates.x())
        px += p->_pixmapOffset.x() + p->leftTabSize();
    else if (p->_puzzleCoordinates.x() < _puzzleCoordinates.x())
        px += p->_pixmapOffset.x() + p->_pixmap.width() - p->rightTabSize();
    else
        px += p->_pixmapOffset.x() + min<int>(_pixmap.width() - leftTabSize() - rightTabSize(), p->_pixmap.width() - p->leftTabSize() - p->rightTabSize()) / 2 + p->leftTabSize();

    // Vertical
    if (p->puzzleCoordinates().y() > _puzzleCoordinates.y())
        py += p->_pixmapOffset.y() + p->topTabSize();
    else if (p->puzzleCoordinates().y() < _puzzleCoordinates.y())
        py += p->_pixmapOffset.y() + p->_pixmap.height() - p->bottomTabSize();
    else
        py += p->_pixmapOffset.y() + min<int>(_pixmap.height() - topTabSize() - bottomTabSize(), p->_pixmap.height() - p->topTabSize() - p->bottomTabSize()) / 2 + p->topTabSize();

    QPointF diff = - _supposedPosition + p->_supposedPosition + QPointF(px, py) - static_cast<QGraphicsItem*>(p)->mapToItem(this, px, py);
    qreal distance = sqrt(diff.x() * diff.x() + diff.y() * diff.y());

    return distance < board->tolerance() && rotationDiff < board->rotationTolerance();
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

void PuzzleItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QDeclarativeItem::mousePressEvent(event);
    event->accept();

    if (!_isDraggingWithTouch)
    {
        if (event->button() == Qt::LeftButton)
        {
            startDrag(event->pos());
        }
        else if (event->button() == Qt::RightButton)
        {
            _isRightButtonPressed = true;
            setCompensatedTransformOriginPoint(centerPoint());
            startRotation(mapToParent(event->pos()) - mapToParent(centerPoint()));
        }
    }
}

void PuzzleItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QDeclarativeItem::mouseReleaseEvent(event);
    event->accept();

    if (event->button() == Qt::LeftButton)
    {
        stopDrag();
    }
    else if (event->button() == Qt::RightButton)
    {
        _isRightButtonPressed = false;

        if (_dragging)
            _dragStart = mapToParent(event->pos()) - pos();
    }
}

void PuzzleItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QDeclarativeItem::mouseMoveEvent(event);
    event->accept();

    if (_isRightButtonPressed)
        handleRotation(mapToParent(event->pos()) - mapToParent(centerPoint()));
    else if (!_isDraggingWithTouch)
        doDrag(event->pos());

    checkMergeableSiblings(event->pos());
}

bool PuzzleItem::sceneEvent(QEvent *event)
{
    if (QDeclarativeItem::sceneEvent(event))
        return true;

    if (!_canMerge)
        return false;

    QTouchEvent *touchEvent = 0;

    if (event->type() == QEvent::TouchBegin || event->type() == QEvent::TouchUpdate || event->type() == QEvent::TouchEnd)
    {
        touchEvent = static_cast<QTouchEvent*>(event);
        event->accept();
    }

    if (event->type() == QEvent::TouchBegin)
    {
        // Touch began, there may be any number of touch points now
        _isDraggingWithTouch = true;
        QPointF midpoint = findMidpoint(touchEvent);
        startDrag(midpoint);

        if (touchEvent->touchPoints().count() >= 2)
            startRotation(mapToParent(touchEvent->touchPoints().at(0).pos()) - mapToParent(touchEvent->touchPoints().at(1).pos()));

        _previousTouchPointCount = touchEvent->touchPoints().count();
        return true;
    }
    else if (event->type() == QEvent::TouchEnd)
    {
        // Touch ended
        stopDrag();
        return true;
    }
    else if (event->type() == QEvent::TouchUpdate)
    {
        QPointF midpoint = findMidpoint(touchEvent);
        setCompensatedTransformOriginPoint(midpoint);

        // If you put one more finger onto an item, this prevents it from jumping
        if (touchEvent->touchPoints().count() != _previousTouchPointCount)
            _dragStart = mapToParent(midpoint) - pos();
        else
            doDrag(midpoint);

        if (_previousTouchPointCount < 2 && touchEvent->touchPoints().count() >= 2)
            startRotation(mapToParent(touchEvent->touchPoints().at(0).pos()) - mapToParent(touchEvent->touchPoints().at(1).pos()));

        // Handling multitouch rotation
        if (touchEvent->touchPoints().count() >= 2)
            handleRotation(mapToParent(touchEvent->touchPoints().at(0).pos()) - mapToParent(touchEvent->touchPoints().at(1).pos()));

        checkMergeableSiblings(midpoint);
        _previousTouchPointCount = touchEvent->touchPoints().count();
        return true;
    }

    return false;
}

void PuzzleItem::verifyPosition()
{
    PuzzleBoard *board = static_cast<PuzzleBoard*>(parent());
    qreal a = rotation();
    QPointF p1 = mapToScene(0, 0),
            p2 = mapToScene(width(), 0),
            p3 = mapToScene(0, height()),
            p4 = mapToScene(width(), height()),
            p(min<qreal>(min<qreal>(p1.x(), p2.x()), min<qreal>(p3.x(), p4.x())), min<qreal>(min<qreal>(p1.y(), p2.y()), min<qreal>(p3.y(), p4.y())));

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
    foreach (QGraphicsItem *item, ((QDeclarativeItem*)parent())->childItems())
        if (item != this)
            item->stackBefore(this);
}

void PuzzleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    // Uncomment for or various debugging purposes
    //painter->drawRect(boundingRect());
    //painter->drawEllipse(mapFromScene(pos()), 10, 10);
    //painter->fillPath(_fakeShape, QBrush(QColor(0, 0, 255, 130)));
    painter->fillPath(_stroke, QBrush(QColor(75, 75, 75, 255)));
    painter->drawPixmap(_pixmapOffset, _pixmap);
}

QPainterPath PuzzleItem::shape() const
{
    return _fakeShape;
}
