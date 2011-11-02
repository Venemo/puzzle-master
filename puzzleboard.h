#ifndef PUZZLEBOARD_H
#define PUZZLEBOARD_H

#include <QGraphicsScene>
#include <QTimer>

#if defined(HAVE_QACCELEROMETER)
#include <QAccelerometer>
#endif

class PuzzleItem;

class PuzzleBoard : public QGraphicsScene
{
    Q_OBJECT

    QSize _originalPixmapSize;
    qreal _originalScaleRatio;
    QTimer *_fixedFPSTimer;
#if defined(HAVE_QACCELEROMETER)
    QtMobility::QAccelerometer *accelerometer;
#endif

public:
    explicit PuzzleBoard(QObject *parent = 0);
    virtual void startGame(const QPixmap &pixmap, unsigned rows, unsigned cols, bool allowMultitouch) = 0;
    const QSize &originalPixmapSize() const;
    void setNeighbours(int x, int y);
    void setOriginalPixmapSize(const QSize &size);
    PuzzleItem *find(const QPoint &puzzleCoordinates);
    bool isDropshadowActive() const;
    bool isAccelerometerActive() const;
    qreal originalScaleRatio() const;
    void setOriginalScaleRatio(qreal value);

protected:
    virtual void accelerometerMovement(qreal x, qreal y, qreal z) = 0;

signals:
    void gameStarted();
    void gameEnded();
    void gameWon();
    void loaded();
    void loadProgressChanged(int progress);

private slots:
    void accelerometerReadingChanged();

public slots:
    virtual void surrenderGame() = 0;
    void enableDropshadow();
    void disableDropshadow();
    void enableAccelerometer();
    void disableAccelerometer();
    virtual void disable() = 0;
    virtual void enable() = 0;
    void enableFixedFPS();
    void disableFixedFPS();

};

#endif // PUZZLEBOARD_H
