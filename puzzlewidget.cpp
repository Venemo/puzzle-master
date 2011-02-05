#include <QtGui>
#include "puzzlewidget.h"

QPointF operator*(const QPoint &point, const QSize &size)
{
    return QPointF(point.x() * size.width(), point.y() * size.height());
}

static int randomInt(int low, int high)
{
    // Random number between low and high
    return qrand() % ((high + 1) - low) + low;
}

static int max(int i, int j)
{
    return i > j? i : j;
}

PuzzleWidget::PuzzleWidget(const QPixmap &pixmap, const QSize &unitSize, QGraphicsItem *parent, QGraphicsScene *scene) :
    QObject(scene),
    QGraphicsPixmapItem(pixmap, parent, scene),
    PuzzlePiece(),
    _unit(unitSize),
    _dragging(false),
    _canMerge(false),
    _tolerance(5),
    _weight(randomInt(50, 950) / 1000.0)
{
}

bool PuzzleWidget::canMerge() const
{
    return _canMerge;
}

void PuzzleWidget::setMerge(bool canMerge)
{
    _canMerge = canMerge;
}

void PuzzleWidget::enableMerge()
{
    setMerge(true);
}

void PuzzleWidget::disableMerge()
{
    setMerge(false);
}

double PuzzleWidget::weight()
{
    return _weight;
}

bool PuzzleWidget::merge(PuzzlePiece *piece)
{
    PuzzleWidget *w = (PuzzleWidget*)piece;
    if (PuzzlePiece::merge(piece))
    {
        w->_canMerge = _canMerge = false;

        QPoint vector = w->position() - position();
        int x1, x2, y1, y2, u1, v1;
        if (vector.x() >= 0)
        {
            x1 = 0;
            u1 = 0;
            x2 = vector.x() * _unit.width();
        }
        else
        {
            x1 = - vector.x() * _unit.width();
            u1 = - vector.x();
            x2 = 0;
        }
        if (vector.y() >= 0)
        {
            y1 = 0;
            v1 = 0;
            y2 = vector.y() * _unit.height();
        }
        else
        {
            y1 = - vector.y() * _unit.height();
            v1 = - vector.y();
            y2 = 0;
        }

        QPixmap pix(max(x1 + pixmap().width(), x2 + w->pixmap().width()),
                    max(y1 + pixmap().height(), y2 + w->pixmap().height()));
        pix.fill(Qt::transparent);

        QPainter p;
        p.begin(&pix);
        p.setClipping(false);

        p.drawPixmap(x1, y1, pixmap());
        p.drawPixmap(x2, y2, w->pixmap());

        p.end();
        setPixmap(pix);
        setPosition(position() - QPoint(u1, v1));
        setPos(pos().x() - x1, pos().y() - y1);
        _dragStart = _dragStart + QPointF(x1, y1);
        w->hide();
        delete w;
        _canMerge = true;

        if (neighbours().count() == 0)
        {
            _dragging = false;
            _canMerge = false;
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
            QPropertyAnimation *anim = new QPropertyAnimation(this, "pos", this);
            anim->setEndValue(QPoint(0, 0));
            anim->setDuration(1000);
            connect(anim, SIGNAL(finished()), this, SIGNAL(noNeighbours()));
            anim->start(QAbstractAnimation::DeleteWhenStopped);
#else
            setPosition(0, 0);
            emit noNeighbours();
#endif
        }

        return true;
    }
    return false;
}

void PuzzleWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && _canMerge)
    {
        _dragging = true;
        _dragStart = event->pos();
        raise();
    }
}

void PuzzleWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        _dragging = false;
}

void PuzzleWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (_dragging)
    {
        setPos(pos() + event->pos() - _dragStart);

        if (_canMerge)
        {
            foreach (PuzzlePiece *p, neighbours())
            {
                PuzzleWidget *w = (PuzzleWidget*) p;
                QPointF posDiff1 = pos() - w->pos();
                QPointF posDiff2 = (position() * _unit - w->position() * _unit);
                QPointF relative = posDiff1 - posDiff2;

                if (abs(relative.x()) < _tolerance && abs(relative.y()) < _tolerance)
                {
                    merge(w);
                }
            }

            verifyPosition();
        }
    }
}

void PuzzleWidget::verifyPosition()
{
    int x = pos().x();// + position().x() * _unit.width();
    int maxX = scene()->width() - _unit.width() / 2;
    int minX = - pixmap().width() + _unit.width() / 2;

    int y = pos().y();// + position().y() * _unit.height();
    int maxY = scene()->height() - _unit.height() / 2;
    int minY = - pixmap().height() + _unit.height() / 2;

    if (!(x < maxX && x > (minX) && y < maxY && y > (minY)))
    {
        int pX = x >= maxX ? pos().x() - 50 : (x <= minX ? pos().x() + 50 : pos().x());
        int pY = y >= maxY ? pos().y() - 50 : (y <= minY ? pos().y() + 50 : pos().y());
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

void PuzzleWidget::shuffle(QList<PuzzleWidget *> *list, int x, int y, int width, int height)
{
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    QParallelAnimationGroup *group = new QParallelAnimationGroup();
#endif
    for (int i = 0; i < x * y; i++)
    {
        PuzzleWidget *widget = (PuzzleWidget*)list->operator [](i);
        QPoint newPos(randomInt(0, width), randomInt(0, height));

#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
        connect(group, SIGNAL(finished()), widget, SLOT(enableMerge()));
        QPropertyAnimation *anim = new QPropertyAnimation(widget, "pos", group);
        anim->setEndValue(newPos);
        anim->setDuration(1500);
        anim->setEasingCurve(QEasingCurve(QEasingCurve::OutInBack));
        group->addAnimation(anim);
        if (randomInt(0, 10) > 5)
            widget->raise();
#else
        widget->setPosition(newPos);
#endif

    }
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    group->start(QAbstractAnimation::DeleteWhenStopped);
#endif

}

void PuzzleWidget::raise()
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
            else if (item != this && item->zValue() == this->zValue())
            {
                item->stackBefore(this);
            }
        }
        setZValue(max);
    }

}
