
#include <QtGui>
#include <QtDeclarative>

#include "puzzleboard.h"
#include "puzzleitem.h"

PuzzleBoard::PuzzleBoard(QDeclarativeItem *parent) :
    QDeclarativeItem(parent),
    _originalScaleRatio(1),
    _tolerance(5),
    _rotationTolerance(10),
    _allowMultitouch(false),
    _fixedFPSTimer(0)
{
#if defined(HAVE_QACCELEROMETER)
    accelerometer = new QtMobility::QAccelerometer(this);
    connect(accelerometer, SIGNAL(readingChanged()), this, SLOT(accelerometerReadingChanged()));
#endif
}

void PuzzleBoard::setNeighbours(int x, int y)
{
    foreach(QGraphicsItem *gi, childItems())
    {
        PuzzleItem *p = (PuzzleItem*)gi;
        if (p->puzzleCoordinates().x() != x - 1)
            p->addNeighbour(find(p->puzzleCoordinates() + QPoint(1, 0)));

        if (p->puzzleCoordinates().y() != y - 1)
            p->addNeighbour(find(p->puzzleCoordinates() + QPoint(0, 1)));
    }
}

PuzzleItem *PuzzleBoard::find(const QPoint &puzzleCoordinates)
{
    foreach(QGraphicsItem *gi, childItems())
    {
        PuzzleItem *p = (PuzzleItem*)gi;
        if (p->puzzleCoordinates() == puzzleCoordinates)
            return p;
    }
    return 0;
}

bool PuzzleBoard::isDropshadowActive() const
{
    foreach (QGraphicsItem *item, childItems())
    {
        if (dynamic_cast<QGraphicsDropShadowEffect*>(item->graphicsEffect()))
        {
            return true;
        }
    }
    return false;
}

void PuzzleBoard::enableDropshadow()
{
    QColor c = DROPSHADOW_COLOR_DEFAULT;
    foreach (QGraphicsItem *item, childItems())
    {
        QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
        effect->setBlurRadius(DROPSHADOW_RADIUS_DEFAULT);
        effect->setOffset(0);
        effect->setColor(c);
        item->setGraphicsEffect(effect);
    }
}

void PuzzleBoard::disableDropshadow()
{
    foreach (QGraphicsItem *item, childItems())
    {
        item->setGraphicsEffect(0);
    }
}

bool PuzzleBoard::isAccelerometerActive() const
{
#if defined(HAVE_QACCELEROMETER)
    return accelerometer->isActive();
#else
    return false;
#endif
}

void PuzzleBoard::accelerometerReadingChanged()
{
#if defined(HAVE_QACCELEROMETER)
    QtMobility::QAccelerometerReading *reading = accelerometer->reading();
#if defined(Q_WS_MAEMO_5)
    if (views().at(0)->width() < views().at(0)->height())
        accelerometerMovement(reading->y(), - reading->x(), reading->z());
    else
#elif defined(Q_OS_SYMBIAN)
    if (views().at(0)->width() > views().at(0)->height())
        accelerometerMovement(- reading->y(), reading->x(), reading->z());
    else
#endif
        accelerometerMovement(reading->x(), reading->y(), reading->z());
#endif
}

void PuzzleBoard::enableAccelerometer()
{
#if defined(HAVE_QACCELEROMETER)
    accelerometer->connectToBackend();
    accelerometer->start();
#endif
}

void PuzzleBoard::disableAccelerometer()
{
#if defined(HAVE_QACCELEROMETER)
    accelerometer->stop();
#endif
}

void PuzzleBoard::enableFixedFPS()
{
    if (_fixedFPSTimer)
    {
        _fixedFPSTimer->stop();
        _fixedFPSTimer->deleteLater();
    }

    _fixedFPSTimer = new QTimer(this);
    _fixedFPSTimer->setInterval(20);

    foreach (QGraphicsView *view, scene()->views())
    {
        view->setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
        connect(_fixedFPSTimer, SIGNAL(timeout()), view->viewport(), SLOT(update()));
    }

    _fixedFPSTimer->start();
}

void PuzzleBoard::disableFixedFPS()
{
    if (_fixedFPSTimer)
    {
        _fixedFPSTimer->stop();
        _fixedFPSTimer->deleteLater();
        _fixedFPSTimer = 0;
    }

    foreach (QGraphicsView *view, scene()->views())
    {
        view->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    }
}

void PuzzleBoard::startGame(const QString &imageUrl, unsigned rows, unsigned cols, bool allowMultitouch)
{
    QPixmap image(imageUrl);

    if (image.isNull())
    {
        qDebug() << "image is null, not starting game.";
        return;
    }

    int w = min<int>(width(), image.width());
    int h = min<int>(height(), image.height());

    if (w == 0 || h == 0)
    {
        qDebug() << "The size of the image or the size of this item is not okay, not starting game.";
        return;
    }

    _allowMultitouch = allowMultitouch;

    QPixmap pixmap = image.scaled(w, h, Qt::KeepAspectRatio);
    setOriginalPixmapSize(pixmap.size());
    setOriginalScaleRatio(1);
    _unit = QSize(pixmap.width() / cols, pixmap.height() / rows);
    QPainter p;

    qreal w0 = (width() - pixmap.width()) / 2;
    qreal h0 = (height() - pixmap.height()) / 2;

    for (unsigned i = 0; i < cols; i++)
    {
        for (unsigned j = 0; j < rows; j++)
        {
            // creating the pixmap for the piece
            QPixmap px(_unit.width(), _unit.height());
            px.fill(Qt::transparent);
            p.begin(&px);
            p.setRenderHint(QPainter::HighQualityAntialiasing);
            p.drawPixmap(0, 0, pixmap, i * _unit.width(), j * _unit.height(), _unit.width(), _unit.height());
            p.end();

            // creating the piece
            PuzzleItem *item = new PuzzleItem(px, this);
            item->setWidth(_unit.width());
            item->setHeight(_unit.height());
            item->setPuzzleCoordinates(QPoint(i, j));
            item->setSupposedPosition(item->puzzleCoordinates() * _unit);
            connect(item, SIGNAL(noNeighbours()), this, SIGNAL(gameWon()));

            QPointF oldPos(w0 + (i * _unit.width()),
                           h0 + (j * _unit.height()));
            item->setPos(oldPos);
            item->show();

            emit loadProgressChanged(i * rows + j + 1);
        }
    }
    setNeighbours(cols, rows);
    emit loaded();

    QTimer::singleShot(1000, this, SLOT(shuffle()));
}

void PuzzleBoard::shuffle()
{
    QParallelAnimationGroup *group = new QParallelAnimationGroup();
    QEasingCurve easingCurve(QEasingCurve::InExpo);

    foreach (QGraphicsItem *gi, childItems())
    {
        PuzzleItem *item = (PuzzleItem*)gi;
        QPointF newPos(randomInt(0, originalPixmapSize().width() - _unit.width()), randomInt(0, originalPixmapSize().height() - _unit.width()));

        connect(group, SIGNAL(finished()), item, SLOT(enableMerge()));
        QPropertyAnimation *anim = new QPropertyAnimation(item, "pos", group);
        anim->setEndValue(newPos);
        anim->setDuration(2000);
        anim->setEasingCurve(easingCurve);
        group->addAnimation(anim);

        if (_allowMultitouch)
        {
            QPropertyAnimation *rotateAnimation = new QPropertyAnimation(item, "rotation", group);
            rotateAnimation->setEndValue(randomInt(0, 359));
            rotateAnimation->setDuration(2000);
            rotateAnimation->setEasingCurve(easingCurve);
            group->addAnimation(rotateAnimation);
        }
        if (randomInt(0, 10) > 5)
            item->raise();
    }

    connect(group, SIGNAL(finished()), this, SLOT(disableFixedFPS()));
    connect(group, SIGNAL(finished()), this, SIGNAL(gameStarted()));
    enableFixedFPS();
    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void PuzzleBoard::assemble()
{
    QParallelAnimationGroup *group = new QParallelAnimationGroup();
    QEasingCurve easingCurve(QEasingCurve::OutExpo);

    foreach (QGraphicsItem *gi, childItems())
    {
        PuzzleItem *item = (PuzzleItem*)gi;
        item->disableMerge();
        QPointF newPos((item->scene()->width() - originalPixmapSize().width()) / 2 + (item->puzzleCoordinates().x() * _unit.width()),
                       (item->scene()->height() - originalPixmapSize().height()) / 2 + (item->puzzleCoordinates().y() * _unit.height()));

        QPropertyAnimation *anim = new QPropertyAnimation(item, "pos", group);
        anim->setEndValue(newPos);
        anim->setDuration(2000);
        anim->setEasingCurve(easingCurve);
        group->addAnimation(anim);

        if (_allowMultitouch)
        {
            QPropertyAnimation *rotateAnimation = new QPropertyAnimation(item, "rotation", group);
            rotateAnimation->setEndValue(0);
            rotateAnimation->setDuration(2000);
            rotateAnimation->setEasingCurve(easingCurve);
            group->addAnimation(rotateAnimation);
        }
    }

    connect(group, SIGNAL(finished()), this, SLOT(disableFixedFPS()));
    connect(group, SIGNAL(finished()), this, SIGNAL(gameEnded()));
    enableFixedFPS();
    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void PuzzleBoard::surrenderGame()
{
    assemble();
}

void PuzzleBoard::accelerometerMovement(qreal x, qreal y, qreal z)
{
    Q_UNUSED(z);
    foreach (QGraphicsItem *item, childItems())
    {
        PuzzleItem *widget = (PuzzleItem*) item;
        if (widget->canMerge())
        {
            widget->setPos(widget->pos().x() - x * widget->weight() / 2, widget->pos().y() + y * widget->weight() / 2);
            widget->verifyPosition();
        }
    }
}

void PuzzleBoard::enable()
{
    foreach (QGraphicsItem *item, childItems())
    {
        PuzzleItem *widget = (PuzzleItem*) item;
        widget->enableMerge();
    }
}

void PuzzleBoard::disable()
{
    foreach (QGraphicsItem *item, childItems())
    {
        PuzzleItem *widget = (PuzzleItem*) item;
        widget->disableMerge();
    }
}
