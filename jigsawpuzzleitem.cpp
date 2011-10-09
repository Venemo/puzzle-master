#include <QtGui>
#include "jigsawpuzzleitem.h"
#include "jigsawpuzzleboard.h"
#include "util.h"

JigsawPuzzleItem::JigsawPuzzleItem(const QPixmap &pixmap, QGraphicsItem *parent, QGraphicsScene *scene) :
    QObject(scene),
    PuzzleItem(pixmap, parent, scene),
    _dragging(false),
    _canMerge(false),
    _weight(randomInt(100, 950) / 1000.0)
{
    setAcceptTouchEvents(true);
}

bool JigsawPuzzleItem::canMerge() const
{
    return _canMerge;
}

void JigsawPuzzleItem::setMerge(bool canMerge)
{
    _canMerge = canMerge;
}

void JigsawPuzzleItem::enableMerge()
{
    setMerge(true);
}

void JigsawPuzzleItem::disableMerge()
{
    setMerge(false);
}

double JigsawPuzzleItem::weight()
{
    return _weight;
}

bool JigsawPuzzleItem::merge(JigsawPuzzleItem *item)
{
    if (isNeighbourOf(item) && _canMerge && item->_canMerge)
    {
        qDebug() << "merging" << puzzleCoordinates() << "and" << item->puzzleCoordinates();
        item->_canMerge = _canMerge = false;

        foreach (PuzzleItem *n, item->neighbours())
        {
            item->removeNeighbour(n);
            this->addNeighbour(n);
        }

        QPoint vector = item->puzzleCoordinates() - puzzleCoordinates();
        int x1, x2, y1, y2, u1, v1;
        if (vector.x() >= 0)
        {
            x1 = 0;
            u1 = 0;
            x2 = vector.x() * unit().width();
        }
        else
        {
            x1 = - vector.x() * unit().width();
            u1 = - vector.x();
            x2 = 0;
        }
        if (vector.y() >= 0)
        {
            y1 = 0;
            v1 = 0;
            y2 = vector.y() * unit().height();
        }
        else
        {
            y1 = - vector.y() * unit().height();
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

        setPixmap(pix);
        setPuzzleCoordinates(puzzleCoordinates() - QPoint(u1, v1));
        setPos(pos().x() - x1, pos().y() - y1);

        _dragStart += QPointF(x1, y1);
        item->hide();
        item->deleteLater();
        _canMerge = true;

        if (neighbours().count() == 0)
        {
            _dragging = false;
            _canMerge = false;
            QPointF newPos((scene()->width() - pixmap().width()) / 2, (scene()->height() - pixmap().height()) / 2);
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
            QPropertyAnimation *anim = new QPropertyAnimation(this, "pos", this);
            anim->setEndValue(newPos);
            anim->setDuration(1000);
            anim->setEasingCurve(QEasingCurve(QEasingCurve::OutElastic));
            connect(anim, SIGNAL(finished()), this, SIGNAL(noNeighbours()));
            anim->start(QAbstractAnimation::DeleteWhenStopped);
#else
            setPos(newPos);
            emit noNeighbours();
#endif
        }

        return true;
    }
    return false;
}

void JigsawPuzzleItem::startDrag(QPointF pos)
{
    if (_canMerge && !_dragging)
    {
        _dragging = true;
        _dragStart = pos;
        raise();
    }
}

void JigsawPuzzleItem::stopDrag()
{
    _dragging = false;
    verifyPosition();
    verifyCoveredSiblings();
}

void JigsawPuzzleItem::doDrag(QPointF position)
{
    if (_dragging)
    {
        setPos(pos() + position - _dragStart);

        if (_canMerge)
        {
            foreach (PuzzleItem *p, neighbours())
            {
                JigsawPuzzleItem *w = (JigsawPuzzleItem*) p;
                QPointF posDiff1 = pos() - w->pos();
                QPointF posDiff2 = (puzzleCoordinates() * unit() - w->puzzleCoordinates() * unit());
                QPointF relative = posDiff1 - posDiff2;

                if (abs((int)relative.x()) < tolerance() && abs((int)relative.y()) < tolerance())
                {
                    merge(w);
                }
            }
        }
    }
}

void JigsawPuzzleItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    PuzzleItem::mousePressEvent(event);

    if (event->button() == Qt::LeftButton)
        startDrag(event->pos());
}

void JigsawPuzzleItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    PuzzleItem::mouseReleaseEvent(event);

    if (event->button() == Qt::LeftButton)
        stopDrag();
}

void JigsawPuzzleItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    PuzzleItem::mouseMoveEvent(event);
    doDrag(event->pos());
}

bool JigsawPuzzleItem::sceneEvent(QEvent *event)
{
    if (PuzzleItem::sceneEvent(event))
        return true;

    //qDebug() << "scene event..." << event->type();

    if (event->type() == QEvent::TouchBegin)
    {
        QTouchEvent *touchEvent = (QTouchEvent*) event;
        qDebug() << "touch begin received at" << puzzleCoordinates();

//        // We don't handle more than one touch point per piece yet
//        if (touchEvent->touchPoints().count() > 1)// || touchEvent->touchPoints().at(0).isPrimary())
//        {
//            event->ignore();
//            return false;
//        }

        startDrag(touchEvent->touchPoints().at(0).pos());

        event->accept();
        return true;
    }
    else if (event->type() == QEvent::TouchEnd)
    {
        QTouchEvent *touchEvent = (QTouchEvent*) event;

        // If there are no more points, we stop dragging
        if (touchEvent->touchPoints().count() == 0)
        {
            stopDrag();
        }

        event->accept();
        return true;
    }
    else if (event->type() == QEvent::TouchUpdate)
    {
        QTouchEvent *touchEvent = (QTouchEvent*) event;

        // We don't handle more than one touch point per piece yet
        if (touchEvent->touchPoints().count() > 1)// || touchEvent->touchPoints().at(0).isPrimary())
        {
            event->ignore();
            return false;
        }

        doDrag(touchEvent->touchPoints().at(0).pos());

        event->accept();
        return true;
    }

    return false;
}

void JigsawPuzzleItem::verifyPosition()
{
    int x = (int)pos().x();
    int maxX = (int)scene()->width() - (pixmap().width() / 2);
    int minX = - pixmap().width() + (pixmap().width() / 2);

    int y = (int)pos().y();
    int maxY = (int)scene()->height() - (pixmap().height()  / 2);
    int minY = - pixmap().height() + (pixmap().height() / 2);

    if (!(x < maxX && x > (minX) && y < maxY && y > (minY)))
    {
        int pX = CLAMP(x, minX + unit().width() / 2, maxX - unit().width() / 2);
        int pY = CLAMP(y, minY + unit().height() / 2, maxY - unit().height() / 2);
        _dragging = false;
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
        QPropertyAnimation *anim = new QPropertyAnimation(this, "pos", this);
        anim->setEndValue(QPointF(pX, pY));
        anim->setDuration(200);
        anim->setEasingCurve(QEasingCurve(QEasingCurve::OutBounce));
        if (canMerge())
        {
            disableMerge();
            connect(anim, SIGNAL(finished()), this, SLOT(enableMerge()));
        }
        anim->start(QAbstractAnimation::DeleteWhenStopped);
#else
        setPos(pX, pY);
#endif
    }
}

void JigsawPuzzleItem::raise()
{
    QGraphicsItem *maxItem = this;
    foreach (QGraphicsItem *item, scene()->items())
    {
        if (item->zValue() > maxItem->zValue())
        {
            maxItem = item;
        }
    }
    if (maxItem != this)
    {
        qreal max = maxItem->zValue();
        foreach (QGraphicsItem *item, scene()->items())
        {
            if (item->zValue() > this->zValue())
            {
                item->setZValue(item->zValue() - 1);
            }
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
            else if (item != this && item->zValue() == this->zValue())
            {
                item->stackBefore(this);
            }
#endif
        }
        setZValue(max);
    }
}

void JigsawPuzzleItem::verifyCoveredSiblings()
{
    foreach (QGraphicsItem *gi, scene()->items())
    {
        JigsawPuzzleItem *item = (JigsawPuzzleItem*)gi;
        if (item != this &&
                item->zValue() < zValue() &&
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
