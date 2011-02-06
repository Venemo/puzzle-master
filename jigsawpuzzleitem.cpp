#include <QtGui>
#include "jigsawpuzzleitem.h"
#include "jigsawpuzzleboard.h"

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

JigsawPuzzleItem::JigsawPuzzleItem(const QPixmap &pixmap, const QSize &unitSize, QGraphicsItem *parent, QGraphicsScene *scene) :
    QObject(scene),
    PuzzleItem(pixmap, parent, scene),
    _unit(unitSize),
    _dragging(false),
    _canMerge(false),
    _tolerance(5),
    _weight(randomInt(50, 950) / 1000.0)
{
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

void JigsawPuzzleItem::setTolerance(int t)
{
    _tolerance = t;
}

int JigsawPuzzleItem::tolerance()
{
    return _tolerance;
}

bool JigsawPuzzleItem::merge(PuzzleItem *piece)
{
    JigsawPuzzleItem *w = (JigsawPuzzleItem*)piece;
    if (PuzzleItem::merge(piece))
    {
        w->_canMerge = _canMerge = false;

        QPoint vector = w->puzzleCoordinates() - puzzleCoordinates();
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
        setPuzzleCoordinates(puzzleCoordinates() - QPoint(u1, v1));
        setPos(pos().x() - x1, pos().y() - y1);
        _dragStart = _dragStart + QPointF(x1, y1);
        w->hide();
        delete w;
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

void JigsawPuzzleItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && _canMerge)
    {
        _dragging = true;
        _dragStart = event->pos();
        raise();
    }
}

void JigsawPuzzleItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        _dragging = false;
}

void JigsawPuzzleItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (_dragging)
    {
        setPos(pos() + event->pos() - _dragStart);

        if (_canMerge)
        {
            foreach (PuzzleItem *p, neighbours())
            {
                JigsawPuzzleItem *w = (JigsawPuzzleItem*) p;
                QPointF posDiff1 = pos() - w->pos();
                QPointF posDiff2 = (puzzleCoordinates() * _unit - w->puzzleCoordinates() * _unit);
                QPointF relative = posDiff1 - posDiff2;

                if (abs(relative.x()) < _tolerance && abs(relative.y()) < _tolerance)
                {
                    merge(w);
                }
            }

            verifyPosition();
            verifyCoveredSiblings();
        }
    }
}

void JigsawPuzzleItem::verifyPosition()
{
    int x = pos().x();// + puzzleCoordinates().x() * _unit.width();
    int maxX = scene()->width() - _unit.width() / 2;
    int minX = - pixmap().width() + _unit.width() / 2;

    int y = pos().y();// + puzzleCoordinates().y() * _unit.height();
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

void JigsawPuzzleItem::shuffle(QList<JigsawPuzzleItem *> *list, int width, int height)
{
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    QParallelAnimationGroup *group = new QParallelAnimationGroup();
#endif
    foreach (JigsawPuzzleItem *item, *list)
    {
        QPointF newPos(randomInt(0, width - item->_unit.width()), randomInt(0, height - item->_unit.width()));

#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
        connect(group, SIGNAL(finished()), item, SLOT(enableMerge()));
        QPropertyAnimation *anim = new QPropertyAnimation(item, "pos", group);
        anim->setEndValue(newPos);
        anim->setDuration(2000);
        anim->setEasingCurve(QEasingCurve(QEasingCurve::OutElastic));
        group->addAnimation(anim);
        if (randomInt(0, 10) > 5)
            item->raise();
#else
        item->setPos(newPos);
#endif

    }
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    connect(group, SIGNAL(finished()), group, SLOT(deleteLater()));
    QTimer::singleShot(1000, group, SLOT(start()));
#endif
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
            else if (item != this && item->zValue() == this->zValue())
            {
                item->stackBefore(this);
            }
        }
        setZValue(max);
    }
}

void JigsawPuzzleItem::verifyCoveredSiblings()
{
    foreach (QGraphicsItem *gi, scene()->items())
    {
        JigsawPuzzleItem *item = (JigsawPuzzleItem*)gi;
        if (item != this && item->zValue() < zValue() && item->pos().x() >= pos().x() && item->pos().y() >= pos().y() && item->pixmap().width() < pixmap().width() && item->pixmap().height() < pixmap().height())
        {
            item->raise();
            break;
        }
    }
}

void JigsawPuzzleItem::assemble(QList<JigsawPuzzleItem *> *list, int width, int height)
{
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    QParallelAnimationGroup *group = new QParallelAnimationGroup();
#endif
    foreach (JigsawPuzzleItem *item, *list)
    {
        QPointF newPos((item->scene()->width() - width) / 2 + (item->puzzleCoordinates().x() * item->_unit.width()),
                      (item->scene()->height() - height) / 2 + (item->puzzleCoordinates().y() * item->_unit.height()));
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
        QPropertyAnimation *anim = new QPropertyAnimation(item, "pos", group);
        anim->setEndValue(newPos);
        anim->setDuration(2000);
        anim->setEasingCurve(QEasingCurve(QEasingCurve::OutElastic));
        group->addAnimation(anim);
#else
        widget->setPos(newPos);
#endif

    }
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    group->start(QAbstractAnimation::DeleteWhenStopped);
#endif
}
