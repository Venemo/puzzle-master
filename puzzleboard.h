#ifndef PUZZLEBOARD_H
#define PUZZLEBOARD_H

#include <QGraphicsScene>

#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
#include <QAccelerometer>
#endif

class PuzzleBoard : public QGraphicsScene
{
    Q_OBJECT
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
    QtMobility::QAccelerometer *accelerometer;
#endif

public:
    explicit PuzzleBoard(QObject *parent = 0);
    virtual void startGame(const QPixmap &pixmap, unsigned rows, unsigned cols) = 0;
    bool isDropshadowActive();
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
    bool isAccelerometerActive();
#endif

protected:
    virtual void accelerometerMovement(qreal x, qreal y, qreal z) = 0;

signals:
    void gameStarted();
    void gameWon();
    void loaded();
    void loadProgressChanged(int progress);

private slots:
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
    void accelerometerReadingChanged();
#endif

public slots:
    virtual void surrenderGame() = 0;
    void enableDropshadow();
    void disableDropshadow();
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
    void enableAccelerometer();
    void disableAccelerometer();
#endif

};

#endif // PUZZLEBOARD_H
