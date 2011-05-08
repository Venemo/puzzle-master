#include "puzzleboard.h"
#include "puzzleitem.h"

PuzzleBoard::PuzzleBoard(QObject *parent) :
    QGraphicsScene(parent),
    _originalScaleRatio(1)
{
#if defined(HAVE_QACCELEROMETER)
    accelerometer = new QtMobility::QAccelerometer(this);
    connect(accelerometer, SIGNAL(readingChanged()), this, SLOT(accelerometerReadingChanged()));
#endif
}

const QSize &PuzzleBoard::originalPixmapSize()
{
    return _originalPixmapSize;
}

void PuzzleBoard::setOriginalPixmapSize(const QSize &size)
{
    _originalPixmapSize = size;
}

qreal PuzzleBoard::originalScaleRatio()
{
    return _originalScaleRatio;
}

void PuzzleBoard::setOriginalScaleRatio(qreal value)
{
    _originalScaleRatio = value;
}

void PuzzleBoard::setNeighbours(int x, int y)
{
    if (items().count() != x * y)
    {
        qDebug() << "The Puzzle piece list was inconsistent with x and y";
        return;
    }

    foreach(QGraphicsItem *gi, items())
    {
        PuzzleItem *p = (PuzzleItem*)gi;
        if (p->puzzleCoordinates().x() != x - 1)
            p->addNeighbour(find(p->puzzleCoordinates() + QPoint(1, 0)));

        if (p->puzzleCoordinates().y() != y - 1)
            p->addNeighbour(find(p->puzzleCoordinates() + QPoint(0, 1)));
    }
}

PuzzleItem *PuzzleBoard::find(QPoint puzzleCoordinates)
{
    foreach(QGraphicsItem *gi, items())
    {
        PuzzleItem *p = (PuzzleItem*)gi;
        if (p->puzzleCoordinates() == puzzleCoordinates)
            return p;
    }
    return 0;
}

bool PuzzleBoard::isDropshadowActive()
{
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    foreach (QGraphicsItem *item, items())
    {
        if (dynamic_cast<QGraphicsDropShadowEffect*>(item->graphicsEffect()))
        {
            return true;
        }
    }
#endif
    return false;
}

void PuzzleBoard::enableDropshadow()
{
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    QColor c = DROPSHADOW_COLOR_DEFAULT;
    foreach (QGraphicsItem *item, items())
    {
        QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
        effect->setBlurRadius(DROPSHADOW_RADIUS_DEFAULT);
        effect->setOffset(0);
        effect->setColor(c);
        item->setGraphicsEffect(effect);
    }
#endif
}

void PuzzleBoard::disableDropshadow()
{
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    foreach (QGraphicsItem *item, items())
    {
        item->setGraphicsEffect(0);
    }
#endif
}

bool PuzzleBoard::isAccelerometerActive()
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
