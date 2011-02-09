#include "puzzleboard.h"
#include "puzzleitem.h"

PuzzleBoard::PuzzleBoard(QObject *parent) :
    QGraphicsScene(parent)
{
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
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

#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
bool PuzzleBoard::isDropshadowActive()
{
    foreach (QGraphicsItem *gi, items())
    {
        if (PuzzleItem *item = dynamic_cast<PuzzleItem*>(gi))
        {
            if (QGraphicsDropShadowEffect *effect = dynamic_cast<QGraphicsDropShadowEffect*>(item->graphicsEffect()))
            {
                Q_UNUSED(effect);
                return true;
            }
        }
    }
    return false;
}

void PuzzleBoard::enableDropshadow()
{
    QColor c(0, 0, 0, 200);
    foreach (QGraphicsItem *gi, items())
    {
        if (PuzzleItem *item = dynamic_cast<PuzzleItem*>(gi))
        {
            QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
            effect->setBlurRadius(20);
            effect->setOffset(0);
            effect->setColor(c);
            item->setGraphicsEffect(effect);
        }
    }
}

void PuzzleBoard::disableDropshadow()
{
    foreach (QGraphicsItem *gi, items())
    {
        if (PuzzleItem *item = dynamic_cast<PuzzleItem*>(gi))
        {
            item->setGraphicsEffect(0);
        }
    }
}
#endif

#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
bool PuzzleBoard::isAccelerometerActive()
{
    return accelerometer->isActive();
}

void PuzzleBoard::accelerometerReadingChanged()
{
    QtMobility::QAccelerometerReading *reading = accelerometer->reading();
    accelerometerMovement(reading->x(), reading->y(), reading->z());
}

void PuzzleBoard::enableAccelerometer()
{
    accelerometer->connectToBackend();
    accelerometer->start();
}

void PuzzleBoard::disableAccelerometer()
{
    accelerometer->stop();
}
#endif
