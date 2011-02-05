#include "puzzleboard.h"

PuzzleBoard::PuzzleBoard(QObject *parent) :
    QGraphicsScene(parent)
{
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
    accelerometer = new QtMobility::QAccelerometer(this);
    connect(accelerometer, SIGNAL(readingChanged()), this, SLOT(accelerometerReadingChanged()));
#endif
}

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
