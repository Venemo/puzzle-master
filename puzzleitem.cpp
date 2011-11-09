
#include <QtGui>
#include <QtDeclarative>
#include "puzzleitem.h"

extern QPainterPath qt_regionToPath(const QRegion &region);

PuzzleItem::PuzzleItem(const QPixmap &pixmap, QDeclarativeItem *parent)
    : QDeclarativeItem(parent),
      _canMerge(false),
      _weight(randomInt(100, 950) / 1000.0),
      _dragging(false),
      _isDraggingWithTouch(false),
      _previousRotationValue(0),
      _previousTouchPointCount(0)
{
    setPixmap(pixmap);
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    setFlag(QGraphicsItem::ItemStacksBehindParent, false);
    setFlag(QGraphicsItem::ItemNegativeZStacksBehindParent, false);
    setFlag(QGraphicsItem::ItemClipsToShape, true);
    setAcceptTouchEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton);
    setTransformOriginPoint(pixmap.width() / 2, pixmap.height() / 2);
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

void PuzzleItem::emphasise()
{
    if (QGraphicsDropShadowEffect *effect = qobject_cast<QGraphicsDropShadowEffect*>(this->graphicsEffect()))
    {
        effect->setColor(DROPSHADOW_COLOR_SELECTED);
        effect->setBlurRadius(DROPSHADOW_RADIUS_SELECTED);
    }
}

void PuzzleItem::deEmphasise()
{
    if (QGraphicsDropShadowEffect *effect = qobject_cast<QGraphicsDropShadowEffect*>(this->graphicsEffect()))
    {
        effect->setColor(DROPSHADOW_COLOR_DEFAULT);
        effect->setBlurRadius(DROPSHADOW_RADIUS_DEFAULT);
    }
}

void PuzzleItem::enableMerge()
{
    setCanMerge(true);
}

void PuzzleItem::disableMerge()
{
    setCanMerge(false);
}

bool PuzzleItem::merge(PuzzleItem *item, const QPointF &dragPosition)
{
    if (isNeighbourOf(item) && _canMerge && item->_canMerge)
    {
        item->_canMerge = _canMerge = false;

        foreach (PuzzleItem *n, item->neighbours())
        {
            item->removeNeighbour(n);
            this->addNeighbour(n);
        }

        QPoint vector = item->puzzleCoordinates() - puzzleCoordinates();
        QPointF positionVector = item->supposedPosition() - supposedPosition();
        QPointF old00 = mapToParent(0, 0), oldPos = pos();

        int x1, x2, y1, y2, u1, v1;
        if (vector.x() >= 0)
        {
            x1 = 0;
            u1 = 0;
            x2 = positionVector.x();
        }
        else
        {
            x1 = - positionVector.x();
            u1 = - vector.x();
            x2 = 0;
        }
        if (vector.y() >= 0)
        {
            y1 = 0;
            v1 = 0;
            y2 = positionVector.y();
        }
        else
        {
            y1 = - positionVector.y();
            v1 = - vector.y();
            y2 = 0;
        }

        QPixmap pix(max(x1 + pixmap().width(), x2 + item->pixmap().width()),
                    max(y1 + pixmap().height(), y2 + item->pixmap().height()));
        pix.fill(Qt::transparent);

        QPainter p;
        p.begin(&pix);
        p.drawPixmap(x1, y1, pixmap());
        p.drawPixmap(x2, y2, item->pixmap());
        p.end();

        _puzzleCoordinates -= QPoint(u1, v1);
        _supposedPosition = QPointF(min<qreal>(item->supposedPosition().x(), supposedPosition().x()), min<qreal>(item->supposedPosition().y(), supposedPosition().y()));
        setPixmap(pix);
        setShape(_shape.translated(x1, y1).united(item->shape().translated(x2, y2)));
        setWidth(_pixmap.width());
        setHeight(_pixmap.height());
        setPos(pos() + old00 - mapToParent(x1, y1));
        _dragStart = mapToParent(dragPosition + QPointF(x1, y1)) - pos();

        item->hide();
        item->deleteLater();
        _canMerge = true;

        if (neighbours().count() == 0)
        {
            _dragging = false;
            _canMerge = false;
            QPointF newPos((((QDeclarativeItem*)parent())->width() - pixmap().width()) / 2, (((QDeclarativeItem*)parent())->height() - pixmap().height()) / 2);

            QParallelAnimationGroup *group = new QParallelAnimationGroup(this);
            QPropertyAnimation *anim = new QPropertyAnimation(this, "pos", group);
            anim->setEndValue(newPos);
            anim->setDuration(1000);
            anim->setEasingCurve(QEasingCurve(QEasingCurve::OutElastic));

            QPropertyAnimation *rotateAnimation = new QPropertyAnimation(this, "rotation", group);
            rotateAnimation->setEndValue(0);
            rotateAnimation->setDuration(1000);
            rotateAnimation->setEasingCurve(QEasingCurve(QEasingCurve::OutElastic));

            connect(group, SIGNAL(finished()), this, SIGNAL(noNeighbours()));

            group->addAnimation(anim);
            group->addAnimation(rotateAnimation);
            group->start(QAbstractAnimation::DeleteWhenStopped);
        }

        return true;
    }
    return false;
}

void PuzzleItem::startDrag(const QPointF &p)
{
    if (_canMerge && !_dragging)
    {
        _dragging = true;
        _dragStart = mapToParent(p) - pos();
        raise();
        emphasise();
    }
}

void PuzzleItem::stopDrag()
{
    _dragging = false;
    _isDraggingWithTouch = false;
    //verifyPosition();
    //verifyCoveredSiblings();
    deEmphasise();
}

void PuzzleItem::doDrag(const QPointF &position)
{
    if (_dragging)
    {
        setPos(mapToParent(position) - _dragStart);

        if (_canMerge)
        {
            foreach (PuzzleItem *p, neighbours())
            {
                QPointF diff = - _supposedPosition + p->_supposedPosition - ((QGraphicsItem*)p)->mapToItem(this, 0, 0);

                if (abs((int)diff.x()) < tolerance() && abs((int)diff.y()) < tolerance() && abs(p->rotation() - rotation()) < rotationTolerance())
                {
                    merge(p, position);
                }
            }
        }
    }
}

void PuzzleItem::handleRotation(const QPointF &v)
{
    qreal a = angle(_rotationStartVector, v) * 180 / M_PI;

    if (!isnan(a))
    {
        a += _previousRotationValue;

        while (a >= 360)
            a -= 360;
        while (a <= 360)
            a += 360;

        setRotation(a);
    }
}

void PuzzleItem::setCompensatedTransformOriginPoint(const QPointF &point)
{
    QPointF compensation = mapToParent(0, 0);
    setTransformOriginPoint(point);
    compensation -= mapToParent(0, 0);
    setPos(pos() + compensation);
    _dragStart -= compensation;
}

void PuzzleItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QDeclarativeItem::mousePressEvent(event);
    event->accept();

    if (event->button() == Qt::LeftButton)
        startDrag(event->pos());
}

void PuzzleItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QDeclarativeItem::mouseReleaseEvent(event);
    event->accept();

    if (event->button() == Qt::LeftButton)
        stopDrag();
}

void PuzzleItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QDeclarativeItem::mouseMoveEvent(event);
    event->accept();
    if (!_isDraggingWithTouch)
        doDrag(event->pos());
}

bool PuzzleItem::sceneEvent(QEvent *event)
{
    if (QDeclarativeItem::sceneEvent(event))
        return true;

    if (!_canMerge)
        return false;

    QTouchEvent *touchEvent = (QTouchEvent*) event;

    if (event->type() == QEvent::TouchBegin)
    {
        // There is exactly one touch point now
        _dragging = false;
        _isDraggingWithTouch = true;
        startDrag(touchEvent->touchPoints().at(0).pos());

        event->accept();
        return true;
    }
    else if (event->type() == QEvent::TouchEnd)
    {
        // There is only one touch point which is now released
        stopDrag();
        event->accept();
        return true;
    }
    else if (event->type() == QEvent::TouchUpdate)
    {
        QPointF midpoint(0, 0);

        // Finding the midpoint
        foreach (const QTouchEvent::TouchPoint &touchPoint, touchEvent->touchPoints())
            midpoint += touchPoint.pos();

        midpoint /= touchEvent->touchPoints().count();
        setCompensatedTransformOriginPoint(midpoint);

        if (touchEvent->touchPoints().count() != _previousTouchPointCount)
        {
            // If you put one more finger onto an item, this prevents it from jumping
            _dragStart = mapToParent(midpoint) - pos();
        }
        else
        {
            doDrag(midpoint);
        }

        if (_previousTouchPointCount < 2 && touchEvent->touchPoints().count() == 2)
        {
            // Starting rotation
            _previousRotationValue = rotation();
            _rotationStartVector = mapToParent(touchEvent->touchPoints().at(0).pos()) - mapToParent(touchEvent->touchPoints().at(1).pos());
        }

        _previousTouchPointCount = touchEvent->touchPoints().count();

        if (touchEvent->touchPoints().count() >= 2)
        {
            handleRotation(mapToParent(touchEvent->touchPoints().at(0).pos()) - mapToParent(touchEvent->touchPoints().at(1).pos()));
        }

        event->accept();
        return true;
    }

    return false;
}

void PuzzleItem::verifyPosition()
{
    QPointF p = mapToParent(0, 0);

    int x = (int)p.x();
    int maxX = (int)((QDeclarativeItem*)parent())->width() - (pixmap().width() / 2);
    int minX = - pixmap().width() + (pixmap().width() / 2);

    int y = (int)p.y();
    int maxY = (int)((QDeclarativeItem*)parent())->height() - (pixmap().height()  / 2);
    int minY = - pixmap().height() + (pixmap().height() / 2);

    if (!(x < maxX && x > (minX) && y < maxY && y > (minY)))
    {
        int pX = CLAMP(x, minX + unit().width() / 2, maxX - unit().width() / 2);
        int pY = CLAMP(y, minY + unit().height() / 2, maxY - unit().height() / 2);

        _dragging = false;
        _isDraggingWithTouch = false;
        _canMerge = false;

        QPropertyAnimation *anim = new QPropertyAnimation(this, "pos", this);
        connect(anim, SIGNAL(finished()), this, SLOT(enableMerge()));

        anim->setEndValue(QPointF(pX, pY));
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

void PuzzleItem::verifyCoveredSiblings()
{
    foreach (QGraphicsItem *gi, ((QDeclarativeItem*)parent())->childItems())
    {
        PuzzleItem *item = (PuzzleItem*)gi;
        if (item != this &&
                item->pos().x() >= pos().x() &&
                item->pos().y() >= pos().y() &&
                item->pos().x() + item->pixmap().width() < pos().x() + pixmap().width() &&
                item->pos().y() + item->pixmap().height() < pos().y() + pixmap().height() &&
                item->pixmap().width() < pixmap().width() &&
                item->pixmap().height() < pixmap().height())
        {
            item->raise();
            break;
        }
    }
}

void PuzzleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
#if QT_VERSION < QT_VERSION_CHECK(4, 8, 0)
    // Hack against the rendering artifacts when an L-shaped item is rotated.
    QDeclarativeItem::paint(painter, option, widget);
    painter->drawPixmap(0, 0, _pixmap);
#else
    QDeclarativeItem::paint(painter, option, widget);
    painter->drawPixmap(0, 0, _pixmap);
#endif
}

QPainterPath PuzzleItem::shape() const
{
    return _shape;
}
