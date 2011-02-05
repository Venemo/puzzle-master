#include <QtGui>
#include "puzzlewidget.h"

static int randomInt(int low, int high)
{
    // Random number between low and high
    return qrand() % ((high + 1) - low) + low;
}

PuzzleWidget::PuzzleWidget(const QPixmap &pixmap, const QSize &unitSize, QWidget *parent) :
    QLabel(parent), PuzzlePiece(), _unit(unitSize), _dragging(false), _canMerge(false), _tolerance(5), _weight(randomInt(50, 950) / 1000.0)
{
    setPixmap(pixmap);
    setMask(pixmap.mask());
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
        //_weight *= sqrt(w->_weight);
        QPixmap pix(pixmap()->width(), pixmap()->height());
        pix.fill(Qt::transparent);

        QPainter p;
        p.begin(&pix);
        p.setClipping(false);

        p.drawPixmap(0, 0, *pixmap());
        p.drawPixmap(0, 0, *w->pixmap());

        p.end();

        setPixmap(pix);
        setMask(pix.mask());

        w->deleteLater();
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
#endif
        }

        return true;
    }
    return false;
}

void PuzzleWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && _canMerge)
    {
        _dragging = true;
        _dragStart = event->pos();
        raise();
    }
}

void PuzzleWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        _dragging = false;
}

void PuzzleWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (_dragging)
    {
        setGeometry(QRect(geometry().topLeft() + event->pos() - _dragStart, rect().size()));
        //qDebug() << "moved" << position();

        if (_canMerge)
        {
            foreach (PuzzlePiece *p, neighbours())
            {
                PuzzleWidget *w = (PuzzleWidget*) p;
                QPoint posDiff = pos() - w->pos();

                if (abs(posDiff.x()) < _tolerance && abs(posDiff.y()) < _tolerance)
                {
                    merge(w);
                }
            }
        }
    }
}

void PuzzleWidget::moveEvent(QMoveEvent *ev)
{
    QLabel::moveEvent(ev);

    int x = (pos().x() + mask().boundingRect().x());
    int maxX = parentWidget()->width() - _unit.width() / 2;
    int minX = - _unit.width() / 2;
    int y = (pos().y() + mask().boundingRect().y());
    int maxY = parentWidget()->height() - _unit.height() / 2;
    int minY = - _unit.height() / 2;

    int xx = mask().boundingRect().width() - _unit.width();
    int yy = mask().boundingRect().height() - _unit.height();

    if (!(x < maxX && x > (minX - xx) && y < maxY && y > (minY - yy)))
    {
        int pX = x >= maxX ? pos().x() - 50 : (x <= minX ? pos().x() + 50 : pos().x());
        int pY = y >= maxY ? pos().y() - 50 : (y <= minY ? pos().y() + 50 : pos().y());
        _canMerge = false;
        _dragging = false;
        QPropertyAnimation *anim = new QPropertyAnimation(this, "pos", this);
        anim->setEndValue(QPoint(pX, pY));
        anim->setDuration(200);
        anim->setEasingCurve(QEasingCurve(QEasingCurve::OutBounce));
        connect(anim, SIGNAL(finished()), this, SLOT(enableMerge()));
        anim->start(QAbstractAnimation::DeleteWhenStopped);
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
        QPoint newPos(randomInt(0, width) - widget->position().x() * widget->_unit.width(), randomInt(0, height) - widget->position().y() * widget->_unit.height());

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
