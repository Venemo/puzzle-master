#ifndef PUZZLEBOARD_H
#define PUZZLEBOARD_H

#include <QGraphicsScene>
#include <QTimer>

#if defined(HAVE_QACCELEROMETER)
#include <QAccelerometer>
#endif

#include "util.h"

class PuzzleItem;

class PuzzleBoard : public QGraphicsScene
{
    Q_OBJECT
    Q_PROPERTY(bool isDropshadowActive READ isDropshadowActive NOTIFY isDropshadowActiveChanged)
    Q_PROPERTY(bool isAccelerometerActive READ isAccelerometerActive NOTIFY isAccelerometerActiveChanged)
    GENPROPERTY(QSize, _originalPixmapSize, originalPixmapSize, setOriginalPixmapSize, originalPixmapSizeChanged)
    Q_PROPERTY(QSize originalPixmapSize READ originalPixmapSize WRITE setOriginalPixmapSize NOTIFY originalPixmapSizeChanged)
    GENPROPERTY(qreal, _originalScaleRatio, originalScaleRatio, setOriginalScaleRatio, originalScaleRatioChanged)
    Q_PROPERTY(qreal originalScaleRatio READ originalScaleRatio WRITE setOriginalScaleRatio NOTIFY originalScaleRatioChanged)

    QTimer *_fixedFPSTimer;
#if defined(HAVE_QACCELEROMETER)
    QtMobility::QAccelerometer *accelerometer;
#endif

public:
    explicit PuzzleBoard(QObject *parent = 0);
    virtual void startGame(const QPixmap &pixmap, unsigned rows, unsigned cols, bool allowMultitouch) = 0;
    void setNeighbours(int x, int y);
    PuzzleItem *find(const QPoint &puzzleCoordinates);
    bool isDropshadowActive() const;
    bool isAccelerometerActive() const;

protected:
    virtual void accelerometerMovement(qreal x, qreal y, qreal z) = 0;

signals:
    void isDropshadowActiveChanged();
    void isAccelerometerActiveChanged();
    void originalPixmapSizeChanged();
    void originalScaleRatioChanged();

    void gameStarted();
    void gameEnded();
    void gameWon();
    void loaded();
    void loadProgressChanged(int progress);

private slots:
    void accelerometerReadingChanged();

public slots:
    Q_INVOKABLE virtual void surrenderGame() = 0;
    Q_INVOKABLE void enableDropshadow();
    Q_INVOKABLE void disableDropshadow();
    Q_INVOKABLE void enableAccelerometer();
    Q_INVOKABLE void disableAccelerometer();
    Q_INVOKABLE virtual void disable() = 0;
    Q_INVOKABLE virtual void enable() = 0;
    Q_INVOKABLE void enableFixedFPS();
    Q_INVOKABLE void disableFixedFPS();

};

#endif // PUZZLEBOARD_H
